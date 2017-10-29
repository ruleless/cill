#ifndef __CMD_COMMANDFACTORY_H__
#define __CMD_COMMANDFACTORY_H__

#include "command.h"

NAMESPACE_BEG(cmd)

class CMD_CLASS CommandFactory
{
  public:
    virtual std::string getName() = 0;
    virtual Command* createCommand(Command::Listener *pListener) = 0;
};

NAMESPACE_END // namespace cmd

#endif // __CMD_COMMANDFACTORY_H__
