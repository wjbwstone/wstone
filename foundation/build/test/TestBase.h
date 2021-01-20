#ifndef _TEST_TESTBASE_2020_02_27_13_43_00_H
#define _TEST_TESTBASE_2020_02_27_13_43_00_H

using namespace ws;

void testCryptSpe() {
	auto minkey = "";
	auto maxkey = System::getRandomString(266, 276);
	auto plain = System::getRandomString(1024, 234566);
	plain = "https://www.baidu.com/a=b&a='++'";
	auto cipher = Crypt::md5(plain, false);
	auto cipherM = MD5::encode(plain);
	CHECK(cipher == cipherM);

	Crypt::rc4Encrypt(plain, maxkey.c_str());
	RC4::encrypt(plain, maxkey.c_str());

	cipher = Crypt::toBase64(plain, true);
	plain = Crypt::fromBase64(cipher, true);
	cipher = Crypt::toBase64(plain, false);
	cipherM = Base64::encode(plain.c_str(), plain.size());
	auto recover = Base64::decode(cipher.c_str(), cipher.size());
	CHECK(cipher == cipherM);
	CHECK(recover == plain);

	cipher = Crypt::md5(__emptyString, false);
	cipherM = MD5::encode(__emptyString);
	CHECK(cipherM == cipher);
	CHECK(Crypt::md5Verify(cipherM, __emptyString));

	cipher = Crypt::md5File("M:\\Hasher.exe", false);
	cipherM = MD5::encodeFile("M:\\Hasher.exe");
	CHECK(cipherM == cipher);
	CHECK(Crypt::md5VerifyFile(cipherM, "M:\\Hasher.exe"));

	ws::ustring fdata;
	CHECK(File::load("M:\\Hasher.exe", fdata));
	auto ivalue = Crypt::crc32(__emptyString);
	auto ivalueM = Crypt::crc32File("M:\\Hasher.exe");
	CHECK(ivalue == ivalueM);
	Crypt::crc32VerifyFile(ivalue, "M:\\Hasher.exe");

	cipher = Crypt::sha1(__emptyString);
	cipher = Crypt::sha256(__emptyString);
	cipher = Crypt::sha512(__emptyString);
	cipherM = Crypt::sha512File("M:\\Hasher.exe");
	cipher = Crypt::hmacSha512(__emptyString, minkey);
	cipher = Crypt::aesEcb128Encrypt(fdata, minkey);
	cipher = Crypt::toBase64(cipher);
	cipher = Crypt::aesEcb256Encrypt(__emptyString, minkey);
	cipher = Crypt::aesCbc128Encrypt(__emptyString, minkey, minkey);

	cipher = ws::Crypt::aesGcm128Encrypt(__emptyString,
		minkey, maxkey.c_str(), minkey);
	recover = ws::Crypt::aesGcm128Decrypt(cipher, 
		minkey, maxkey.c_str(), minkey);
	auto bResult = ws::String::equalNoCase(__emptyString, recover);
	CHECK(bResult);

	cipher = Crypt::chaCha20Encrypt(__emptyString, minkey, minkey);

	std::pair<ws::ustring, ws::ustring> vkeys;
	Crypt::generatePubPriKey(vkeys);
	cipher = Crypt::rsaEncrypt(plain, vkeys.first);
	cipherM = Crypt::rsaDecrypt(cipher, vkeys.second);

	plain = "this is a test for me 中国人";
	cipher = Crypt::hmacMD5(plain, maxkey.c_str());
	cipher = Crypt::hmacSha1(plain, maxkey.c_str());
	cipher = Crypt::hmacSha256(plain, maxkey.c_str());
	cipher = Crypt::hmacSha512(plain, maxkey.c_str());

	ustring chachaMAC;
	cipher = Crypt::chaCha20EncryptPoly1305(plain, maxkey.c_str(),
		maxkey.c_str(), chachaMAC);
	recover = Crypt::chaCha20DecryptPoly1305(cipher, maxkey.c_str(),
		maxkey.c_str(), chachaMAC);
	CHECK(recover == plain);
}

void randMax() {
	ws::vprint("{} {} {} {} {} {} {} {} {} {}",
		ws::System::getEqualProbability(5),
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5), 
		ws::System::getEqualProbability(5));
}

