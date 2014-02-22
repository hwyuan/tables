/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Prefix Hash Table

#include <algorithm>
#include <iostream>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include "name-tree.hpp"

/*

- KNOWN ISSUES -
Smart pointers have been used in NFD (fib.cpp and pit.cpp, while currently in Name-Tree, we
store native pointers. We need to double check to make sure this would
not create any problems.

*/

namespace nfd{
namespace nt{

#define HT_OLD_ENTRY 0
#define HT_NEW_ENTRY 1

int debug = 0;

NameTree::NameTree(int nBuckets)
{ 
  if (debug > 4) 
    std::cout << "Name::Tree()" << std::endl;

  m_n = 0; // Number of items stored in the table
  m_nBuckets = nBuckets;
  m_buckets = new Node*[m_nBuckets]; // array of node pointers
  m_loadFactor = 0.5;
  m_resizeFactor = 2;

  // Initialize the pointer array
  for (int i = 0; i < m_nBuckets; i++)
    m_buckets[i] = 0;
}

NameTree::~NameTree()
{ 
  for (int i = 0; i < m_nBuckets; i++)
  {
    if (m_buckets[i] != 0) 
      delete m_buckets[i];
  }

  delete [] m_buckets;
}

// Interface for using different hash functions
uint32_t
NameTree::hash(const Name& prefix)
{
  // fixed value. Used for debugging.
  uint32_t ret = 0;

  // Boost hash
  // requires the /boost/functional/hash.hpp header file
  boost::hash<std::string> string_hash; 
  ret = string_hash(prefix.toUri());

  // City hash
  // std::string uri = prefix.toUri();
  // ret = CityHash32(uri.c_str(), uri.length());

  return ret;
}

// \return{HT_OLD_ENTRY and HT_NEW_ENTRY}
int
NameTree::insert(const Name& prefix, Entry ** retNpe)
{
  uint32_t hashValue = hash(prefix);
  uint32_t loc = hashValue % m_nBuckets;

  if (debug > 4)
  {
    std::string uri = prefix.toUri();
    std::cout << "uri " << uri << " hash value = " << 
      hashValue << "  loc = " << loc << std::endl;
  }

  // Check if this Name has been stored
  Node* temp = m_buckets[loc];
  Node* tempPre = temp;  // initialize tempPre to temp

  for (temp = m_buckets[loc]; temp != 0; temp = temp->m_next)
  {
    if (temp->m_npe != 0)
    {
      if (prefix.equals(temp->m_npe->m_prefix) == 1)
      {
        *retNpe = temp->m_npe;
        return HT_OLD_ENTRY;
      }
    }
    tempPre = temp;
  }

  if (debug > 4) 
    std::cout << "Did not find " << prefix.toUri() << 
                ", need to insert it to the table\n";

  // If no bucket is empty occupied, we need to create a new node, and it is 
  // linked from tempPre
  Node* node = new Node();
  node->m_pre = tempPre;

  if (tempPre == 0)
  {
    m_buckets[loc] = node;
  } else {
    tempPre->m_next = node;
  }

  // Create a new NPE
  Entry* npe = new Entry(prefix);
  npe->setHash(hashValue);
  node->m_npe = npe; // link the NPE to its Node
  npe->setNode(node);

  *retNpe = npe;

  return HT_NEW_ENTRY;
}


// Name Prefix Seek. Create NPE if not found
Entry* 
NameTree::seek(const Name& prefix)
{
  if (debug > 5) 
    std::cout << "NameTree::seek()\n";

  Entry* npe = 0;
  Entry* parent = 0;

  for (size_t i = 0; i <= prefix.size(); i++){
    Name temp = prefix.getPrefix(i);

    // insert() will create the entry if it does not exist.
    int res = insert(temp, &npe);

    if (res == HT_NEW_ENTRY){
      m_n++; /* Increase the counter */
      npe->m_parent = parent;

      if (parent != 0){
        parent->m_children++;
        parent->m_childrenList.push_back(npe);
      }
    }

    if (m_n > (int) (m_loadFactor * (double) m_nBuckets)){
      resize(m_resizeFactor * m_nBuckets);
    }

    parent = npe;
  }
    return npe;
}


// Exact Match
// Return the address of the node that contains this prefix; 
// Return 0 if not found
Entry* 
NameTree::lookup(const Name& prefix)
{
  uint32_t hashValue = hash(prefix);
  uint32_t loc = hashValue % m_nBuckets;

  std::string uri = prefix.toUri();
  if (debug > 4)
    std::cout << "uri " << uri << " hash value = " << hashValue <<
                                    "  loc = " << loc << std::endl;

  Entry* npe = 0;
  Node* ntn = 0;

  for (ntn = m_buckets[loc]; ntn != 0; ntn = ntn->m_next)
  {
    npe = ntn->m_npe;
    if (npe != 0){
      if (hashValue == npe->getHash() && prefix.equals(npe->m_prefix) == 1)
      {
        if (debug > 4) std::cout << "found " << uri << std::endl;
        return npe;
      }
    } // if npe
  } // for ntn
  return 0;
}

// Longest Prefix Match
// Return the longest matching NPE address
// start from the full name, and then remove 1 name comp each time
Entry*
NameTree::lpm(const Name& prefix)
{
  Entry* npe = 0;

  for(int i = prefix.size(); i >= 0; i--)
  {
    npe = lookup(prefix.getPrefix(i));
    if(npe != 0)
      return npe;
  }
  return 0;
}

// return{false: failure, true: success}
bool
NameTree::deleteNpeIfEmpty(Entry* npe)
{
  if (debug > 5) 
    std::cout << "name-tree.cpp deleteNpeIfEmpty()\n";

  assert(npe != 0);

  // first check if this NPE can be deleted 
  if (npe->m_children == 0 && npe->m_fib == 0 && npe->m_pitList.size() == 0){

    if (debug > 5) 
      std::cout << "Empty NPE\n";

    // update child-related info in the parent NPE 
    Entry* parent = npe->m_parent;

    if (parent != 0){
      parent->m_children--;

      int check = 0;
      size_t size = parent->m_childrenList.size();
      for (size_t i = 0; i < size; i++)
      {
        if (parent->m_childrenList[i] == npe)
        {
          parent->m_childrenList[i] = parent->m_childrenList[size-1];
          parent->m_childrenList.pop_back();
          check = 1;
          break;
        }
      }

      assert(check == 1);
    }

    // remove this NPE and its Name Tree Node 
    Node* node = npe->getNode();
    Node* nodePre = node->m_pre;

    // configure the pre node
    if (nodePre != 0)
    {
      nodePre->m_next = node->m_next;
    } else {
      m_buckets[npe->m_hash % m_nBuckets] = node->m_next;
    }

    // link the pre node with the next node (skip the deleted one)
    if (node->m_next) 
      node->m_next->m_pre = nodePre;

    delete npe;
    delete node;
    m_n--;

    if (parent != 0) 
      deleteNpeIfEmpty(parent);

    return true;

  } // if this npe is empty

  return false;
}

std::vector<nt::Entry *> *
NameTree::fullEnumerate()
{
  std::vector<nt::Entry *> * ret = new std::vector<nt::Entry *>;
  ret->clear();

  Node* node = 0;

  for (int i = 0; i < m_nBuckets; i++)
  {
    for (node = m_buckets[i]; node != 0; node = node->m_next)
    {
      if (node->m_npe)
      {
        // std::cout << "Bucket" << i << "\t" << node->m_npe->m_prefix.toUri() << std::endl;
        ret->push_back(node->m_npe);
      }
    }
  }

  return ret;
}

// Helper function for partialEnumerate()
void 
NameTree::partialEnumerateAddChildren(Entry* npe, std::vector<Entry *> * ret)
{
  assert(npe != 0);

  ret->push_back(npe);
  for (size_t i = 0; i < npe->m_childrenList.size(); i++)
  {
    Entry* temp = npe->m_childrenList[i];
    partialEnumerateAddChildren(temp, ret);
  }
}

std::vector<Entry *> *
NameTree::partialEnumerate(const Name& prefix)
{
  std::vector<Entry *> * ret = new std::vector<nt::Entry *>;
  ret->clear();

  // find the hash bucket corresponding to that prefix
  Entry * npe = lookup(prefix);

  if (npe == 0){
    return ret;
  } else {
    // go through its children list via depth-first-search
    ret->push_back(npe);
    for (size_t i = 0; i < npe->m_childrenList.size(); i++)
    {
      Entry* temp = npe->m_childrenList[i];
      partialEnumerateAddChildren(temp, ret);
    }
  }

  return ret;
}

// Hash Table Resize
void
NameTree::resize(int newNBuckets)
{
  if (debug > 5) 
    std::cout << "NameTree::resize()" << std::endl;

  Node** newBuckets = new Node*[newNBuckets];
  int count = 0;

  // referenced ccnx hashtb.c hashtb_rehash() 

  Node** pp = 0;
  Node* p = 0;
  Node* pre = 0;
  Node* q = 0; // record p->m_next
  int i;
  uint32_t h;
  uint32_t b;
  
  for (i = 0; i < newNBuckets; i++)
    newBuckets[i] = 0;

  for (i = 0; i < m_nBuckets; i++)
  {
    for (p = m_buckets[i]; p != 0; p = q) 
    {
      count++;
      q = p->m_next;
      assert(p->m_npe != 0); // XXX FIXME keep the assert statement during testing phase
      h = p->m_npe->m_hash;
      b = h % newNBuckets;
      for (pp = &newBuckets[b]; *pp != 0; pp = &((*pp)->m_next))
      {
        pre = *pp;
        continue;
      }
      p->m_pre = pre;
      p->m_next = *pp; // Actually *pp always == 0 in this case
      *pp = p;
    }
  }

  assert(count == m_n);

  m_nBuckets = newNBuckets;
  Node** oldBuckets = m_buckets;
  m_buckets = newBuckets;

  delete oldBuckets;
}

// For debugging
void 
NameTree::dump()
{
  if (debug != 0)
  {
    Node* node = 0;
    Entry* npe;

    if (debug > 4) 
      std::cout << "dump() --------------------------\n";

    for (int i = 0; i < m_nBuckets; i++){
      for (node = m_buckets[i]; node != 0; node = node->m_next){
        npe = node->m_npe;

        // if the NPE exist, dump its information 
        if (npe != 0){
          std::cout << "Bucket" << i << "\t" << npe->m_prefix.toUri() << std::endl;
          std::cout << "\t\tHash " << npe->m_hash << std::endl;

          if (npe->m_parent != 0){
            std::cout << "\t\tparent->" << npe->m_parent->m_prefix.toUri();
          } else {
            std::cout << "\t\tROOT";
          }
          std::cout << std::endl;

          if (npe->m_children != 0){
            std::cout << "\t\tchildren = " << npe->m_children << std::endl;
            
            for (size_t j = 0; j < npe->m_childrenList.size(); j++){
              std::cout << "\t\t\tChild " << j << " " << 
                npe->m_childrenList[j]->getPrefix().toUri() << std::endl;
            }
          }

        } // if npe != 0

      } // for node
    } // for int i

    std::cout << "Bucket count = " << m_nBuckets << std::endl;
    std::cout << "Stored item = " << m_n << std::endl;
    std::cout << "--------------------------\n";
  }
}

} // namespace nt
} // namespace nfd
