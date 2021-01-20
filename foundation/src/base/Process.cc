#include <foundation/include/base/Process.h>

#include <helper/CommonError.h>
#include <helper/CommandLine.h>
#include <helper/StringConvert.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/String.h>

namespace ws {

static ProcessPtr __createPorcessByService(
	const Path& fpath,
	const std::vector<ustring>& vcmd,
	bool hideWindow /* = false */,
	bool useServicePrivilege /* = false */) {
	HANDLE hTokenUser = nullptr;
	if(!Process::getLoginUserToken(hTokenUser)) {
		return nullptr;
	}

	HANDLE hToken = nullptr;
	if(!useServicePrivilege) {
		if(!DuplicateTokenEx(hTokenUser, MAXIMUM_ALLOWED, nullptr,
			SecurityIdentification, TokenPrimary, &hToken)) {
			CloseHandle(hTokenUser);
			return nullptr;
		}

	} else {
		HANDLE hThis = nullptr;
		if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hThis)) {
			CloseHandle(hTokenUser);
			return nullptr;
		}

		if(!DuplicateTokenEx(hThis, MAXIMUM_ALLOWED, nullptr,
			SecurityIdentification, TokenPrimary, &hToken)) {
			CloseHandle(hThis);
			CloseHandle(hTokenUser);
			return nullptr;
		}

		u32 sid = 0;
		Process::getLoginUserSessionID(sid);
		SetTokenInformation(hToken, TokenSessionId, &sid, sizeof(DWORD));
		CloseHandle(hThis);
	}

	STARTUPINFOW si = {0};
	si.cb = sizeof(STARTUPINFOA);
	si.lpDesktop = L"WinSta0\\Default";
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = hideWindow ? LOWORD(SW_HIDE) : LOWORD(SW_NORMAL);

	LPVOID lpEnv = nullptr;
	if(!CreateEnvironmentBlock(&lpEnv, hTokenUser, false)) {
		CloseHandle(hToken);
		CloseHandle(hTokenUser);
		return nullptr;
	}
	
	PROCESS_INFORMATION pi = {0};
	auto cmd = CommandLine::quoteString(fpath) + CommandLine::parse(vcmd);
	auto pdir = Path::parentPath(fpath);
	auto success = CreateProcessAsUserW(hToken, 
		(LPWSTR)Utf8ToUnicode(fpath).c_str(),
		(LPWSTR)Utf8ToUnicode(cmd).c_str(),
		nullptr, nullptr, false,
		NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE |
		CREATE_UNICODE_ENVIRONMENT,
		lpEnv, Utf8ToUnicode(pdir).c_str(), &si, &pi);
	auto lastError = GetLastError();

	if(nullptr != lpEnv) {
		DestroyEnvironmentBlock(lpEnv);
	}

	CloseHandle(hToken);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	CloseHandle(hTokenUser);

	if(success) {
		auto exename = Path::fileFullName(fpath);
		return std::make_shared<Process>(pi.dwProcessId, exename);
	}

	auto p = std::make_shared<Process>(Process::_invalidID);
	SetLastError(lastError);
	return p;
}

static ProcessPtr __createPorcess(
	const Path& fpath,
	const std::vector<ustring>& vcmd,
	bool hideWindow /* = false */) {
	STARTUPINFOW si = {0};
	si.cb = sizeof(STARTUPINFOW);
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = hideWindow ? LOWORD(SW_HIDE) : LOWORD(SW_NORMAL);

	PROCESS_INFORMATION pi = {0};
	auto cmd = CommandLine::quoteString(fpath) + CommandLine::parse(vcmd);
	auto pdir = Path::parentPath(fpath);
	auto success = CreateProcessW(fpath.apiPath().c_str(),
		(LPWSTR)Utf8ToUnicode(cmd).c_str(),
		nullptr, nullptr, false, 0, nullptr,
		Utf8ToUnicode(pdir).c_str(), &si, &pi);
	auto lastError = GetLastError();

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if(success) {
		auto exename = Path::fileFullName(fpath);
		return std::make_shared<Process>(pi.dwProcessId, exename);
	}

	auto p = std::make_shared<Process>(Process::_invalidID);
	SetLastError(lastError);
	return p;
}

Process::Process(u32 pid, 
	const ustring& executename /* = __emptyString */, 
	u32 access /* = PROCESS_ALL_ACCESS */) : 
	_id(_invalidID),
	_handle(nullptr),
	_access(0) {
	open(pid, executename, access);
}

Process::~Process() {
	close();
}

u32 Process::getID() const {
	return _id;
}

bool Process::valid() const {
	return (_invalidID != getID() && nullptr != getHandle());
}

HANDLE Process::getHandle() const {
	if(nullptr == _handle) {
		auto p = const_cast<Process*>(this);
		p->_handle = OpenProcess(_access, FALSE, p->getID());
	}

	return _handle;
}

const Path& Process::getPath() const {
	if(!_path.apiPath().empty()) {
		return _path;
	}

	const static u32 fixed = MAX_PATH;

	wstring buffer(fixed, L'\0');
	auto len = GetModuleFileNameExW(getHandle(), 0, &buffer[0], fixed);
	if((fixed == len) || (ERROR_INSUFFICIENT_BUFFER == GetLastError())) {
		for(u32 i = 2; i < 1025; i *= 2) {
			buffer.clear();
			buffer.resize(fixed * i, L'\0');
			len = GetModuleFileNameExW(getHandle(), 0, &buffer[0], fixed * i);
			if((fixed != len) && ERROR_SUCCESS == GetLastError()) {
				buffer.resize(len);
				break;
			}
		}
	} else {
		buffer.resize(len);
	}

	auto p = const_cast<Process*>(this);
	p->_path.setPath(UnicodeToUtf8(buffer), false);
	return _path;
}