void testTime() {
	auto ms = Time::millisecond();
	auto us = Time::microsecond();
	std::tm ltm = Time::toTmLocalTime();
	auto ut = Time::toUnixTime(ltm);

	auto snow = Time::now();
	gmtime_s(&ltm, &snow);
	auto sut = Time::toUnixTime(ltm) - Time::localTimeBias();

	auto now = ws::Time::toSystemLocalTime();
	ms = Time::toUnixTime(now);
	auto vs = ws::Time::toSystemLocalTime(ms);

	us = Time::toUnixTime("2020-09-8 12:1:09");
	Time::toDatetimeString(us);
	Time::toDateString(us);
	Time::toTimeString(us);
	us = Time::toDayPastTime();
	us = Time::toDayStartTime();
	us = Time::toWeek();
	us = Time::toWeekStartTime();
	us = Time::toMonthStartTime();
	us = Time::toDays();
}

void testRand() {
	auto count = 10;
	while(--count) {
		randMax();
	}
}

void testMemoryFile() {
	MemoryFile mfile;
	mfile.open("M:\\libyxserver.log");

	ws::ustring fdata("this just a test");
	mfile.write(fdata);
	mfile.read(fdata, mfile.size());

	mfile.close();

	mfile.open("M:\\maomao.log", false, 8192);
	fdata = ws::System::getRandomString(8192, 8192);
	mfile.write(fdata);
	mfile.read(fdata, mfile.size());
}

void testHeap() {
	ws::Heap<int, std::less<int>> maxH;
	maxH.push(1);
	maxH.push(8);
	maxH.push(3);
	maxH.push(4);
	maxH.push(5);
	maxH.push(1);
	auto value = maxH.top();
	maxH.erase(0);
	value = maxH.top();
	maxH.push(1);
	maxH.clear();

	maxH.push(1);
	maxH.push(3);
	maxH.push(8);
	maxH.push(3);
	maxH.push(4);
	maxH.push(5);

	value = maxH.top();
	maxH.push(value);
	value = maxH.top();
}

void testClock() {
	ws::Clock s;
	ws::vprint("now : {}", s.nowMS());
	Sleep(1000);
	ws::vprint("now-elasped : {}-{}", s.nowMS(), s.elapsedMS());
	Sleep(1100);
	ws::vprint("now-elasped : {}-{}", s.nowMS(), s.elapsedMS());
	Sleep(2200);
	ws::vprint("now-elasped : {}-{}", s.nowMS(), s.elapsedMS());

	auto p = s;
	Sleep(1000);
	ws::vprint("now-elasped : {}-{}", p.nowMS(), p.elapsedMS());
	ws::print("clock test over");
	_getch();
}

void testPipe() {
	ws::Pipe vp;
	ws::ustring svalue;
	ws::u32 sreadBytes = 0;
	vp.write("123456789987654321");
	vp.read(svalue, sreadBytes);
	vp.read(svalue, sreadBytes);
	sreadBytes = 1;
	vp.read(svalue, sreadBytes);
}

void testSystem() {
	auto rdtring = System::getRandomString(10, 1000);
	auto vr = System::getUniqueRandomNumber(100, 100);
	std::sort(vr.begin(), vr.end());

	bool bValue = false;
	ws::u64 u64Value = 0;
	ws::ustring stringValue;

	stringValue = System::syserr(5);
	ws::vprint("system error 5 : {}", stringValue);

	stringValue = System::syserr(0);
	ws::vprint("system error 0 : {}", stringValue);

	stringValue = System::syserr();
	ws::vprint("system error default : {}", stringValue);

	stringValue = ws::System::getAppDirectory();
	ws::vprint("app directory : {}", stringValue);

	stringValue = ws::System::getAppName();
	ws::vprint("app name : {}", stringValue);

	stringValue = ws::System::getSysTempDir();
	ws::vprint("system temp directory : {}", stringValue);

	stringValue = ws::System::getComputerName();
	ws::vprint("compute name : {}", stringValue);

	u64Value = ws::System::getUniqueID();
	ws::vprint("guid : {}", u64Value);

	u64Value = ws::System::getPhysicsMemory();
	ws::vprint("物理总内存 : {} MB", u64Value);

	u64Value = ws::System::getPhysicsmemoryUsed();
	ws::vprint("物理使用内存 : {} MB", u64Value);

	u64Value = ws::System::getVirtualMemory();
	ws::vprint("虚拟总内存 : {} MB", u64Value);

	u64Value = ws::System::getVirtualMemoryUsed();
	ws::vprint("虚拟使用内存 : {} MB", u64Value);

	u64Value = ws::System::getCPUUsage();
	ws::vprint("cpu使用 : {} ", u64Value);

	u64Value = ws::System::getCPUCount();
	ws::vprint("cpu核心数 : {} ", u64Value);

	CHECK(false == ws::System::isX86System());
	CHECK(true == ws::System::getHostMac(stringValue));

	stringValue = ws::System::getAppDirectory().append("curl.dll");
	ws::System::ModuleVersion mversion(stringValue);
	mversion.print();
	//不存在的文件
	mversion.build(ws::System::getAppDirectory().append("mao.exe").c_str());
	mversion.print();
	//不完整的资源，中文
	mversion.build(ws::System::getAppDirectory().append("libYXAudiod.dll").c_str());
	mversion.print();
	//空资源
	mversion.build(ws::System::getAppDirectory().append("lgj_data.dll").c_str());
	mversion.print();

	std::vector<ustring> vstring;
	ws::System::getHostIP(vstring);
	for(const auto& ip : vstring) {
		ws::vprint("计算机IP地址 : {}", ip);
	}

	std::vector<std::pair<ws::ustring, std::vector<ws::ustring>>> vmacip;
	ws::System::getHostMacIP(vmacip);
	for(const auto& vmac : vmacip) {
		ws::vprint("MAC地址 ： {}", vmac.first);
		for(const auto& ip : vmac.second) {
			ws::vprint("\t IP地址 : {}", ip);
		}
	}

	System::format(NULL);
	ws::print(System::format("this is a test %s", "sklfje"));
	ws::print(System::format("this is a test %s %s", 0, 0));
	System::debugLog("this is a test %s", "sklfje");
	System::outputWindowLog("this is a test %s", "sklfje");
}

