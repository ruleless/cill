#ifndef __CMD_COMMANDLINE_H__
#define __CMD_COMMANDLINE_H__

#include "cmdbase.h"
#include "command.h"
#include "input.h"

NAMESPACE_BEG(cmd)

class CMD_CLASS CommandLine : public Command::Listener, public Input::Listener
{  
  public:
    CommandLine();
    virtual ~CommandLine();

    bool initialise();
    void finalisze();

    void run();
    Command* _parseUserCommand(const std::vector<std::string> &argv);
    bool _parseSysCommand(const std::vector<std::string> &argv);

    virtual void onCommandDestroy(Command *pCmd);
    virtual bool onOutputStream(Command *pCmd, const core::obuf& ob);
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
    core::ThreadPool *mpThreadPool;
    core::obuf mOutStream;  

    Input *mpInput;

    bool mbQuit;
    
    typedef std::set<Command *> CommandList;
    CommandList mCommandList;
};

extern CMD_VAR CommandLine *gpCommandLine;

NAMESPACE_END // namespace cmd

#endif // __CMD_COMMANDLINE_H__
