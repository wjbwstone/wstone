#ifndef _TEST_TESTZIP_2020_02_27_14_25_57_H
#define _TEST_TESTZIP_2020_02_27_14_25_57_H

#include <foundation/include/base/Base64.h>
#include <foundation/include/base/System.h>

namespace TestZip {

void testCompress() {
	auto input = ws::System::getRandomString(5000, 100000);
	ustring output; {
		kProfile();
		ws::Zip::compress(input, output, true);
		//output = ws::Base64::encode(output.c_str(), output.size());
	}

	ustring newout;{
		kProfile();
		ws::Zip::uncompress(output, newout, true);
	}
	CHECK(newout == input);

	input =	ws::System::getRandomString(1024, 2048);
	ws::Zip::compress(input, output);
	ws::Zip::uncompress(output, newout);
	CHECK(newout == input);

	auto fpath = "F:\\temp2";
	ws::Zip::compressFile(fpath, "F:\\lizhi.zip");

	ws::Zip::uncompressFile("F:\\lizhi.zip", "F:\\mytemp");
	
	ws::print("tar测试完毕，输入任意键执行下一步");
	_getch();
}

void testLZ4() {
	std::string input;
	input = ws::System::getRandomString(100000, 1000000);
	ustring output; {
		kProfile();
		ws::lz4::compress(input, output);
	}

	ustring newout; {
		kProfile();
		ws::lz4::uncompress(output, newout);
	}
	CHECK(newout == input);

	ustring f = "F:\\lizhi.tar";
	ustring od = "F:\\mao.compress";
	{
		kProfile();
		ws::lz4::compressFile(f, od);
	}

	{
		kProfile();
		ws::lz4::uncompressFile(od, "F:\\mao.uncompress");
	}

	ws::print("tar测试完毕，输入任意键执行下一步");
	_getch();
}

void testTar() {
	ws::ustring oldFile("F:\\temp");
	ws::ustring tarFile("F:\\temp.tar");
	ws::ustring newFile("F:\\newtemp");

	{
		kProfile();
		ws::tar::compressFile(oldFile, tarFile);
	}

	{
		kProfile();
		ws::tar::uncompressFile(tarFile, newFile);
	}

	oldFile = "F:\\temp.pdf";
	tarFile = "F:\\temppdf.tar";
	newFile = "F:\\newtemppdf";

	{
		kProfile();
		ws::tar::compressFile(oldFile, tarFile);
	}

	{
		kProfile();
		ws::tar::uncompressFile(tarFile, newFile);
	}

	ws::print("tar测试完毕，输入任意键执行下一步");
	_getch();
}

}


TEST_CASE("Zip") {
	//TestZip::testTar();
	//TestZip::testLZ4();
	TestZip::testCompress();
}


#endif //_TEST_TESTZIP_2020_02_27_14_25_57_H