void testService() {
	ServiceHelper _he;
	//_he.createService("mao", "this is a test", "F:\\lizhi.exe", vcmd);
	_he.isStoped("mao");
	//_he.isRunning("xysql");
	//_he.stopService("xyupdateserver_t");
	//_he.stopService("xysql");
	//_he.startService("xysql");
	//_he.setServiceDescription("xysql", "这个就是逍遥的 mysql 服务");
}


void testRegistry() {
	Registry rkey(HKEY_CURRENT_USER, "\\software2\\lizhi\\DOGNXI",
		KEY_ALL_ACCESS, Registry::kOpenAlways);
	auto userkey = rkey.getCurrentUserKey(KEY_ALL_ACCESS);
	if(!rkey.save("F:\\test\\mao2\\lizhi.reg")) {
		ws::vprint("保存注册表数据失败 : {}", GetLastError());
	}

	std::vector<ustring> names;
	rkey.getSubKeyName(names);
	names.clear();
	rkey.getValuesKeyName(names);
	rkey.open(HKEY_CURRENT_USER, "software\\lizhi", KEY_ALL_ACCESS);

	rkey.delKey("mao");

	rkey.close();
	rkey.open(HKEY_CURRENT_USER, "software\\lizhi", KEY_ALL_ACCESS);
	rkey.setI32("int", 12);
	rkey.setI64("int64", System::getUniqueID());

	ustring binSetString = "123456789";
	binSetString.push_back('\0');
	binSetString.append("中国人荔枝面看来是的计费shi");
	binSetString.push_back('\0');
	binSetString.append("1234567:");
	rkey.setBin("bin", binSetString);
	rkey.setCString("string", "1234567");

	rkey.flush();
	rkey.close();

	rkey.open(HKEY_CURRENT_USER, "software\\lizhi", KEY_ALL_ACCESS);
	i32 intValue = 0;
	auto ok = rkey.getI32("int", intValue);

	i64 int64Value = 0;
	rkey.getI64("int64", int64Value);

	ustring binString;
	rkey.getBin("bin", binString);
	File::save("F:\\lizhi.log", binString);

	ustring cString;
	rkey.getString("string", cString);

	auto s = rkey.save("F:\\test\\lizhi.reg");

	rkey.delValue("int");
	rkey.close();
	s = rkey.load("F:\\test\\lizhi.reg");
	rkey.flush();
	rkey.close();
}

void testIdentify() {
	Identity a("511023198812144192");
	a.print();
}

void testURLCode() {
	ustring o = "$-_.+!*'(), #&~）（-+";
	auto f = URL::encode(o);
	auto s = URL::decode(f);
	CHECK(o == s);

	o = "http://www/135.png?adb=3453456&db=1234234";
	f = URL::encode(o);
	s = URL::decode(f);
	CHECK(o == s);
}

