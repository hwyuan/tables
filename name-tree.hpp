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

	NamePrefixEntry * m_npe; // Name Prefix Entry Head
	NameTreeNode * m_next; // Next Name Tree Node (to resovle hash collision)
	
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
<<<<<<< HEAD

=======
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845

	// NameTree Lookup
	NamePrefixEntry * 
	lookup(ndn::Name prefix);
<<<<<<< HEAD


	// NameTree Lookup
	NamePrefixEntry * 
	lookup(ndn::Name prefix, NameTreeNode ** retNode, NameTreeNode ** retNodePre);
=======
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845

	// NameTree Longest Prefix Lookup
	NamePrefixEntry *
	lpm(ndn::Name prefix);
<<<<<<< HEAD
=======

>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845

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
<<<<<<< HEAD
	NameTreeNode ** m_buckets; // Name Tree Buckets in the NPHT

	// NameTree Insert
	int
	insert(ndn::Name prefix, NamePrefixEntry ** ret_npe);
=======
	NameTreeNode * m_buckets; // Name Tree Buckets in the NPHT


>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
 };

} // namespace nfd


#endif // NFD_TABLE_NAME_TREE_HPP


