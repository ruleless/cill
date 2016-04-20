#ifndef __CORE_TIMESTAMP_H__
#define __CORE_TIMESTAMP_H__

#include "CillConfig.h"

#ifdef unix
#define USE_RDTSC
#else
//#define USE_RDTSC
#endif // unix

NAMESPACE_BEG(core)

//--------------------------------------------------------------------------
// TimeMethod
enum TimingMethod
{
	RDTSC_TIMING_METHOD, // ��CPU�ϵ�������������ʱ��������,���뼶�ļ�ʱ����
	GET_TIME_OF_DAY_TIMING_METHOD,
	NO_TIMING_METHOD,
};

extern TimingMethod gTimingMethod;
extern const char* getTimingMethodName();
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// timestamp()
#ifdef unix
inline uint64 timestamp_rdtsc()
{
	uint32 rethi, retlo;
	__asm__ __volatile__ (
		"rdtsc":
		"=d"    (rethi),
		"=a"    (retlo)
						  );
	return uint64(rethi) << 32 | retlo; 
}

// ʹ�� gettimeofday. ���Դ�ű���RDTSC20��-600����
// ���⣬��һ�����⣺2.4�ں��£��������ε���gettimeofday�Ŀ��ܷ���һ������ǵ����ߡ�
#include <sys/time.h>

inline uint64 timestamp_gettimeofday()
{
	timeval tv;
	gettimeofday(&tv, NULL);
	return 1000000ULL * uint64( tv.tv_sec ) + uint64( tv.tv_usec );
}

inline uint64 timestamp()
{
#ifdef USE_RDTSC
	return timestamp_rdtsc();
#else // #ifdef USE_RDTSC
	if (gTimingMethod == RDTSC_TIMING_METHOD)
		return timestamp_rdtsc();
	else if (gTimingMethod == GET_TIME_OF_DAY_TIMING_METHOD)
		return timestamp_gettimeofday();
	else
		return 0;
#endif // #ifdef USE_RDTSC
}
#elif defined(_WIN32) // #ifdef unix
#ifdef USE_RDTSC
#pragma warning (push)
#pragma warning (disable: 4035)
inline uint64 timestamp()
{
	__asm rdtsc
}
#pragma warning (pop)
#else // #ifdef USE_RDTSC
#include <windows.h>

inline uint64 timestamp()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return counter.QuadPart;
}
#endif // #ifdef USE_RDTSC

#else // #ifdef unix
#    error Unsupported platform!
#endif // #ifdef unix
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// getTimeStamp()
#if defined (unix)

#if defined (__i386__)
inline uint64 getTimeStamp()
{
	uint64 x;
	__asm__ volatile("rdtsc":"=A"(x));
	return x;
}
#elif defined (__x86_64__) // #if defined (__i386__)
inline uint64 getTimeStamp()
{
	unsigned hi,lo;
	__asm__ volatile("rdtsc":"=a"(lo),"=d"(hi));
	return ((uint64_t)lo)|(((uint64_t)hi)<<32);
}
#else
#    error Unsupported platform!
#endif // #if defined (__i386__)

#elif defined (__WIN32__) || defined(_WIN32) || defined(WIN32) // #if defined (unix)
inline uint64 getTimeStamp()
{
	__asm
	{
		_emit 0x0F;
		_emit 0x31;
	}
}
#else // #if defined (unix)
#    error Unsupported platform!
#endif // #if defined (unix)
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
extern uint64 stampsPerSecond();
extern double stampsPerSecondD();

extern ulong getTickCount();

inline double stampsToSeconds( uint64 stamps )
{
	return double(stamps)/stampsPerSecondD();
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// TimeStamp
class TimeStamp
{
  public:
	TimeStamp(uint64 stamps = 0) : stamp_(stamps) {}

	operator uint64 &() { return stamp_; }
	operator uint64() const { return stamp_; }
	
	uint64 stamp() { return stamp_; }

	double inSeconds() const;
	void setInSeconds(double seconds);

	TimeStamp ageInStamps() const;
	double ageInSeconds() const;

	static double toSeconds( uint64 stamps );
	static TimeStamp fromSeconds( double seconds );

	uint64 stamp_;
};
//--------------------------------------------------------------------------

NAMESPACE_END // namespace core

#endif // __CORE_TIMESTAMP_H__
