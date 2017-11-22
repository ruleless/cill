#include "threadpool.h"
#include "corestd.h"
#include "str_buffer.h"

//--------------------------------------------------------------------------
NAMESPACE_BEG(core)

// TPThread
THREAD_ID TPThread::createThread(void)
{
#if PLATFORM == PLATFORM_WIN32
    mTid = (THREAD_ID)_beginthreadex(NULL, 0, &TPThread::threadFunc, (void*)this, NULL, 0);
#else
    if(pthread_create(&mTid, NULL, TPThread::threadFunc, (void*)this)!= 0)
    {
        ErrorPrint("createThread error!");
    }
#endif
    return mTid;
}

bool TPThread::join(void)
{
#if PLATFORM == PLATFORM_WIN32
    int i = 0;

    while(true)
    {
        ++i;
        DWORD dw = WaitForSingleObject(id(), 3000);

        switch (dw)
        {
        case WAIT_OBJECT_0:
            return true;
        case WAIT_TIMEOUT:
            if(i > 20)
            {
                ErrorPrint("TPThread::join() can't join thread(%p)", this);
                return false;
            }
            else
            {
                WarningPrint("TPThread::join() waiting for thread(%p), try %d times", this, i);
            }
            break;
        case WAIT_FAILED:
        default:
            ErrorPrint("TPThread::join() can't join thread(%p)", this);
            return false;
        };
    }
#else
    void* status;
    if(pthread_join(id(), &status))
    {
        ErrorPrint("TPThread::join() can't join thread(%p)", this);
        return false;
    }
#endif

    return true;
}

void TPThread::onTaskCompleted(void)
{
    mpThreadPool->addFiniTask(mpCurrTask);
    mpCurrTask = NULL;
    mpThreadPool->addFreeThread(this);
}

bool TPThread::onWaitCondSignal(void)
{
#if PLATFORM == PLATFORM_WIN32
    if(mThreadWaitSecond <= 0)
    {
        mState = ThreadState_Sleep;
        WaitForSingleObject(mCond, INFINITE);
        ResetEvent(mCond);
    }
    else
    {
        mState = ThreadState_Sleep;
        uint32 ret = WaitForSingleObject(mCond, mThreadWaitSecond * 1000);
        ResetEvent(mCond);

        // 如果是因为超时了， 说明这个线程很久没有被用到， 我们应该注销这个线程。
        // 通知ThreadPool注销自己
        if (ret == WAIT_TIMEOUT)
        {
            mpThreadPool->removeHangThread(this);
            return false;
        }
        else if(ret != WAIT_OBJECT_0)
        {
            ErrorPrint("TPThread::onWaitCondSignal() WaitForSingleObject is error, ret=%d", ret);
        }
    }
#else
    if(mThreadWaitSecond <= 0)
    {
        lock();
        mState = ThreadState_Sleep;
        pthread_cond_wait(&mCond, &mMutex);
        unlock();
    }
    else
    {
        struct timeval now;
        struct timespec timeout;
        gettimeofday(&now, NULL);
        timeout.tv_sec = now.tv_sec + mThreadWaitSecond;
        timeout.tv_nsec = now.tv_usec * 1000;

        lock();
        mState = ThreadState_Sleep;
        int ret = pthread_cond_timedwait(&mCond, &mMutex, &timeout);
        unlock();

        // 如果是因为超时了， 说明这个线程很久没有被用到， 我们应该注销这个线程。
        if (ret == ETIMEDOUT)
        {
            // 通知ThreadPool注销自己
            mpThreadPool->removeHangThread(this);
            return false;
        }
        else if(ret != 0)
        {
            ErrorPrint("TPThread::onWaitCondSignal: pthread_cond_timedwait is error, %s", coreStrError());
        }
    }
#endif
    return true;
}

TPTask* TPThread::tryGetTask(void)
{
    return mpThreadPool->popbufferTask();
}

