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

// namespace nfd{

#ifndef NFD_TABLE_NAME_TREE_HPP
#define NFD_TABLE_NAME_TREE_HPP

#include <iostream>
#include "name_tree_entry.hpp"

using namespace std;

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

	// NameTree Insert
	int
	insert(std::string prefix, NamePrefixEntry ** ret_npe);

	// NameTree Delete
	int
	deletePrefix(std::string prefix);

	// NameTree Lookup
	NamePrefixEntry * 
	lookup(std::string prefix);


	// Hash Table Resize
	void
	resize(int newNBuckets);


	// NameTree Seek
	int
	seek(std::string prefix);

	// NameTree Seek, insert all the proper prefixes and build the parent pointers.
	int
	nameTreeSeek(std::string prefix);

	// Resize the hash table
	int
	nameTreeResize();

	// Get current load
	int
	getN();

	// Get Bucket count
	int
	getNBuckets();

	// Get Bucket content (XXX may not be kept all the time)
	int
	getBucketContent(int index);

	// Dump the Name Tree for debugging
	void 
	dump();

private:
	int m_n;	// Number of items being stored
	int m_nBuckets; // Number of hash buckets
	NameTreeNode * m_buckets; // Name Tree Buckets in the NPHT
 };

// } // namespace nfd

#endif // NFD_TABLE_NAME_TREE_HPP


