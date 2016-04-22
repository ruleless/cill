#include "CommandFactoryRegistry.h"

NAMESPACE_BEG(cmd)

bool CommandFactoryRegistry::mbValid = false;

CommandFactoryRegistry::CommandFactoryRegistry()
{
	mbValid = true;
}

CommandFactoryRegistry::~CommandFactoryRegistry()
{
	mbValid = false;
}

CommandFactoryRegistry* CommandFactoryRegistry::getRegistry()
{
	static CommandFactoryRegistry _s_registry;
	return &_s_registry;
}

bool CommandFactoryRegistry::isValid()
{
	return mbValid;
}

void CommandFactoryRegistry::registerFactory(CommandFactory *factory)
{
	mFactories.push_back(factory);
}

void CommandFactoryRegistry::unregisterFactory(CommandFactory *factory)
{
	FactoryList::iterator it = mFactories.begin();
	while (it != mFactories.end())
	{
		if ((*it) == factory)
		{
			it = mFactories.erase(it);
		}
		else
		{
			++it;
		}
	}
}

CommandFactory* CommandFactoryRegistry::getCommandFactory(const std::string cmdName)
{
	FactoryList::iterator it = mFactories.begin();
	for (; it != mFactories.end(); ++it)
	{
		if ((*it)->getName() == cmdName)
		{
			return (*it);
		}
	}
	return NULL;
}

NAMESPACE_END // namespace cmd
