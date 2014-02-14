/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

#ifndef NFD_TABLE_NAMETREE_ENTRY_HPP
#define NFD_TABLE_NAMETREE_ENTRY_HPP

#include <iostream>
#include <string>

using namespace std;

class FIBEntry
{
public:
	FIBEntry();
	~FIBEntry();
	string m_name;
private:
};

class PITEntry
{
public:
	PITEntry();
	~PITEntry();
	string m_name;
private:
};

class NamePrefixEntry	// NameTreeEntry
{
public:
	NamePrefixEntry(const string prefix);
	~NamePrefixEntry();

	const string
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

	string m_prefix;
	uint32_t m_hash;
	uint32_t m_children;				// It is safe to delete an entry only if its children == 0
	NamePrefixEntry * m_parent;			// Pointing to the parent entry.
	NamePrefixEntry * m_pre;
	NamePrefixEntry * m_next;			// Use chaining to resolve hash collisions.
	FIBEntry * m_fib;
	PITEntry * m_pitHead;

private:

};

 #endif // NFD_TABLE_NAMETREE_ENTRY_HPP