void testCrypt() {
	const i8* key = "123456789klsadjfklejklajsdflkej123456789";
	ws::ustring _256key = ws::System::getRandomString(259, 259);
	const i8* ekey = _256key.c_str();
	ws::ustring fdata;
	ws::ustring ofdata;
	ws::File::load("M:\\warehouseexe.zip", fdata);
	ofdata = fdata;
	{
		kProfile();
		ws::Crypt::rc4Encrypt(fdata, key);
	}
	{
		kProfile();
		ws::RC4::encrypt(ofdata, key);
	}
	CHECK(fdata == ofdata);

	ws::Crypt::rc4Decrypt(fdata, key);
	ws::RC4::decrypt(ofdata, key);
	CHECK(fdata == ofdata);
	ws::File::save("M:\\warehouseexe.new.zip", fdata, true, true);

	{
		{
			kProfile();
			ws::print(ws::SHA1::encrypt(fdata));
		}

		{
			kProfile();
			ws::print(ws::Crypt::sha1(fdata));
		}

		{
			kProfile();
			ws::print(ws::Crypt::sha1File("M:\\warehouseexe.zip"));
		}
	}

	auto appDir = ws::System::getAppDirectory();
	ustring fpath = appDir + "Microsoft.DTfW.DHL.manifest";
	ustring oldValue;
	ws::File::load(fpath, oldValue);
	auto cipher = ws::Crypt::md5(oldValue, false);
	//cipher = ws::Crypt::md5File("F:\\temp.zip");
	auto bResult = ws::Crypt::md5Verify(cipher, oldValue);
	CHECK(bResult);

	cipher = ws::Crypt::toBase64(oldValue);
	auto recover = ws::Crypt::fromBase64(cipher);
	bResult = ws::String::equalNoCase(oldValue, recover);
	CHECK(bResult);

	auto crcValue = ws::Crypt::crc32(cipher);
	crcValue = ws::Crypt::crc32File(fpath);

	cipher = ws::Crypt::sha1(oldValue);
	cipher = ws::Crypt::sha1File(fpath);
	bResult = ws::Crypt::sha1VerifyFile(cipher, fpath);
	CHECK(bResult);

	cipher = ws::Crypt::sha256(oldValue);
	cipher = ws::Crypt::sha256File(fpath);
	bResult = ws::Crypt::sha256VerifyFile(cipher, fpath);
	CHECK(bResult);

	cipher = ws::Crypt::hmacSha1(oldValue, key);
	cipher = ws::Crypt::hmacMD5(oldValue, key);

	auto newOldValue = oldValue;
	auto longKey = ws::System::getRandomString(1000, 10000);
	ws::Crypt::rc4Encrypt(newOldValue, longKey.c_str());
	ws::Crypt::rc4Decrypt(newOldValue, longKey.c_str());
	bResult = ws::String::equalNoCase(oldValue, newOldValue);
	CHECK(bResult);

	auto b64 = __emptyString;
	cipher = ws::Crypt::aesEcb128Encrypt(oldValue, key);
	b64 = ws::Crypt::toBase64(cipher);
	recover = ws::Crypt::aesEcb128Decrypt(cipher, key);
	bResult = ws::String::equalNoCase(oldValue, recover);
	CHECK(bResult);

	cipher = ws::Crypt::aesEcb256Encrypt(oldValue, key);
	b64 = ws::Crypt::toBase64(cipher);
	recover = ws::Crypt::aesEcb256Decrypt(cipher, key);
	bResult = ws::String::equalNoCase(oldValue, recover);
	CHECK(bResult);
	
	i8* iv = "1234567890!@#$%";
	cipher = ws::Crypt::aesCbc128Encrypt(oldValue, key, iv);
	b64 = ws::Crypt::toBase64(cipher);
	recover = ws::Crypt::aesCbc128Decrypt(cipher, key, iv);
	bResult = ws::String::equalNoCase(oldValue, recover);
	CHECK(bResult);

	i8* auth = "213455";
	cipher = ws::Crypt::aesGcm128Encrypt(oldValue, key, iv, auth);
	recover = ws::Crypt::aesGcm128Decrypt(cipher, key, iv, auth);
	bResult = ws::String::equalNoCase(oldValue, recover);
	CHECK(bResult);

	cipher = ws::Crypt::chaCha20Encrypt(oldValue, key, iv);
	recover = ws::Crypt::chaCha20Decrypt(cipher, key, iv);
	bResult = ws::String::equalNoCase(oldValue, recover);
	CHECK(bResult);
	
	std::pair<ws::ustring, ws::ustring> keys;
	CHECK(ws::Crypt::generatePubPriKey(keys));
	
	cipher = ws::Crypt::rsaEncrypt(oldValue, keys.first);
	recover = ws::Crypt::rsaDecrypt(cipher, keys.second);
	CHECK(oldValue == recover);
}

