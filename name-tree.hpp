/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Tree (Name Prefix Hash Table)

#ifndef NFD_TABLE_NAME_TREE_HPP
#define NFD_TABLE_NAME_TREE_HPP

#include "common.hpp"
#include "name-tree-entry.hpp"

namespace nfd {
namespace name_tree {

/**
 * @brief Compute the hash value of the given name prefix.
 * @todo 1) have another parameter that specifies the number of components
 * 2) optimize the hash function to return a list of have values for all
 * the (or a certain number of, like a batch operation) prefixes. 3) move
 * the hash-related code to a separate file in /core or ndn-cpp-dev lib.
 */
uint32_t
hashName(const Name& prefix);

/// a predicate to accept or reject an Entry in find operations
typedef function<bool (const Entry& entry)> EntrySelector;

/**
 * @brief a predicate to accept or reject an Entry and its children
 * @return .first indicates whether entry should be accepted;
 *         .second indicates whether entry's children should be visited
 */
typedef function<std::pair<bool,bool> (const Entry& entry)> EntrySubTreeSelector;

struct AnyEntry {
  bool
  operator()(const Entry& entry)
  {
    return true;
  }
};

struct AnyEntrySubTree {
  std::pair<bool, bool>
  operator()(const Entry& entry)
  {
    return std::make_pair(true, true);
  }
};

} // namespace name_tree

/**
 * @brief Class Name Tree
 */
class NameTree : noncopyable
{
public:
  class const_iterator;

  explicit
  NameTree(size_t nBuckets);

  ~NameTree();

  /**
   * @brief Get the number of occupied entries in the Name Tree
   */
  size_t
  size() const;

  /**
   * @brief Get the number of buckets in the Name Tree (NPHT)
   * @details The number of buckets is the one that used to create the hash
   * table, i.e., m_nBuckets.
   */
  size_t
  getNBuckets() const;

  /**
   * @brief Look for the Name Tree Entry that contains this name prefix.
   * @details Starts from the shortest name prefix, and then increase the
   * number of name components by one each time. All non-existing Name Tree
   * Entries will be created.
   * @param prefix The querying name prefix.
   * @return The pointer to the Name Tree Entry that contains this full name
   * prefix.
   */
  shared_ptr<name_tree::Entry>
  lookup(const Name& prefix);

  /**
   * @brief Exact match lookup for the given name prefix.
   * @return a null shared_ptr if this prefix is not found;
   * otherwise return the Name Tree Entry address
   */
  shared_ptr<name_tree::Entry>
  findExactMatch(const Name& prefix) const;

  /**
   * @brief Erase a Name Tree Entry if this entry is empty.
   * @details If a Name Tree Entry contains no Children, no FIB, no PIT, and
   * no Measurements entries, then it can be erased. In addition, its parent entry
   * will also be examined by following the parent pointer until all empty entries
   * are erased.
   * @param entry The pointer to the entry to be erased. The entry pointer should
   * returned by the findExactMatch(), lookup(), or findLongestPrefixMatch() functions.
   */
  bool
  eraseEntryIfEmpty(shared_ptr<name_tree::Entry> entry);

  /**
   * @brief Longest prefix matching for the given name
   * @details Starts from the full name string, reduce the number of name component
   * by one each time, until an Entry is found.
   */
  shared_ptr<name_tree::Entry>
  findLongestPrefixMatch(const Name& prefix,
                         const name_tree::EntrySelector& entrySelector = name_tree::AnyEntry());

  /**
   * @brief Resize the hash table size when its load factor reaches a threshold.
   * @details As we are currently using a hand-written hash table implementation
   * for the Name Tree, the hash table resize() function should be kept in the
   * name-tree.hpp file.
   * @param newNBuckets The number of buckets for the new hash table.
   */
  void
  resize(size_t newNBuckets);

  /**
   * @brief Enumerate all the name prefixes stored in the Name Tree.
   */
  const_iterator
  fullEnumerate(const name_tree::EntrySelector& entrySelector = name_tree::AnyEntry());

