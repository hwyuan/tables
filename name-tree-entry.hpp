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

#ifndef NFD_TABLE_NAME_TREE_ENTRY_HPP
#define NFD_TABLE_NAME_TREE_ENTRY_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdint.h>

#include <ndn-cpp-dev/common.hpp>
#include <ndn-cpp-dev/name.hpp>

namespace nfd{

class FIBEntry;
class PITEntry;
class NameTreeNode;
class NamePrefixEntry;

class FIBEntry
{
public:
	FIBEntry();
	~FIBEntry();
	std::string m_name;
private:
};

class PITEntry
{
public:
	PITEntry();
	~PITEntry();
	std::string m_name;
private:
};

/* similar to CCNx's hashtb node */
class NameTreeNode
{
public:
	NameTreeNode();
	~NameTreeNode();

	void 
	destory();

	NamePrefixEntry * m_npe; // Name Prefix Entry Head
	NameTreeNode * m_pre; // Next Name Tree Node (to resovle hash collision)
	NameTreeNode * m_next; // Next Name Tree Node (to resovle hash collision)
	
private:

};


class NamePrefixEntry	// NamePrefixEntry
{
public:
	NamePrefixEntry(const ndn::Name prefix);
	~NamePrefixEntry();

	const std::string
	getPrefix();

	int 
	setFIBEntry(FIBEntry * fib);

	int 
	deleteFIBEntry(FIBEntry * fib);

	int 
	addPITEntry(PITEntry * pit);

	int 
	deletePITEntry(PITEntry * pit);

	int
	getPITCount();

	void
	setHash(uint32_t hash);

	uint32_t
	getHash();

	void
	setNext(NamePrefixEntry * next);

	void
	setParent(NamePrefixEntry * parent);

	NamePrefixEntry *
	getNext();

	NamePrefixEntry *
	getParent();

	void
	setNode(NameTreeNode* node);

	NameTreeNode*
	getNode();

	uint32_t m_hash;
	ndn::Name m_prefix;
	uint32_t m_children;				// It is safe to delete an entry only if its children == 0
	NamePrefixEntry * m_parent;			// Pointing to the parent entry.
	std::vector<NamePrefixEntry *> m_childrenList; // Children pointers.
	FIBEntry * m_fib;
	std::vector<PITEntry *> m_pitList;

private:
	NameTreeNode * m_node;
};

} // namespace nfd

#endif // NFD_TABLE_NAME_TREE_ENTRY_HPP