void listAllFile(const ustring& path) {
	Directory d(path);
	if(!d.empty()) {
		d.print();
	}

	while(d.next()) {
		d.print();
		if(d.isDirectory()) {
			listAllFile(d.fullPath());
		}
	}
}

void testPathCell(const ustring& path) {
	Path p(path);
	p.print();
}

void testPath() {
	testPathCell("");
	testPathCell("dongxi");
	testPathCell("dotn.exe");
	testPathCell("\\dong.exe");
	testPathCell("\\dong");
	testPathCell("F:\\mao.log");
	testPathCell("F:\\mao");
	testPathCell("F:\\mao\\lizhi\\lizhi.exe");
	testPathCell("F:\\mao2\\lizhi\\lizhi.exe");
	testPathCell("F:\\mao\\lizhi\\lizhi.exe");
	testPathCell("D:\\company\\projects\\yxmanger\\trunk\\1.0"
		"\\programs\\product\\yxconsole\\makesign.bat");
	testPathCell("D:\\company\\projects\\yxmanger\\trunk\\1.0"
		"\\programs\\product\\yxconsole\\makesign.b");
	testPathCell("D:\\company\\projects\\yxmanger\\trunk\\1.0"
		"\\programs\\product\\yxconsole\\");
	testPathCell("F:\\skdfjaskldjflkasjdflkjelkj\\asdklfjaklfjelkj\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\"
		"klajsdfkljaskldjflkjekljfalksdjfe\\klasjdfkljlkajwelkfje\\");
	auto appdir = System::getAppDirectory();
	testPathCell(appdir + "vld_x86.dll");
	testPathCell("mao/fe//fkljfe.exe");
	auto v = 10;
}

void testDirecotry() {
	System::getAppDirectory();
	System::getAppName();
	listAllFile("F:\\test");
	std::vector<ustring> vpaths;
 	Directory::walkDirectory("F:\\temp", vpaths);

	/*auto oldDir = "F:\\testem";
	auto newDir = "F:\\dirTest";
	Directory::copyDirectory(oldDir, newDir);

	oldDir = "F:\\temp";
	newDir = "F:\\dirTemp";
	Directory::copyDirectory(oldDir, newDir);

	Directory::deleteDirectory(newDir);*/
	
	auto v = 10;
}

void testAuxUtils() {
	auto v = 9;
	auto t = 17;
	t = ws::roundUp(t, v);
	t = ws::roundUp(17, 8);
	t = ws::roundUp(17, 3);
	t = ws::roundUp(17, 6);
	t = ws::roundUp(17, 16);
	t = ws::roundUp(16, 2);
	t = ws::roundUp(16, 8);
	t = ws::roundUp(17, 17);
	t = ws::roundUp(16, 16);
	t = ws::roundUp(124, 123);
}

void tThreadRun() {
	while(true) {
		Sleep(1000);
		ws::print("running thread!!!");
	}
}

void testThread() {
	auto pv = ws::Time::toTmLocalTime();


	ws::Thread _localThread;
	ws::u32 _lkey = 0;
	_localThread.createKey(_lkey);
	const char* cc =  "1234567";
	_localThread.setKeyValue(_lkey, (void*)cc);
	const char* p;
	_localThread.getKeyValue(_lkey, (void*&)p);

	_localThread.run(&tThreadRun);

	getchar();
	_localThread.suspend();
	_localThread.suspend();
	getchar();
	_localThread.wakeup();
	getchar();
}

void moniterCallback(ws::FolderMonitor::FileActionType type,
					 const ws::ustring& newpath,
					 const ws::ustring& oldpath) {
	static ws::u32 addcount = 0;
	static ws::u32 removedcount = 0;
	static ws::u32 renanmedcount = 0;
	static ws::u32 modifiedcount = 0;
	switch(type) {
		case ws::FolderMonitor::Added:
			ws::vprint("添加文件 {} : {}", ++addcount, newpath);
			break;
		case ws::FolderMonitor::Removed:
			ws::vprint("删除文件 {} : {}", ++removedcount, newpath);
			break;
		case ws::FolderMonitor::Modified:
			ws::vprint("修改文件 {} : {}", ++modifiedcount, newpath);
			break;
		case ws::FolderMonitor::Renamed:
			ws::vprint("重命名文件 {} : {} -> {}",
				++renanmedcount, oldpath, newpath);
			break;
	}
}