  /**
   * @brief Enumerate all the name prefixes that satisfies the EntrySubTreeSelector.
  */
  const_iterator
  partialEnumerate(const Name& prefix,
    const name_tree::EntrySubTreeSelector& entrySubTreeSelector = name_tree::AnyEntrySubTree());

  /**
   * @brief Enumerate all the name prefixes that satisfy the prefix and entrySelector
   */
  const_iterator
  findAllMatches(const Name& prefix,
                 const name_tree::EntrySelector& entrySelector = name_tree::AnyEntry());

  /**
   * @brief Dump all the information stored in the Name Tree for debugging.
   */
  void
  dump(std::ostream& output);

  const_iterator 
  begin();

  const_iterator
  end();

private:
  size_t m_nItems;  // Number of items being stored
  size_t m_nBuckets; // Number of hash buckets
  double m_loadFactor;
  size_t m_resizeThreshold;
  int m_resizeFactor;
  name_tree::Node** m_buckets; // Name Tree Buckets in the NPHT
  shared_ptr<name_tree::Entry> m_end; // for end()

  /**
   * @brief Create a Name Tree Entry if it does not exist, or return the existing
   * Name Tree Entry address.
   * @details Called by lookup() only.
   * @return The first item is the Name Tree Entry address, the second item is
   * a bool value indicates whether this is an old entry (false) or a new
   * entry (true).
   */
  std::pair<shared_ptr<name_tree::Entry>, bool>
  insert(const Name& prefix);

public:
  enum IteratorType 
  {
    FULL_ENUMERATE_TYPE, 
    PARTIAL_ENUMERATE_TYPE, 
    FIND_ALL_MATCHES_TYPE
  }; 

  class const_iterator : public std::iterator<std::forward_iterator_tag, name_tree::Entry>
  {
  public:
    friend class NameTree;

    const_iterator(NameTree::IteratorType type, 
      const NameTree& nameTree, 
      shared_ptr<name_tree::Entry> entry,
      const name_tree::EntrySelector& entrySelector = name_tree::AnyEntry(), 
      const name_tree::EntrySubTreeSelector& entrySubTreeSelector = name_tree::AnyEntrySubTree());

    ~const_iterator();

    const name_tree::Entry& 
    operator*();

    shared_ptr<name_tree::Entry> 
    operator->();

    const_iterator 
    operator++();

    const_iterator 
    operator++(int);

    bool 
    operator==(const const_iterator& other);

    bool 
    operator!=(const const_iterator& other);

  private:
    bool                                        m_visitChildren;
    const NameTree&                             m_nameTree;
    shared_ptr<name_tree::Entry>                m_entry;
    shared_ptr<name_tree::Entry>                m_subTreeRoot;
    shared_ptr<name_tree::EntrySelector>        m_entrySelector;
    shared_ptr<name_tree::EntrySubTreeSelector> m_entrySubTreeSelector;
    NameTree::IteratorType                      m_type; 
  };
};

inline NameTree::const_iterator::~const_iterator()
{
}

inline size_t
NameTree::size() const
{
  return m_nItems;
}

inline size_t
NameTree::getNBuckets() const
{
  return m_nBuckets;
}

inline const name_tree::Entry& 
NameTree::const_iterator::operator*()
{
  return *m_entry;
}

inline NameTree::const_iterator
NameTree::begin()
{
  return fullEnumerate();
}

inline NameTree::const_iterator
NameTree::end()
{
  const_iterator it(FULL_ENUMERATE_TYPE, *this, m_end);
  return it;
}

inline shared_ptr<name_tree::Entry> 
NameTree::const_iterator::operator->()
{
  return m_entry;
}

inline NameTree::const_iterator 
NameTree::const_iterator::operator++(int)
{
  NameTree::const_iterator temp(*this);
  ++(*this);
  return temp;
}

inline bool 
NameTree::const_iterator::operator==(const NameTree::const_iterator& other)
{
  return m_entry == other.m_entry;
}

inline bool
NameTree::const_iterator::operator!=(const NameTree::const_iterator& other)
{
  return m_entry != other.m_entry;
}

} // namespace nfd

#endif // NFD_TABLE_NAME_TREE_HPP
