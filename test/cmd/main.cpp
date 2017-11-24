#include "cmd/cillcmd.h"

using namespace cmd;

int main(int argc, char *argv[])
{
    log_initialise(AllLog);
    log_reg_console();
    
    gpCommandLine = new CommandLine();
    if (!gpCommandLine->initialise())
    {
        delete gpCommandLine;
        return 1;
    }

    gpCommandLine->run();
    
    gpCommandLine->finalisze();
    delete gpCommandLine;

    log_finalise();
    
    return 0;
}