bool Process::open(u32 pid,
	const ustring& executename /* = __emptyString */,
	u32 access /* = PROCESS_ALL_ACCESS */) {
	if(_invalidID != _id) {
		SetLastError(Error::kStatusNotMatch);
		return false;
	}

	_id = pid;
	_access = access;
	_executename = executename;
	return valid();
}

bool Process::wait(u32 milliseconds /* = INFINITE */) const {
	if(valid()) {
		switch(WaitForSingleObject(getHandle(), milliseconds)) {
			case WAIT_OBJECT_0: return true;
			default: break;
		};
	}

	return false;
}

void Process::close() {
	if(nullptr != _handle) {
		CloseHandle(_handle);
	}

	_access = 0;
	_path.clear();
	_id = _invalidID;
	_handle = nullptr;
	_executename.clear();
}

bool Process::terminate() const {
	if(valid()) {
		if(TerminateProcess(getHandle(), 1)) {
			WaitForSingleObject(getHandle(), 1000);
			return true;
		}
	}

	return false;
}

bool Process::emptyWorkingSet() const {
	if(valid()) {
		return (TRUE == EmptyWorkingSet(getHandle()));
	}

	return false;
}

ustring Process::getExecuteName() const {
	if(_executename.empty()) {
		auto v = getPath().getPath();
		auto found = v.find_last_of(L'\\');
		if(ustring::npos != found) {
			v.erase(0, found + 1);
		}

		auto p = const_cast<Process*>(this);
		p->_executename = v;
	}
	
	return _executename;
}

ProcessPtr Process::currentProcess() {
	return std::make_shared<Process>(GetCurrentProcessId());
}

Path Process::getProcessPath(const ProcessPtr& process /* = nullptr */) {
	auto p = (nullptr != process) ? process : Process::currentProcess();
	return p->getPath();
}

bool Process::emptyProcessWorkingSet(
	const ProcessPtr& process /* = nullptr */) {
	auto p = (nullptr != process) ? process : Process::currentProcess();
	return p->emptyWorkingSet();
}

bool Process::getLoginUserToken(HANDLE& token) {
	auto p = Process::findProcess("explorer.exe");
	if(!OpenProcessToken(p->getHandle(), TOKEN_ALL_ACCESS, &token)) {
		auto sid = WTSGetActiveConsoleSessionId();
		return (TRUE == WTSQueryUserToken(sid, &token));
	}

	return true;
}

bool Process::getLoginUserSessionID(u32& sessionid) {
	auto p = Process::findProcess("explorer.exe");
	if(Process::_invalidID != p->getID()) {
		return (TRUE == ProcessIdToSessionId(p->getID(), (DWORD*)&sessionid));
	} else {
		sessionid = WTSGetActiveConsoleSessionId();
	}

	return (0xFFFFFFFF != sessionid);
}

ProcessPtr Process::openProcess(u32 pid, 
	const ustring& executename /* = __emptyString */,
	u32 access /* = PROCESS_ALL_ACCESS */) {
	return std::make_shared<Process>(pid, executename, access);
}

ProcessPtr Process::findProcess(const ustring& executename,
	u32 access /* = PROCESS_ALL_ACCESS */) {
	std::vector<ProcessPtr> vprocess;
	if(Process::walkProcess(vprocess, access)) {
		for(const auto& it : vprocess) {
			if(String::equalNoCase(executename, it->_executename)) {
				return it;
			}
		}
	}

	return std::make_shared<Process>(_invalidID, executename, access);
}

bool Process::walkProcess(std::vector<ProcessPtr>& vprocess, 
	u32 access /* = PROCESS_ALL_ACCESS */) {
	auto h = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(nullptr != h) {
		PROCESSENTRY32W ps = {0};
		ps.dwSize = sizeof(ps);
		if(Process32FirstW(h, &ps)) {
			do {
				vprocess.push_back(openProcess(
					ps.th32ProcessID,
					UnicodeToUtf8(ps.szExeFile),
					access));
			} while(Process32NextW(h, &ps));
		}

		CloseHandle(h);
		return true;
	}

	return false;
}

bool Process::killProcess(const ustring& executename) {
	auto p = Process::findProcess(executename);
	return p->terminate();
}

ProcessPtr Process::createProcess(const Path& fpath,
	const std::vector<ustring>& vcmd /*= std::vector<ustring>()*/,
	bool hideWindow /* = false */,
	bool callerIsServiceAndNeedInteractive /* = false */,
	bool useServicePrivilege /* = false */) {
	return callerIsServiceAndNeedInteractive ? 
		__createPorcessByService(fpath, vcmd,
			hideWindow, useServicePrivilege) :
		__createPorcess(fpath, vcmd, hideWindow);
}

bool Process::upPrivilege(const ProcessPtr& process /* = nullptr */, 
	const i8* access /* = nullptr */, bool promotion /*= true*/) {
	if(nullptr == access) {
		access = "SeDebugPrivilege";
	}

	HANDLE p = nullptr;
	if(nullptr != process) {
		p = process->getHandle();
	} else {
		p = GetCurrentProcess();
	}

	LUID luid = {0};
	if(!LookupPrivilegeValueW(0, Utf8ToUnicode(access).c_str(), &luid)) {
		return false;
	}

	HANDLE token = nullptr;
	if(!OpenProcessToken(p, TOKEN_ADJUST_PRIVILEGES, &token)) {
		return false;
	}
	
	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = promotion ? SE_PRIVILEGE_ENABLED : 0;
	auto success = AdjustTokenPrivileges(token, FALSE, &tp, sizeof(tp), 0, 0);
	CloseHandle(token);
	return (success == TRUE);
}

}

