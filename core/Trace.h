#ifndef __CORE_TRACE_H__
#define __CORE_TRACE_H__

#ifndef DISABLE_TRACE

#include <iostream>
#include <string>
#include <list>

#include "CillConfig.h"
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

class STrace
{
  public:
	class Listener
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
// Trace API
extern void createTrace(int level = levelAll, bool hasTime = true);
extern void closeTrace();
extern int setTraceLevel(int level);
extern int getTraceLevel();
extern void setTraceHasTime(bool b);
extern bool setTraceHasLimitFrequency(bool limitFrequency);
extern bool hasLimitFrequency();
extern void registerTrace(STrace::Listener* sink);
extern void unregisterTrace(STrace::Listener* sink);
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Set Output Target
extern STrace::Listener* output2Console(int level = levelAll, bool hasTime = true);
extern STrace::Listener* output2Html(const char* filename, int level = levelAll, bool hasTime = true);

#if PLATFORM == PLATFORM_WIN32
extern void addTraceToRichEdit(void* hWndRichEdit, const char* msg, TraceLevel level);
extern void dispatch2RichEdit(STrace::Listener* tl);
extern STrace::Listener* output2RichEdit(void* hwnd, int level = levelAll, bool hasTime = true);
#endif
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// output trace info
extern void output(const char* msg, TraceLevel level = levelTrace);
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core

//--------------------------------------------------------------------------
// Macro Define
#ifdef LIMIT_FREQUENCY // 控制每个输出点的最快频率是1秒
#define LIMIT_FREQ(e) \
	static ulong s_lastTraceTime = core::getTickCount();				\
	if (!core::hasLimitFrequency() || (core::getTickCount() - s_lastTraceTime) > 1000) \
	{ s_lastTraceTime = core::getTickCount(); e}
#else // #ifdef LIMIT_FREQUENCY
#define LIMIT_FREQ(e)	e
#endif

#define Info(x) { if(core::getTraceLevel() & core::levelInfo) { LIMIT_FREQ(core::ostrbuf osb; osb<<x; core::output(osb.c_str(), core::levelInfo);) }}
#define Trace(x) { if(core::getTraceLevel() & core::levelTrace)	{ LIMIT_FREQ(core::ostrbuf osb; osb<<x; core::output(osb.c_str(), core::levelTrace);) }}
#define Warning(x) { if(core::getTraceLevel() & core::levelWarning)	{ LIMIT_FREQ(core::ostrbuf osb; osb<<"WARNING: "<<x; core::output(osb.c_str(), core::levelWarning);) }}
#define Error(x) { if(core::getTraceLevel() & core::levelError) { LIMIT_FREQ(core::ostrbuf osb; osb<<"ERROR: "<<x; core::output(osb.c_str(), core::levelError);) }}
#define Emphasis(x) { if(core::getTraceLevel() & core::levelEmphasis) { LIMIT_FREQ(core::ostrbuf osb; osb<<x; core::output(osb.c_str(), core::levelEmphasis);) }}

#define InfoLn(x) { if(core::getTraceLevel() & core::levelInfo) { LIMIT_FREQ(core::ostrbuf osb; osb<<x<<"\n"; core::output(osb.c_str(), core::levelInfo);) }}
#define TraceLn(x) { if(core::getTraceLevel() & core::levelTrace) { LIMIT_FREQ(core::ostrbuf osb; osb<<x<<"\n"; core::output(osb.c_str(), core::levelTrace);) }}
#define WarningLn(x) { if(core::getTraceLevel() & core::levelWarning) { LIMIT_FREQ(core::ostrbuf osb; osb<<"WARNING: "<<x<<"\n"; core::output(osb.c_str(), core::levelWarning);) }}
#define ErrorLn(x) { if(core::getTraceLevel() & core::levelError) { LIMIT_FREQ(core::ostrbuf osb; osb<<"ERROR: "<<x<<"\n"; core::output(osb.c_str(), core::levelError);) }}
#define EmphasisLn(x) { if(core::getTraceLevel() & core::levelEmphasis)	{ LIMIT_FREQ(core::ostrbuf osb; osb<<x<<"\n"; core::output(osb.c_str(), core::levelEmphasis);) }}

#define InfoOnce(x) { static bool f=true; if(f) { f=!f; Info(x); }}
#define TraceOnce(x) { static bool f=true; if(f) { f=!f; STrace(x); }}
#define WarningOnce(x) { static bool f=true; if(f) { f=!f; Warning(x); }}
#define ErrorOnce(x) { static bool f=true; if(f) { f=!f; Error(x); }}
#define EmphasisOnce(x) { static bool f=true; if(f) { f=!f; Emphasis(x); }}

#define InfoOnceLn(x) { static bool f=true; if(f) { f=!f; InfoLn(x); }}
#define TraceOnceLn(x) { static bool f=true; if(f) { f=!f; TraceLn(x); }}
#define WarningOnceLn(x) { static bool f=true; if(f) { f=!f; WarningLn(x); }}
#define ErrorOnceLn(x) { static bool f=true; if(f) { f=!f; ErrorLn(x); }}
#define EmphasisOnceLn(x) { static bool f=true; if(f) { f=!f; EmphasisLn(x); }}

#define Verify(x) { if(!(x)) { LIMIT_FREQ(core::ostrbuf osb; osb<<"VERIFY: "<<#x<<"\n"; core::output(osb.c_str(), core::levelError);) }}
//--------------------------------------------------------------------------

#else // #ifndef DISABLE_TRACE

//--------------------------------------------------------------------------
// If We don't support trace
#define Info(x)
#define Trace(x)
#define Warning(x)
#define Error(x)
#define Emphasis(x)

#define InfoLn(x)
#define TraceLn(x)
#define WarningLn(x)
#define ErrorLn(x)
#define EmphasisLn(x)

#define InfoOnce(x)
#define TraceOnce(x)
#define WarningOnce(x)
#define ErrorOnce(x)
#define EmphasisOnce(x)

#define InfoOnceLn(x)
#define TraceOnceLn(x)
#define WarningOnceLn(x)
#define ErrorOnceLn(x)
#define EmphasisOnceLn(x)

#define Verify(x)		(x)
//--------------------------------------------------------------------------

#endif // #ifndef DISABLE_TRACE

#endif // __CORE_TRACE_H__
