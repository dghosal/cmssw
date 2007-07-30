#include <Utilities/Testing/interface/CppUnit_testdriver.icpp>
#include <cppunit/extensions/HelperMacros.h>

#define private public
#include "DataFormats/Common/interface/DetSetNew.h"
#include "DataFormats/Common/interface/DetSetVectorNew.h"
#undef private

#include "FWCore/Utilities/interface/EDMException.h"

#include<vector>
#include<algorithm>

struct T {
  T(float iv=0) : v(iv){}
  float v;
  bool operator==(T t) const { return v==t.v;}
};

typedef edmNew::DetSetVector<T> DSTV;
typedef edmNew::DetSet<T> DST;
typedef DSTV::FastFiller FF;


class TestDetSet: public CppUnit::TestFixture
{
  CPPUNIT_TEST_SUITE(TestDetSet);
  CPPUNIT_TEST(default_ctor);
  CPPUNIT_TEST(inserting);
  CPPUNIT_TEST(filling);
  CPPUNIT_TEST(iterator);
  CPPUNIT_TEST(onDemand);

  CPPUNIT_TEST_SUITE_END();

 public:
  TestDetSet(); 
  ~TestDetSet() {}
  void setUp() {}
  void tearDown() {}

  void default_ctor();
  void inserting();
  void filling();
  void iterator();
  void onDemand();

public:
  std::vector<DSTV::data_type> sv;

};

CPPUNIT_TEST_SUITE_REGISTRATION(TestDetSet);

TestDetSet::TestDetSet() : sv(10){ 
  DSTV::data_type v[10] = {0.,1.,2.,3.,4.,5.,6.,7.,8.,9.};
  std::copy(v,v+10,sv.begin());
} 


void TestDetSet::default_ctor() {

  DSTV detsets(2);
  CPPUNIT_ASSERT(detsets.subdetId()==2);
  CPPUNIT_ASSERT(detsets.size()==0);
  CPPUNIT_ASSERT(detsets.empty());
  CPPUNIT_ASSERT(detsets.dataSize()==0);
  detsets.resize(3,10);
  CPPUNIT_ASSERT(detsets.size()==3);
  CPPUNIT_ASSERT(detsets.dataSize()==10);
  CPPUNIT_ASSERT(!detsets.empty());
  // follow is nonsense still valid construct... (maybe it shall throw...)
  DST df(detsets,detsets.item(1));
  CPPUNIT_ASSERT(df.id()==0); 
  CPPUNIT_ASSERT(df.size()==0); 
  CPPUNIT_ASSERT(df.m_data+1==&detsets.m_data.front()); 
  df.set(detsets,detsets.item(2));
  CPPUNIT_ASSERT(df.size()==0); 
  CPPUNIT_ASSERT(df.m_data+1==&detsets.m_data.front()); 
  DSTV detsets2(3);
  // test swap
  detsets.swap(detsets2);
  CPPUNIT_ASSERT(detsets.subdetId()==3);
  CPPUNIT_ASSERT(detsets.size()==0);
  CPPUNIT_ASSERT(detsets.dataSize()==0);
  CPPUNIT_ASSERT(detsets2.subdetId()==2);
  CPPUNIT_ASSERT(detsets2.size()==3);
  CPPUNIT_ASSERT(detsets2.dataSize()==10);
  

}

void TestDetSet::inserting() {

  DSTV detsets(2);
  unsigned int ntot=0;
  for (unsigned int n=1;n<5;++n) {
    ntot+=n;
    unsigned int id=20+n;
    DST df = detsets.insert(id,n);
    CPPUNIT_ASSERT(detsets.size()==n);
    CPPUNIT_ASSERT(detsets.dataSize()==ntot);
    CPPUNIT_ASSERT(detsets.detsetSize(n-1)==n);
    CPPUNIT_ASSERT(df.size()==n); 
    CPPUNIT_ASSERT(df.id()==id); 
    
    std::copy(sv.begin(),sv.begin()+n,df.begin());
    
    std::vector<DST::data_type> v1(n);
    std::vector<DST::data_type> v2(n);
    std::copy(detsets.m_data.begin()+ntot-n,detsets.m_data.begin()+ntot,v2.begin());
    std::copy(sv.begin(),sv.begin()+n,v1.begin());
    CPPUNIT_ASSERT(v1==v2);
  }
  
  // test error conditions
  try {
    DST dfe = detsets.insert(22,6);
    CPPUNIT_ASSERT("insert did not threw"==0);
  } 
  catch (edm::Exception const & err) {
    CPPUNIT_ASSERT(err.categoryCode()==edm::errors::InvalidReference);
  }
  
}

