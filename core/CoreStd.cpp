#include "CoreBase.h"

NAMESPACE_BEG(core)

void sleepms(ulong ms)
{
#if PLATFORM == PLATFORM_WIN32
	::Sleep(ms);
#else
	struct timeval tval;
	tval.tv_sec= ms / 1000;
	tval.tv_usec = (ms * 1000) % 1000000;
	select(0, NULL, NULL, NULL, &tval);
#endif
}

char* coreStrError(int ierrorno = 0)
{
#if PLATFORM == PLATFORM_WIN32
	if(ierrorno == 0)
		ierrorno = GetLastError();

	static char lpMsgBuf[256] = {0};
	
	__snprintf(lpMsgBuf, 256, "errorno=%d",  ierrorno);
	return lpMsgBuf;
#else
	if(ierrorno != 0)
		return strerror(ierrorno);
	return strerror(errno);
#endif
}

int coreLastError()
{
#if PLATFORM == PLATFORM_WIN32
	return GetLastError();
#else
	return errno;
#endif
}

NAMESPACE_END // namespace core
