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
<<<<<<< HEAD
In progress 	2. unit testing
In progress 	3. follow the new coding style
TODO			4. may consier using some hash function from a library
DONE 			5. add LPM function by calling the lookup() function
DONE 			6. add full / partial enumeration function
TODO			7. hash table from boost

ISSUES
- Currently the hash function takes std::string as input, which requires ndn::Name to be converted to string first..

NOTE
- Currently we are using lazy deletion for Name Tree Node.

=======
In progress 	2. unit test
TODO			3. may consier using some hash function from a library
DONE 			4. add LPM function by calling the lookup() function
DONE 			5. add full / partial enumeration function
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
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

int debug = 5;

typedef ndn::Name Name;

typedef ndn::Name Name;

NameTreeNode::NameTreeNode()
{
	m_npe = NULL;
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


NameTree::NameTree(int nBuckets)
{	
	if(debug > 4) std::cout << "Name::Tree()" << std::endl;

	m_n = 0; // Number of items stored in the table
	m_nBuckets = nBuckets;
	m_buckets = new NameTreeNode*[m_nBuckets];

	/* Initialize the pointer array */
	for(int i = 0; i < m_nBuckets; i++)
	{
		m_buckets[i] = NULL;
	}
}


NameTree::~NameTree()
{	
	int i = 0;
	for(i = 0; i < m_nBuckets; i++)
	{
		m_buckets[i]->destory();
	}
	delete [] m_buckets;
}


// XXX FIXME: insert functino should be private, as it does not handle parent pointers
int
<<<<<<< HEAD
NameTree::insert(ndn::Name prefix, NamePrefixEntry ** retNpe)
=======
NameTree::insert(ndn::Name prefix, NamePrefixEntry ** ret_npe)
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
{
	std::string uri = prefix.toUri();
	uint32_t hashValue = CityHash32(uri.c_str(), uri.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug > 4) std::cout << "uri " << uri << " hash value = " << hashValue << "  loc = " << loc << std::endl;

	// Check if this Name has been stored
	// Record the first empty Name Tree Node to insert this new Name

<<<<<<< HEAD
	NameTreeNode * temp = m_buckets[loc];
	NameTreeNode * tempPre = temp;	// initialize tempPre to temp

	for(temp = m_buckets[loc]; temp != NULL; temp = temp->m_next)
	{
		if(temp->m_npe != NULL)
		{
			if(prefix.equals(temp->m_npe->m_prefix) == 1){
				*retNpe = temp->m_npe;
				return HT_OLD_ENTRY;
			}
		}
		tempPre = temp;
=======
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
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
	}

	if(debug > 4) std::cout << "Did not find " << prefix.toUri() << ", need to insert it to the table\n";


	/* If no bucket is empty occupied, we need to create a new node, and it is linked from tempPre */

	NameTreeNode * node = new NameTreeNode();
	if(tempPre == NULL)
	{
		m_buckets[loc] = node;
	} else {
		tempPre->m_next = node;
	}

	/* Create a new NPE */
	NamePrefixEntry * npe = new NamePrefixEntry(prefix);
	npe->setHash(hashValue);
	node->m_npe = npe; // link the NPE to its Node

	* retNpe = npe;

	return HT_NEW_ENTRY;
}


<<<<<<< HEAD
/* Name Prefix Seek. Create NPE if not found */
=======
// Name Prefix Seek. Create NPE if not found.
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
int 
NameTree::seek(ndn::Name prefix)
{
	NamePrefixEntry * npe = NULL;
	NamePrefixEntry * parent = NULL;

	for(size_t i = 0; i <= prefix.size(); i++){

		Name temp = prefix.getPrefix(i);

		int res = insert(temp, &npe);  /* insert() will create the entry if it does not exist. */

		if(res == HT_NEW_ENTRY){
			m_n++; /* Increase the counter */
			npe->m_parent = parent;

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


// Exact Match
// Return the address of the node that contains this prefix; 
// Return NULL if not found
NamePrefixEntry* 
NameTree::lookup(ndn::Name prefix)
{
	std::string uri = prefix.toUri();
	uint32_t hashValue = CityHash32(uri.c_str(), uri.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug > 4) std::cout << "uri " << uri << " hash value = " << hashValue << "  loc = " << loc << std::endl;

<<<<<<< HEAD
	NamePrefixEntry * npe = NULL;
	NameTreeNode * ntn = NULL;
=======
	NamePrefixEntry * ret = m_buckets[loc].m_npeHead;
	while(ret != NULL){
		if(hashValue == ret->getHash() && prefix.equals(ret->m_prefix) == 1){	// found
			if(debug > 4) std::cout << "found " << uri << std::endl;
			break;
		} else {
			ret = ret->m_next;
		}
	}
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845

	for(ntn = m_buckets[loc]; ntn != NULL; ntn = ntn->m_next)
	{
		npe = ntn->m_npe;
		if(npe != NULL){
			if(hashValue == npe->getHash() && prefix.equals(npe->m_prefix) == 1)
			{
				if(debug > 4) std::cout << "found " << uri << std::endl;
				return npe;
			}
		} // if npe
	} // for ntn
	return NULL;
}

<<<<<<< HEAD

// Exact Match
// Return the address of the node that contains this prefix; 
// Return NULL if not found
NamePrefixEntry* 
NameTree::lookup(ndn::Name prefix, NameTreeNode ** retNode, NameTreeNode ** retNodePre)
{
	std::string uri = prefix.toUri();
	uint32_t hashValue = CityHash32(uri.c_str(), uri.length());
	uint32_t loc = hashValue % m_nBuckets;

	if(debug > 4) std::cout << "uri " << uri << " hash value = " << hashValue << "  loc = " << loc << std::endl;

	NamePrefixEntry * npe = NULL;
	NameTreeNode * node = NULL;
	NameTreeNode * nodePre = NULL;

	for(node = m_buckets[loc]; node != NULL; node = node->m_next)
	{
		npe = node->m_npe;
		if(npe != NULL){
			if(hashValue == npe->getHash() && prefix.equals(npe->m_prefix) == 1)
			{
				if(debug > 4) std::cout << "found " << uri << std::endl;
				* retNode = node;
				* retNodePre = nodePre;
				return npe;
			}
		} // if npe
		nodePre = node;
	} // for node
	return NULL;
}


// Longest Prefix Match
// Return the longest matching NPE address
// start from the full name, and then remove 1 name comp each time
NamePrefixEntry *
NameTree::lpm(ndn::Name prefix){

	NamePrefixEntry * npe = NULL;

	for(int i = prefix.size(); i >= 0; i--)
	{
		npe = lookup(prefix.getPrefix(i));
		if(npe != NULL)
			return npe;
	}
	return NULL;
}


/* delete a NPE based on a prefix */
// Return 0: failure
// Return 1: success
=======
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
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
int
NameTree::deletePrefix(ndn::Name prefix)
{
	for(int i = prefix.size(); i >= 0; i--)
	{
		Name temp = prefix.getPrefix(i);
<<<<<<< HEAD
		NameTreeNode * node = NULL;
		NameTreeNode * nodePre = NULL;
		NamePrefixEntry * npe = lookup(temp, &node, &nodePre);
=======
		NamePrefixEntry * npe = lookup(temp);
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845

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

			if(nodePre != NULL)
			{
				nodePre->m_next = node->m_next;
			} else {
				m_buckets[npe->m_hash % m_nBuckets] = node->m_next;
			}

			delete npe;
			delete node;
			m_n--;
		}
	}
    return 1;
}


void
NameTree::fullEnumerate()
{
<<<<<<< HEAD
	NameTreeNode * node = NULL;

	for(int i = 0; i < m_nBuckets; i++)
	{
		for(node = m_buckets[i]; node != NULL; node = node->m_next)
		{
			if(node->m_npe)
			{
				std::cout << "Bucket" << i << "\t" << node->m_npe->m_prefix.toUri() << std::endl;
			}
=======
	for(int i = 0; i < m_nBuckets; i++){
		NamePrefixEntry * temp = m_buckets[i].m_npeHead;
		while(temp != NULL){
			std::cout << "Bucket" << i << "\t" << temp->m_prefix.toUri() << std::endl;
			temp = temp->m_next;
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
		}
	}
}


<<<<<<< HEAD

=======
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
void
NameTree::partialEnumerate(ndn::Name prefix)
{
	// find the hash bucket corresponding to that prefix
	// then enumerate all of its children...
	NamePrefixEntry * npe = lookup(prefix);

	if(npe == NULL){
		/* should probably return a vlaue, currently just print a statement */
		std::cout << "Error::partialEnumerate, prefix does not exist\n";
		return;
	} else {
		/* go through its children list */
		for(size_t i = 0; i < npe->m_childrenList.size(); i++){
			std::cout << npe->m_childrenList[i]->m_prefix.toUri() << std::endl;
		}
	}
}

// Hash Table Resize
void
NameTree::resize(int newNBuckets)
{
	if(debug > 4) std::cout << "NameTree::resize()" << std::endl;

	NameTreeNode ** newBuckets = new NameTreeNode*[newNBuckets];
	int count = 0;

	/* referenced ccnx hashtb.c hashtb_rehash() */

	NameTreeNode ** pp = NULL;
	NameTreeNode * p = NULL;
	NameTreeNode * q = NULL; // record p->m_next
	uint32_t h;
	int i;
	uint32_t b;

	for(i = 0; i < newNBuckets; i++)
	{
		newBuckets[i] = NULL;
	}

	for(i = 0; i < m_nBuckets; i++)
	{
		for(p = m_buckets[i]; p != NULL; p = q) 
		{
			count++;
			q = p->m_next;
			if(p->m_npe == NULL){
				exit(1);
			}
			h = p->m_npe->m_hash;
			b = h % newNBuckets;
			for(pp = &newBuckets[b]; *pp != NULL; pp = &((*pp)->m_next))
			{
				continue;
			}
			p->m_next = *pp; // Actually *pp always == NULL in this case
			*pp = p;
		}
	}

	// XXX FIXME Throw out an exception instead of printing out error messages.
	if(count != m_n){
		std::cout << "Error hash table resize() count != m_n" << std::endl;
		exit(1);
	}

	m_nBuckets = newNBuckets;
	NameTreeNode ** oldBuckets = m_buckets;
	m_buckets = newBuckets;

	delete oldBuckets;
}

// For debugging
void 
NameTree::dump()
{
	NameTreeNode * node = NULL;
	NamePrefixEntry * npe;

	if(debug > 4) std::cout << "dump() --------------------------\n";

	for(int i = 0; i < m_nBuckets; i++){

		for(node = m_buckets[i]; node != NULL; node = node->m_next){

			npe = node->m_npe;

<<<<<<< HEAD
			/* if the NPE exist, dump its information*/
			if(npe != NULL){
				std::cout << "Bucket" << i << "\t" << npe->m_prefix.toUri() << std::endl;
				std::cout << "\t\tHash " << npe->m_hash << std::endl;

				if(npe->m_parent != NULL){
					std::cout << "\t\tparent->" << npe->m_parent->m_prefix.toUri();
				} else {
					std::cout << "\t\tROOT";
				}
				std::cout << std::endl;
=======
			std::cout << "Bucket" << i << "\t" << temp->m_prefix.toUri() << std::endl;
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845

				if(npe->m_children != 0){
					std::cout << "\t\tchildren = " << npe->m_children << std::endl;
				}

<<<<<<< HEAD
			} // if npe != NULL
=======
			if(temp->m_parent != NULL){
				std::cout << "\t\tparent->" << temp->m_parent->m_prefix.toUri();
			} else {
				std::cout << "\t\tROOT";
			}
			std::cout << std::endl;
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845


		} // for node
	} // for int i

	std::cout << "Bucket count = " << m_nBuckets << std::endl;
	std::cout << "Stored item = " << m_n << std::endl;
	std::cout << "--------------------------\n";

}

} // namespace nfd


<<<<<<< HEAD
=======
// XXX TODO: Convert the main() function to the unit tests
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
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
<<<<<<< HEAD

=======
	
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
	NamePrefixEntry * npe = NULL;

	npe = nt->lookup(aName1);
	if(npe) cout << "/a/ exist = " << npe->m_prefix.toUri() << endl;
<<<<<<< HEAD

	npe = nt->lookup(aName2);
	if(npe) cout << "/a/b/ exist = " << npe->m_prefix.toUri() << endl;

	npe = nt->lookup(aName);
	if(npe) cout << "/a/b/c/ exist = " << npe->m_prefix.toUri() << endl;

	Name bName("/a/b/c/d/e/f/g/");

=======

	npe = nt->lookup(aName2);
	if(npe) cout << "/a/b/ exist = " << npe->m_prefix.toUri() << endl;

	npe = nt->lookup(aName);
	if(npe) cout << "/a/b/c/ exist = " << npe->m_prefix.toUri() << endl;

	Name bName("/a/b/c/d/e/f/g/");

>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845
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
<<<<<<< HEAD
=======

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
>>>>>>> 611673fddca5dc3a8c7feab06218e15e51c69845

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

	nt = new NameTree(8);

	Name a("/a");
	Name b("/b");
	Name c("/c");
	Name d("/d");
	Name e("/e");
	Name f("/f");
	Name g("/g");
	Name h("/h");
	Name i("/i");
	Name j("/j");
	Name k("/k");
	Name l("/l");
	Name m("/m");
	Name n("/n");
	Name o("/o");
	Name p("/p");

	nt->seek(a);
	nt->dump();
	nt->seek(b);
	nt->dump();
	nt->seek(c);
	nt->dump();
	nt->seek(d);
	nt->dump();
	nt->seek(e);
	nt->dump();
	nt->seek(f);
	nt->dump();
	nt->seek(g);
	nt->dump();
	nt->seek(h);
	nt->dump();
	nt->seek(i);
	nt->dump();
	nt->seek(j);
	nt->dump();
	nt->seek(k);
	nt->dump();
	nt->seek(l);
	nt->dump();
	nt->seek(m);
	nt->dump();
	nt->seek(n);
	nt->dump();
	nt->seek(o);
	nt->dump();
	nt->seek(p);
	nt->dump();

	return 0;
}

