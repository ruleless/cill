#include "MemTest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(MemTest);


MemTest::MemTest()
{
}

MemTest::~MemTest()
{
}
	
void MemTest::setUp()
{
}
	
void MemTest::tearDown()
{
}

void MemTest::testSimple()
{	
	int alcttimes = 10000;
	for (int i = 0; i < alcttimes; ++i)
	{
		int alctsz = random()%65535;
		if (alctsz > 0)
		{
			int *tt = (int *)malloc(alctsz);
			tt[0] = 10;
			free(tt);
		}
	}
}
