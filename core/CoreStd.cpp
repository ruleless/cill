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

NAMESPACE_END // namespace core