void testForderMoniter() {
	ws::FolderMonitor _moniter;
	_moniter.setFileEventCallback(&moniterCallback);
	_moniter.startMonitor("F:\\temp", true);
	_moniter.startMonitor("F:\\wo", true);
	getchar();
	_moniter.stopMonitorAll();
}

void testCharactorCode() {
	ws::ustring a = "화목하다";
	auto UA = Utf8ToUnicode(a);
	auto aa = ws::WStringToString<949>()(UA);
	auto aaa = ws::StringToWString<949>()(aa);
	auto oa = UnicodeToUtf8(aaa);
}

void testProcess() {
	std::vector<ws::ustring> vcmd;
	vcmd.push_back("M:\\New folder\\mao.exe");
	vcmd.push_back("F:\\mao.exe");
	System::execBin("M:\\Hasher.exe");
	System::execBat("M:\\New folder\\mao.cmd");

	Process::createProcess("M:\\New folder\\Hasher.exe", vcmd);

	std::vector<ProcessPtr> vprocess;
	CHECK(Process::walkProcess(vprocess));
	for(const auto& it : vprocess) {
		ws::print(it->getPath());
	}

	auto pP = Process::findProcess("wps.exe");
	ws::print(pP->getPath());
	pP->getPath();

	//Process::killProcess("explorer.exe");

	pP = Process::createProcess("M:\\Hasher.exe", vcmd);
	Process::currentProcess()->getExecuteName();
	pP->terminate();

	auto _pause = 100;
}

void testCycleList() {
	List<int> ilist;
	ilist.size();
	ilist.push(0);
	ilist.push(1);
	ilist.push(2);
	ilist.push(3);
	ilist.push(4);
	while(ilist.next()) {
		printf("%d\n", ilist.operator->());
	}
	ilist.next();
	ilist.operator->();
	ilist.clear(1);
	auto v = ilist.operator->();

	ilist.move(ilist.size() - 1);
	while(ilist.next()) {
		printf("%d\n", ilist.operator->());
	}

	ilist.clear(3);
	while(ilist.next()) {
		printf("%d\n", ilist.operator->());
	}

	ilist.clear(1);
	while(ilist.next()) {
		printf("%d\n", ilist.operator->());
	}

	v = ilist.operator->();
	ilist.next();
	ilist.clear(1);
	ilist.next();
	ilist.push(2);
	ilist.clear(0);
	v = ilist.operator->();
	ilist.clearAll();
	ilist.push(3);
	ilist.push(4);
	ilist.push(5);
	v = ilist.get(1);
	v = ilist.operator->();
	ilist.move(2);
	while(ilist.next()) {
		printf("%d\n", ilist.operator->());
	}

	v = ilist.operator->();
	auto b = ilist.next();
	ilist.move(0);

	while(ilist.next()) {
		printf("%d\n", ilist.operator->());
	}

	Console::write("List test over", Console::kGreen);
	_getch();
}

void workForPrint(ws::u32 index) {
	ws::vprint("threadpool the message is : {}", index);
	Sleep(10);
}

void testThreadPool() {
	ustring name = "中国人";
	ThreadPool _thread(2);
	do {
		_thread.post(std::bind(&workForPrint, 1000));
		_thread.post(std::bind(&workForPrint, 2000));
		_thread.post(std::bind(&workForPrint, 11000));
		_thread.post(std::bind(&workForPrint, 11000));
		Sleep(100);
		_thread.post(std::bind(&workForPrint, 11000));
		_thread.post(std::bind(&workForPrint, 100000));
		ws::vprint("threadpool size : {}", _thread.size());
		ws::vprint("wait {} seconds", 10);
		Sleep(10000);
		_thread.stop();
	} while(false);
}

void workTimer(ws::u32 tid) {
	ws::vprint("[{}]当前ID ： {}", ws::Time::toDatetimeString(), tid);
}

void TimerHandler(ws::u32 tid) {
	std::cout << "TimerHandler" << tid << std::endl;
}

