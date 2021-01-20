#ifndef __TESTBUGTRAP_2020_03_24_12_23_10_H
#define __TESTBUGTRAP_2020_03_24_12_23_10_H


namespace TestBugtrap {

bool test() {
	//ws::IBugTrap::install(L"192.168.3.128", 18081); //本地虚拟机
	//ws::IBugTrap::install(L"39.108.58.192", 80); //
	ws::IBugTrap::install();
	char* p = nullptr;
	std::string a = p;
	std::string mao = "小妖";
	auto wmao = Utf8ToUnicode(mao);
	auto h = FindWindowW(nullptr, wmao.c_str());
	printf("%s\n", mao.c_str());
	return false;
}


}

TEST_CASE("Bugtrap") {
	CHECK(TestBugtrap::test());
}

#endif //__TESTBUGTRAP_2020_03_24_12_23_10_H