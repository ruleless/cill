#include "TimeStamp.h"

NAMESPACE_BEG(core)

//--------------------------------------------------------------------------
// TimeMethod
#ifdef USE_RDTSC
TimingMethod gTimingMethod = RDTSC_TIMING_METHOD;
#else // USE_RDTSC
static const TimingMethod DEFAULT_TIMING_METHOD = GET_TIME_OF_DAY_TIMING_METHOD;
TimingMethod gTimingMethod = DEFAULT_TIMING_METHOD;
#endif // USE_RDTSC

const char* getTimingMethodName()
{
	switch (gTimingMethod)
	{
		case NO_TIMING_METHOD:
			return "none";
		case RDTSC_TIMING_METHOD:
			return "rdtsc";
		case GET_TIME_OF_DAY_TIMING_METHOD:
			return "gettimeofday";
		default:
			return "Unknown";
	}
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// calcStampsPerSecond()
#ifdef unix
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

static uint64 calcStampsPerSecond_rdtsc()
{
	struct timeval tvBefore, tvSleep = {0, 500000},	tvAfter;
	uint64 stampBefore,	stampAfter;

	gettimeofday(&tvBefore, NULL);
	gettimeofday(&tvBefore, NULL);

	gettimeofday(&tvBefore, NULL);
	stampBefore = timestamp();

	select(0, NULL, NULL, NULL, &tvSleep);

	gettimeofday(&tvAfter, NULL);
	gettimeofday(&tvAfter, NULL);

	gettimeofday(&tvAfter, NULL);
	stampAfter = timestamp();

	uint64 microDelta = (tvAfter.tv_usec + 1000000 - tvBefore.tv_usec) % 1000000;
	uint64 stampDelta = stampAfter - stampBefore;

	return (stampDelta * 1000000ULL) / microDelta;
}

static uint64 calcStampsPerSecond_gettimeofday()
{
	return 1000000ULL;
}

static uint64 calcStampsPerSecond()
{
#ifdef USE_RDTSC
	return calcStampsPerSecond_rdtsc();
#else // USE_RDTSC
	if (gTimingMethod == RDTSC_TIMING_METHOD)
		return calcStampsPerSecond_rdtsc();
	else if (gTimingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return calcStampsPerSecond_gettimeofday();
	else
	{
		ErrorLn("calcStampsPerSecond:Unknown timing method: "<<getTimingMethodName());
		return 0;
	}
#endif // USE_RDTSC
}

#elif defined(_WIN32) // #ifdef unix

#include <windows.h>

#ifdef USE_RDTSC
static uint64 calcStampsPerSecond()
{	
	LARGE_INTEGER tvBefore, tvAfter;
	DWORD tvSleep = 500;
	uint64 stampBefore,	stampAfter;
	
	Sleep(100);
	
	QueryPerformanceCounter(&tvBefore);
	QueryPerformanceCounter(&tvBefore);

	QueryPerformanceCounter(&tvBefore);
	stampBefore = timestamp();

	Sleep(tvSleep);

	QueryPerformanceCounter(&tvAfter);
	QueryPerformanceCounter(&tvAfter);

	QueryPerformanceCounter(&tvAfter);
	stampAfter = timestamp();

	uint64 countDelta = tvAfter.QuadPart - tvBefore.QuadPart;
	uint64 stampDelta = stampAfter - stampBefore;

	LARGE_INTEGER	frequency;
	QueryPerformanceFrequency(&frequency);

	return (uint64)((stampDelta * uint64(frequency.QuadPart) ) / countDelta);
}
#else // USE_RDTSC
static uint64 calcStampsPerSecond()
{
	LARGE_INTEGER rate;
	QueryPerformanceFrequency(&rate);
	return rate.QuadPart;
}
#endif // USE_RDTSC
#endif // #ifdef unix
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// 每秒cpu所耗时间
uint64 stampsPerSecond()
{
	static uint64 _stampsPerSecondCache = calcStampsPerSecond();
	return _stampsPerSecondCache;
}

// 每秒cpu所耗时间 double版本
double stampsPerSecondD()
{
	static double stampsPerSecondCacheD = double(stampsPerSecond());
	return stampsPerSecondCacheD;
}

ulong getTickCount()
{
#if defined (__WIN32__) || defined(_WIN32) || defined(WIN32)
	return ::GetTickCount();
#elif defined(unix)
	struct timeval tv;
	struct timezone tz;
	gettimeofday(&tv , &tz);
	return (ulong)((tv.tv_sec & 0xfffff) * 1000 + tv.tv_usec / 1000);
#endif
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
double TimeStamp::toSeconds(uint64 stamps)
{
	return double(stamps) / stampsPerSecondD();
}

TimeStamp TimeStamp::fromSeconds(double seconds)
{
	return uint64(seconds * stampsPerSecondD());
}

double TimeStamp::inSeconds() const
{
	return toSeconds(stamp_);
}

void TimeStamp::setInSeconds(double seconds)
{
	stamp_ = fromSeconds(seconds);
}

TimeStamp TimeStamp::ageInStamps() const
{
	return timestamp() - stamp_;
}

double TimeStamp::ageInSeconds() const
{
	return toSeconds(this->ageInStamps());
}
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core
