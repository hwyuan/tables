/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Prefix Hash Table

/*
TODO List
DONE 			0. add namespace nfd 
DONE 			1. convert to use TLV Name instead of strings
In progress 	2. unit test
TODO			3. may consier using some hash function from a library
DONE 			4. add LPM function by calling the lookup() function
DONE 			5. add full / partial enumeration function
*/

#include <algorithm>
#include <iostream>
#include <sstream>

#include <boost/algorithm/string.hpp>

#include "name-tree.hpp"
#include "city.hpp"

namespace nfd{

#define HT_OLD_ENTRY 0
#define HT_NEW_ENTRY 1

int debug = 0;

typedef ndn::Name Name;

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
	if(debug) std::cout << "Name::Tree()" << std::endl;
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


// XXX FIXME: insert functino should be private, as it does not handle parent pointers
int
NameTree::insert(ndn::Name prefix, NamePrefixEntry ** ret_npe)
{
	std::string uri = prefix.toUri();
	uint32_t hashValue = CityHash32(uri.c_str(), uri.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug > 4) std::cout << "uri " << uri << " hash value = " << hashValue << "  loc = " << loc << std::endl;

	// First see if this string is already stored
	NamePrefixEntry * temp = m_buckets[loc].m_npeHead;
	NamePrefixEntry * temp_pre = m_buckets[loc].m_npeHead;

	while(temp != NULL)
	{
		if(prefix.equals(temp->m_prefix) == 1) // The same name prefix has been inserted
		{
			*ret_npe = temp;
			return HT_OLD_ENTRY;
		} else { // Not the same, keep searching XXX FIXME: need to change to isolate NT node and NT entry
			temp_pre = temp;
			temp = temp->m_next;
		}
	}

	if(debug > 4) std::cout << "Did not find this entry, need to insert it to the table\n";
	// If name prefix does not exist, insert this entry.

	NamePrefixEntry * npe = new  NamePrefixEntry(prefix);
	npe->setHash(hashValue);
	npe->m_next = NULL;
	npe->m_pre = temp_pre;

	if(temp_pre == NULL)
	{
		m_buckets[loc].m_npeHead = npe;
	} else {
		temp_pre->m_next = npe;
	}

	*ret_npe = npe;

	return HT_NEW_ENTRY;
}


// Hash Table Resize
void
NameTree::resize(int newNBuckets)
{
	if(debug) std::cout << "NameTree::resize()" << std::endl;

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
		std::cout << "Error" << std::endl;
		exit(1);
	}

	m_nBuckets = newNBuckets;
	NameTreeNode * oldBuckets = m_buckets;
	m_buckets = newBuckets;

	delete[] oldBuckets;
}


// Name Prefix Seek. Create NPE if not found.
int 
NameTree::seek(ndn::Name prefix)
{
	NamePrefixEntry * npe = NULL;
	NamePrefixEntry * parent = NULL;

	for(size_t i = 0; i <= prefix.size(); i++){

		Name temp = prefix.getPrefix(i);

		int res = insert(temp, &npe); // insert() will create the entry if it does not exist.
		npe->m_parent = parent;

		if(res == HT_NEW_ENTRY){
			m_n++; // Increase the counter
			if(parent){
				parent->m_children++;
				parent->m_childrenList.push_back(npe);
			}
		}

		// Threshold for resizing the hash table (50% load, increase by twice)
		// XXX FIXME: Resizing threshold should probably be configurable 
		if(m_n > 0.5 * m_nBuckets){
			resize(2 * m_nBuckets);
		}

		parent = npe;
	}
    return 0;
}


// Return the address of the node that contains this prefix; 
// Return NULL if not found
NamePrefixEntry* 
NameTree::lookup(ndn::Name prefix)
{
	std::string uri = prefix.toUri();
	uint32_t hashValue = CityHash32(uri.c_str(), uri.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug > 4) std::cout << "uri " << uri << " hash value = " << hashValue << "  loc = " << loc << std::endl;

	NamePrefixEntry * ret = m_buckets[loc].m_npeHead;
	while(ret != NULL){
		if(hashValue == ret->getHash() && prefix.equals(ret->m_prefix) == 1){	// found
			if(debug > 4) std::cout << "found " << uri << std::endl;
			break;
		} else {
			ret = ret->m_next;
		}
	}

	return ret;
}

