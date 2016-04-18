#include "CoreTest.h"

#include "CillConfig.h"
#include "Buffer.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CoreTest);

CoreTest::CoreTest()
{
}

CoreTest::~CoreTest()
{
}
	
void CoreTest::setUp()
{
}
	
void CoreTest::tearDown()
{
}

void CoreTest::printPlat()
{
	printf("%s\n", PLATFORM_TEXT);
}

void CoreTest::testBuffer()
{
	srand(time(NULL));
	
	core::obuf ob;
	std::vector<int> ia;
	for (int i = 0; i < 100000; ++i)
	{
		int n = rand() % 65535;
		ia.push_back(n);
		ob<<n;
	}
	
	core::InBuffer ib(ob.data(), ob.size());
	std::vector<int>::const_iterator it = ia.begin();
	for (; it != ia.end(); ++it)
	{
		int n = 0;
		ib>>n;
		CPPUNIT_ASSERT(n == *it);
	}
}
