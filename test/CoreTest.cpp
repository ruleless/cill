#include "CoreTest.h"

#include "CillConfig.h"
#include "Buffer.h"
#include "StrBuffer.h"
#include "StrConvertor.h"

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

void CoreTest::testStrBuffer()
{
	core::ostrbuf osb;
	osb<<1;
	osb<<2;
	osb<<3;
	printf("osb:%s", osb.c_str());
	CPPUNIT_ASSERT(strcmp("123", osb.c_str()) == 0);
}

void CoreTest::testStrConvertor()
{
	for (int i = 999; i < 9999; ++i)
	{
		std::string s = core::val2str(i);
		char dest[MAX_BUF];
		__snprintf(dest, sizeof(dest), "%d", i);
		CPPUNIT_ASSERT(strcmp(dest, s.c_str()) == 0);
		
		CPPUNIT_ASSERT(core::str2value<int>(dest) == i);
	}	
}
