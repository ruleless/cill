#ifndef __CILL_LOG_H__
#define __CILL_LOG_H__

#include <iostream>
#include <string>
#include <list>

#if PLATFORM == PLATFORM_WIN32
# include <windows.h>
# include <process.h>
#endif

//--------------------------------------------------------------------------
// 线程定义
#if PLATFORM == PLATFORM_WIN32
# ifndef THREAD_ID
#  define THREAD_ID                          HANDLE
#  define THREAD_SINGNAL                     HANDLE
#  define THREAD_SINGNAL_INIT(x)             x = CreateEvent(NULL, TRUE, FALSE, NULL)
#  define THREAD_SINGNAL_DELETE(x)           CloseHandle(x)
#  define THREAD_SINGNAL_SET(x)              SetEvent(x)
#  define THREAD_MUTEX                       CRITICAL_SECTION
#  define THREAD_MUTEX_INIT(x)               InitializeCriticalSection(&x)
#  define THREAD_MUTEX_DELETE(x)             DeleteCriticalSection(&x)
#  define THREAD_MUTEX_LOCK(x)               EnterCriticalSection(&x)
#  define THREAD_MUTEX_UNLOCK(x)             LeaveCriticalSection(&x)
# endif
#else
# ifndef THREAD_ID
#  define THREAD_ID                          pthread_t
#  define THREAD_SINGNAL                     pthread_cond_t
#  define THREAD_SINGNAL_INIT(x)             pthread_cond_init(&x, NULL)
#  define THREAD_SINGNAL_DELETE(x)           pthread_cond_destroy(&x)
#  define THREAD_SINGNAL_SET(x)              pthread_cond_signal(&x);
#  define THREAD_MUTEX                       pthread_mutex_t
#  define THREAD_MUTEX_INIT(x)               pthread_mutex_init (&x, NULL)
#  define THREAD_MUTEX_DELETE(x)             pthread_mutex_destroy(&x)
#  define THREAD_MUTEX_LOCK(x)               pthread_mutex_lock(&x)
#  define THREAD_MUTEX_UNLOCK(x)             pthread_mutex_unlock(&x)
# endif
#endif
//--------------------------------------------------------------------------

enum LogLevel // 消息级别
{
    DebugLog      = 0x01,
    InfoLog       = 0x02,
    WarningLog    = 0x04,
    ErrorLog      = 0x08,
    EmphasisLog   = 0x10,

    AllLog = DebugLog|InfoLog|WarningLog|ErrorLog|EmphasisLog,
};

NAMESPACE_BEG(cill)

//--------------------------------------------------------------------------
// 日志输出需继承该口
class ILogWriter
{
  public:
    ILogWriter() : mLevel(AllLog), mHasTime(true) {}

    virtual ~ILogWriter() {}

    bool hasTime() const
    {
        return mHasTime;
    }

    void hasTime(bool b)
    {
        mHasTime = b;
    }

    void setLevel(int lvl)
    {
        mLevel = lvl;
    }

    int getLevel() const
    {
        return mLevel;
    }

    virtual void onLog(const char* msg, const char* time, LogLevel level) {}

  protected:
    int mLevel;
    bool mHasTime;
};
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Log
class Log
{
  public:
    Log();
    ~Log();

    bool initialise(int level, bool hasTime);
    void finalise();

    void _flushOutlist();

    int getLogLevel() const;
    int setLogLevel(int level);

    bool hasTime(bool b);

    bool setLimitFrequency(bool limitFrequency);
    bool hasLimitFrequency() const;

    void registerLog(Listener* sink);
    void unregisterLog(Listener* sink);

    void output(const char *msg, LogLevel level);

#if PLATFORM == PLATFORM_WIN32
    static unsigned __stdcall _logProc(void *arg);
#else
    static void* _logProc(void* arg);
#endif

  protected:
    struct _MSG
    {
        std::string msg;
        time_t time;
        TraceLevel level;
    };

    typedef std::list<_MSG> MsgList;
    typedef std::list<ILogWriter *> WriterList;

    WriterList mLogWriter;
    int mLevel;
    bool mHasTime;
    bool mLimitFrequency;

    volatile bool mbExit;
    bool mInited;
    THREAD_ID mTid;
    THREAD_MUTEX mMutex;
    THREAD_SINGNAL mCond;
    MsgList mMsgs1;
    MsgList mMsgs2;
    MsgList *mInlist;
    MsgList *mOutlist;
};

extern Log *gLogger;
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Trace CORE_API
CORE_API void createTrace(int level = levelAll, bool hasTime = true);
CORE_API void closeTrace();
CORE_API int setTraceLevel(int level);
CORE_API int getTraceLevel();
CORE_API void setTraceHasTime(bool b);
CORE_API bool setTraceHasLimitFrequency(bool limitFrequency);
CORE_API bool hasLimitFrequency();
CORE_API void registerTrace(STrace::Listener* sink);
CORE_API void unregisterTrace(STrace::Listener* sink);
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Set Output Target
CORE_API STrace::Listener* output2Console(int level = levelAll, bool hasTime = true);
CORE_API STrace::Listener* output2Html(const char* filename, int level = levelAll, bool hasTime = true);
CORE_API STrace::Listener* output2File(const tchar* filename, int level = levelAll, bool hasTime = true);

