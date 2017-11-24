#ifndef __MEMTEST_H__
#define __MEMTEST_H__

#include "cppunit/TestFixture.h"
#include "cppunit/extensions/HelperMacros.h"
#include "core/corebase.h"

class MemTest : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(MemTest);
    CPPUNIT_TEST(testSimple);
    CPPUNIT_TEST_SUITE_END();
  public:
    MemTest();
    virtual ~MemTest();
    
    virtual void setUp();
    
    virtual void tearDown();

    void testSimple();
};

#endif // __MEMTEST_H__
