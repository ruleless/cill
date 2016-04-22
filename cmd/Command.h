#ifndef __CMD_COMMAND_H__
#define __CMD_COMMAND_H__

#include "CmdBase.h"

using namespace std;

NAMESPACE_BEG(cmd)

class Command : public TPTask
{
  public:
	struct Listener 
	{
		virtual void onCommandDestroy(Command *pCmd) = 0;
		virtual bool onOutputStream(Command *pCmd, const obuf& ob) = 0;
	};
	
	Command(Listener *pListener);
	virtual ~Command();

	bool parseCommond(const vector<string> &argv, obuf &outStream);
	virtual bool _parseArg(const vector<string> &argv, obuf &outStream) { return true; }

	virtual bool process();
	virtual ulong _runCommand() = 0;

	virtual TPTask::TPTaskState presentMainThread();

	inline bool forceQuit()
	{
		return mForceQuit;
	}
	inline void forceQuit(bool b)
	{
		mForceQuit = b;
	}
  protected:
	obuf mMyStream;
	string mCommandName;
	ulong mExRunTime;
	ulong mNextInterval;
	bool mForceQuit;
	Listener *mpListener;
};

NAMESPACE_END // namespace cmd

#endif // __CMD_COMMAND_H__
