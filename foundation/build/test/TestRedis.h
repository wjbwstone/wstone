#ifndef _TEST_TESTREDIS_2020_02_27_13_25_36_H
#define _TEST_TESTREDIS_2020_02_27_13_25_36_H

static void redisError(ws::SimpleLogSink::Level level, 
	const ws::i8 *err, ws::u32 bytes) {
	MLOG((ws::LoggerLevel)level, "redislog : {}", ws::ustring(err, bytes));
}

static void testClient() {
	ws::libredis::Redis redis;
	redis.setLogPrinter(redisError);
	redis.connect("localhost", 63791, "test_654321");
	redis.selectDB(1);

	std::vector<ws::ustring> vret;
	redis.getKeys(vret, "zd");

	vret.clear();
	redis.getKeys(vret);

	CHECK_FALSE(redis.exist(nullptr));

	for (const auto &it : vret)
	{
		CHECK(redis.exist(it.c_str()));
	}

	CHECK(redis.delKey("mao2"));
	ws::ustring svalue("123456\0ADB", 10);
	CHECK(redis.setData("mao2", svalue));
	ws::ustring value;
	CHECK(redis.getData("mao2", value));
	CHECK(value == svalue);

	CHECK(redis.rename("mao2", "mao3"));
	CHECK(redis.getData("mao3", value));
	CHECK(value == svalue);

	CHECK(redis.setData("mao4", "4", 1));
	Sleep(1000);
	CHECK_FALSE(redis.exist("mao4"));

	CHECK(redis.save());

	vret.clear();
	CHECK(redis.getSetData("maoset1", vret));
	vret.push_back("1");
	vret.push_back("3");
	vret.push_back("1");
	vret.push_back("6");
	vret.push_back(svalue);
	CHECK(redis.addSetData("maoset1", vret));
	CHECK(redis.addSetData("maoset1", "10"));
	CHECK(redis.addSetData("maoset1", ws::__emptyString));

	vret.clear();
	redis.getSetData("maoset1", vret);

	CHECK(redis.delSetData("maoset1", "1"));

	CHECK(redis.setQueueData("maolist", "23"));
	CHECK(redis.setQueueData("maolist", "24"));
	CHECK(redis.setQueueData("maolist", "25"));

	CHECK(redis.setQueueData("maolist2", "23", false));
	CHECK(redis.setQueueData("maolist2", "24", false));
	CHECK(redis.setQueueData("maolist2", "25", false));
}

TEST_CASE("redis")
{
	testClient();
}

#endif //_TEST_TESTREDIS_2020_02_27_13_25_36_H