#ifndef __FAKECOMMAND_H__
#define __FAKECOMMAND_H__

#include "cmd/CillCmd.h"

class FakeCommand : public cmd::Command
{
  public:
	COMMAND_DECLARATION(FakeCommand);
  public:
	FakeCommand(Listener *pListener)
		:Command(pListener)
	{
	}

	virtual ~FakeCommand() {}

	virtual ulong _runCommand();
};

#endif // __FAKECOMMAND_H__
