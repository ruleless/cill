#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/ui/text/TestRunner.h"
#include "log/log_inc.h"

int main(int argc, char *argv[])
{
    log_initialise(AllLog);
    // core::output2Console();
    log_reg_console();
    
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    runner.run();

    log_finalise();
    // getchar();
    exit(0);
}
