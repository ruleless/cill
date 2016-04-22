#include "Command.h"

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

bool Command::parseCommond(const vector<string> &argv, obuf &outStream)
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
	mExRunTime = getTickCount();

	return true;
}

TPTask::TPTaskState Command::presentMainThread()
{
	TPTask::TPTaskState retState = TPTask_Completed;
	if (mForceQuit || 0 == mNextInterval)
	{
		retState = TPTask_Completed;
	}
	else if (getTickCount() < mExRunTime+mNextInterval)
	{
		retState = TPTask_ContinueMainThread;
	}
	else
	{
		retState = TPTask_ContinueChildThread;
	}
	
	if (mpListener->onOutputStream(this, mMyStream))
	{
		mMyStream.clear();
	}

	return retState;
}

NAMESPACE_END // namespace cmd
