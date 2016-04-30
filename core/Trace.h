#ifndef __CORE_TRACE_H__
#define __CORE_TRACE_H__

#ifndef DISABLE_TRACE

#include <iostream>
#include <string>
#include <list>

#include "CoreBase.h"
#include "TimeStamp.h"
#include "StrBuffer.h"

#if PLATFORM == PLATFORM_WIN32
#	include <windows.h>
#	include <process.h>
#endif

NAMESPACE_BEG(core)

//--------------------------------------------------------------------------
enum TraceLevel // 消息级别
{
	levelInfo		= 0x01,
	levelTrace		= 0x02,
	levelWarning	= 0x04,
	levelError		= 0x08,
	levelEmphasis	= 0x10,

	levelAll = levelInfo|levelTrace|levelWarning|levelError|levelEmphasis,
};

class CORE_CLASS STrace
{
  public:
	class CORE_CLASS Listener
	{
	  protected:
		int mLevel;
		bool mHasTime;
	  public:
		Listener() : mLevel(levelAll), mHasTime(true) {}

		virtual ~Listener() {}

		bool hasTime() const
		{
			return mHasTime;
		}

		void hasTime(bool b)
		{
			mHasTime = b;
		}

		void setTraceLevel(int lvl)
		{
			mLevel = lvl;
		}

		int getTraceLevel() const
		{
			return mLevel;
		}

		virtual void onTrace(const char* msg, const char* time, TraceLevel level) {}
	};
  protected:
	struct _MSG
	{
		std::string msg;
		time_t time;
		TraceLevel level;
	};

	typedef std::list<_MSG> MsgList;
	typedef std::list<Listener *> ListenerList;

	ListenerList mSinks;
	int	mLevel;
	bool mHasTime;
	bool mLimitFrequency;

	volatile bool mbExit;
	bool mInited;
	THREAD_ID mTid;
	THREAD_MUTEX mMutex;
	THREAD_SINGNAL mCond;
	MsgList	mMsgs1;
	MsgList	mMsgs2;
	MsgList *mInlist;
	MsgList *mOutlist;
  public:
	STrace();
	~STrace();

	bool initialise(int level, bool hasTime);
	void finalise();

	void _flushOutlist();

	int getTraceLevel() const;
	int setTraceLevel(int level);

	bool hasTime(bool b);

	bool setTraceLimitFrequency(bool limitFrequency);
	bool hasLimitFrequency() const;

	void registerTrace(Listener* sink);
	void unregisterTrace(Listener* sink);

	void output(const char* msg, TraceLevel level);

#if PLATFORM == PLATFORM_WIN32
	static unsigned __stdcall _traceProc(void *arg);
#else
	static void* _traceProc(void* arg);
#endif
};

extern STrace* gTrace;
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
	static core::ulong s_lastTraceTime = core::getTickCount();				\
	if (!core::hasLimitFrequency() || (core::getTickCount() - s_lastTraceTime) > 1000) \
	{ s_lastTraceTime = core::getTickCount(); e}
#else // #ifdef LIMIT_FREQUENCY
#define LIM_FREQ(e)	e
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

#define logVerify(x)		(x)
//--------------------------------------------------------------------------

#endif // #ifndef DISABLE_TRACE

#endif // __CORE_TRACE_H__