#if PLATFORM == PLATFORM_WIN32
unsigned __stdcall TPThread::threadFunc(void *arg)
#else
void* TPThread::threadFunc(void* arg)
#endif
{
    TPThread *tptd = static_cast<TPThread *>(arg);
    ThreadPool *pThreadPool = tptd->threadPool();

    bool isRun = true;
    tptd->resetDoneTasks();

#if PLATFORM == PLATFORM_WIN32
#else
    pthread_detach(pthread_self());
#endif

    tptd->onStart();

    while(isRun)
    {
        if(tptd->task() != NULL)
        {
            isRun = true;
        }
        else
        {
            tptd->resetDoneTasks();
            isRun = tptd->onWaitCondSignal();
        }

        if(!isRun || pThreadPool->isDestroyed())
        {
            if(!pThreadPool->hasThread(tptd))
                tptd = NULL;

            goto __THREAD_END__;
        }

        TPTask *task = tptd->task();
        if(task == NULL)
            continue;

        tptd->mState = ThreadState_Busy;

        while(task && !tptd->threadPool()->isDestroyed())
        {
            tptd->incDoneTasks();
            tptd->onProcessTaskStart(task);
            tptd->processTask(task); // 处理该任务
            tptd->onProcessTaskEnd(task);

            TPTask *task1 = tptd->tryGetTask(); // 尝试继续从任务队列里取出一个繁忙的未处理的任务

            if(!task1)
            {
                tptd->onTaskCompleted();
                break;
            }
            else
            {
                pThreadPool->addFiniTask(task);
                task = task1;
                tptd->task(task1);
            }
        }
    }

__THREAD_END__:
    if(tptd)
    {
        TPTask *task = tptd->task();
        if(task)
        {
            WarningPrint("TPThread::threadFunc() task %p not finish, thread %p will exit.", task, tptd);
            delete task;
        }

        tptd->onEnd();
        tptd->mState = ThreadState_End;
        tptd->resetDoneTasks();
    }

#if PLATFORM == PLATFORM_WIN32
    return 0;
#else
    pthread_exit(NULL);
    return NULL;
#endif
}

NAMESPACE_END // namespace core
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
SINGLETON_INIT(core::ThreadPool);

NAMESPACE_BEG(core)
        
// ThreadPool       
int ThreadPool::timeout = 300;

ThreadPool::ThreadPool()
        :mIsInitialize(false)
        ,mIsDestroyed(false)
        ,mBufferedTaskList()
        ,mFinishedTaskList()
        ,mFiniTaskListCount(0)
        ,mBufferedTaskListMutex()
        ,mThreadStateListMutex()
        ,mFinishedTaskListMutex()
        ,mBusyThreadList()
        ,mFreeThreadList()
        ,mAllThreadList()
        ,mMaxThreadCount(0)
        ,mExtraNewAddThreadCount(0)
        ,mCurrentThreadCount(0)
        ,mCurrentFreeThreadCount(0)
        ,mNormalThreadCount(0)
{
    THREAD_MUTEX_INIT(mThreadStateListMutex);
    THREAD_MUTEX_INIT(mBufferedTaskListMutex);
    THREAD_MUTEX_INIT(mFinishedTaskListMutex);
}

ThreadPool::~ThreadPool()
{
    assert(mIsDestroyed && mAllThreadList.size() == 0 &&
           "mIsDestroyed && mAllThreadList.size() == 0");
}

void ThreadPool::finalise()
{
    destroy();
}

