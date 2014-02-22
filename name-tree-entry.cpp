/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Tree Entry (i.e., Name Prefix Entry)

#include <algorithm>
#include <iostream>
#include "name-tree-entry.hpp"

namespace nfd{
namespace nt{

Node::Node()
{
	m_npe = 0;
	m_pre = 0;
	m_next = 0;
}

Node::~Node()
{
}

Entry::Entry(const Name& name)
{
	m_hash = 0; // XXX Double check to make sure let default = 0 is fine
	m_prefix = name;
	m_parent = 0;
	m_children = 0;
	m_childrenList.clear();
	m_fib = 0;
	m_pitList.clear();
}

Entry::~Entry()
{
}

void
Entry::setHash(uint32_t hash)
{
	m_hash = hash;
}

void
Entry::setParent(Entry* parent)
{
	m_parent = parent;
}

// Need to figure out the return value
bool
Entry::setFibEntry(FibEntry* fib)
{
	m_fib = fib;
	return true;
}

bool 
Entry::deleteFibEntry(FibEntry* fib)
{
	if(m_fib != fib)
		return false;
	m_fib = 0;
	return true;
}

bool
Entry::insertPitEntry(PitEntry* pit)
{
	m_pitList.push_back(pit);
	return true;
}

bool
Entry::deletePitEntry(PitEntry* pit)
{
	for (size_t i = 0; i < m_pitList.size(); i++){
		if (m_pitList[i] == pit){
			m_pitList[i] = m_pitList[m_pitList.size() - 1]; // assign last item to pos
			m_pitList.pop_back();
			return true; // success
		}
	}
	return false; // failure
}

// Need to figure out the return value
bool
Entry::setMeasurementsEntry(MeasurementsEntry* measurements)
{
	m_measurements = measurements;
	return true;
}

void
Entry::setNode(Node* node)
{
	m_node = node;
}

} // namespace nt
} // namespace nfd
