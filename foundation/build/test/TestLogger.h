#ifndef _TEST_TESTLOGGER_2020_02_27_13_25_36_H
#define _TEST_TESTLOGGER_2020_02_27_13_25_36_H

TEST_CASE("Logger") {
	for(auto i = 0; i < 100; i++) {
		if(0 == i % 2) {
			MLOGD("当前数据信息 ： {}, {:p}", i, (void*)i);
			Log::getInstance()->_logger->deleteLevel(ws::kLoggerLevelAll);
		} else if(0 == i % 3) {
			MLOGI("当前数据信息 ： {}, {:p}", i, (void*)i);
		} else if(0 == i % 67) {
			MLOGE("当前数据信息 ： {}, {:p}", i, (void*)i);
			Log::getInstance()->_logger->addLevel(ws::kLoggerLevelAll);
		} else if(0 == i % 11) {
			MLOGW("当前数据信息 ： {}, {:p}", i, (void*)i);
		} else if(0 == i % 23) {
			MLOGT("当前数据信息 ： {}, {:p}", i, (void*)i);
		}
	}

	MLOGI("{},{}, {},{},{},{},{},{},{},{},"
		"{},{}, {},{},{},{},{},{},{},{}",
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

	auto s = fmt::format("{}", 132);
	fmt::print("{0:.2f}\n", 12345.2342345);
	char* p = nullptr;
	s = fmt::format("{}", p);
	s = fmt::format("{}-{}", 1234);
	s = fmt::format("{}-{}", 1234, 34534, 3245);

	fmt::printf("ddt : %d", 23);
	fmt::printf("ddt : %d", nullptr);
	fmt::printf("ddt : %s", nullptr);
	fmt::printf("ddt : %d %s", 1);

	fmt::format(FMT_STRING("skthi {:d}"), "sdfe");
}

TEST_CASE("logger_other") {
	auto v = (420 * 60 + 1);
	auto b = v / 60;
}

#endif //_TEST_TESTLOGGER_2020_02_27_13_25_36_H