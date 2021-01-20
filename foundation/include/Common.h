#ifndef _LIBCOMMON_COMMON_2020_02_27_13_04_33_H
#define _LIBCOMMON_COMMON_2020_02_27_13_04_33_H

/*******************************************************************
*@brief  : 所有错误ID请使用GetLastERROR查看
*模块开启介绍(1开启，0关闭。默认关闭）
*USE_COMMON_ZIP
*USE_COMMON_CURL
*USE_COMMON_MYSQL
*USE_COMMON_EXCEL
********************************************************************/

#if _MSC_VER < 1900 //TODO:暂时如此
#include "vld/vld.h"
#endif

#include <helper/All.h>

#include "base/MD5.h"
#include "base/RC4.h"
#include "base/File.h"
#include "base/SHA1.h"
#include "base/Path.h"
#include "base/Time.h"
#include "base/Pipe.h"
#include "base/Timer.h"
#include "base/Event.h"
#include "base/Base64.h"
#include "base/String.h"
#include "base/System.h"
#include "base/URLCode.h"
#include "base/Console.h"
#include "base/Process.h"
#include "base/Registry.h"
#include "base/Identity.h"
#include "base/Directory.h"
#include "base/Condition.h"
#include "base/MemoryFile.h"
#include "base/MemoryPool.h"
#include "base/ByteBuffer.h"
#include "base/ThreadPool.h"
#include "base/WIN32Service.h"
#include "base/WIN32Resource.h"
#include "base/FolderMonitor.h"

#include "json/json.h"
#include "crypt/Crypt.h"
#include "regex/regex.h"
#include "archive/Tar.h"
#include "redis/Redis.h"
#include "logger/Logger.h"
#include "zxing/libzxing.h"
#include "sqlite/SqliteIDB.h"
#include "language/Language.h"
#include "archive/Z4Compress.h"

#if USE_COMMON_ZIP
#include "archive/Zip.h"
#endif

#if USE_COMMON_CURL
#include "curl/libcurl.h"
#endif

#if USE_COMMON_MYSQL
#include "mysql/IMySQLManager.h"
#endif

#if USE_COMMON_EXCEL
#include "excel/libxl.h"
#endif

#if USE_COMMON_BUGTRAP
#include "archive/Zip.h"
#include "bugtrap/IBugTrap.h"
#endif

#ifdef _DEBUG
#pragma comment(lib, "libcommond.lib")
#else
#pragma comment(lib, "libcommon.lib")
#endif //_DEBUG

#endif //_LIBCOMMON_COMMON_2020_02_27_13_04_33_H