#ifndef __CMD_COMMANDFACTORYIMPL_H__
#define __CMD_COMMANDFACTORYIMPL_H__

#include "command_factory.h"

NAMESPACE_BEG(cmd)

template <class CommandType>
class CommandFactoryImpl : public CommandFactory
{
  public:
    virtual std::string getName()
    {
        return CommandType::getName();
    }

    virtual Command* createCommand(Command::Listener *pListener)
    {
        return CommandType::createCommand(pListener);
    }
};

NAMESPACE_END // namespace cmd

#endif // __CMD_COMMANDFACTORYIMPL_H__