#if PLATFORM == PLATFORM_WIN32
CORE_API void addTraceToRichEdit(void* hWndRichEdit, const char* msg, TraceLevel level);
CORE_API void dispatch2RichEdit(STrace::Listener* tl);
CORE_API extern STrace::Listener* output2RichEdit(void* hwnd, int level = levelAll, bool hasTime = true);
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// output trace info
CORE_API void output(const char* msg, TraceLevel level = levelTrace);
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core

//--------------------------------------------------------------------------
// Macro Define
#ifdef LIMIT_FREQUENCY // 控制每个输出点的最快频率是1秒
#define LIM_FREQ(e) \
    static core::ulong s_lastTraceTime = core::getTickCount();              \
    if (!core::hasLimitFrequency() || (core::getTickCount() - s_lastTraceTime) > 1000) \
    { s_lastTraceTime = core::getTickCount(); e}
#else // #ifdef LIMIT_FREQUENCY
#define LIM_FREQ(e) e
#endif

#define logInfo(x) { if(core::getTraceLevel() & core::levelInfo) { LIM_FREQ(core::ostrbuf __osb; __osb<<x; core::output(__osb.c_str(), core::levelInfo);) }}
#define logTrace(x) { if(core::getTraceLevel() & core::levelTrace) { LIM_FREQ(core::ostrbuf __osb; __osb<<x; core::output(__osb.c_str(), core::levelTrace);) }}
#define logWarning(x) { if(core::getTraceLevel() & core::levelWarning) { LIM_FREQ(core::ostrbuf __osb; __osb<<"WARNING: "<<x; core::output(__osb.c_str(), core::levelWarning);) }}
#define logError(x) { if(core::getTraceLevel() & core::levelError) { LIM_FREQ(core::ostrbuf __osb; __osb<<"ERROR: "<<x; core::output(__osb.c_str(), core::levelError);) }}
#define logEmphasis(x) { if(core::getTraceLevel() & core::levelEmphasis) { LIM_FREQ(core::ostrbuf __osb; __osb<<x; core::output(__osb.c_str(), core::levelEmphasis);) }}

#define logInfoLn(x) { if(core::getTraceLevel() & core::levelInfo) { LIM_FREQ(core::ostrbuf __osb; __osb<<x<<"\n"; core::output(__osb.c_str(), core::levelInfo);) }}
#define logTraceLn(x) { if(core::getTraceLevel() & core::levelTrace) { LIM_FREQ(core::ostrbuf __osb; __osb<<x<<"\n"; core::output(__osb.c_str(), core::levelTrace);) }}
#define logWarningLn(x) { if(core::getTraceLevel() & core::levelWarning) { LIM_FREQ(core::ostrbuf __osb; __osb<<"WARNING: "<<x<<"\n"; core::output(__osb.c_str(), core::levelWarning);) }}
#define logErrorLn(x) { if(core::getTraceLevel() & core::levelError) { LIM_FREQ(core::ostrbuf __osb; __osb<<"ERROR: "<<x<<"\n"; core::output(__osb.c_str(), core::levelError);) }}
#define logEmphasisLn(x) { if(core::getTraceLevel() & core::levelEmphasis) { LIM_FREQ(core::ostrbuf __osb; __osb<<x<<"\n"; core::output(__osb.c_str(), core::levelEmphasis);) }}

#define logInfoOnce(x) { static bool f=true; if(f) { f=!f; Info(x); }}
#define logTraceOnce(x) { static bool f=true; if(f) { f=!f; STrace(x); }}
#define logWarningOnce(x) { static bool f=true; if(f) { f=!f; Warning(x); }}
#define logErrorOnce(x) { static bool f=true; if(f) { f=!f; Error(x); }}
#define logEmphasisOnce(x) { static bool f=true; if(f) { f=!f; Emphasis(x); }}

#define logInfoOnceLn(x) { static bool f=true; if(f) { f=!f; InfoLn(x); }}
#define logTraceOnceLn(x) { static bool f=true; if(f) { f=!f; TraceLn(x); }}
#define logWarningOnceLn(x) { static bool f=true; if(f) { f=!f; WarningLn(x); }}
#define logErrorOnceLn(x) { static bool f=true; if(f) { f=!f; ErrorLn(x); }}
#define logEmphasisOnceLn(x) { static bool f=true; if(f) { f=!f; EmphasisLn(x); }}

#define logVerify(x) { if(!(x)) { LIM_FREQ(core::ostrbuf __osb; __osb<<"VERIFY: "<<#x<<"\n"; core::output(__osb.c_str(), core::levelError);) }}
//--------------------------------------------------------------------------

#else // #ifndef DISABLE_TRACE

//--------------------------------------------------------------------------
// If We don't support trace
#define logInfo(x)
#define logTrace(x)
#define logWarning(x)
#define logError(x)
#define logEmphasis(x)

#define logInfoLn(x)
#define logTraceLn(x)
#define logWarningLn(x)
#define logErrorLn(x)
#define logEmphasisLn(x)

#define logInfoOnce(x)
#define logTraceOnce(x)
#define logWarningOnce(x)
#define logErrorOnce(x)
#define logEmphasisOnce(x)

#define logInfoOnceLn(x)
#define logTraceOnceLn(x)
#define logWarningOnceLn(x)
#define logErrorOnceLn(x)
#define logEmphasisOnceLn(x)

#define logVerify(x)        (x)
//--------------------------------------------------------------------------

#endif // __CILL_LOG_H__
