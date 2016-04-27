#include "FakeCommand.h"

COMMAND_IMPLEMENTION(FakeCommand, "fake");
COMMAND_FACTORY_REGISTRATION(FakeCommand);

core::ulong FakeCommand::_runCommand()
{
	mMyStream<<"I am Fake Command!\n";
	return 1000;
}