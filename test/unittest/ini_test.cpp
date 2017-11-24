#include "ini_test.h"
#include "core/ini.h"
#include "core/time_stamp.h"

CPPUNIT_TEST_SUITE_REGISTRATION(IniTest);

using namespace core;

IniTest::IniTest()
{}

IniTest::~IniTest()
{}
    
void IniTest::setUp()
{}
    
void IniTest::tearDown()
{}

void IniTest::simpleRead()
{
    Ini ini("test.ini");
    ini.setInt("", "liuy", 1);
    ini.setInt("", "liuy1", 2000);
    ini.setInt("s", "liuy1", 2000);
    ini.setInt("s", "liuy2", 2000);
}

void IniTest::testIniWrite()
{
    uint64 begT = getTickCount();
    Ini ini = Ini("utest.ini");
    uint64 endT = getTickCount();
    printf("testIniWrite! ini read time:%"PRI64"ms\n", (endT-begT));

    begT = getTickCount();
    static const int s_testTime = 500;
    for (int i = 0; i < s_testTime; ++i)
    {
        char sec[MAX_BUF];
        __snprintf(sec, sizeof(sec), "sec%d", i);
        for (int i = 0; i < s_testTime; ++i)
        {
            char key[MAX_BUF];
            __snprintf(key, sizeof(key), "key%d", i);
            ini.setInt(sec, key, i);
        }
    }
    endT = getTickCount();
    printf("testIniWrite! ini set time:%"PRI64"ms\n", (endT-begT));
}

void IniTest::testIniRead()
{
    uint64 begT = getTickCount();
    Ini ini = Ini("utest.ini");
    uint64 endT = getTickCount();
    printf("testIniRead! ini read time:%"PRI64"ms\n", (endT-begT));

    begT = getTickCount();
    static const int s_testTime = 500;
    for (int i = 0; i < s_testTime; ++i)
    {
        char sec[MAX_BUF];
        __snprintf(sec, sizeof(sec), "sec%d", i);
        for (int i = 0; i < s_testTime; ++i)
        {
            char key[MAX_BUF];
            __snprintf(key, sizeof(key), "key%d", i);
            CPPUNIT_ASSERT(ini.getInt(sec, key) == i);
        }
    }
    endT = getTickCount();
    printf("testIniRead! ini get time:%"PRI64"ms\n", (endT-begT));
}
