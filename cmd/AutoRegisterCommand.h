#ifndef __CMD_AUTOREGISTERCOMMAND_H__
#define __CMD_AUTOREGISTERCOMMAND_H__

#include "CommandFactoryImpl.h"
#include "CommandFactoryRegistry.h"

NAMESPACE_BEG(cmd)

template<class CommandType>
class AutoRegisterCommand
{
  public:
	AutoRegisterCommand()
			:mRegistry(CommandFactoryRegistry::getRegistry())
	{
		mRegistry->registerFactory(&mFactory);
	}

	~AutoRegisterCommand()
	{
		if (CommandFactoryRegistry::isValid())
		{
			mRegistry->unregisterFactory(&mFactory);
		}
	}
  private:
	CommandFactoryRegistry *mRegistry;
	CommandFactoryImpl<CommandType> mFactory;
};

NAMESPACE_END // namespace cmd


#define COMMAND_DECLARATION(CommandType)							\
	static std::string getName();									\
	static cmd::Command* createCommand(cmd::Command::Listener *)


#define COMMAND_IMPLEMENTION(CommandType, cmdName)						\
	std::string CommandType::getName()									\
	{																	\
		return cmdName;													\
	}																	\
	cmd::Command* CommandType::createCommand(cmd::Command::Listener *pListener) \
	{																	\
		return new CommandType(pListener);								\
	}																	\
	

#define COMMAND_FACTORY_REGISTRATION(CommandType)						\
		static cmd::AutoRegisterCommand<CommandType> _autoRegisterRegistry_##CommandType

#endif // __CMD_AUTOREGISTERCOMMAND_H__
