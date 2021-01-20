#ifndef _LIBLOG_LOGCOMMON_2020_01_15_13_39_58_H
#define _LIBLOG_LOGCOMMON_2020_01_15_13_39_58_H

#include <helper/Types.h>
#include <helper/Singleton.h>
#include <helper/StringConvert.h>

#include <foundation/include/base/RC4.h>
#include <foundation/include/base/Time.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/File.h>
#include <foundation/include/base/System.h>
#include <foundation/include/base/Console.h>
#include <foundation/include/base/Directory.h>
#include <foundation/include/base/ThreadPool.h>

#include <foundation/include/logger/Logger.h>

namespace ws {

namespace LogCommon {

ustring getLogDir();
const char* getLogExt();
bool canDeleteFiles();
void fileDeletePolicy();

}

}

#endif //_LIBLOG_LOGCOMMON_2020_01_15_13_39_58_H