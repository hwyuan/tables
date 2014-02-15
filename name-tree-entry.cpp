/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

 // Name Prefix Entry

/*

TODO:
1. namespace nfd
2. name components
3. addFIB, addPIT

*/


#include "name-tree-entry.hpp"
#include <algorithm>
#include <iostream>

namespace nfd{

FIBEntry::FIBEntry()
{
}

FIBEntry::~FIBEntry()
{
}

PITEntry::PITEntry()
{
}

PITEntry::~PITEntry()
{
}

NamePrefixEntry::NamePrefixEntry(const std::string prefix)
{
	m_hash = 0;
	m_children = 0;
	m_prefix = prefix;
	m_parent = NULL;
	m_childrenList.clear();
	m_pre = NULL;
	m_next = NULL;
	m_fib = NULL;
	m_pitHead.clear();
}

NamePrefixEntry::~NamePrefixEntry()
{
}

// Need to figure out the return value
int
NamePrefixEntry::setFIBEntry(FIBEntry * fib){
	m_fib = fib;
	return 0;
}

int
NamePrefixEntry::addPITEntry(PITEntry * pit){
	m_pitHead.push_back(pit);

	return 0;
}

int
NamePrefixEntry::deletePITEntry(PITEntry * pit){
	// delete this PIT

	// XXXX FIXME: check if this NPE can be deleted
	// basically, if there is nothing left in the NPE, this NPE needs to be deleted
	// And deleting this NPE may results in its parents also being deleted.
	// - Can be handled by calling the NT's deletePrefix() method
	// - But need to decide if we are going to call this method here or in PIT
	// - At the moment, it seems that calling the method in PIT is more reasonable

	return 0;
}

int
NamePrefixEntry::getPITCount(){
	return (int)m_pitHead.size(); // convert size_t to int
}


void
NamePrefixEntry::setHash(uint32_t hash)
{
	m_hash = hash;
}

uint32_t
NamePrefixEntry::getHash()
{
	return m_hash;
}


void
NamePrefixEntry::setNext(NamePrefixEntry * next)
{
	m_next = next;
}

void
NamePrefixEntry::setParent(NamePrefixEntry * parent)
{
	m_parent = parent;
}

NamePrefixEntry *
NamePrefixEntry::getNext()
{
	return m_next;
}

NamePrefixEntry *
NamePrefixEntry::getParent()
{
	return m_parent;
}

} // namespace nfd