void testTimer() {
	Timer _timer;
	_timer.start();
	Sleep(1000);
	_timer.add(1, 6000, std::bind(&workTimer, 1));
	_timer.add(2, 6000, std::bind(&workTimer, 2));
	_timer.add(3, 12000, std::bind(&workTimer, 3));
	Sleep(1000);
	_timer.suspend(1);
	_timer.suspend(3);
	getchar();
	_timer.wakeup(1);
	_timer.suspend(3);
	getchar();
	_timer.erase(2);
	_timer.exist(1);
	getchar();
	_timer.wakeup(3);
	getchar();
	_timer.add(2, 2000, std::bind(&workTimer, 2));
	getchar();
	_timer.stop();
	getchar();
	_timer.start();
	_timer.add(2, 2000, std::bind(&workTimer, 2));
	getchar();
	_timer.stop();
}

void testString() {
	ustring f = "中国人们SSSSS";
	CHECK(true == String::endWith(f, "SS"));
	CHECK(false == String::endWith(f, "S#"));
	CHECK(false == String::startWith(f, "S#"));
	CHECK(true == String::startWith(f, "中国人"));
	CHECK(true == String::startWith(f, "中"));

	ustring mao = "23;23;2312";
	std::vector<ustring> vmao;
	String::split(mao, ";", vmao);
	CHECK(3 == vmao.size());
	CHECK("2312" == vmao[2]);

	vmao.clear();
	String::split(mao, "23", vmao);
	CHECK(4 == vmao.size());
	CHECK("12" == vmao[2]);

	vmao.clear();
	String::split(mao, "s", vmao);
	CHECK(0 == vmao.size());

	vmao.clear();
	mao = "";
	String::split(mao, "", vmao);

	vmao.clear();
	mao = "aaaa";
	String::split(mao, "a", vmao);

	vmao.clear();
	mao = "aaaa";
	String::split(mao, "aa", vmao);

	String::replace(f, "S", "中国");
	CHECK("中国人们中国中国中国中国中国" == f);

	f = "0X123";
	CHECK(291 == String::hexToI32(f.c_str()));
	f = "123";
	CHECK(291 == String::hexToI32(f.c_str()));

	f = "0A";
	CHECK(10 == String::hexToI32(f.c_str()));

	f = "0F";
	CHECK(15 == String::hexToI32(f.c_str()));

	f = "1F";
	CHECK(31 == String::hexToI32(f.c_str()));

	f = "10";
	CHECK(16 == String::hexToI32(f.c_str()));

	u32 bf = 12345;
	f = "11000000111001";
	CHECK(f == String::u32ToBinString(bf));
	CHECK("0" == String::u32ToBinString(0));
	CHECK("1" == String::u32ToBinString(1));
	CHECK("10" == String::u32ToBinString(2));
	CHECK("11" == String::u32ToBinString(3));
	CHECK("1000" == String::u32ToBinString(8));
	CHECK("10000000000" == String::u32ToBinString(1024));

	CHECK("10000000000000000000000000000000000000000" ==
		String::u64ToBinString(1099511627776));
	CHECK(String::binStringToU64(
		"10000000000000000000000000000000000000000") == 1099511627776);

	CHECK(String::binStringToU32(f) == bf);
	CHECK(String::binStringToU32("0") == (0));
	CHECK(String::binStringToU32("1") == (1));
	CHECK(String::binStringToU32("10") == (2));
	CHECK(String::binStringToU32("11") == (3));
	CHECK(String::binStringToU32("1000") == (8));
	CHECK(String::binStringToU32("10000000000") == (1024));

	CHECK("11111111111111111111111111111111" ==
		String::u32ToBinString(UINT_MAX));
}

void testConsole() {
	Console::setTitle("中饭");

	Console::write("默认颜色");
	Console::write("红色的内容", Console::kRed);
	Console::write("蓝色的内容", Console::kBlue);
	Console::write("绿的内容", Console::kGreen);
	Console::write("默认颜色");

	Console::write("测试完毕 console");
	Console::write("big test to input skldfjelkjklasjdfkljklsdjflkj"
		"alksdjflkj aslkdjflkjaklsdjfkalsjdfklajsdklfjalskdjfklasdjflkje");
	ws::ustring fdata;
	Console::read(fdata);
	Console::write(fdata);
	Console::read(fdata, 0);
	Console::read(fdata, 10);
}