void ThreadPool::destroy()
{
    mIsDestroyed = true;

    // 等待所有线程结束
    int itry = 0;
    while(true)
    {
        sleepms(16);
        itry++;

        ostrbuf taskaddrs;
        THREAD_MUTEX_LOCK(mThreadStateListMutex);

        int count = mAllThreadList.size();
        std::list<TPThread *>::iterator itr = mAllThreadList.begin();
        for(; itr != mAllThreadList.end(); ++itr)
        {
            if((*itr))
            {
                if((*itr)->state() != TPThread::ThreadState_End)
                {
                    (*itr)->sendCondSignal();
                    taskaddrs << (void *)(*itr);
                }
                else
                {
                    count--;
                }
            }
        }

        THREAD_MUTEX_UNLOCK(mThreadStateListMutex);

        if(count <= 0)
        {
            break;
        }
        else
        {
            WarningPrint("ThreadPool::destroy() waiting for thread(%d)[%s], try %d times",
                         count, taskaddrs.c_str(), itry);
        }
    }

    // 销毁所有线程对象
    THREAD_MUTEX_LOCK(mThreadStateListMutex);
    sleepms(100);
    std::list<TPThread*>::iterator itr = mAllThreadList.begin();
    for(; itr != mAllThreadList.end(); ++itr)
    {
        if((*itr))
        {
            delete (*itr);
            (*itr) = NULL;
        }
    }
    mAllThreadList.clear();
    THREAD_MUTEX_UNLOCK(mThreadStateListMutex);

    // 销毁所有已完成任务对象
    THREAD_MUTEX_LOCK(mFinishedTaskListMutex);
    if(mFinishedTaskList.size() > 0)
    {        
        WarningPrint("ThreadPool::~ThreadPool() Discarding %d finished tasks.", (int)mFinishedTaskList.size());

        std::list<TPTask*>::iterator finiiter = mFinishedTaskList.begin();
        for(; finiiter != mFinishedTaskList.end(); ++finiiter)
        {
            delete (*finiiter);
        }

        mFinishedTaskList.clear();
        mFiniTaskListCount = 0;
    }
    THREAD_MUTEX_UNLOCK(mFinishedTaskListMutex);

    // 销毁所有没来得及处理的任务对象
    THREAD_MUTEX_LOCK(mBufferedTaskListMutex);
    if(mBufferedTaskList.size() > 0)
    {
        WarningPrint("ThreadPool::~ThreadPool() Discarding %d buffered tasks.", mBufferedTaskList.size());

        while(mBufferedTaskList.size() > 0)
        {
            TPTask* tptask = mBufferedTaskList.front();
            mBufferedTaskList.pop();
            delete tptask;
        }
    }
    THREAD_MUTEX_UNLOCK(mBufferedTaskListMutex);

    THREAD_MUTEX_DELETE(mThreadStateListMutex);
    THREAD_MUTEX_DELETE(mBufferedTaskListMutex);
    THREAD_MUTEX_DELETE(mFinishedTaskListMutex);
    
    InfoPrint("ThreadPool::destroy() successfully!");
}

bool ThreadPool::createThreadPool(uint32 inewThreadCount, uint32 inormalMaxThreadCount, uint32 imaxThreadCount)
{
    assert(!mIsInitialize);

    mExtraNewAddThreadCount = inewThreadCount;
    mNormalThreadCount = inormalMaxThreadCount;
    mMaxThreadCount = imaxThreadCount;

    for(uint32 i = 0; i < mNormalThreadCount; ++i)
    {
        TPThread* tptd = createThread(0);

        if(!tptd)
        {
            ErrorPrint("ThreadPool::createThreadPool create is error!");
            return false;
        }

        mCurrentFreeThreadCount++;
        mCurrentThreadCount++;
        mFreeThreadList.push_back(tptd);
        mAllThreadList.push_back(tptd);
    }

    mIsInitialize = true;
    sleepms(100);
    return true;
}

TPThread* ThreadPool::createThread(int threadWaitSecond)
{
    TPThread* tptd = new TPThread(this, threadWaitSecond);
    tptd->createThread();
    return tptd;
}

void ThreadPool::bufferTask(TPTask* tptask)
{
    THREAD_MUTEX_LOCK(mBufferedTaskListMutex);

    mBufferedTaskList.push(tptask);

    size_t size = mBufferedTaskList.size();
    if(size > THREAD_BUSY_SIZE)
    {
        WarningPrint("ThreadPool::bufferTask: task buffered(%d)!", (int)size);
    }

    THREAD_MUTEX_UNLOCK(mBufferedTaskListMutex);
}

TPTask* ThreadPool::popbufferTask(void)
{
    TPTask* tptask = NULL;
    THREAD_MUTEX_LOCK(mBufferedTaskListMutex);

    size_t size = mBufferedTaskList.size();
    if(size > 0)
    {
        tptask = mBufferedTaskList.front();
        mBufferedTaskList.pop();

        if(size > THREAD_BUSY_SIZE)
        {
            WarningPrint("ThreadPool::popbufferTask: task buffered(%d)!", (int)size);
        }
    }

    THREAD_MUTEX_UNLOCK(mBufferedTaskListMutex);

    return tptask;
}

