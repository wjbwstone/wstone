#include <foundation/include/base/System.h>

#include <helper/Format.h>
#include <helper/CommandLine.h>
#include <helper/StringConvert.h>

#include <foundation\include\json\json.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/Time.h>
#include <foundation/include/base/String.h>
#include <foundation/include/base/Process.h>
#include <foundation/include/base/Console.h>

namespace ws {

namespace System {

static bool __validFormat(const i8* format) {
	auto p = format;
	bool needCheck = false;
	while(nullptr != p && '\0' != *p) {
		if(needCheck) {
			needCheck = false;
			if(nullptr == strchr("idusSp%acxXofFnl.+-#0", *p)) {
				assert(false && "Incorrect format character!!!");
				return false;
			}

		} else if('%' == *p) {
			needCheck = true;
		}

		++p;
	}

	return (nullptr != format);
}

static void __moduleValue(void* d, u32 id, const i8* key, ustring& val) {
	assert(nullptr != d && nullptr != key);
	
	val.clear();
	u32 bytes = 0;
	i8 pkey[64] = {0};
	sprintf_s(pkey, "\\StringFileInfo\\%08x\\%s", id, key);

	wchar* ret = nullptr;
	if(VerQueryValueW(d, Utf8ToUnicode(pkey).c_str(), (void**)&ret, &bytes)) {
		val = UnicodeToUtf8(ret);
	}
}

ModuleVersion::ModuleVersion(const i8* fpath) {
	build(fpath);
}

ModuleVersion::ModuleVersion(const ustring& fpath)  {
	build(fpath.c_str());
}

void ModuleVersion::build(const i8* fpath) {
	_companyname.clear();
	_filedesc.clear();
	_fileversion.clear();
	_internalname.clear();
	_leagalcopyright.clear();
	_originalfilename.clear();
	_productname.clear();
	_productversion.clear();
	if(nullptr == fpath) {
		return;
	}

	u32 bytes = GetFileVersionInfoSizeW(Utf8ToUnicode(fpath).c_str(), 0);
	if(0 == bytes) {
		return;
	}

	ws::ustring d;
	d.resize(bytes);
	auto data = (void*)(d.data());
	if(!GetFileVersionInfoW(Utf8ToUnicode(fpath).c_str(), 0, bytes, data)) {
		return;
	}

	bytes = 0;
	u32* pLanguageID = nullptr;
	if(!VerQueryValueW(data, L"\\VarFileInfo\\Translation", 
		(void**)&pLanguageID, &bytes)) {
		return;
	}
	auto id = MAKELONG(HIWORD(pLanguageID[0]), LOWORD(pLanguageID[0]));

	__moduleValue(data, id, "ProductName", _productname);
	__moduleValue(data, id, "CompanyName", _companyname);
	__moduleValue(data, id, "FileDescription", _filedesc);
	__moduleValue(data, id, "FileVersion", _fileversion);
	__moduleValue(data, id, "InternalName", _internalname);
	__moduleValue(data, id, "ProductVersion", _productversion);
	__moduleValue(data, id, "LegalCopyright", _leagalcopyright);
	__moduleValue(data, id, "OriginalFilename", _originalfilename);
}

ustring ModuleVersion::print(bool printToStdout /* = true */) const {
	FastJson jroot;
	jroot["fileversion"] = _fileversion;
	jroot["productversion"] = _productversion;
	jroot["productname"] = _productname;
	jroot["internalname"] = _internalname;
	jroot["originalfilename"] = _originalfilename;
	jroot["filedesc"] = _filedesc;
	jroot["companyname"] = _companyname;
	jroot["leagalcopyright"] = _leagalcopyright;

	auto v = jroot.toPrettyString();
	if(printToStdout) {
		ws::print(v);
	}

	return v;
}

static u64 __getWorkid() {
	u64 workid = 0;
	ustring macaddr;
	if(getHostMac(macaddr)) {
		std::vector<ustring> macs;
		String::split(macaddr, "-", macs);
		for(const auto& it : macs) {
			if(!it.empty()) {
				workid += (String::hexToI32(it.c_str()));
			}
		}

	} else {
		workid = 9527 + getEqualProbability(100000);
	}

	return workid;
}

u64 getUniqueID() {
	/**
	 * 雪花算法结构
	 * 第一位未使用，接下来的41位为毫秒级时间(41位的长度可以使用69年)，
	 * 然后是10位workerId,最后12位是毫秒内的计数
	 * 本方法第一位利用起来，可使用 69 * 2 年
	 */
	static u32 sequenceid = 0x0;
	static u64 workid = (__getWorkid() & 0x3FF) << 12;

	u64 uid = Time::millisecond() << 22;
	sequenceid = InterlockedIncrement(&sequenceid);
	return (uid | workid | (sequenceid & 0xFFF));
}

static u32 __getRand(u32 range) {
	if(0 == range) {
		return 0;
	}

	u32 number = 0;
	u32 tempt = range;
	while(tempt > 0) {
		number++;
		tempt = tempt / 10;
	}

	u32 ret = 0;
	while(number--) {
		ret = (10 * ret) + (rand() % 10);
	}

	return (ret >= range) ? __getRand(range) : ret;
}

void getEqualProbability(u32 maxvalue, u32 count, std::vector<u32>& ret) {
	/*if(0 != count) { //模拟
		srand(std::random_device()());
		while(count--) {
			ret.push_back(__getRand(maxvalue));
		}
	}*/

	if(0 != count) {
		std::random_device dev;
		std::mt19937 random(dev());
		std::uniform_int_distribution<unsigned> dis(0, maxvalue - 1);
		while(count--) {
			ret.push_back(dis(random));
		}
	}
}

u32 getEqualProbability(u32 maxvalue) {
	srand(std::random_device()());
	return __getRand(maxvalue);
}

ustring getRandomString(u32 minCount, u32 maxCount) {
	if(minCount > maxCount) {
		assert(maxCount >= minCount && "Bad arguments!!!");
		maxCount = minCount;
	}

	ustring ret;
	auto bytes = getEqualProbability(maxCount - minCount) + minCount;
	ret.resize(bytes);
	srand(std::random_device()());
	while(bytes--) {
		ret[bytes] = rand() % 127 + 1;
	}

	return ret;
}

std::vector<u32> getUniqueRandomNumber(u32 maxValue, u32 count) {
	std::vector<u32> vret;
	if(0 == maxValue) {
		return vret;
	}

	auto sequence = new u32[maxValue];
	if(nullptr == sequence) {
		assert(false && "Oops!!!, Your maxValue is big");
		return vret;
	}

	for(u32 i = 0; i < maxValue; i++) {
		sequence[i] = i;
	}

	std::random_device device;
	std::mt19937 random(device());
	for(u32 i = 0, end = maxValue; i < maxValue; i++) {
		if(vret.size() == count) {
			break;
		}

		auto k = random() % (end--);
		vret.push_back(sequence[k]);
		sequence[k] = sequence[end];
	}

	ws::Safedelete::checkedDeleteArray(sequence);
	return vret;
}

u64 getMachineMemory(u32 type) {
	MEMORYSTATUSEX ms;
	ms.dwLength = sizeof(ms);
	if(!::GlobalMemoryStatusEx(&ms)) {
		assert(false);
		return 0;
	}

	switch(type) {
		case 0: return (ws::u64)ms.ullTotalPhys; //物理总内存
		case 1: return (ws::u64)ms.ullAvailPhys; //物理可有内存
		case 2: return (ws::u64)(ms.ullTotalPhys - ms.ullAvailPhys);
		case 3: return (ws::u64)ms.ullTotalVirtual; //虚拟总内存
		case 4: return (ws::u64)ms.ullAvailVirtual; //虚拟可有内存
		case 5: return (ws::u64)(ms.ullTotalVirtual - ms.ullAvailVirtual);
	}

	assert(false);
	return 0;
}

u64 getPhysicsMemory() {
	return getMachineMemory(0) / (1024 * 1024);
}

u64 getPhysicsmemoryUsed() {
	return getMachineMemory(2) / (1024 * 1024);
}

u64 getVirtualMemory() {
	return getMachineMemory(3) / (1024 * 1024);
}

u64 getVirtualMemoryUsed() {
	return getMachineMemory(5) / (1024 * 1024);
}

static u64 __escapedFileTime(FILETIME pre, FILETIME after) {
	auto a = (u64)pre.dwHighDateTime << 32 | pre.dwLowDateTime;
	auto b = (u64)after.dwHighDateTime << 32 | after.dwLowDateTime;
	return (b - a);
}

u32 getCPUUsage() {
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	auto preidleTime = idleTime;
	auto prekernelTime = kernelTime;
	auto preuserTime = userTime;

	Sleep(1000);
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	auto idle = __escapedFileTime(preidleTime, idleTime);
	auto kernel = __escapedFileTime(prekernelTime, kernelTime);
	auto user = __escapedFileTime(preuserTime, userTime);
	return (u32)((kernel + user - idle) * 100 / (kernel + user));
}

u32 getCPUCount() {
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	return (4 > si.dwNumberOfProcessors) ? 4 : si.dwNumberOfProcessors;
}

ustring getAppFullPath() {
	return Process::getProcessPath().getPath();
}

ustring getAppDirectory() {
	auto p = getAppFullPath();
	auto found = p.find_last_of(L'\\');
	if(ustring::npos != found) {
		p.erase(found + 1);
	}
	
	return p;
}

ustring getAppName() {
	auto p = getAppFullPath();
	auto found = p.find_last_of(L'\\');
	if(ustring::npos != found) {
		p.erase(0, found + 1);
	}

	return p;
}

ustring getComputerName() {
	wstring ret;
	DWORD len = MAX_COMPUTERNAME_LENGTH + 1;
	ret.resize(len);
	GetComputerNameW((wchar*)ret.c_str(), &len);
	return UnicodeToUtf8(ret.substr(0, len));
}

ustring getSysTempDir() {
	wstring ret;
	ret.resize(MAX_PATH * 2);
	auto len = GetTempPathW((MAX_PATH * 2) - 1, (wchar*)ret.c_str());
	ret.resize(len);
	return UnicodeToUtf8(ret);
}

bool isX86System() {
	BOOL isX64 = false;
	IsWow64Process(GetCurrentProcess(), &isX64);
	return !isX64;
}

bool isWindwsXP() {
	OSVERSIONINFOEX os = {0};
	os.dwOSVersionInfoSize = sizeof(os);
	::GetVersionEx((OSVERSIONINFO*)&os);
	return (os.dwMajorVersion == 5) && 
		(os.dwMinorVersion == 1 || os.dwMinorVersion == 2);
}

bool isWindows7() {
	OSVERSIONINFOEX os = {0};
	os.dwOSVersionInfoSize = sizeof(os);
	::GetVersionEx((OSVERSIONINFO*)&os);
	return (os.dwMajorVersion == 6 && os.dwMinorVersion == 1);
}

bool getHostIP(ustring& ip) {
	ip.clear();
	std::vector<ustring> ips;
	if(getHostIP(ips)) {
		if(!ips.empty()) {
			ip = ips[0];
		}

		return true;
	}

	return false;
}

bool getHostMac(ustring& mac) {
	mac.clear();
	std::vector<std::pair<ustring, std::vector<ustring>>> macs;
	if(getHostMacIP(macs)) {
		if(!macs.empty()) {
			mac = macs.begin()->first;
		}

		return true;
	}

	return false;
}

bool getHostMacIP(
	std::vector<std::pair<ustring, std::vector<ustring>>>& infos) {
	DWORD bsize = sizeof(IP_ADAPTER_ADDRESSES);
	auto addresses = (IP_ADAPTER_ADDRESSES*)malloc(bsize);
	if(nullptr == addresses) {
		return false;
	}

	if(ERROR_BUFFER_OVERFLOW == 
		GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, addresses, &bsize)) {
		free(addresses);
		addresses = (IP_ADAPTER_ADDRESSES*)malloc(bsize);
		if(nullptr == addresses) {
			return false;
		}
	}

