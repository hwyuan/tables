/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */


/*

TODO:
1. add namespace nfd
2. currenlty, all the class members are in public, some of them should be moved to private 
3. Remove the m_pre pointer in the NameTreeNode class

*/


#ifndef NFD_TABLE_NAME_TREE_HPP
#define NFD_TABLE_NAME_TREE_HPP

#include <iostream>
#include <ndn-cpp-dev/common.hpp>

#include "name-tree-entry.hpp"

typedef ndn::Name Name;

namespace nfd{

/** \class NameTree
 *  \brief represents the Name Prefix Hash Table
 */


class NameTree
{
public:
	NameTree(int nBuckets);
	~NameTree();

	// Initialize the name tree data structure
	int 
	initNameTree(int size);

	// NameTree Delete
	int
	deletePrefix(const Name prefix);

	// NameTree Delete Name Prefix Entry
	int
	deleteNPEIfEmpty(NamePrefixEntry* npe);

	// NameTree Lookup
	NamePrefixEntry * 
	lookup(const Name prefix);

	// NameTree Longest Prefix Lookup
	NamePrefixEntry *
	lpm(ndn::Name prefix);

	// Hash Table Resize
	void
	resize(int newNBuckets);

	// NameTree Seek
	int
	seek(const Name prefix);

	// Get current load
	int
	getN();

	// Get Bucket count
	int
	getNBuckets();

	// Enumerate all the non-empty NPHT entries
	void
	fullEnumerate();

	// Enumerate all the children of a specified prefix
	void
	partialEnumerate(const Name prefix);

	// Dump the Name Tree for debugging
	void 
	dump();

private:
	int m_n;	// Number of items being stored
	int m_nBuckets; // Number of hash buckets
	NameTreeNode ** m_buckets; // Name Tree Buckets in the NPHT
	double m_loadFactor;
	int m_resizeFactor;

	// NameTree Insert
	int
	insert(ndn::Name prefix, NamePrefixEntry ** ret_npe);

	// NameTree Lookup
	NamePrefixEntry * 
	lookup(const Name prefix, NameTreeNode ** retNode, NameTreeNode ** retNodePre);

 };

} // namespace nfd

#endif // NFD_TABLE_NAME_TREE_HPP




