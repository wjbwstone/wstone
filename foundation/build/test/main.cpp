#include "stdafx.h"

#include "TestBase.h"

/*
#include "TestCURL.h"
#include "TestLogger.h"
#include "TestMysql.h"
#include "TestRegex.h"
#include "TestJson.h"
#include "TestSqlite.h"
#include "TestRedis.h"
#include "TestZip.h"
#include "TestBugtrap.h"
#include "TestZXing.h"
*/

TEST_CASE("wmain") {
	Log::destroyInstance();
}