void TestDetSet::filling() {
  
  DSTV detsets(2);
  unsigned int ntot=0;
  for (unsigned int n=1;n<5;++n) {
    unsigned int id=20+n;
    FF ff(detsets, id);
    CPPUNIT_ASSERT(detsets.size()==n);
    CPPUNIT_ASSERT(detsets.dataSize()==ntot);
    CPPUNIT_ASSERT(detsets.detsetSize(n-1)==0);
    CPPUNIT_ASSERT(ff.item.offset==int(detsets.dataSize())); 
    CPPUNIT_ASSERT(ff.item.size==0); 
    CPPUNIT_ASSERT(ff.item.id==id); 
    ntot+=1;
    ff.push_back(3.14);
    CPPUNIT_ASSERT(detsets.dataSize()==ntot);
    CPPUNIT_ASSERT(detsets.detsetSize(n-1)==1);
    CPPUNIT_ASSERT(detsets.m_data.back().v==3.14f);
    CPPUNIT_ASSERT(ff.item.offset==int(detsets.dataSize())-1); 
    CPPUNIT_ASSERT(ff.item.size==1);  
    ntot+=n-1;
    ff.resize(n);
    CPPUNIT_ASSERT(detsets.dataSize()==ntot);
    CPPUNIT_ASSERT(detsets.detsetSize(n-1)==n);
    CPPUNIT_ASSERT(ff.item.offset==int(detsets.dataSize()-n)); 
    CPPUNIT_ASSERT(ff.item.size==n);  
    
    std::copy(sv.begin(),sv.begin()+n,ff.begin());

    std::vector<DST::data_type> v1(n);
    std::vector<DST::data_type> v2(n);
    std::copy(detsets.m_data.begin()+ntot-n,detsets.m_data.begin()+ntot,v2.begin());
    std::copy(sv.begin(),sv.begin()+n,v1.begin());
    CPPUNIT_ASSERT(v1==v2);
  }

  // test abort and empty
  {
    FF ff1(detsets, 30);
    CPPUNIT_ASSERT(detsets.size()==5);
    CPPUNIT_ASSERT(detsets.exists(30));
    CPPUNIT_ASSERT(detsets.isValid(30));
  }
  CPPUNIT_ASSERT(detsets.size()==4);
  CPPUNIT_ASSERT(!detsets.exists(30));
 {
   FF ff1(detsets, 30,true);
   CPPUNIT_ASSERT(detsets.size()==5);
   CPPUNIT_ASSERT(detsets.exists(30));
   CPPUNIT_ASSERT(detsets.isValid(30));
  }
 CPPUNIT_ASSERT(detsets.size()==5);
 CPPUNIT_ASSERT(detsets.exists(30));

 {
   unsigned int cs = detsets.dataSize();
   FF ff1(detsets, 31);
   ff1.resize(4);
   CPPUNIT_ASSERT(detsets.size()==6);
   CPPUNIT_ASSERT(detsets.dataSize()==cs+4);
   CPPUNIT_ASSERT(detsets.exists(31));
   CPPUNIT_ASSERT(detsets.isValid(31));
   ff1.abort();
   CPPUNIT_ASSERT(detsets.size()==5);
   CPPUNIT_ASSERT(detsets.dataSize()==cs);
   CPPUNIT_ASSERT(!detsets.exists(31));
 }
 CPPUNIT_ASSERT(detsets.size()==5);
 CPPUNIT_ASSERT(!detsets.exists(31));
 

  // test error conditions
  try {
    FF ff1(detsets, 22);
    CPPUNIT_ASSERT(" fast filler did not threw"==0);
  } 
  catch (edm::Exception const & err) {
    CPPUNIT_ASSERT(err.categoryCode()==edm::errors::InvalidReference);
  }
  
  try {
    FF ff1(detsets, 44);
    FF ff2(detsets, 45);
    CPPUNIT_ASSERT(" fast filler did not threw"==0);
  } catch (edm::Exception const &err) {
    CPPUNIT_ASSERT(err.categoryCode()==edm::errors::LogicError);
  }
 
 
}

namespace {
  struct VerifyIter{
    VerifyIter(TestDetSet * itest, unsigned int in=1, int iincr=1):
      n(in), incr(iincr), test(*itest){}
    
