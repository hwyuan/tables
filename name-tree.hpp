/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */


/*

TODO:
1. add namespace nfd
2. currenlty, all the class members are in public, some of them should be moved to private 

*/


#ifndef NFD_TABLE_NAME_TREE_HPP
#define NFD_TABLE_NAME_TREE_HPP

#include <iostream>
#include <ndn-cpp-dev/common.hpp>

#include "name-tree-entry.hpp"

namespace nfd{

/** \class NameTree
 *  \brief represents the Name Prefix Hash Table
 */

class NameTreeNode
{
public:
	NameTreeNode();
	~NameTreeNode();

	void 
	destory();

	NamePrefixEntry * m_npeHead; // Name Prefix Entry Head

private:

};


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
	deletePrefix(ndn::Name prefix);

	// NameTree Lookup
	NamePrefixEntry * 
	lookup(ndn::Name prefix);

	// NameTree Longest Prefix Lookup
	NamePrefixEntry *
	lpm(ndn::Name prefix);


	// Hash Table Resize
	void
	resize(int newNBuckets);


	// NameTree Seek
	int
	seek(ndn::Name prefix);

	// NameTree Seek, insert all the proper prefixes and build the parent pointers.
	int
	nameTreeSeek(ndn::Name prefix);

	// Resize the hash table
	int
	nameTreeResize();

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
	partialEnumerate(ndn::Name prefix);


	// Dump the Name Tree for debugging
	void 
	dump();

	// NameTree Insert
	int
	insert(ndn::Name prefix, NamePrefixEntry ** ret_npe);

private:
	int m_n;	// Number of items being stored
	int m_nBuckets; // Number of hash buckets
	NameTreeNode * m_buckets; // Name Tree Buckets in the NPHT


 };

} // namespace nfd


#endif // NFD_TABLE_NAME_TREE_HPP


