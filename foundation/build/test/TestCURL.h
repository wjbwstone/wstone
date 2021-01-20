#ifndef _TEST_TESTCURL_2020_02_27_16_07_44_H
#define _TEST_TESTCURL_2020_02_27_16_07_44_H

namespace TestCURL {

static ws::libcurl s_curl;

#define CURLMGR (&s_curl)

bool myget(const char* url) {
	std::string ret;
	ws::u32 taskid = 0;
	CURLMGR->setDebug(true);
	CURLMGR->setGZip(true);
	CURLMGR->setHandleTimeOout(30);
	CURLMGR->get(url, ret, taskid);
	ws::File::save("F:\\maomao.log", ret, true, true);
	return true;
}

bool getFtp(const char* url) {
	ws::u32 taskid = 0;
	auto ok = CURLMGR->getFtpFile(
		url, "wstone", "123456", "F:\\limaomao.txt", taskid);
	ws::ustring ret;
	//CURLMGR->get("47.97.205.81:28087", ret, taskid);
	return true;
}

bool sendMail() {
	/*auto smtpServer = "smtp.aliyun.com";
	auto fromEmail = "wjbwstone@aliyun.com";
	auto fromEmailPwd = "wjb!@#16888";
	auto toEmail = "wjbwstone@163.com";
	auto emailBody = "this is just a test from me!!!";
	EmailSender email_sender(smtpServer, 25, fromEmail,
		fromEmailPwd, fromEmail);
	email_sender.addRecvEmailAddr(toEmail, toEmail);
	email_sender.setEmailContent("test", emailBody);
	//email_sender.addAttachment(filename2);
	email_sender.send();
	return true;*/

	std::vector<ws::ustring> tos;
	tos.push_back("wjbwstone@163.com");
	ws::u32 taskid = 0;
	auto success = CURLMGR->sendSmtpMail(
		"wjbwstone@aliyun.com",
		"wjb!@#16888",
		"smtp.aliyun.com",
		25,
		"wjbwstone@aliyun.com",
		tos,
		"test",
		"this is a test fro libcurl",
		taskid);
	return success;
}

bool post(const char* url) {
	ws::u32 taskid = 0;
	return CURLMGR->getFile(url, "F:\\bgm.png", taskid);
}

}

TEST_CASE("CURL") {
	TestCURL::myget("https://www.kanzhun.com/");
	TestCURL::sendMail();
	getchar();
}


#endif //_TEST_TESTCURL_2020_02_27_16_07_44_H