void testResource() {
	auto wpath = ws::System::getAppDirectory();
	auto moduleFile = (wpath + "serverDetectd.exe");

	Win32Resource::unpack(moduleFile.c_str(), "exe1",
		120, (wpath + "maomao.exe").c_str());
	Win32Resource::unpack(moduleFile.c_str(), "exe1", 121,
		(wpath + "maomao.exe").c_str());
	Win32Resource::unpack(moduleFile.c_str(), "exe1", 123, 
		(wpath + "maomao.exe").c_str());
	Win32Resource::unpack(moduleFile.c_str(), "exe1", 126,
		(wpath + "maomao.hpp").c_str());
	Win32Resource::unpack(moduleFile.c_str(), "U1", 100,
		(wpath + "maomao2.exe").c_str());

	Win32Resource::unpack(moduleFile.c_str(), 
		"U1", 104, (wpath + "myrtext.hpp").c_str());
	Win32Resource::unpack(moduleFile.c_str(), 
		"u1", 105, (wpath + "myrtext.hpp").c_str());

	Win32Resource::pack(moduleFile.c_str(), 
		"U1", 101, (wpath + "maomao.exe").c_str());

	Win32Resource::unpack(moduleFile.c_str(), "U1", 101,
		(wpath + "maomao_ui_101.exe").c_str());

	moduleFile = ws::System::getAppFullPath();
	Win32Resource::unpack(moduleFile.c_str(), "U1", 101,
		(wpath + "maomao_ui_101.exe").c_str());
}

class MemoryItem {
public:
	MemoryItem(u32 v = 0) : _int(v) {

	}

	~MemoryItem() {
		_int = 0;
		_str.clear();
	}

	void setInt(u32 v) { _int = v; }
	u32 getInt() const { return _int; }
	void setString(const ustring& v) { _str = v; }
	const ustring& getString() const { return _str; }

protected:
	u32 _int;
	ustring _str;
};

void testMemoryPool_ManyChunk(
	std::vector<MemoryItem*>& vitems,
	MemoryPool& pool,
	u32 objCount) {
	static u32 __count = 0;
	for(u32 i = 0; i < objCount; ++i) {
		auto* p = pool.create<MemoryItem>();
		p->setInt(++__count);
		p->setString(fmt::format("{}", __count));
		ws::vprint("{}-{}", p->getInt(), p->getString());
		vitems.push_back(p);
	}
}

void testMemoryPool() {
	u32 objCount = 10;
	MemoryPool pool(sizeof(MemoryItem), objCount, 3);

	std::vector<MemoryItem*> vitems;
	{
		kProfile();
		testMemoryPool_ManyChunk(vitems, pool, objCount);
		testMemoryPool_ManyChunk(vitems, pool, objCount * 2 - 1);
	}
	
	for(auto it : vitems) {
		pool.destroy(it);
	}

	vitems.clear();
	testMemoryPool_ManyChunk(vitems, pool, objCount);
	testMemoryPool_ManyChunk(vitems, pool, objCount * 2 - 1);
	for(auto it : vitems) {
		pool.destroy(it);
	}

	auto p = new(pool.alloc()) MemoryItem(122);
	p->setString(fmt::format("{}", 122));
	ws::vprint("{}-{}", p->getInt(), p->getString());
	p->~MemoryItem();

	p = new(pool.alloc()) MemoryItem(123);
	p->setString(fmt::format("{}", 123));
	ws::vprint("{}-{}", p->getInt(), p->getString());
	p->~MemoryItem();

	p = new(pool.alloc()) MemoryItem(124);
	p->setString(fmt::format("{}", 124));
	ws::vprint("{}-{}", p->getInt(), p->getString());
	p->~MemoryItem();

	pool.release((void*)0x12ff125F);

	auto pChar = pool.create<i8>();
	*pChar = 2;
	pool.destroy(pChar);
}

void testQBuffer() {
	ws::ByteBuffer buf(6);
	buf.writeU16(0x1020);
	buf.writeU32(0x10203040);
	auto vint = buf.peekU16(0);
	auto vsint = buf.peekU32(2);
	vint = vsint = 0;
	vint = buf.readU16();
	vsint = buf.readU32();
	vsint = buf.readU64();
	auto p = buf.read(2);
	buf.write("this is a test", 10);
	p = buf.read(10);
	buf.clear();
}

TEST_CASE("Common") {
	testThread();
	testTime();
	testQBuffer();
	testConsole();
	testMemoryPool();
	testResource();
	testURLCode();
	testCycleList();
	testClock();
	testCryptSpe();
	testCharactorCode();
	testRand();
	testMemoryFile();
	testHeap();
	testString();
	testProcess();
	testDirecotry();
	testTimer();
	testThreadPool();
	testRegistry();
	testPath();
	testCrypt();
}

#endif //_TEST_TESTBASE_2020_02_27_13_43_00_H