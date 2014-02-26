/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

/**
 *  KNOWN ISSUES
 *  
 *  - The purpose of this commit is to get feedback on the general design.
 *  
 *  - This code will be updated if NameTree functions are updated.
 *  
 *  - Pit::Pit() function is kept in this code, as forwarder.cpp line 19 calls it.
 *  As PIT is based on NameTree, the forwarder should create a NameTree first, and
 *  then create the PIT with function Pit::Pit(NameTree Pointer).
 *  
 *  - To remove a PIT entry, we need to first perform a lookup on NameTree
 *  to locate its NameTree Entry, and then call NameTreeEntry->deletePitEntry()
 *  function. Alternatively, we could store a pointer at each PIT-Entry, which
 *  would speed up this procedure with the cost of additional memory space. Maybe
 *  this could also be part of the PIT/FIB/Measurement shortcut, where all of these
 *  entries have pointers to their NameTreeEntry. Which could be part of task
 *  #1202, shortcuts between FIB, PIT, Measurements.
 *  
 *  - Currently, remove() function has no return value.
 *  
 *  - Function findParent() is shown in the UML but not described on Redmine or 
 *  mock design.
 *  
 */

#include "pit.hpp"

namespace nfd {

Pit::Pit()
{
}

Pit::Pit(NameTree* nt) : m_nt(nt)
{
}

Pit::~Pit()
{
}

static inline bool
operator==(const Exclude& a, const Exclude& b)
{
  const Block& aBlock = a.wireEncode();
  const Block& bBlock = b.wireEncode();
  return aBlock.size() == bBlock.size() &&
         0 == memcmp(aBlock.wire(), bBlock.wire(), aBlock.size());
}

static inline bool
predicate_PitEntry_similar_Interest(shared_ptr<pit::Entry> entry,
                                    const Interest& interest)
{
  const Interest& pi = entry->getInterest();
  return pi.getName().equals(interest.getName()) &&
         pi.getMinSuffixComponents() == interest.getMinSuffixComponents() &&
         pi.getMaxSuffixComponents() == interest.getMaxSuffixComponents() &&
         // TODO PublisherPublicKeyLocator (ndn-cpp-dev #1157)
         pi.getExclude() == interest.getExclude() &&
         pi.getChildSelector() == interest.getChildSelector() &&
         pi.getMustBeFresh() == interest.getMustBeFresh();
}

std::pair<shared_ptr<pit::Entry>, bool>
Pit::insert(const Interest& interest)
{
  // 1.) First seek() the Interest Name in the NameTree
  // 2.) If it is guaranteed that this Interest already has a NameTree Entry (done
  // by other functions), we could use lookup() instead.
  // 3.) Alternatively, we could try to do lookup() first, if not found, then seek().
  shared_ptr<name_tree::Entry> nameTreeEntry = m_nt->seek(interest.getName());

  BOOST_ASSERT(static_cast<bool>(nameTreeEntry));

  std::vector<shared_ptr<pit::Entry> >& pitEntries = nameTreeEntry->getPitEntries();

  // then check if this Interest is already in the PIT entries
  for (size_t i = 0; i < pitEntries.size(); i++)
  {
    if (predicate_PitEntry_similar_Interest(pitEntries[i], interest))
    {
      return std::make_pair(pitEntries[i], false);
    }
  }

  shared_ptr<pit::Entry> entry = make_shared<pit::Entry>(interest);
  nameTreeEntry->insertPitEntry(entry);

  return std::make_pair(entry, true);
}

shared_ptr<pit::DataMatchResult>
Pit::findAllDataMatches(const Data& data) const
{
  shared_ptr<pit::DataMatchResult> result = make_shared<pit::DataMatchResult>();

  shared_ptr<name_tree::Entry> nameTreeEntry;

  // 1.) We are not using lookup() as it is possible that a Data packet (/a/b/c)
  // does not have a corresponding NameTree Entry (/a/b/c), but could still
  // satisfly NameTree Entry (/a) or (/a/b) 
  // 2.) We can call NameTree::LPM now as LPM currently starts from full name, and remove 
  // one name component each time (i.e., starts from the end).
  // 3.) If LPM is modified to start from front, we cannot use it here. And in that case,
  // it might be better to have another function that can perform LPM from the end (e.g., 
  // findLongestPrefixMatchFromEnd) 
  for (nameTreeEntry = m_nt->longestPrefixMatch(data.getName()); 
                               static_cast<bool>(nameTreeEntry);
                               nameTreeEntry = nameTreeEntry->getParent())
  {
    std::vector<shared_ptr<pit::Entry> >& pitEntries = nameTreeEntry->getPitEntries();
    for (size_t i = 0; i < pitEntries.size(); i++)
    {
      if (pitEntries[i]->getInterest().matchesName(data.getName()))
      {
        result->push_back(pitEntries[i]);
      }
    }
  }

  return result;
}

void
Pit::remove(shared_ptr<pit::Entry> pitEntry)
{
  // first get the NPE
  shared_ptr<name_tree::Entry> nameTreeEntry = m_nt->lookup(pitEntry->getName());

  BOOST_ASSERT(static_cast<bool>(nameTreeEntry));

  // remove this PIT entry
  if (static_cast<bool>(nameTreeEntry)) 
  {
    nameTreeEntry->deletePitEntry(pitEntry);
    m_nt->deleteEntryIfEmpty(nameTreeEntry);
  }
}

} // namespace nfd


