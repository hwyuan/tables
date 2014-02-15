/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Prefix Hash Table

/*

TODO:
0. add namespace nfd
1. convert to use TLV Name instead of strings
2. unit testing
3. follow the new coding style
4. add LPM function by calling the lookup() function
5. may consier using some hash function from a library

*/

#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "name_tree.hpp"
#include "city.hpp"

using namespace std;


#define HT_OLD_ENTRY 0
#define HT_NEW_ENTRY 1

int debug = 0;

NameTreeNode::NameTreeNode()
{
	m_npeHead = NULL;
}

NameTreeNode::~NameTreeNode()
{
	// Currently handled by explicitly by the destory function
}


void
NameTreeNode::destory()
{
	NamePrefixEntry * tempEntry = m_npeHead;
	while(m_npeHead != NULL){
		m_npeHead = m_npeHead->m_next;
		delete tempEntry;
	}
}


NameTree::NameTree(int nBuckets)
{	
	if(debug) cout << "Name::Tree()" << endl;
	m_n = 0;
	m_nBuckets = nBuckets;
	m_buckets = new NameTreeNode[m_nBuckets];
}


NameTree::~NameTree()
{	
	int i = 0;
	for(i = 0; i < m_nBuckets; i++)
	{
		m_buckets[i].destory();
	}

	delete [] m_buckets;
}


int
NameTree::insert(string prefix, NamePrefixEntry ** ret_npe){

	uint32_t hashValue = CityHash32(prefix.c_str(), prefix.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug) cout << "string " << prefix << " hash value = " << hashValue << "  loc = " << loc << endl;

	// First see if this string is already stored
	NamePrefixEntry * temp = m_buckets[loc].m_npeHead;
	NamePrefixEntry * temp_pre = m_buckets[loc].m_npeHead;

	while(temp != NULL){
		if(prefix.compare(temp->m_prefix) == 0){ // The same prefix has been inserted to the NPHT before
			*ret_npe = temp;
			return HT_OLD_ENTRY;
		} else { // Not the same, keep searching
			temp_pre = temp;
			temp = temp->m_next;
		}
	}

	// if(debug) cout << "Did not find this entry, need to insert it to the table" << endl;
	// If not exist, insert this entry
	NamePrefixEntry * npe = new NamePrefixEntry(prefix);
	npe->setHash(hashValue);
	npe->m_next = NULL;
	npe->m_pre = temp_pre;
	
	if(temp_pre == NULL){
		m_buckets[loc].m_npeHead = npe;
	} else {
		temp_pre->m_next = npe;
	}

	*ret_npe = npe;

	return HT_NEW_ENTRY;
}


// Hash Table Resize
void
NameTree::resize(int newNBuckets){

	if(debug) cout << "NameTree::resize()" << endl;

	// dump();

	NameTreeNode * newBuckets = new NameTreeNode[newNBuckets];


	int count = 0;

	int i = 0;
	int loc = 0;
	for(i = 0; i < m_nBuckets; i++){

		//rehash every item
		if(m_buckets[i].m_npeHead != NULL){ // TODO add occupied bit to the NPHT node.
	
			NamePrefixEntry * temp = m_buckets[i].m_npeHead;
			while(temp){

				NamePrefixEntry * tempNext = temp->m_next;

				count++;

				loc = temp->m_hash % newNBuckets;

				NamePrefixEntry * newTemp = newBuckets[loc].m_npeHead;

				if(newTemp == NULL){
					newBuckets[loc].m_npeHead = temp;
					temp->m_pre = NULL;
					temp->m_next = NULL;
				} else {
					while(newTemp->m_next != NULL){
						newTemp = newTemp->m_next;
					}
					// now newTemp->next == NULL;
					newTemp->m_next = temp;
					temp->m_pre = newTemp;
					temp->m_next = NULL;
				}

				temp = tempNext;
			}
		}

	}

	if(count != m_n){
		cout << "Error" << endl;
		exit(1);
	}

	m_nBuckets = newNBuckets;
	NameTreeNode * oldBuckets = m_buckets;
	m_buckets = newBuckets;

	delete[] oldBuckets;
}


