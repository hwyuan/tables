/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Prefix Hash Table

#ifndef NFD_TABLE_NAME_TREE_HPP
#define NFD_TABLE_NAME_TREE_HPP

#include <iostream>

#include "common.hpp"
#include "name-tree-entry.hpp"
#include <boost/functional/hash.hpp>

namespace nfd{
namespace nt{

/** \class NameTree
 *  \brief represents the Name Prefix Hash Table
 */
class NameTree
{
public:
  NameTree(int nBuckets);

  ~NameTree();

  // Get current load
  const int
  getN();

  // Get Bucket count
  const int
  getNBuckets();

  uint32_t 
  hash(const Name& prefix);

  // NameTree Seek
  Entry *
  seek(const Name& prefix);

  // NameTree Lookup
  Entry * 
  lookup(const Name& prefix);

  // NameTree Delete Name Prefix Entry
  bool
  deleteNpeIfEmpty(Entry* npe);

  // NameTree Longest Prefix Lookup
  Entry *
  lpm(const ndn::Name& prefix);

  // Hash Table Resize
  void
  resize(int newNBuckets);

  // Enumerate all the non-empty NPHT entries
  std::vector<nt::Entry *> *
  fullEnumerate();

  // Enumerate all the children of a specified prefix
 

  std::vector<Entry *> *
  partialEnumerate(const Name& prefix);

  // Dump the Name Tree for debugging
  void 
  dump();

private:
  int m_n;  // Number of items being stored
  int m_nBuckets; // Number of hash buckets
  double m_loadFactor;
  int m_resizeFactor;
  Node** m_buckets; // Name Tree Buckets in the NPHT

  // NameTree Insert, can called only by seek() function
  int
  insert(const ndn::Name& prefix, Entry** retNpe);

  void 
  partialEnumerateAddChildren(Entry* npe, std::vector<Entry *> * ret);

 };

inline const int
NameTree::getN()
{
  return m_n;
}

inline const int
NameTree::getNBuckets()
{
  return m_nBuckets;
}

} // namespace nt
} // namespace nfd

#endif // NFD_TABLE_NAME_TREE_HPP
