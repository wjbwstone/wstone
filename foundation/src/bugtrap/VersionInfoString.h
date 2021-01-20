#pragma once

#include "VersionInfo.h"
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#include <tchar.h>
#define VER_FILE_VERSION            VERSION_MAJOR, VERSION_MINOR, REVISION_NUMBER, BUILD_NUMBER
#define VER_FILE_VERSION_STR        _T(STRINGIZE(VERSION_MAJOR)) \
                                    _T(".") _T(STRINGIZE(VERSION_MINOR)) \
                                    _T(".") _T(STRINGIZE(REVISION_NUMBER)) \
                                    _T(".") _T(STRINGIZE(BUILD_NUMBER))

#define VER_PRODUCT_VERSION         VER_FILE_VERSION
#define VER_PRODUCT_VERSION_STR     VER_FILE_VERSION_STR
