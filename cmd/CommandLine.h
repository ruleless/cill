#ifndef __CMD_COMMANDLINE_H__
#define __CMD_COMMANDLINE_H__

#include "CmdBase.h"
#include "Command.h"
#include "Input.h"

using namespace std;

NAMESPACE_BEG(cmd)

class CommandLine : public Command::Listener, public Input::Listener
{  
  public:
	CommandLine();
	virtual ~CommandLine();

	bool initialise();
	void finalisze();

	void run();
	Command* _parseUserCommand(const vector<string> &argv);
	bool _parseSysCommand(const vector<string> &argv);

	virtual void onCommandDestroy(Command *pCmd);
	virtual bool onOutputStream(Command *pCmd, const obuf& ob);
	virtual void onInputDestroy(Input *pInput);

	inline bool quit()
	{
		return mbQuit;
	}
	inline void quit(bool b)
	{
		mbQuit = b;
	}
  protected:
	ThreadPool *mpThreadPool;
	bool mbQuit;
	obuf mOutStream;

	Input *mpInput;
	
	typedef std::set<Command *> CommandList;
	CommandList mCommandList;
};

extern CommandLine *gpCommandLine;

NAMESPACE_END // namespace cmd

#endif // __CMD_COMMANDLINE_H__
