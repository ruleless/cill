#include "cmd/CillCmd.h"

using namespace cmd;

int main(int argc, char *argv[])
{
	core::createTrace();
	core::output2Console();
	core::output2Html("log.html");
	
    gpCommandLine = new CommandLine();
	if (!gpCommandLine->initialise())
	{
		delete gpCommandLine;
		return 1;
	}

	gpCommandLine->run();
	
	gpCommandLine->finalisze();
	delete gpCommandLine;

	core::closeTrace();
	
    return 0;
}
