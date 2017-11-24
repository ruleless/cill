#include "core_test.h"

#include "core/corebase.h"
#include "core/corestd.h"
#include "core/buffer.h"
#include "core/memorystream.h"
#include "core/str_buffer.h"
#include "core/str_convertor.h"
#include "core/str_helper.h"
#include "core/objectpool.h"
#include "core/smart_pointer.h"
#include "core/threadpool.h"
#include "core/time_stamp.h"

CPPUNIT_TEST_SUITE_REGISTRATION(CoreTest);

using namespace core;

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
#if PLATFORM == PLATFORM_WIN32
    for (int i = 0; i < 100; ++i)
#else
    for (int i = 0; i < 100000; ++i)
#endif
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

void CoreTest::testMemoryStream()
{
    srand(time(NULL));

    core::MemoryStream *pStream = core::MemoryStream::ObjPool().createObject();
    CPPUNIT_ASSERT(pStream);

    std::vector<int> ia;
    int sz = 0;
#if PLATFORM == PLATFORM_WIN32
    for (int i = 0; i < 100; ++i)
#else
    for (int i = 0; i < 100000; ++i)
#endif
    {
        int n = rand() % 65535;
        ia.push_back(n);
        sz += sizeof(n);
        *pStream<<n;
    }
    CPPUNIT_ASSERT(pStream->length() == sz);

    std::vector<int>::const_iterator it = ia.begin();
    for (; it != ia.end(); ++it)
    {
        int n = 0;
        *pStream>>n;
        CPPUNIT_ASSERT(n == *it);
    }

    CPPUNIT_ASSERT(pStream->length() == 0);
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

void CoreTest::testTrace()
{
    InfoPrint("I am Info");
    DebugPrint("I am Trace");
    WarningPrint("I am Warning");
    ErrorPrint("I am Error");
    EmphasisPrint("I am Emphasis");
}

void CoreTest::testTimeStamp()
{
    core::uint64 exstamp = 0;
    for (int i = 0; i < 10; ++i)
    {
        core::uint64 tmstamp = core::timestamp();
        printf("idx:%d timestamp:%"PRIu64"  delta:%"PRIu64"\n",
               i, tmstamp, exstamp ? tmstamp-exstamp : 0);
        exstamp = tmstamp;
    }

    core::ulong ms = 250;
    core::ulong curtick = core::getTickCount();
    printf("curtick:%lu\n", curtick);
    core::sleepms(ms);
    printf("tick after sleep %lu ms:%lu delta:%lu\n",
           ms, core::getTickCount(), core::getTickCount()-curtick);
}

//--------------------------------------------------------------------------
class PoolTest : public core::PoolObject
{
  public:
    static core::ObjectPool<PoolTest>& ObjPool()
    {
        static core::ObjectPool<PoolTest> s_pool("PoolTest");
        return s_pool;
    }

    virtual void onReclaimObject()
    {
        // printf("onReclaimObject():0x%"PRIx64"\n", this);
    }

    virtual void onEabledPoolObject()
    {
        // printf("onEabledPoolObject():0x%"PRIx64"\n", this);
    }

    virtual size_t getPoolObjectBytes()
    {
        return 0;
    }

    PoolTest() {}
    virtual ~PoolTest()
    {
        // printf("~PoolTest():0x%"PRIx64"\n", this);
    }
};

void CoreTest::testPoolObject()
{
    std::vector<PoolTest *> tests;
#if PLATFORM == PLATFORM_WIN32
    for (int i = 0; i < 500; ++i)
#else
    for (int i = 0; i < 500000; ++i)
#endif
    {
        PoolTest *o = NULL;
        CPPUNIT_ASSERT(o = PoolTest::ObjPool().createObject());
        tests.push_back(o);
    }

    std::vector<PoolTest *>::iterator it = tests.begin();
    for (; it != tests.end(); ++it)
    {
        PoolTest::ObjPool().reclaimObject(*it);
    }
}
//--------------------------------------------------------------------------

void CoreTest::testStrHelper()
{
    std::string str = "  teststr  ";
    core::ltrim(str);
    CPPUNIT_ASSERT(strcmp(str.c_str(), "teststr  ") == 0);
    core::rtrim(str);
    CPPUNIT_ASSERT(strcmp(str.c_str(), "teststr") == 0);

    str = "abcDeFGH";
    std::string retStr = core::stringToLower(str);
    CPPUNIT_ASSERT(strcmp(retStr.c_str(), "abcdefgh") == 0);
    retStr = core::stringToUpper(str);
    CPPUNIT_ASSERT(strcmp(retStr.c_str(), "ABCDEFGH") == 0);

    CPPUNIT_ASSERT(core::strIHasSuffix("abcd", "cd"));
    CPPUNIT_ASSERT(core::strIHasSuffix("abcd", "CD"));
    CPPUNIT_ASSERT(core::strIHasSuffix("abcd", "Cd"));
    CPPUNIT_ASSERT(!core::strIHasSuffix("abcdef", "Cd"));
    CPPUNIT_ASSERT(core::strIHasSuffix("abcd.txt", ".TXT"));
}

//--------------------------------------------------------------------------
class TestClass : public core::RefCountable
{
  public:
    TestClass() : core::RefCountable()
    {
        DebugPrint("New a TestClass. addr:%p", this);
    }

    virtual ~TestClass()
    {
        DebugPrint("Delete a Test Class. addr:%p", this);
    }
};

typedef core::SmartPointer<TestClass> TestClassPtr;

void CoreTest::testSmartPointer()
{
    TestClassPtr t(new TestClass);
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
class TestTask : public core::TPTask
{
  public:
    TestTask()
    {
        DebugPrint("TestTask created!");
    }

    virtual ~TestTask()
    {
        DebugPrint("TestTask deleting!");
    }

    virtual bool process()
    {
        DebugPrint("process TestTask!");
        return true;
    }
};

void CoreTest::testThreadPool()
{
    new core::ThreadPool();

    core::ThreadPool::getSingletonPtr()->createThreadPool(2, 4, 8);
    for (int i = 0; i < 8; ++i)
    {
        CPPUNIT_ASSERT(core::ThreadPool::getSingletonPtr()->addTask(new TestTask()));
    }

    core::sleepms(200);

    core::ThreadPool::getSingletonPtr()->finalise();

    delete core::ThreadPool::getSingletonPtr();
}
//--------------------------------------------------------------------------
