/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

/*

TODO

- To guarantee correctness, keep the mock design in the functions
- For each function, run both name-tree code and mock design
  and then compare their results. They should always match.

*/


#include "pit.hpp"
#include <algorithm>

namespace nfd {

Pit::Pit()
{
}

Pit::Pit(NameTree * nt){
  m_nt = nt;
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


// This function can probably kept the same in the PIT with NPHT
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
  // #### NPHT DESIGN

  // ## insert to the NPHT

  // get the name from the Interest
  Name iName = interest.getName();
  // lookup the NPHT
  NamePrefixEntry * npe = nt.lookup(iName)  // if npe == NULL, there is an error

  // let the NPE handle the actual PIT insertion
  npe.addPIT(interest);

  // #### MOCK DESIGN

  std::list<shared_ptr<pit::Entry> >::iterator it = std::find_if(
    m_table.begin(), m_table.end(),
    bind(&predicate_PitEntry_similar_Interest, _1, interest));
  if (it != m_table.end()) return std::make_pair(*it, false);
  
  shared_ptr<pit::Entry> entry = make_shared<pit::Entry>(interest);
  m_table.push_back(entry);
  return std::make_pair(entry, true);
}

shared_ptr<pit::DataMatchResult>
Pit::findAllDataMatches(const Data& data) const
{

  // #### NPHT DESIGN

  // ## Perform all match lookups via NPHT lookup function
  Name dName = data.getName();

  // npe = nt lookup the longest prefix
  NamePrefixEntry * npe = nt->lpm(dName);

  // check npe's PIT entries
  pit::Entry * pit = npe->getPIT();

  // todo: compare the names stored in the PIT

  // follow npe's parent pointer, and check all the corresponding PIT entries
  npe = npe->getParent();
  if(npe){
    // XXXX
  }

  // #### MOCK DESIGN

  shared_ptr<pit::DataMatchResult> result = make_shared<pit::DataMatchResult>();
  for (std::list<shared_ptr<pit::Entry> >::const_iterator it = m_table.begin();
       it != m_table.end(); ++it) {
    shared_ptr<pit::Entry> entry = *it;
    if (entry->getInterest().matchesName(data.getName())) {
      result->push_back(entry);
    }
  }
  return result;
}


void
Pit::remove(shared_ptr<pit::Entry> pitEntry)
{
  // #### NPHT DESIGN


  // XXX DESIGN: which function(s) will call the remove function

  // ## remove pitEntry from NPHT

  // find the prefix
  // npe = nt.lookup(pitEntry_name);

  // deletePIT should be a function in NPE
  // npe->deletePIT(pitEntry);

  // #### MOCK DESIGN

  m_table.remove(pitEntry);
}

} // namespace nfd


int main(){

  using namespace nfd;
  using namespace std;

  // Initialize the NameTree
  int ntSize = 1024;
  NameTree * nt = new NameTree(ntSize);

  // Initialize the PIT
  PIT * pit = new PIT(nt);

}

