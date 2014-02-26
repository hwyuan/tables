/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (C) 2014 Named Data Networking Project
 * See COPYING for copyright and distribution information.
 */

#include "table/pit.hpp"
#include "../face/dummy-face.hpp"

#include <boost/test/unit_test.hpp>

namespace nfd {

BOOST_AUTO_TEST_SUITE(TablePit)

BOOST_AUTO_TEST_CASE(EntryInOutRecords)
{
  shared_ptr<Face> face1 = make_shared<DummyFace>();
  shared_ptr<Face> face2 = make_shared<DummyFace>();
  Name name("ndn:/KuYfjtRq");
  Interest interest(name);
  Interest interest1(name, static_cast<ndn::Milliseconds>(2528));
  interest1.setNonce(25559);
  Interest interest2(name, static_cast<ndn::Milliseconds>(6464));
  interest2.setNonce(19004);
  Interest interest3(name, static_cast<ndn::Milliseconds>(3585));
  interest3.setNonce(24216);
  Interest interest4(name, static_cast<ndn::Milliseconds>(8795));
  interest4.setNonce(17365);
  
  pit::Entry entry(interest);
  
  BOOST_CHECK(entry.getInterest().getName().equals(name));
  BOOST_CHECK(entry.getName().equals(name));
  
  const pit::InRecordCollection& inRecords1 = entry.getInRecords();
  BOOST_CHECK_EQUAL(inRecords1.size(), 0);
  const pit::OutRecordCollection& outRecords1 = entry.getOutRecords();
  BOOST_CHECK_EQUAL(outRecords1.size(), 0);
  
  // insert InRecord
  time::Point before1 = time::now();
  pit::InRecordCollection::iterator in1 =
    entry.insertOrUpdateInRecord(face1, interest1);
  time::Point after1 = time::now();
  const pit::InRecordCollection& inRecords2 = entry.getInRecords();
  BOOST_CHECK_EQUAL(inRecords2.size(), 1);
  BOOST_CHECK(in1 == inRecords2.begin());
  BOOST_CHECK_EQUAL(in1->getFace(), face1);
  BOOST_CHECK_EQUAL(in1->getLastNonce(), interest1.getNonce());
  BOOST_CHECK_GE(in1->getLastRenewed(), before1);
  BOOST_CHECK_LE(in1->getLastRenewed(), after1);
  BOOST_CHECK_LE(std::abs(in1->getExpiry() - in1->getLastRenewed()
    - time::milliseconds(interest1.getInterestLifetime())),
    (after1 - before1));
  
  // insert OutRecord
  time::Point before2 = time::now();
  pit::OutRecordCollection::iterator out1 =
    entry.insertOrUpdateOutRecord(face1, interest1);
  time::Point after2 = time::now();
  const pit::OutRecordCollection& outRecords2 = entry.getOutRecords();
  BOOST_CHECK_EQUAL(outRecords2.size(), 1);
  BOOST_CHECK(out1 == outRecords2.begin());
  BOOST_CHECK_EQUAL(out1->getFace(), face1);
  BOOST_CHECK_EQUAL(out1->getLastNonce(), interest1.getNonce());
  BOOST_CHECK_GE(out1->getLastRenewed(), before2);
  BOOST_CHECK_LE(out1->getLastRenewed(), after2);
  BOOST_CHECK_LE(std::abs(out1->getExpiry() - out1->getLastRenewed()
    - time::milliseconds(interest1.getInterestLifetime())),
    (after2 - before2));
  
  // update InRecord
  time::Point before3 = time::now();
  pit::InRecordCollection::iterator in2 =
    entry.insertOrUpdateInRecord(face1, interest2);
  time::Point after3 = time::now();
  const pit::InRecordCollection& inRecords3 = entry.getInRecords();
  BOOST_CHECK_EQUAL(inRecords3.size(), 1);
  BOOST_CHECK(in2 == inRecords3.begin());
  BOOST_CHECK_EQUAL(in2->getFace(), face1);
  BOOST_CHECK_EQUAL(in2->getLastNonce(), interest2.getNonce());
  BOOST_CHECK_LE(std::abs(in2->getExpiry() - in2->getLastRenewed()
    - time::milliseconds(interest2.getInterestLifetime())),
    (after3 - before3));

  // insert another InRecord
  pit::InRecordCollection::iterator in3 =
    entry.insertOrUpdateInRecord(face2, interest3);
  const pit::InRecordCollection& inRecords4 = entry.getInRecords();
  BOOST_CHECK_EQUAL(inRecords4.size(), 2);
  BOOST_CHECK_EQUAL(in3->getFace(), face2);
  
  // delete all InRecords
  entry.deleteInRecords();
  const pit::InRecordCollection& inRecords5 = entry.getInRecords();
  BOOST_CHECK_EQUAL(inRecords5.size(), 0);

  // insert another OutRecord
  pit::OutRecordCollection::iterator out2 =
    entry.insertOrUpdateOutRecord(face2, interest4);
  const pit::OutRecordCollection& outRecords3 = entry.getOutRecords();
  BOOST_CHECK_EQUAL(outRecords3.size(), 2);
  BOOST_CHECK_EQUAL(out2->getFace(), face2);
  
  // delete OutRecord
  entry.deleteOutRecord(face2);
  const pit::OutRecordCollection& outRecords4 = entry.getOutRecords();
  BOOST_REQUIRE_EQUAL(outRecords4.size(), 1);
  BOOST_CHECK_EQUAL(outRecords4.begin()->getFace(), face1);
  
}

BOOST_AUTO_TEST_CASE(EntryNonce)
{
  Name name("ndn:/qtCQ7I1c");
  Interest interest(name);
  
  pit::Entry entry(interest);
  
  BOOST_CHECK_EQUAL(entry.addNonce(25559), true);
  BOOST_CHECK_EQUAL(entry.addNonce(25559), false);
  BOOST_CHECK_EQUAL(entry.addNonce(19004), true);
  BOOST_CHECK_EQUAL(entry.addNonce(19004), false);
}

BOOST_AUTO_TEST_CASE(Insert)
{
  Name name1("ndn:/5vzBNnMst");
  Name name2("ndn:/igSGfEIM62");
  Exclude exclude0;
  Exclude exclude1;
  exclude1.excludeOne(Name::Component("u26p47oep"));
  Exclude exclude2;
  exclude2.excludeOne(Name::Component("FG1Ni6nYcf"));

  // base
  Interest interestA(name1, -1, -1, exclude0, -1, false, -1, -1.0, 0);
  // A+exclude1
  Interest interestB(name1, -1, -1, exclude1, -1, false, -1, -1.0, 0);
  // A+exclude2
  Interest interestC(name1, -1, -1, exclude2, -1, false, -1, -1.0, 0);
  // A+MinSuffixComponents
  Interest interestD(name1, 2, -1, exclude0, -1, false, -1, -1.0, 0);
  // A+MaxSuffixComponents
  Interest interestE(name1, -1,  4, exclude0, -1, false, -1, -1.0, 0);
  // A+ChildSelector
  Interest interestF(name1, -1, -1, exclude0,  1, false, -1, -1.0, 0);
  // A+MustBeFresh
  Interest interestG(name1, -1, -1, exclude0, -1,  true, -1, -1.0, 0);
  // A+Scope
  Interest interestH(name1, -1, -1, exclude0, -1, false,  2, -1.0, 0);
  // A+InterestLifetime
  Interest interestI(name1, -1, -1, exclude0, -1, false, -1, 2000, 0);
  // A+Nonce
  Interest interestJ(name1, -1, -1, exclude0, -1, false, -1, -1.0, 2192);
  // different Name+exclude1
  Interest interestK(name2, -1, -1, exclude1, -1, false, -1, -1.0, 0);
  
  NameTree nt(16); // Will resize if needed
  Pit pit(&nt);

  std::pair<shared_ptr<pit::Entry>, bool> insertResult;
  
  insertResult = pit.insert(interestA);
  BOOST_CHECK_EQUAL(insertResult.second, true);
  
  insertResult = pit.insert(interestB);
  BOOST_CHECK_EQUAL(insertResult.second, true);
  
  insertResult = pit.insert(interestC);
  BOOST_CHECK_EQUAL(insertResult.second, true);
  
  insertResult = pit.insert(interestD);
  BOOST_CHECK_EQUAL(insertResult.second, true);
  
  insertResult = pit.insert(interestE);
  BOOST_CHECK_EQUAL(insertResult.second, true);
  
  insertResult = pit.insert(interestF);
  BOOST_CHECK_EQUAL(insertResult.second, true);
  
  insertResult = pit.insert(interestG);
  BOOST_CHECK_EQUAL(insertResult.second, true);
  
  insertResult = pit.insert(interestH);
  BOOST_CHECK_EQUAL(insertResult.second, false);// only guiders differ
  
  insertResult = pit.insert(interestI);
  BOOST_CHECK_EQUAL(insertResult.second, false);// only guiders differ
  
  insertResult = pit.insert(interestJ);
  BOOST_CHECK_EQUAL(insertResult.second, false);// only guiders differ
  
  insertResult = pit.insert(interestK);
  BOOST_CHECK_EQUAL(insertResult.second, true);
}

BOOST_AUTO_TEST_CASE(Remove)
{
  Interest interest(Name("ndn:/z88Admz6A2"));

  NameTree nt(16); // Will resize if needed
  Pit pit(&nt);

  std::pair<shared_ptr<pit::Entry>, bool> insertResult;
  
  insertResult = pit.insert(interest);
  BOOST_CHECK_EQUAL(insertResult.second, true);

  insertResult = pit.insert(interest);
  BOOST_CHECK_EQUAL(insertResult.second, false);
  
  pit.remove(insertResult.first);

  insertResult = pit.insert(interest);
  BOOST_CHECK_EQUAL(insertResult.second, true);
}

BOOST_AUTO_TEST_CASE(FindAllDataMatches)
{
  Name nameA  ("ndn:/A");
  Name nameAB ("ndn:/A/B");
  Name nameABC("ndn:/A/B/C");
  Name nameD  ("ndn:/D");
  Interest interestA (nameA );
  Interest interestAB(nameAB);
  Interest interestD (nameD );

  NameTree nt(16); // Will resize if needed
  Pit pit(&nt);
  pit.insert(interestA );
  pit.insert(interestAB);
  pit.insert(interestD );
  
  Data data(nameABC);
  
  shared_ptr<pit::DataMatchResult> matches = pit.findAllDataMatches(data);
  int count = 0;
  bool hasA  = false;
  bool hasAB = false;
  bool hasD  = false;
  for (pit::DataMatchResult::iterator it = matches->begin();
       it != matches->end(); ++it) {
    ++count;
    shared_ptr<pit::Entry> entry = *it;
    if (entry->getName().equals(nameA )) {
      hasA  = true;
    }
    if (entry->getName().equals(nameAB)) {
      hasAB = true;
    }
    if (entry->getName().equals(nameD )) {
      hasD  = true;
    }
  }
  BOOST_CHECK_EQUAL(count, 2);
  BOOST_CHECK_EQUAL(hasA , true);
  BOOST_CHECK_EQUAL(hasAB, true);
  BOOST_CHECK_EQUAL(hasD , false);
}

BOOST_AUTO_TEST_SUITE_END()

} // namespace nfd
