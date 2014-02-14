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

- Haowei
*/


#include "name_tree_entry.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

NamePrefixEntry::NamePrefixEntry(const string prefix){

	// cout << "inserting " << prefix << endl;
	m_hash = 0;
	m_children = 0;
	m_prefix = prefix;
	m_parent = NULL;
	m_pre = NULL;
	m_next = NULL;
	m_fib = NULL;
	m_pitHead = NULL;
	// cout << "done" << endl;
}

NamePrefixEntry::~NamePrefixEntry(){
}

void
NamePrefixEntry::setHash(uint32_t hash){
	m_hash = hash;
}

uint32_t
NamePrefixEntry::getHash(){
	return m_hash;
}


void
NamePrefixEntry::setNext(NamePrefixEntry * next){
	m_next = next;
}

void
NamePrefixEntry::setParent(NamePrefixEntry * parent){
	m_parent = parent;
}

NamePrefixEntry *
NamePrefixEntry::getNext(){
	return m_next;
}

NamePrefixEntry *
NamePrefixEntry::getParent(){
	return m_parent;
}