// Return the longest matching NPE address
// start from the full name, and then remove 1 name comp each time
NamePrefixEntry *
NameTree::lpm(ndn::Name prefix){

	NamePrefixEntry * ret = NULL;

	for(int i = prefix.size(); i >= 0; i--){

		NamePrefixEntry * npe = lookup(prefix.getPrefix(i));
		if(npe != NULL){
			ret = npe;
			break;
		}
	}
	return ret;
}


// delete a NPE based on a prefix
// XXX return value = ?
int
NameTree::deletePrefix(ndn::Name prefix)
{
	for(int i = prefix.size(); i >= 0; i--)
	{
		Name temp = prefix.getPrefix(i);
		NamePrefixEntry * npe = lookup(temp);

		if(npe == NULL) return 0;

		// if this entry can be deleted (only if it has no child and no fib and no pit)
		if(npe->m_children == 0 && npe->m_fib == NULL && npe->m_pitHead.size() == 0){
			// then this entry can be deleted and its parent reduces one child
			if(npe->m_parent)
			{
				NamePrefixEntry * parent = npe->m_parent;
				parent->m_children--;	//Root node does not have parent.
				// delete npe from the parent's children list
				int check = 0;
				size_t childrenListSize = parent->m_childrenList.size();
				for(size_t i = 0; i < childrenListSize; i++)
				{
					if(parent->m_childrenList[i] == npe) // compareing memory address
					{
						parent->m_childrenList[i] = parent->m_childrenList[childrenListSize-1];
						parent->m_childrenList.pop_back();
						check = 1;
						break;
					}
				}

				assert(check == 1);
			}
			
			if(npe->m_pre != NULL)
			{
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
NameTree::fullEnumerate()
{
	for(int i = 0; i < m_nBuckets; i++){
		NamePrefixEntry * temp = m_buckets[i].m_npeHead;
		while(temp != NULL){
			std::cout << "Bucket" << i << "\t" << temp->m_prefix.toUri() << std::endl;
			temp = temp->m_next;
		}
	}
}


void
NameTree::partialEnumerate(ndn::Name prefix)
{
	// find the hash bucket corresponding to that prefix
	// then enumerate all of its children...
	NamePrefixEntry * npe = lookup(prefix);

	if(npe == NULL){
		// should probably return a vlaue, currently just print a statement
		std::cout << "Error::partialEnumerate, prefix does not exist\n";
		return;
	} else {
		// go through its children list 
		for(size_t i = 0; i < npe->m_childrenList.size(); i++){
			std::cout << npe->m_childrenList[i]->m_prefix.toUri() << std::endl;
		}
	}
}


// For debugging
void 
NameTree::dump()
{
	for(int i = 0; i < m_nBuckets; i++){


		NamePrefixEntry * temp = m_buckets[i].m_npeHead;
		while(temp != NULL){

			std::cout << "Bucket" << i << "\t" << temp->m_prefix.toUri() << std::endl;

			std::cout << "\t\tHash " << temp->m_hash << std::endl;

			if(temp->m_parent != NULL){
				std::cout << "\t\tparent->" << temp->m_parent->m_prefix.toUri();
			} else {
				std::cout << "\t\tROOT";
			}
			std::cout << std::endl;

			if(temp->m_children != 0){
				std::cout << "\t\tchildren = " << temp->m_children << std::endl;
			}

			temp = temp->m_next;
		}
	}

	std::cout << "Bucket count = " << m_nBuckets << std::endl;
	std::cout << "Stored item = " << m_n << std::endl;
}

} // namespace nfd


// XXX TODO: Convert the main() function to the unit tests
int main()
{

	using namespace std;
	using namespace nfd;
	
	// if(debug) cout << "Testing Name Prefix Hash Table Implementation" << endl;

	int nameTreeSize = 1024;

	NameTree * nt = new NameTree(nameTreeSize);

	Name aName("/a/b/c");
	Name aName1("/a");
	Name aName2("/a/b");
	nt->seek(aName);
	
	NamePrefixEntry * npe = NULL;

	npe = nt->lookup(aName1);
	if(npe) cout << "/a/ exist = " << npe->m_prefix.toUri() << endl;

	npe = nt->lookup(aName2);
	if(npe) cout << "/a/b/ exist = " << npe->m_prefix.toUri() << endl;

	npe = nt->lookup(aName);
	if(npe) cout << "/a/b/c/ exist = " << npe->m_prefix.toUri() << endl;

	Name bName("/a/b/c/d/e/f/g/");

	npe = nt->lookup(bName);
	if(npe){
		cout << "Error...lookup /a/b/c/d/ \n";
		exit(1);
	} else {
		cout << "succeeded, did not find /a/b/c/d/e/f/g/" << endl;
	}

	npe = nt->lpm(bName);
	if(npe) cout << "/a/b/c/d/e/f/g/'s LPM prefix is " << npe->m_prefix.toUri() << endl;

	Name abcd("/a/b/c/d");
	nt->seek(abcd);
	npe = nt->lpm(bName);
	cout << "after inserint /a/b/c/d/" << endl;;
	if(npe) cout << "/a/b/c/d/e/f/g/'s LPM prefix is " << npe->m_prefix.toUri() << endl;

	cout << "Now insert /a/b/c/d/e/f/g/ via insert(), not seek()" << endl;
	nt->insert(bName, &npe);

	cout << "Perform lookup on /a/b/c/d/e/f/g/" << endl;
	npe = nt->lookup(bName);
	if(npe && npe->m_prefix.equals(bName) == 1)cout << "insert() passed part 1/2" << endl;

	Name bName1("/a/b/c/d/e/f/");
	cout << "Perform lookup on /a/b/c/d/e/f/, should return NULL" << endl;
	npe = nt->lookup(bName1);
	if(npe){
		cout << "Error lookup() at line " << __LINE__ << endl;
		exit(1);
	} else {
		cout << "insert() passed part 2/2" << endl;
	}

	cout << "--------------------------\n";
	cout << "full enumerateionn\n";
	cout << "--------------------------\n";

	nt->fullEnumerate();

	cout << "--------------------------\n";
	cout << "partial enumerateion\n";
	cout << "--------------------------\n";
	
	Name cName1("/a/b/c/1/");
	Name cName2("/a/b/c/2/");
	Name cName3("/a/b/c/3/");
	Name cName4("/a/b/c/4/");
	Name cName5("/a/b/d/1/");
	Name cName6("/a/b/e/1/");
	Name cName7("/a/b/f/1/");
	Name cName8("/a/b/g/1/");
	Name cName9("/a/c/g/1/");

	nt->seek(cName1);
	nt->seek(cName2);
	nt->seek(cName3);
	nt->seek(cName4);
	nt->seek(cName5);
	nt->seek(cName6);
	nt->seek(cName7);
	nt->seek(cName8);
	nt->seek(cName9);

	cout << "/a/:children" << endl;
	Name dName1("a");
	nt->partialEnumerate(dName1);
	cout << "--------------------------\n";

	cout << "/a/b/ children:" << endl;
	Name dName2("a/b");
	nt->partialEnumerate(dName2);
	cout << "--------------------------\n";

	cout << "/a/b/c/ children:" << endl;
	Name dName3("a/b/c");
	nt->partialEnumerate(dName3);
	cout << "--------------------------\n";

	cout << "/does_not_exist/: children:" << endl;
	Name dName4("/does_not_exist");
	nt->partialEnumerate(dName4);
	cout << "--------------------------\n";

	return 0;
}