// Name Prefix Seek
// Build the NPHT with parent pointers
// Lookup each name prefix, if the name prefix does not exit, then create the node.
int 
NameTree::seek(string prefix){

	std::vector<std::string> strs;
	boost::split(strs, prefix, boost::is_any_of("/"));

	NamePrefixEntry * npe = NULL;
	NamePrefixEntry * parent = NULL;


	string endMark = "/";
	int end = boost::algorithm::ends_with(prefix, endMark);

	string temp = "";
	for(size_t i = 0; i < strs.size()-end; i++){
		temp += strs[i]; 
		temp += "/";

		int res = insert(temp, &npe); // insert() will create the entry if it does not exist.
		npe->m_parent = parent;

		if(res == HT_NEW_ENTRY){
			m_n++; // Increase the counter
			if(parent) parent->m_children++;
		}

		if(m_n > 0.5 * m_nBuckets){
			resize(3 * m_nBuckets);
		}

		parent = npe;
	}

    return 0;
}


// Return the address of the node that contains this prefix; return NULL if not found
NamePrefixEntry* 
NameTree::lookup(string prefix){
	uint32_t hashValue = CityHash32(prefix.c_str(), prefix.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug > 4) cout << "string " << prefix << " hash value = " << hashValue << "  loc = " << loc << endl;

	NamePrefixEntry * ret = m_buckets[loc].m_npeHead;
	while(ret != NULL){
		if(hashValue == ret->getHash() && prefix.compare(ret->m_prefix) == 0){	// found
			if(debug > 4)cout << "found " << prefix << endl;
			break;
		} else {
			ret = ret->m_next;
		}
	}

	return ret;
}


// Need to figure out the return values
int
NameTree::deletePrefix(string prefix){
// delete a NPE based on a prefix

	std::vector<std::string> strs;
	std::vector<std::string> strsReverse;
	boost::split(strs, prefix, boost::is_any_of("/"));

	string endMark = "/";
	int end = boost::algorithm::ends_with(prefix, endMark);

	string temp = "";
	strsReverse.clear();
	for(size_t i = 0; i < strs.size()-end; i++){
		temp += strs[i]; 
		temp += "/";
		strsReverse.push_back(temp);
	}

	int strsReverseSize = strsReverse.size();

	for(int i = strsReverseSize - 1; i >= 0; i--){

		temp = strsReverse[i];

		NamePrefixEntry * npe = lookup(temp);

		// if this entry can be deleted (only if it has no children and no fib and no pit)
		if(npe->m_children == 0 && npe->m_fib == NULL && npe->m_pitHead == NULL){
			// then this entry can be deleted and its parent reduces one child
			if(npe->m_parent) npe->m_parent->m_children--;	//Root node does not have parent.
			if(npe->m_pre != NULL){
				npe->m_pre->m_next = npe->m_next;
			} else {
				m_buckets[npe->m_hash % m_nBuckets].m_npeHead = npe->m_next;
			}
			delete npe;
			m_n--;
		}
	}

    return 0;
}

void 
NameTree::dump()
{
	int i = 0;
	for(i = 0; i < m_nBuckets; i++){


		NamePrefixEntry * temp = m_buckets[i].m_npeHead;
		while(temp != NULL){

			cout << "Bucket" << i << "\t" << temp->m_prefix << endl;

			cout << "\t\tHash " << temp->m_hash << endl;

			if(temp->m_parent != NULL){
				cout << "\t\tparent->" << temp->m_parent->m_prefix;
			} else {
				cout << "\t\tROOT";
			}
			cout << endl;

			if(temp->m_children != 0){
				cout << "\t\tchildren = " << temp->m_children << endl;
			}

			temp = temp->m_next;
		}
	}

	cout << "Bucket count = " << m_nBuckets << endl;
	cout << "Stored item = " << m_n << endl;
}

int main(){
	
	// if(debug) cout << "Testing Name Prefix Hash Table Implementation" << endl;

	int nameTreeSize = 1024;

	NameTree * nt = new NameTree(nameTreeSize);

	cout << "-----------------------------------------------------\n";
	cout << "Testing /, /a, /a/b, /a/b/c, /a/b/d, /a/b/d/e, /a/b/d/f, /a/b/d/g \n" << endl;

	nt->seek("/a/b/c");
	nt->seek("/a/b/d");
	nt->seek("/a/b/d/e/");
	nt->seek("/a/b/d/f/");
	nt->seek("/a/b/d/g/");

	nt->dump();

	cout << "-----------------------------------------------------\n";
	cout << "Testing, delete /a, /a/b/c" << endl;

	nt->deletePrefix("/a");
	nt->deletePrefix("/a/b/c");

	nt->dump();

	return 0;
}


