#ifndef __CORETEST_H__
#define __CORETEST_H__

#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

class CoreTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(CoreTest);
	CPPUNIT_TEST(printPlat);
	CPPUNIT_TEST(testBuffer);
	CPPUNIT_TEST(testStrBuffer);
	CPPUNIT_TEST(testStrConvertor);
	CPPUNIT_TEST(testTrace);
	CPPUNIT_TEST(testTimeStamp);

	CPPUNIT_TEST(testPoolObject);
	
	CPPUNIT_TEST_SUITE_END();
  public:
    CoreTest();
    virtual ~CoreTest();
	
	virtual void setUp();
	
	virtual void tearDown();

	void printPlat();
	void testBuffer();
	void testStrBuffer();
	void testStrConvertor();
	void testTrace();
	void testTimeStamp();

	void testPoolObject();
};

#endif
