#include "stdafx.h"

#include "..\..\include\bugtrap\IBugTrap.h"

#include "BugTrap.h"

namespace ws {

namespace IBugTrap {

std::wstring getAppDir() {
	std::wstring apppath;
	apppath.resize(MAX_PATH + 1);
	::GetModuleFileNameW(nullptr, (wchar_t*)apppath.c_str(), MAX_PATH);
	auto found = apppath.find_last_of(L"\\");
	if(std::wstring::npos != found) {
		return apppath.substr(0, found + 1);
	}

	assert(false);
	return apppath;
}

std::wstring getAppName() {
	std::wstring apppath;
	apppath.resize(MAX_PATH + 1);
	auto len = ::GetModuleFileNameW(
		nullptr, (wchar_t*)apppath.c_str(), MAX_PATH);
	auto found = apppath.find_last_of(L"\\");
	if(std::wstring::npos != found) {
		return apppath.substr(found + 1, len - found - 1);
	}

	assert(false);
	return apppath;
}

extern "C" IBUGTRAP_API void install(const wchar* serverip, u16 port) {
	::SetErrorMode(
		SEM_FAILCRITICALERRORS | 
		SEM_NOGPFAULTERRORBOX |
		SEM_NOOPENFILEERRORBOX);

#ifdef _DEBUG
	_set_error_mode(_OUT_TO_MSGBOX);
#else
	_set_error_mode(_OUT_TO_STDERR);
#endif // _DEBUG

	if(nullptr == serverip) {
		auto appdir = getAppDir() + L"dump";
		BT_SetReportFilePath(appdir.c_str());
		BT_SetActivityType(BTA_SAVEREPORT);

	} else {
		wchar strport[32] = {0};
		wstring _url = L"http://";
		_url.append(serverip).append(L":").
			append(_itow(port, strport, 10)).
			append(L"/bugtrap/RequestHandler.aspx");
		BT_SetActivityType(BTA_SENDREPORT);
		BT_SetSupportServer(_url.c_str(), port);
	}

	auto appname = getAppName();
	BT_SetAppName(appname.c_str());
	BT_SetFlags(BTF_DETAILEDMODE);
	BT_SetDumpType(MiniDumpWithFullMemoryInfo);
	BT_InstallSehFilter();
}

}

}