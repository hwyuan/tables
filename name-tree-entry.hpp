/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Tree Entry (i.e., Name Prefix Entry)

#ifndef NFD_TABLE_NAME_TREE_ENTRY_HPP
#define NFD_TABLE_NAME_TREE_ENTRY_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>
#include "common.hpp"
#include "table/fib-entry.hpp"
#include "table/pit-entry.hpp"
#include "table/measurements-entry.hpp"


namespace nfd{
namespace nt{

class Node;
class Entry;

typedef fib::Entry FibEntry;
typedef pit::Entry PitEntry;
typedef measurements::Entry MeasurementsEntry;
// Name Tree node (similar to CCNx's hashtb node)
class Node
{
public:
  Node();

  ~Node();
  
  // variables are in public as this is just a data structure
  Entry* m_npe; // Name Prefix Entry Head
  Node* m_pre; // Next Name Tree Node (to resovle hash collision)
  Node* m_next; // Next Name Tree Node (to resovle hash collision)
private:
};

// Name Prefix Entry
class Entry
{
public:

  Entry(const Name& prefix);

  ~Entry();

  const Name&
  getPrefix();

  void
  setHash(uint32_t hash);

  const uint32_t
  getHash();

  void
  setParent(Entry * parent);

  Entry *
  getParent();

  const int 
  getChildren();

  const int
  getChildrenListSize();

  std::vector<nt::Entry *>&
  getChildrenList();

  bool 
  setFibEntry(FibEntry * fib);

  FibEntry* 
  getFibEntry();

  bool
  deleteFibEntry(FibEntry * fib);

  bool 
  insertPitEntry(PitEntry * pit);

  std::vector<PitEntry *>&
  getPITList();
  
  bool 
  deletePitEntry(PitEntry * pit);

  bool
  setMeasurementsEntry(MeasurementsEntry* measurements);

  MeasurementsEntry*
  getMeasurementsEntry();

  void
  setNode(Node* node);

  Node*
  getNode();

  uint32_t m_hash;
  Name m_prefix;
  Entry * m_parent;     // Pointing to the parent entry.
  uint32_t m_children;        // It is safe to delete an entry only if its children == 0
  std::vector<Entry *> m_childrenList; // Children pointers.
  FibEntry* m_fib;
  std::vector<PitEntry *> m_pitList;
  MeasurementsEntry* m_measurements;

private:
  Node* m_node;
};

inline const Name&
Entry::getPrefix()
{
  return m_prefix;
}

inline const uint32_t
Entry::getHash()
{
  return m_hash;
}

inline Entry*
Entry::getParent()
{
  return m_parent;
}

inline const int
Entry::getChildren()
{
  return m_children;
}

inline const int
Entry::getChildrenListSize()
{
  return (int)m_pitList.size(); // convert size_t to int
}

inline std::vector<nt::Entry *>&
Entry::getChildrenList()
{
  return m_childrenList;
}

inline FibEntry*
Entry::getFibEntry()
{
  return m_fib;
}

inline std::vector<PitEntry *>&
Entry::getPITList()
{
  return m_pitList;
}

inline MeasurementsEntry*
Entry::getMeasurementsEntry()
{
  return m_measurements;
}

inline Node*
Entry::getNode()
{
  return m_node;
}

} // namespace nt
} // namespace nfd

#endif // NFD_TABLE_NAME_TREE_ENTRY_HPP