    void operator()(DST const & df) {
      CPPUNIT_ASSERT(df.id()==20+n); 
      CPPUNIT_ASSERT(df.size()==n); 
      std::vector<DST::data_type> v1(n);
      std::vector<DST::data_type> v2(n);
      std::copy(df.begin(),df.end(),v2.begin());
      std::copy(test.sv.begin(),test.sv.begin()+n,v1.begin());
      CPPUNIT_ASSERT(v1==v2);
      n+=incr;
    }
    
    unsigned int n;
    int incr;
    TestDetSet & test;
  };

  struct Getter : public DSTV::Getter {
    Getter(TestDetSet * itest):ntot(0), test(*itest){}

    void fill(FF& ff) {
      int n=ff.id()-20;
      CPPUNIT_ASSERT(n>0);
      ff.resize(n);
      std::copy(test.sv.begin(),test.sv.begin()+n,ff.begin());
      ntot+=n;
    }

    unsigned int ntot;
    TestDetSet & test;
  };

}

void TestDetSet::iterator() {
  DSTV detsets(2);
  for (unsigned int n=1;n<5;++n) {
    unsigned int id=20+n;
    FF ff(detsets,id);
    ff.resize(n);
    std::copy(sv.begin(),sv.begin()+n,ff.begin());
  }
  CPPUNIT_ASSERT(std::for_each(detsets.begin(),detsets.end(),VerifyIter(this)).n==5);

  try {
    {
      CPPUNIT_ASSERT(detsets.exists(22));
      CPPUNIT_ASSERT(!detsets.exists(44));
      DST df = *detsets.find(22);
      CPPUNIT_ASSERT(df.id()==22);
      CPPUNIT_ASSERT(df.size()==2);
    }
    {
      DST df = detsets[22];
      CPPUNIT_ASSERT(df.id()==22);
      CPPUNIT_ASSERT(df.size()==2);
    }
  }
  catch (edm::Exception const &) {
    CPPUNIT_ASSERT("DetSetVector threw when not expected"==0);
  }
  
  try{
    DSTV::const_iterator p = detsets.find(44);
    CPPUNIT_ASSERT(p==detsets.end());
  } 
  catch (edm::Exception const &) {
    CPPUNIT_ASSERT("find threw edm exception when not expected"==0);
    
  }
  try{
    DST df = detsets[44];
    CPPUNIT_ASSERT("[] did not threw"==0);
  } 
  catch (edm::Exception const & err) {
       CPPUNIT_ASSERT(err.categoryCode()==edm::errors::InvalidReference);
  }
}
#include <boost/assign/std/vector.hpp>
// for operator =+
using namespace boost::assign;


void TestDetSet::onDemand() {
  boost::shared_ptr<Getter> pg(new Getter(this));
  Getter & g = *pg;
  std::vector<unsigned int> v; v+= 21,23,25,27;
  DSTV detsets(pg,v,2);
  CPPUNIT_ASSERT(g.ntot==0);
  try {
    {
      CPPUNIT_ASSERT(detsets.exists(21));
      CPPUNIT_ASSERT(!detsets.exists(22));
      CPPUNIT_ASSERT(!detsets.isValid(21));
      CPPUNIT_ASSERT(!detsets.isValid(22));
      DST df = *detsets.find(21);
      CPPUNIT_ASSERT(df.id()==21);
      CPPUNIT_ASSERT(df.size()==1);
      CPPUNIT_ASSERT(detsets.isValid(21));
      CPPUNIT_ASSERT(!detsets.isValid(23));
      CPPUNIT_ASSERT(g.ntot==1);
    }
    {
      DST df = detsets[25];
      CPPUNIT_ASSERT(df.id()==25);
      CPPUNIT_ASSERT(df.size()==5);
      CPPUNIT_ASSERT(g.ntot==1+5);
    }
  }
  catch (edm::Exception const &) {
    CPPUNIT_ASSERT("DetSetVector threw when not expected"==0);
  }

  CPPUNIT_ASSERT(std::for_each(detsets.begin(),detsets.end(),VerifyIter(this,1,2)).n==9);
  CPPUNIT_ASSERT(g.ntot==1+3+5+7);

  
  try{
    DSTV::const_iterator p = detsets.find(22);
    CPPUNIT_ASSERT(p==detsets.end());
  } 
  catch (edm::Exception const &) {
    CPPUNIT_ASSERT("find threw edm exception when not expected"==0);
    
  }
  try{
    DST df = detsets[22];
    CPPUNIT_ASSERT("[] did not threw"==0);
  } 
  catch (edm::Exception const & err) {
       CPPUNIT_ASSERT(err.categoryCode()==edm::errors::InvalidReference);
  }
}
