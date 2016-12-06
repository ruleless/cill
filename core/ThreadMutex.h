#ifndef __CORE_THREADMUTEX_H__
#define __CORE_THREADMUTEX_H__
    
#include "CoreBase.h"

NAMESPACE_BEG(core)

class CORE_CLASS ThreadMutex 
{
public:
    ThreadMutex(void)
    {
        THREAD_MUTEX_INIT(mMutex);
    }

    virtual ~ThreadMutex(void) 
    { 
        THREAD_MUTEX_DELETE(mMutex);
    }   
    
    void lockMutex(void)
    {
        THREAD_MUTEX_LOCK(mMutex);
    }

    void unlockMutex(void)
    {
        THREAD_MUTEX_UNLOCK(mMutex);
    }
protected:
    THREAD_MUTEX mMutex;
};

NAMESPACE_END // namespace core

#endif // __CORE_THREADMUTEX_H__
