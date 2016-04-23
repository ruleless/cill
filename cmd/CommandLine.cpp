#include "CommandLine.h"
#include "CommandFactoryRegistry.h"

using core::ThreadPool;

NAMESPACE_BEG(cmd)

CommandLine *gpCommandLine = NULL;

CommandLine::CommandLine()
		:mpThreadPool(NULL)
		,mOutStream()
		,mpInput(NULL)
		,mbQuit(false)
		,mCommandList()
{
}

CommandLine::~CommandLine()
{
}

bool CommandLine::initialise()
{
	mpThreadPool = new core::ThreadPool();
	if (!mpThreadPool->createThreadPool(10, 20, 30))
	{
		return false;
	}

	mpInput = new Input(this);
	mpThreadPool->addTask(mpInput);

	return true;
}

void CommandLine::finalisze()
{
	mpThreadPool->finalise();
	delete mpThreadPool;
	mpThreadPool = NULL;
}

void CommandLine::run()
{
	std::string hint = ">";
	while (true)
	{
		core::sleepms(16);

		if (hint.length() > 0)
		{
			printf("%s", hint.c_str());
			fflush(stdout);
			hint.clear();
		}

		if (mpInput)
		{
			std::string cmdLine = mpInput->getCmdLine();
			if (cmdLine.length() > 0)
			{
				core::replace(cmdLine, "\r", " ");
				core::replace(cmdLine, "\n", " ");

				std::vector<std::string> argv;
				core::split<>(cmdLine, ' ', argv);
				Command *pCommand = _parseUserCommand(argv);
				if (pCommand)
				{
					mCommandList.insert(pCommand);
					mpThreadPool->addTask(pCommand);
				}
				else if (argv.size() > 0)
				{
					if (!_parseSysCommand(argv))
					{
						hint = "Command Invalid!\n>";
					}
				}
				else
				{
					hint = ">";
				}

				mpInput->clearCmdLine();
			}
		}

		mpThreadPool->onMainThreadTick();
		if (mOutStream.size() > 0)
		{
			core::InBuffer ib(mOutStream.data(), mOutStream.size());
			while (ib.space() > 0)
			{
				const char *buf = NULL;
				ib>>buf;
				printf("%s", buf);
			}
			mOutStream.clear();
			hint = ">";
		}

		if (mbQuit)
		{
			if (mpThreadPool->currentFreeThreadCount() == mpThreadPool->currentThreadCount() && mpThreadPool->finiTaskSize() == 0)
			{
				break;
			}
		}
	}
}

Command* CommandLine::_parseUserCommand(const std::vector<std::string> &argv)
{
	Command *pCommand = NULL;
	if (argv.size() > 0)
	{
		CommandFactory *pFactory = CommandFactoryRegistry::getRegistry()->getCommandFactory(argv[0]);
		if (pFactory)
		{
			pCommand = pFactory->createCommand(this);
		}		
	}

	if (pCommand)
	{
		if (!pCommand->parseCommond(argv, mOutStream))
		{
			delete pCommand;
			pCommand = NULL;
		}
	}
	return pCommand;
}

bool CommandLine::_parseSysCommand(const std::vector<std::string> &argv)
{
	bool isSysCmd = true;
	bool stopAllCmd = false;
	if (argv[0] == "q")
	{
		stopAllCmd = true;
	}
	else if (argv[0] == "quit")
	{
		stopAllCmd = true;
		mbQuit = true;
	}
	else
	{
		isSysCmd = false;
	}

	CommandList::iterator it = mCommandList.begin();
	for (; it != mCommandList.end(); ++it)
	{
		(*it)->forceQuit(true);
	}

	return isSysCmd;
}

void CommandLine::onCommandDestroy(Command *pCmd)
{
	CommandList::iterator it = mCommandList.find(pCmd);
	mCommandList.erase(it);
}

bool CommandLine::onOutputStream(Command *pCmd, const core::obuf& ob)
{
	mOutStream.push_back(ob.data(), ob.size());
	return true;
}

void CommandLine::onInputDestroy(Input *pInput)
{
	mpInput = NULL;
}

NAMESPACE_END // namespace cmd
