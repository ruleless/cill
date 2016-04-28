#ifndef __CORE_THREADGUARD_H__
#define __CORE_THREADGUARD_H__

#include "CoreBase.h"
#include "ThreadMutex.h"

NAMESPACE_BEG(core)

class CORE_CLASS ThreadGuard
{
public:
	explicit ThreadGuard(ThreadMutex* mutexPtr) : mpMutex(mutexPtr)
	{
		mpMutex->lockMutex();
	}

	virtual ~ThreadGuard(void) 
	{ 
		mpMutex->unlockMutex();
	}	
protected:
	ThreadMutex* mpMutex;
};

NAMESPACE_END // namespace core

#endif // __CORE_THREADGUARD_H__
