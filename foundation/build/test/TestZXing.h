#ifndef _TEST_TESTZXING_2020_02_27_17_20_34_H
#define _TEST_TESTZXING_2020_02_27_17_20_34_H


namespace TestZXing {

bool writeQRCode(const char* fpath) {
	return ws::libzxing::writeQRCode(
		fpath, "this is at 四川小妖网络科技有限公司 - wstine www.baidu.com",
		128, 128);
}

bool readQRCode(const char* fpaht) {
	ws::ustring ret;
	auto success = ws::libzxing::readQRCode(fpaht, ret);
	printf("%s\n", ret.c_str());
	return success;
}

}


TEST_CASE("ZXing") {
	CHECK(TestZXing::writeQRCode("F:\\lizhi.png"));
	CHECK(TestZXing::readQRCode("F:\\lizhi.png"));
}

#endif //_TEST_TESTZXING_2020_02_27_17_20_34_H