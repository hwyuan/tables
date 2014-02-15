/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

// Name Prefix Hash Table

/*

TODO:

DONE 			0. add namespace nfd 
Need discussion 1. convert to use TLV Name instead of strings
In progress 	2. unit testing
In progress 	3. follow the new coding style
TODO			4. may consier using some hash function from a library
DONE 			5. add LPM function by calling the lookup() function
DONE 			6. add full / partial enumeration function

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
NameTree::insert(std::string prefix, NamePrefixEntry ** ret_npe)
{
	uint32_t hashValue = CityHash32(prefix.c_str(), prefix.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug) std::cout << "string " << prefix << " hash value = " << hashValue << "  loc = " << loc << std::endl;

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


// Name Prefix Seek
// Build the NPHT with parent pointers
// Lookup each name prefix, if the name prefix does not exit, then create the node.
int 
NameTree::seek(std::string prefix)
{
	std::vector<std::string> strs;
	boost::split(strs, prefix, boost::is_any_of("/"));

	NamePrefixEntry * npe = NULL;
	NamePrefixEntry * parent = NULL;

	std::string endMark = "/";
	int end = boost::algorithm::ends_with(prefix, endMark);

	std::string temp = "";
	for(size_t i = 0; i < strs.size()-end; i++){
		temp += strs[i]; 
		temp += "/";

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
NameTree::lookup(std::string prefix)
{
	uint32_t hashValue = CityHash32(prefix.c_str(), prefix.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug > 4) std::cout << "string " << prefix << " hash value = " << hashValue << "  loc = " << loc << std::endl;

	NamePrefixEntry * ret = m_buckets[loc].m_npeHead;
	while(ret != NULL){
		if(hashValue == ret->getHash() && prefix.compare(ret->m_prefix) == 0){	// found
			if(debug > 4) std::cout << "found " << prefix << std::endl;
			break;
		} else {
			ret = ret->m_next;
		}
	}

	return ret;
}

// Return the longest matching prefix
// XXX FIXME: return the lpm with a FIB entry? or return the lpm with a PIT entry?
NamePrefixEntry *
NameTree::lpm(std::string prefix){

	NamePrefixEntry * ret = NULL;
	// start from the full name, and then remove 1 name comp each time
	
	// XXX FIXME: below is used to parse the name prefix, so that NPHT begines
	// the query from the longest name prefix.
	std::vector<std::string> strs;
	std::vector<std::string> strsReverse;
	boost::split(strs, prefix, boost::is_any_of("/"));

	std::string endMark = "/";
	int end = boost::algorithm::ends_with(prefix, endMark);

	std::string temp = "";
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
		if(npe != NULL){
			ret = npe;
			break;
		}
	}

	return ret;
}


// XXX FIXME: figure out the return values
int
NameTree::deletePrefix(std::string prefix)
{
	// delete a NPE based on a prefix

	std::vector<std::string> strs;
	std::vector<std::string> strsReverse;
	boost::split(strs, prefix, boost::is_any_of("/"));

	std::string endMark = "/";
	int end = boost::algorithm::ends_with(prefix, endMark);

	std::string temp = "";
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

		if(npe == NULL) return 0;

		// if this entry can be deleted (only if it has no children and no fib and no pit)
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
NameTree::fullEnumerate(){
	for(int i = 0; i < m_nBuckets; i++){
		NamePrefixEntry * temp = m_buckets[i].m_npeHead;
		while(temp != NULL){
			std::cout << "Bucket" << i << "\t" << temp->m_prefix << std::endl;
			temp = temp->m_next;
		}
	}
}


void
NameTree::partialEnumerate(std::string prefix){

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
			std::cout << npe->m_childrenList[i]->m_prefix << std::endl;
		}
	}
}


void 
NameTree::dump()
{
	for(int i = 0; i < m_nBuckets; i++){


		NamePrefixEntry * temp = m_buckets[i].m_npeHead;
		while(temp != NULL){

			std::cout << "Bucket" << i << "\t" << temp->m_prefix << std::endl;

			std::cout << "\t\tHash " << temp->m_hash << std::endl;

			if(temp->m_parent != NULL){
				std::cout << "\t\tparent->" << temp->m_parent->m_prefix;
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
int test()
{

	using namespace std;
	using namespace nfd;
	
	// if(debug) cout << "Testing Name Prefix Hash Table Implementation" << endl;

	int nameTreeSize = 1024;

	NameTree * nt = new NameTree(nameTreeSize);

	nt->seek("/a/b/c");
	NamePrefixEntry * npe = NULL;

	npe = nt->lookup("/a/");
	if(npe) cout << "/a/ exist = " << npe->m_prefix << endl;

	npe = nt->lookup("/a/b/");
	if(npe) cout << "/a/b/ exist = " << npe->m_prefix << endl;

	npe = nt->lookup("/a/b/c/");
	if(npe) cout << "/a/b/c/ exist = " << npe->m_prefix << endl;


	npe = nt->lookup("/a/b/c/d/e/f/g/");
	if(npe){
		cout << "Error...lookup /a/b/c/d/ \n";
		exit(1);
	} else {
		cout << "succeeded, did not find /a/b/c/d/e/f/g/" << endl;
	}

	npe = nt->lpm("/a/b/c/d/e/f/g/");
	if(npe) cout << "/a/b/c/d/e/f/g/'s LPM prefix is " << npe->m_prefix << endl;

	nt->seek("/a/b/c/d/");
	npe = nt->lpm("/a/b/c/d/e/f/g/");
	cout << "after inserint /a/b/c/d/" << endl;;
	if(npe) cout << "/a/b/c/d/e/f/g/'s LPM prefix is " << npe->m_prefix << endl;

	cout << "Now insert /a/b/c/d/e/f/g/ via insert(), not seek()" << endl;
	nt->insert("/a/b/c/d/e/f/g/", &npe);

	cout << "Perform lookup on /a/b/c/d/e/f/g/" << endl;
	npe = nt->lookup("/a/b/c/d/e/f/g/");
	if(npe && npe->m_prefix.compare("/a/b/c/d/e/f/g/") == 0)cout << "insert() passed part 1/2" << endl;

	cout << "Perform lookup on /a/b/c/d/e/f/, should return NULL" << endl;
	npe = nt->lookup("/a/b/c/d/e/f/");
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
	
	nt->seek("/a/b/c/1/");
	nt->seek("/a/b/c/2/");
	nt->seek("/a/b/c/3/");
	nt->seek("/a/b/c/4/");
	nt->seek("/a/b/d/1/");
	nt->seek("/a/b/e/1/");
	nt->seek("/a/b/f/1/");
	nt->seek("/a/b/g/1/");

	cout << "/a/:children" << endl;
	nt->partialEnumerate("/a/");
	cout << "--------------------------\n";

	cout << "/a/b/ children:" << endl;
	nt->partialEnumerate("/a/b/");
	cout << "--------------------------\n";

	cout << "/a/b/c/ children:" << endl;
	nt->partialEnumerate("/a/b/c/");
	cout << "--------------------------\n";

	cout << "/does_not_exist/: children:" << endl;
	nt->partialEnumerate("/does_not_exist/");
	cout << "--------------------------\n";

	return 0;
}


