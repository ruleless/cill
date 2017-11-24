#ifndef __KLIBTEST_H__
#define __KLIBTEST_H__

#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"

class klibTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(klibTest);
	CPPUNIT_TEST(testList);
	CPPUNIT_TEST_SUITE_END();
  public:
    klibTest();
    virtual ~klibTest();
	
	virtual void setUp();
	
	virtual void tearDown();

	void testList();
};

#endif // __KLIBTEST_H__
