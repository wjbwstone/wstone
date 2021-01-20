#include "catch_default_main.hpp"

#include "catch_session.h"
#include "catch_interfaces_reporter.h"
#include "catch_reporter_registrars.hpp"
#include "../reporters/catch_reporter_console.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

using namespace Catch;

CATCH_REGISTER_REPORTER("console", ConsoleReporter)

#ifndef __OBJC__

int main(int argc,  char * argv[]) {
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	_set_error_mode(_OUT_TO_MSGBOX);

    return Catch::Session().run( argc, argv );
}

#else // __OBJC__

// Objective-C entry point
int main (int argc, char * const argv[]) {
#if !CATCH_ARC_ENABLED
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
#endif

    Catch::registerTestMethods();
    int result = Catch::Session().run( argc, (char**)argv );

#if !CATCH_ARC_ENABLED
    [pool drain];
#endif

    return result;
}

#endif // __OBJC__