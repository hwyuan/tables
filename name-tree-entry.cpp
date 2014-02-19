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

#include <algorithm>
#include <iostream>
#include "name-tree-entry.hpp"


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


NameTreeNode::NameTreeNode()
{
	m_npe = NULL;
	m_pre = NULL;
	m_next = NULL;
}


NameTreeNode::~NameTreeNode()
{
	// Currently handled by explicitly by the destory function
}


void
NameTreeNode::destory()
{
	NamePrefixEntry * tempEntry = m_npe;
	if(tempEntry != NULL) delete tempEntry;

	if(m_next) m_next->destory();
}


NamePrefixEntry::NamePrefixEntry(const ndn::Name name)
{
	m_hash = 0; // XXX Double check to make sure let default = 0 is fine
	m_children = 0;
	m_prefix = name;
	m_parent = NULL;
	m_childrenList.clear();
	m_fib = NULL;
	m_pitList.clear();
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
NamePrefixEntry::deleteFIBEntry(FIBEntry * fib){
	m_fib = NULL;

	return 0;
}

int
NamePrefixEntry::addPITEntry(PITEntry * pit){
	m_pitList.push_back(pit);
	return 0;
}

int
NamePrefixEntry::deletePITEntry(PITEntry * pit){
	// delete this PIT

	// XXXX FIXME: check if this NPE can be deleted
	// basically, if there is nothing left in the NPE, this NPE needs to be deleted
	// And deleting this NPE may results in its parents also being deleted.
	for(size_t i = 0; i < m_pitList.size(); i++){
		if(m_pitList[i] == pit){
			m_pitList[i] = m_pitList[m_pitList.size() - 1]; // assign last item to pos
			m_pitList.pop_back();
			return 1; // success
		}
	}
	return 0; // failure
}


int
NamePrefixEntry::getPITCount(){
	return (int)m_pitList.size(); // convert size_t to int
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
NamePrefixEntry::setParent(NamePrefixEntry * parent)
{
	m_parent = parent;
}

NamePrefixEntry *
NamePrefixEntry::getParent()
{
	return m_parent;
}

void
NamePrefixEntry::setNode(NameTreeNode * node)
{
	m_node = node;
}

NameTreeNode *
NamePrefixEntry::getNode()
{
	return m_node;
}



} // namespace nfd


