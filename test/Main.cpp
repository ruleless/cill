#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cppunit/extensions/TestFactoryRegistry.h"
#include "cppunit/ui/text/TestRunner.h"
#include "core/Trace.h"

int main(int argc, char *argv[])
{
	core::createTrace();
	core::output2Console();
	core::output2Html("log.html");
	
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
	runner.run();

	core::closeTrace();
	
	exit(0);
}
