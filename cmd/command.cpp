#include "command.h"

NAMESPACE_BEG(cmd)

Command::Command(Listener *pListener)
        :mMyStream()
        ,mCommandName("")
        ,mExRunTime(0)
        ,mNextInterval(0)
        ,mForceQuit(false)
        ,mpListener(pListener)
{
}

Command::~Command()
{
    mpListener->onCommandDestroy(this);
}

bool Command::parseCommond(const std::vector<std::string> &argv, core::obuf &outStream)
{
    if (argv.size() >= 1)
    {
        mCommandName = argv[0];
        return _parseArg(argv, outStream);
    }
    return false;
}

bool Command::process()
{
    mNextInterval = _runCommand();
    mExRunTime = core::getTickCount();

    return true;
}

core::TPTask::TPTaskState Command::presentMainThread()
{
    core::TPTask::TPTaskState retState = core::TPTask::TPTask_Completed;
    if (mForceQuit || 0 == mNextInterval)
    {
        retState = core::TPTask::TPTask_Completed;
    }
    else if (core::getTickCount() < mExRunTime+mNextInterval)
    {
        retState = core::TPTask::TPTask_ContinueMainThread;
    }
    else
    {
        retState = core::TPTask::TPTask_ContinueChildThread;
    }
    
    if (mpListener->onOutputStream(this, mMyStream))
    {
        mMyStream.clear();
    }

    return retState;
}

NAMESPACE_END // namespace cmd
