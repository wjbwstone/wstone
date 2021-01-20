#ifndef __TESTREGEX_2020_03_04_16_21_04_H
#define __TESTREGEX_2020_03_04_16_21_04_H

namespace TestRegex {

static void test1() {
	ws::vprint("skldjfklej", 23);
	ws::print("skldfjklej");
	ws::print(ws::System::getRandomString(10, 20));

	ws::SimpleLogSink* _simplelogger = new ws::SimpleLogSink();
	SMLOGD("sdklfjelkj", 234234);
	REQUIRE(true == ws::regex::match("\\d+", "31234", true));
	REQUIRE(true == ws::regex::match("\\d?", "1233", false));

	auto s = "^[1][34578][0-9]{9}$";
	auto re = ws::regex::match(s, "18702854971", true);

	std::vector<ustring> ret;
	REQUIRE(true == ws::regex::match("(\\w+):(\\d+)", "ruby:1234", ret));
	int i = 100;
}

}

TEST_CASE("regex") {
	TestRegex::test1();
}

#endif //__TESTREGEX_2020_03_04_16_21_04_H

