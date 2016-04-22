#include "Input.h"
#include "CommandLine.h"

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

TPTask::TPTaskState Input::presentMainThread()
{
	if (gpCommandLine->quit())
	{
		return TPTask_Completed;
	}

	if (mInputBuffer.size() > 0)
	{
		mNextCmdLine = mInputBuffer;
		mInputBuffer.clear();
	}

	TPTask::TPTaskState retState = 	TPTask_ContinueChildThread;
	if (mNextCmdLine.size() > 0)
	{
		retState = TPTask_ContinueMainThread;
	}
	
	return retState;
}

NAMESPACE_END // namespace cmd
