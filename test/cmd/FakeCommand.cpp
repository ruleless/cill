#include "FakeCommand.h"

COMMAND_IMPLEMENTION(FakeCommand, "fake");
COMMAND_FACTORY_REGISTRATION(FakeCommand);

ulong FakeCommand::_runCommand()
{
	mMyStream<<"I am Fake Command!\n";
	return 1000;
}