	bool success = false;
	if(NO_ERROR == 
		GetAdaptersAddresses(AF_UNSPEC, 0, nullptr, addresses, &bsize)) {
		for(auto pos = addresses; pos != nullptr; pos = pos->Next) {
			if(6 != pos->PhysicalAddressLength) {
				continue;
			}

			std::vector<ustring> ips;
			for(auto v = pos->FirstUnicastAddress; nullptr != v; v = v->Next) {
				i8 ipbuffer[256] = {0};
				auto ipaddr = v->Address.lpSockaddr;
				if(AF_INET == ipaddr->sa_family) {
					inet_ntop(PF_INET,
						&((sockaddr_in*)ipaddr)->sin_addr,
						ipbuffer, sizeof(ipbuffer));
				} else if(AF_INET6 == ipaddr->sa_family) {
					inet_ntop(PF_INET6,
						&((sockaddr_in6*)ipaddr)->sin6_addr,
						ipbuffer, sizeof(ipbuffer));
				}
				ips.push_back(ipbuffer);
			}

			i8 buffer[32] = {0};
			sprintf_s(buffer, "%02X-%02X-%02X-%02X-%02X-%02X",
				ws::i32(pos->PhysicalAddress[0]),
				ws::i32(pos->PhysicalAddress[1]),
				ws::i32(pos->PhysicalAddress[2]),
				ws::i32(pos->PhysicalAddress[3]),
				ws::i32(pos->PhysicalAddress[4]),
				ws::i32(pos->PhysicalAddress[5]));
			success = true;
			infos.push_back(std::make_pair(buffer, ips));
		}
	}

