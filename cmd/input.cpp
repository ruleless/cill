#include "input.h"
#include "commandline.h"

NAMESPACE_BEG(cmd)

Input::Input(Listener *pListener)
        :mInputBuffer()
        ,mNextCmdLine()
        ,mpListener(pListener)
{
}

Input::~Input()
{
    if (mpListener)
    {
        mpListener->onInputDestroy(this);
    }
}

bool Input::process()
{
    char line[MAX_BUF];
    fgets(line, MAX_BUF, stdin);
    mInputBuffer = line;
    return true;
}

core::TPTask::TPTaskState Input::presentMainThread()
{
    if (gpCommandLine->quit())
    {
        return core::TPTask::TPTask_Completed;
    }

    if (mInputBuffer.size() > 0)
    {
        mNextCmdLine = mInputBuffer;
        mInputBuffer.clear();
    }

    core::TPTask::TPTaskState retState = core::TPTask::TPTask_ContinueChildThread;
    if (mNextCmdLine.size() > 0)
    {
        retState = core::TPTask::TPTask_ContinueMainThread;
    }
    
    return retState;
}

NAMESPACE_END // namespace cmd