bool ThreadPool::addFreeThread(TPThread* tptd)
{
    THREAD_MUTEX_LOCK(mThreadStateListMutex);

    std::list<TPThread *>::iterator itr;
    itr = find(mBusyThreadList.begin(), mBusyThreadList.end(), tptd);

    if(itr != mBusyThreadList.end())
    {
        mBusyThreadList.erase(itr);
    }
    else
    {
        THREAD_MUTEX_UNLOCK(mThreadStateListMutex);

        ErrorPrint("ThreadPool::addFreeThread mBusyThreadList not found thread.");

        delete tptd;
        return false;
    }

    mFreeThreadList.push_back(tptd);
    mCurrentFreeThreadCount++;

    THREAD_MUTEX_UNLOCK(mThreadStateListMutex);
    return true;
}

bool ThreadPool::addBusyThread(TPThread* tptd)
{
    THREAD_MUTEX_LOCK(mThreadStateListMutex);
    std::list<TPThread*>::iterator itr;
    itr = find(mFreeThreadList.begin(), mFreeThreadList.end(), tptd);

    if(itr != mFreeThreadList.end())
    {
        mFreeThreadList.erase(itr);
    }
    else
    {
        THREAD_MUTEX_UNLOCK(mThreadStateListMutex);
        ErrorPrint("ThreadPool::addBusyThread: mFreeThreadList not found thread.");

        delete tptd;
        return false;
    }

    mBusyThreadList.push_back(tptd);
    --mCurrentFreeThreadCount;
    THREAD_MUTEX_UNLOCK(mThreadStateListMutex);

    return true;
}

void ThreadPool::addFiniTask(TPTask* tptask)
{
    THREAD_MUTEX_LOCK(mFinishedTaskListMutex);
    mFinishedTaskList.push_back(tptask);
    ++mFiniTaskListCount;
    THREAD_MUTEX_UNLOCK(mFinishedTaskListMutex);
}

bool ThreadPool::removeHangThread(TPThread* tptd)
{
    THREAD_MUTEX_LOCK(mThreadStateListMutex);
    std::list<TPThread*>::iterator itr, itr1;
    itr = find(mFreeThreadList.begin(), mFreeThreadList.end(), tptd);
    itr1 = find(mAllThreadList.begin(), mAllThreadList.end(), tptd);

    if(itr != mFreeThreadList.end() && itr1 != mAllThreadList.end())
    {
        mFreeThreadList.erase(itr);
        mAllThreadList.erase(itr1);
        --mCurrentThreadCount;
        --mCurrentFreeThreadCount;

        InfoPrint("ThreadPool::removeHangThread() thread %u is destroy. "
                  "currentFreeThreadCount:%d, currentThreadCount:%d",
                  (unsigned)tptd->id(), mCurrentFreeThreadCount, mCurrentThreadCount);

        if (tptd)
            delete tptd;
    }
    else
    {
        THREAD_MUTEX_UNLOCK(mThreadStateListMutex);

        ErrorPrint("ThreadPool::removeHangThread: not found thread.");

        return false;
    }

    THREAD_MUTEX_UNLOCK(mThreadStateListMutex);
    return true;
}

