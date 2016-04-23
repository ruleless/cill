#ifndef __CMD_COMMAND_H__
#define __CMD_COMMAND_H__

#include "CmdBase.h"

NAMESPACE_BEG(cmd)

class Command : public core::TPTask
{
  public:
	struct Listener 
	{
		virtual void onCommandDestroy(Command *pCmd) = 0;
		virtual bool onOutputStream(Command *pCmd, const core::obuf& ob) = 0;
	};
	
	Command(Listener *pListener);
	virtual ~Command();
	
	bool parseCommond(const std::vector<std::string> &argv, core::obuf &outStream);
	virtual bool _parseArg(const std::vector<std::string> &argv, core::obuf &outStream) { return true; }

	virtual bool process();
	virtual core::ulong _runCommand() = 0;

	virtual core::TPTask::TPTaskState presentMainThread();

	inline bool forceQuit()
	{
		return mForceQuit;
	}
	inline void forceQuit(bool b)
	{
		mForceQuit = b;
	}
  protected:	
	core::obuf mMyStream;
	std::string mCommandName;
	core::ulong mExRunTime;
	core::ulong mNextInterval;
	bool mForceQuit;
	Listener *mpListener;
};

NAMESPACE_END // namespace cmd

#endif // __CMD_COMMAND_H__