	free(addresses);
	return success;
}

bool getHostIP(std::vector<ustring>& vip) {
	WSADATA tdata;
	WSAStartup(MAKEWORD(2, 2), &tdata);

	i8 name[MAX_PATH] = {0};
	if(0 == gethostname(name, sizeof(name))) {
		hostent* he = gethostbyname(name);
		if(nullptr == he) {
			WSACleanup();
			return false;
		}

		i8 ip[16] = {0};
		for(int n = 0; he->h_addr_list[n]; ++n) {
			strcpy_s(ip, inet_ntoa(*(struct in_addr*)he->h_addr_list[n]));
			vip.push_back(ip);
		}
	}

	WSACleanup();
	return true;
}

bool restartSelf(const std::vector<ustring>& vcmd) {
	auto appname = getAppName();
	auto filePath = getAppFullPath();
	auto runcmd = fmt::format(
		"/c taskkill /f /im \"{}\" & start \"\" \"{}\" {}", 
		appname, filePath, CommandLine::parse(vcmd));
	auto ret = ::ShellExecuteW(nullptr, L"open", L"cmd", 
		Utf8ToUnicode(runcmd).c_str(), 
		Utf8ToUnicode(getAppDirectory()).c_str(), SW_NORMAL);
	return (32 < (int)ret);
}

bool execBin(const ustring& fpath, const std::vector<ustring>& vcmd) {
	Path _path(fpath);
	auto cmds = CommandLine::parse(vcmd);
	auto qpath = CommandLine::quoteString(_path);
	auto ret = ShellExecuteW(nullptr, L"open", 
		Utf8ToUnicode(qpath).c_str(),
		Utf8ToUnicode(cmds).c_str(), 
		Utf8ToUnicode(_path.parentPath()).c_str(), SW_NORMAL);
	return (32 < (int)ret);
}

bool execBat(const ustring& batcmd) {
	auto cmd = fmt::format(L"/c \"{}\"", Utf8ToUnicode(batcmd));
	auto ret = ShellExecuteW(nullptr, L"open", L"cmd", cmd.c_str(), 
		nullptr, SW_HIDE);
	return (32 < (int)ret);
}

ustring syserr(u32 errid) {
	LPWSTR v;
	auto n = FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errid, 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&v, 0, nullptr);
	if(0 != n) {
		if(2 <= n && '\r' == v[n - 2] && '\n' == v[n - 1]) {
			n -= 2;
			v[n] = '\0';
		}

		auto r = UnicodeToUtf8((wchar*)v);
		LocalFree(v);
		return r;
	}

	return "unknown";
}

ustring vformat(const i8* pattern, va_list args) {
	if(nullptr != pattern && __validFormat(pattern)) {
		auto len = _vscprintf(pattern, args) + 1;
		ustring ret(len, '\0');
		vsprintf_s(&ret[0], len, pattern, args);
		ret.erase(--ret.end());
		return ret;
	}

	return __emptyString;
}

ustring format(const i8* pattern, ...) {
	va_list args;
	va_start(args, pattern);
	auto v = vformat(pattern, args);
	va_end(args);
	return v;
}

void debugLog(const i8* pattern, ...) {
#ifdef _DEBUG
	va_list args;
	va_start(args, pattern);
	Console::write(vformat(pattern, args));
	va_end(args);
#endif
}

void outputWindowLog(const i8* pattern, ...) {
	va_list args;
	va_start(args, pattern);
	auto v = vformat(pattern, args).insert(0, "WJB: ");
	OutputDebugStringW(Utf8ToUnicode(v).c_str());
#ifdef _DEBUG
	OutputDebugStringW(L"\r\n");
#endif
	va_end(args);
}

} //Systemtool

} //wstone