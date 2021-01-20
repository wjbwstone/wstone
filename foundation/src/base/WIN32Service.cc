#include <foundation/include/base/WIN32service.h>

#include <foundation/include/base/String.h>
#include <foundation/include/base/Process.h>
#include <foundation/include/base/System.h>

#include <helper/CommandLine.h>
#include <helper/StringConvert.h>

namespace ws {

static Win32Service* __win32Servicer = nullptr;

Win32Service::Win32Service() : _hStatus(nullptr) {
	ZeroMemory(&_status, sizeof(_status));
	_status.dwCurrentState = SERVICE_STOPPED;
	_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS;
	__win32Servicer = this;
}

Win32Service::~Win32Service() {
	__win32Servicer = nullptr;
}

void Win32Service::launch(i32 argc, const wchar* argv[]) {
	_vcmd = CommandLine::parse(argc, argv);

#ifdef _DEBUG
	run(_vcmd);
#else
	if(_vcmd.size() >= 2) {
		if(CommandLine::command(_vcmd, "--install", 0)) {
			auto vcmd = CommandLine::parse(argc, argv, 4);
			vcmd.insert(vcmd.begin(), _vcmd[1]);

			ServiceHelper sh;
			auto exe = System::getAppFullPath();
			auto cmds = exe + CommandLine::parse(vcmd);
			auto desc = (_vcmd.size() >= 3) ? _vcmd[2] : __emptyString;
			auto ok = sh.createService(_vcmd[1], exe, desc, vcmd);

			System::outputWindowLog("安装服务%s ： [%s-%s]", 
				ok ? "成功" : "失败",_vcmd[1].c_str(), cmds.c_str());
			return;
		}
	}
	
	if(_vcmd.size() >= 1) {
		auto v = Utf8ToUnicode(_vcmd[0]);
		SERVICE_TABLE_ENTRYW t[] = {//共享服务类型v参数才有效
			{(LPWSTR)v.c_str(), serviceMain},
			{NULL, NULL}
		};

		System::outputWindowLog("系统启动服务 ： %s", _vcmd[0].c_str());
		if(!::StartServiceCtrlDispatcherW(t)) {
			System::outputWindowLog("系统启动服务失败 : %d", GetLastError());
		}

	} else {
		System::outputWindowLog("启动服务失败： 参数不正确");
	}
#endif
}

void WINAPI Win32Service::serviceMain(DWORD dwArgc, LPWSTR* lpszArgv) {
	if(nullptr != __win32Servicer) {
		__win32Servicer->run(dwArgc, lpszArgv);
	}
}

void Win32Service::run(DWORD dwArgc, LPWSTR* lpszArgv) {
	//通过SCM start等系统方法录入的参数
	_hStatus = ::RegisterServiceCtrlHandlerExW(
		Utf8ToUnicode(_vcmd[0]).c_str(), serviceHandler, this);
	if(nullptr == _hStatus) {
		System::outputWindowLog("注册服务handle失败 ： %d", GetLastError());
		return;
	}

	reportStatus(SERVICE_START_PENDING, NO_ERROR, 3000);

	auto cmds = CommandLine::parse(_vcmd);
	System::outputWindowLog("服务运行参数 ： %s", cmds.c_str());

	reportStatus(SERVICE_RUNNING, NO_ERROR, 0);
	run(_vcmd);
	reportStatus(SERVICE_STOPPED, NO_ERROR, 0);
}

DWORD WINAPI Win32Service::serviceHandler(
	DWORD dwControl,
	DWORD dwEventType,
	LPVOID lpEventData,
	LPVOID lpContext) {
	auto service = static_cast<Win32Service*>(lpContext);
	auto ret = ERROR_CALL_NOT_IMPLEMENTED;
	switch(dwControl) {
		case SERVICE_CONTROL_STOP:
		case SERVICE_CONTROL_SHUTDOWN:
			ret = NO_ERROR;
			service->reportStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
			service->stop();
		break;

		default: break;
	}

	return ret;
}

void Win32Service::reportStatus(DWORD state, DWORD exitCode, DWORD waitHint) {
	_status.dwCurrentState = state;
	_status.dwWin32ExitCode = exitCode;
	_status.dwWaitHint = waitHint;

	if(state == SERVICE_START_PENDING) {
		_status.dwControlsAccepted = 0;
	} else {
		_status.dwControlsAccepted = 
			SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
	}

	if( (state == SERVICE_RUNNING) ||
		(state == SERVICE_STOPPED)) {
		_status.dwCheckPoint = 0;
	} else {
		static DWORD dwCheckPoint = 1;
		_status.dwCheckPoint = dwCheckPoint++;
	}

	::SetServiceStatus(_hStatus, &_status);
}

ServiceHelper::Service::Service(ServiceHelper* scm, 
	const ustring& name, u32 access) {
	assert(nullptr != scm);
	_hService = nullptr;
	if(nullptr != scm) {
		_hService = ::OpenServiceW(scm->_hManger, 
			Utf8ToUnicode(name).c_str(), access);
	}
}

ServiceHelper::Service::~Service() {
	if(nullptr != _hService) {
		::CloseServiceHandle(_hService);
		_hService = nullptr;
	}
}

ServiceHelper::ServiceHelper() : _hManger(nullptr) {
	_hManger = ::OpenSCManager(nullptr, nullptr, SC_MANAGER_ALL_ACCESS);
}

ServiceHelper::~ServiceHelper() {
	if(nullptr != _hManger) {
		::CloseServiceHandle(_hManger);
		_hManger = nullptr;
	}
}

bool ServiceHelper::createService(const ustring& name, 
	const ustring& exePath, const ustring& desc, 
	const std::vector<ustring>& vcmd) {
	Service hTemp(this, name, SERVICE_QUERY_CONFIG);
	if(nullptr != hTemp._hService) {
		return true;
	}

	auto wname = Utf8ToUnicode(name);
	auto binpath = CommandLine::quoteString(exePath) + 
		CommandLine::parse(vcmd);
	auto hService = ::CreateServiceW(
		_hManger, wname.c_str(), wname.c_str(), SERVICE_ALL_ACCESS,
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS,
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
		Utf8ToUnicode(binpath).c_str(),
		nullptr, nullptr, nullptr, nullptr, nullptr);
	if(nullptr == hService) {
		return false;
	}

	setServiceDescription(hService, desc);
	::CloseServiceHandle(hService);
	return true;
}

bool ServiceHelper::setServiceDescription(const ustring& name, 
	const ustring& desc) {
	return setServiceDescription(Service(this, name), desc);
}

bool ServiceHelper::deleteService(const ustring& name) {
	return (TRUE == ::DeleteService(Service(this, name)));
}

bool ServiceHelper::forceDeleteService(const ustring& name,
	const ustring& binname) {
	if(!stopService(name)) {
		auto v = Process::killProcess(binname);
		deleteService(name);
		Sleep(2000);
		return v;
	}

	return true;
}

bool ServiceHelper::isRunning(const ustring& name) {
	assert(nullptr != _hManger);
	return isServiceStatus(name, SERVICE_RUNNING);
}

bool ServiceHelper::isStoped(const ustring& name) {
	assert(nullptr != _hManger);
	return isServiceStatus(name, SERVICE_STOPPED);
}

bool ServiceHelper::startService(const ustring& name) {
	Service hService(this, name);
	SERVICE_STATUS_PROCESS ss;
	if(!queryServiceStatus(hService, ss)) {
		return false;
	}

	if( SERVICE_STOPPED != ss.dwCurrentState &&
		SERVICE_STOP_PENDING != ss.dwCurrentState) {
		return true;
	}

	if(SERVICE_STOP_PENDING == ss.dwCurrentState) {
		if(!waitServiceStatus(hService, 30, SERVICE_STOPPED)) {
			return false;
		}
	}

	if(::StartService(hService, 0, nullptr)) {
		return waitServiceStatus(hService, 30, SERVICE_RUNNING);
	}

	return false;
}

bool ServiceHelper::stopService(const ustring& name) {
	SERVICE_STATUS_PROCESS ss;
	Service hService(this, name);
	if(!queryServiceStatus(hService, ss)) {
		return false;
	}

	if(SERVICE_STOPPED == ss.dwCurrentState) {
		return true;
	}

	if(SERVICE_STOP_PENDING == ss.dwCurrentState) {
		return waitServiceStatus(hService, 30, SERVICE_STOPPED);
	}
	
	if(::ControlService(hService, SERVICE_CONTROL_STOP, 
		(LPSERVICE_STATUS)&ss)) {
		return waitServiceStatus(hService, 30, SERVICE_STOPPED);
	}

	return false;
}

bool ServiceHelper::isServiceStatus(const ustring& name, DWORD status) {
	SERVICE_STATUS_PROCESS ss;
	Service hService(this, name);
	queryServiceStatus(hService, ss);
	return (status == ss.dwCurrentState);
}

bool ServiceHelper::queryServiceStatus(SC_HANDLE hService,
	SERVICE_STATUS_PROCESS& status) {
	memset((void*)&status, 0, sizeof(SERVICE_STATUS_PROCESS));
	
	if(nullptr != hService) {
		DWORD dwBytesNeeded = 0;
		if(::QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO,
			(LPBYTE)&status, sizeof(status), &dwBytesNeeded)) {
			return true;
		}
	}

	return false;
}

bool ServiceHelper::waitServiceStatus(SC_HANDLE hService, i32 tryCount, 
	DWORD status) {
	SERVICE_STATUS_PROCESS ss;
	do {
		if(queryServiceStatus(hService, ss)) {
			if(status == ss.dwCurrentState) {
				return true;
			} else {
				Sleep(2000);
			}

		} else {
			break;
		}
	} while(0 > (--tryCount));

	return false;
}

bool ServiceHelper::setServiceDescription(SC_HANDLE hService, 
	const ustring& description) {
	SERVICE_DESCRIPTIONW sd;
	ZeroMemory(&sd, sizeof(SERVICE_DESCRIPTIONW));
	auto dvalue = Utf8ToUnicode(description);
	sd.lpDescription = (LPWSTR)(dvalue).c_str();
	return (TRUE == ::ChangeServiceConfig2W(hService,
		SERVICE_CONFIG_DESCRIPTION, &sd));
}

}