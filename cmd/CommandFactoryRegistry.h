#ifndef __CMD_COMMANDFACTORYREGISTRY_H__
#define __CMD_COMMANDFACTORYREGISTRY_H__

#include "CommandFactory.h"

NAMESPACE_BEG(cmd)

class CommandFactoryRegistry
{
  public:
	CommandFactoryRegistry();
	~CommandFactoryRegistry();

	static CommandFactoryRegistry* getRegistry();
	static bool isValid();

	void registerFactory(CommandFactory *factory);
	void unregisterFactory(CommandFactory *factory);

	CommandFactory* getCommandFactory(const std::string cmdName);
  protected:
	typedef std::vector<CommandFactory *> FactoryList;
	
	FactoryList mFactories;
	static bool mbValid;  
};

NAMESPACE_END // namespace cmd

#endif // __CMD_COMMANDFACTORYREGISTRY_H__