void ThreadPool::onMainThreadTick()
{
    std::vector<TPTask *> finitasks;

    THREAD_MUTEX_LOCK(mFinishedTaskListMutex);

    if(mFinishedTaskList.size() == 0)
    {
        THREAD_MUTEX_UNLOCK(mFinishedTaskListMutex);
        return;
    }

    std::copy(mFinishedTaskList.begin(), mFinishedTaskList.end(), std::back_inserter(finitasks));
    mFinishedTaskList.clear();
    THREAD_MUTEX_UNLOCK(mFinishedTaskListMutex);

    std::vector<TPTask *>::iterator finiiter  = finitasks.begin();

    for(; finiiter != finitasks.end(); )
    {
        TPTask::TPTaskState state = (*finiiter)->presentMainThread();

        switch(state)
        {
        case TPTask::TPTask_Completed:
            delete (*finiiter);
            finiiter = finitasks.erase(finiiter);
            --mFiniTaskListCount;
            break;
        case TPTask::TPTask_ContinueChildThread:
            this->addTask((*finiiter));
            finiiter = finitasks.erase(finiiter);
            --mFiniTaskListCount;
            break;
        case TPTask::TPTask_ContinueMainThread:
            THREAD_MUTEX_LOCK(mFinishedTaskListMutex);
            mFinishedTaskList.push_back((*finiiter));
            THREAD_MUTEX_UNLOCK(mFinishedTaskListMutex);
            ++finiiter;
            break;
        default:
            assert(false);
            break;
        }
    }
}

bool ThreadPool::hasThread(TPThread* pTPThread)
{
    bool ret = true;

    THREAD_MUTEX_LOCK(mThreadStateListMutex);

    std::list<TPThread*>::iterator itr1 = find(mAllThreadList.begin(), mAllThreadList.end(), pTPThread);
    if(itr1 == mAllThreadList.end())
        ret = false;

    THREAD_MUTEX_UNLOCK(mThreadStateListMutex);

    return ret;
}

std::string ThreadPool::printThreadWorks()
{
    ostrbuf ret;

    THREAD_MUTEX_LOCK(mThreadStateListMutex);
    int i = 0;
    std::list<TPThread*>::iterator itr = mBusyThreadList.begin();
    for(; itr != mBusyThreadList.end(); ++itr)
    {
        ret<<(void*)(*itr)<<":"<<(*itr)->printWorkState()<<", ";
        i++;

        if(i > 1024)
            break;
    }

    THREAD_MUTEX_UNLOCK(mThreadStateListMutex);
    return ret.c_str();
}

bool ThreadPool::addTask(TPTask* tptask)
{
    THREAD_MUTEX_LOCK(mThreadStateListMutex);
    if(mCurrentFreeThreadCount > 0)
    {
        std::list<TPThread *>::iterator itr = mFreeThreadList.begin();
        TPThread* tptd = (TPThread *)(*itr);
        mFreeThreadList.erase(itr);
        mBusyThreadList.push_back(tptd);
        --mCurrentFreeThreadCount;

        InfoPrint("ThreadPool::addTask() currFree:%d, currThreadCount:%d",
                  mCurrentFreeThreadCount, mCurrentThreadCount);

        tptd->task(tptask); // 给线程设置新任务

#if PLATFORM == PLATFORM_WIN32
        if(tptd->sendCondSignal()== 0)
#else
        if(tptd->sendCondSignal()!= 0)
#endif
        {
            ErrorPrint("ThreadPool::addTask: pthread_cond_signal is error!");
            THREAD_MUTEX_UNLOCK(mThreadStateListMutex);
            return false;
        }

        THREAD_MUTEX_UNLOCK(mThreadStateListMutex);
        return true;
    }

    bufferTask(tptask);

    if(isThreadCountMax())
    {
        THREAD_MUTEX_UNLOCK(mThreadStateListMutex);

        WarningPrint("ThreadPool::addTask: can't createthread, the poolsize is full(%d).", mMaxThreadCount);

        return false;
    }

    for(uint32 i = 0; i < mExtraNewAddThreadCount; ++i)
    {
        TPThread* tptd = createThread(300); // 设定5分钟未使用则退出的线程
        if(!tptd)
        {
            ErrorPrint("ThreadPool::addTask() the ThreadPool create thread error! %s", coreStrError());
        }

        mAllThreadList.push_back(tptd); // 所有的线程列表
        mFreeThreadList.push_back(tptd); // 闲置的线程列表
        ++mCurrentThreadCount;
        ++mCurrentFreeThreadCount;
    }

    InfoPrint("ThreadPool::addTask: new Thread, currThreadCount: %d", mCurrentThreadCount);

    THREAD_MUTEX_UNLOCK(mThreadStateListMutex);
    return true;
}

NAMESPACE_END // namespace core
//--------------------------------------------------------------------------
