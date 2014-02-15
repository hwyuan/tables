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

namespace nfd{

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


class NamePrefixEntry	// NamePrefixEntry
{
public:
	NamePrefixEntry(const std::string prefix);
	~NamePrefixEntry();

	const std::string
	getPrefix();

	int 
	addFIBEntry();

	int 
	addPITEntry();

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

	std::string m_prefix;
	uint32_t m_hash;
	uint32_t m_children;				// It is safe to delete an entry only if its children == 0
	NamePrefixEntry * m_parent;			// Pointing to the parent entry.
	std::vector<NamePrefixEntry *> m_childrenList; // Children pointers.
	NamePrefixEntry * m_pre;
	NamePrefixEntry * m_next;			// Use chaining to resolve hash collisions.
	FIBEntry * m_fib;
	std::vector<PITEntry *> m_pitHead;

private:

};

} // namespace nfd

#endif // NFD_TABLE_NAME_TREE_ENTRY_HPP





