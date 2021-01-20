#include <foundation/include/base/Registry.h>

#include <helper/StringConvert.h>

#include <foundation/include/base/File.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/String.h>
#include <foundation/include/base/System.h>
#include <foundation/include/base/Process.h>
#include <foundation/include/base/Directory.h>

namespace ws {

Registry::Registry() : _key(nullptr) {

}

Registry::Registry(HKEY key, const i8* subkey, u32 access,
	CreateMode mode /* = kOpenExisting */, bool use64 /* = true */) : 
	_key(nullptr) {
	open(key, subkey, access, mode, use64);
}

Registry::~Registry() {
	close();
}

bool Registry::open(HKEY rootKey, const i8* subKey, u32 access, 
	CreateMode mode /* = kOpenExisting */, bool use64 /* = true */) {
	if(nullptr == subKey || nullptr != _key) {
		SetLastError(ws::Error::kStatusNotMatch);
		return false;
	}

	auto wSubKey = Utf8ToUnicode(subKey);
	if(0 != wSubKey.size() && ('\\' == wSubKey[0] || L'/' == wSubKey[0])) {
		wSubKey.erase(wSubKey.begin());
	}

	if(use64) {
		access |= KEY_WOW64_64KEY;
	}

	HKEY hSubKey = nullptr;
	auto code = ERROR_SUCCESS;

	if(kOpenExisting == mode) {
		if(ERROR_SUCCESS != (code = RegOpenKeyExW(
			rootKey, wSubKey.c_str(), 0, access, &hSubKey))) {
			SetLastError(code);
			return false;
		}

	} else {
		DWORD dwDispost = REG_CREATED_NEW_KEY;
		if(ERROR_SUCCESS != (code = RegCreateKeyExW(
			rootKey, wSubKey.c_str(), 0, 0, 0,
			access, 0, &hSubKey, &dwDispost))) {
			SetLastError(code);
			return false;
		}
	}

	_key = hSubKey;
	return true;
}

void Registry::flush() {
	if(nullptr != _key) {
		RegFlushKey(_key);
	}
}

void Registry::close() {
	if(nullptr != _key) {
		RegCloseKey(_key);
		_key = nullptr;
	}
}

bool Registry::save(const ustring& fpath) {
	Process::upPrivilege(nullptr, "SeBackupPrivilege");
	Path p(fpath);
	if(Path::isExist(p)) {
		File::del(p);
	}

	Directory::createDirectory(p.parentPath());

	auto t = Utf8ToUnicode(fpath);
	auto code = ::RegSaveKeyExW(_key, t.c_str(), NULL, REG_LATEST_FORMAT);
	return (code == ERROR_SUCCESS) ? true : (SetLastError(code), false);
}

bool Registry::load(const ustring& fpath) {
	Process::upPrivilege(nullptr, "SeRestorePrivilege");

	auto t = Utf8ToUnicode(fpath);
	auto code = ::RegRestoreKeyW(_key, t.c_str(), REG_FORCE_RESTORE);
	return (code == ERROR_SUCCESS) ? true : (SetLastError(code), false);
}

bool Registry::setValue(const i8* valueName, i32 type, const i8* value, 
	i32 bytes) {
	if(nullptr == valueName || nullptr == value || nullptr == _key) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	auto code = ERROR_SUCCESS;
	auto t = Utf8ToUnicode(valueName);

	if(REG_SZ == type || REG_MULTI_SZ == type || REG_MULTI_SZ == type) {
		auto wValue = ws::StringToWString<CP_UTF8>()(value, bytes);
		code = RegSetValueExW(_key, t.c_str(), 0, type,
			(BYTE*)wValue.c_str(), sizeof(wchar) * (wValue.size() + 1));
	} else {
		code = RegSetValueExW(_key, t.c_str(), 0, type, (BYTE*)value, bytes);
	}

	return (code == ERROR_SUCCESS) ? true : (SetLastError(code), false);
}

bool Registry::setI32(const i8* valueName, i32 value) {
	return setValue(valueName, REG_DWORD, (i8*)&value, sizeof(i32));
}

bool Registry::setI64(const i8* valueName, i64 value) {
	return setValue(valueName, REG_QWORD, (i8*)&value, sizeof(i64));
}

bool Registry::setCString(const i8* valueName, const i8* value) {
	return setValue(valueName, REG_SZ, value, strlen(value));
}

bool Registry::setBin(const i8* valueName, const ustring& value) {
	return setValue(valueName, REG_BINARY, value.c_str(), value.size());
}

bool Registry::getValue(const i8* valueName, i32 type, i8* value, 
	i32& bytes) {
	if(nullptr == valueName || nullptr == _key) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	auto code = ERROR_SUCCESS;
	auto t = Utf8ToUnicode(valueName);

	if((nullptr != value) &&
		(REG_SZ == type || REG_MULTI_SZ == type || REG_MULTI_SZ == type)) {
		assert(false && "Dont use this method!!!");
		return false;
	} else {
		code = RegQueryValueExW(
			_key, t.c_str(), 0, (LPDWORD)&type,
			(BYTE*)value, (LPDWORD)&bytes);
	}

	return (code == ERROR_SUCCESS) ? true : (SetLastError(code), false);
}

bool Registry::getValue(const i8* valueName, wstring& value, i32 bytes) {
	if(nullptr == valueName || nullptr == _key) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	i32 type = REG_SZ;
	auto t = Utf8ToUnicode(valueName);
	auto code = RegQueryValueExW(_key, t.c_str(), 0,
		(LPDWORD)&type, (BYTE*)value.c_str(), (LPDWORD)&bytes);

	return (code == ERROR_SUCCESS) ? true : (SetLastError(code), false);
}

bool Registry::getI32(const i8* valueName, i32& value) {
	i32 bytes = sizeof(i32);
	return getValue(valueName, REG_DWORD, (i8*)&value, bytes);
}

bool Registry::getI64(const i8* valueName, i64& value) {
	i32 bytes = sizeof(i64);
	return getValue(valueName, REG_QWORD, (i8*)&value, bytes);
}

bool Registry::getString(const i8* valueName, ustring& value) {
	i32 bytes = sizeof(i64);
	if(getValue(valueName, REG_SZ, nullptr, bytes)) {
		wstring wvalue;
		wvalue.resize((bytes - 1) / sizeof(wchar));
		if(getValue(valueName, wvalue, bytes)) {
			value = UnicodeToUtf8(wvalue);
			return true;
		}
	}

	return false;
}

bool Registry::getBin(const i8* valueName, ustring& value) {
	i32 bytes = sizeof(i64);
	if(getValue(valueName, REG_BINARY, nullptr, bytes)) {
		value.resize(bytes);
		return getValue(valueName,
			REG_BINARY, const_cast<i8*>(value.data()), bytes);
	}

	return false;
}

HKEY Registry::getCurrentUserKey(u32 access, 
	bool callerIsService /* = false */) {
	HKEY key = HKEY_CURRENT_USER;

	if(!callerIsService) {
		if(ERROR_SUCCESS != RegOpenCurrentUser(access, &key)) {
			key = HKEY_CURRENT_USER;
		}

	} else {
		HANDLE userToken;
		if(Process::getLoginUserToken(userToken)) {
			if(ImpersonateLoggedOnUser(userToken)) {
				if(ERROR_SUCCESS != RegOpenCurrentUser(access, &key)) {
					key = HKEY_CURRENT_USER;
				}
				
				//修改会话权限，可能导致进程提权无效 save方法返回1314
				RevertToSelf();
			}

			CloseHandle(userToken);
		}
	}

	return key;
}

bool Registry::delKey(const i8* subKey, bool use64 /*= true*/) {
	if(nullptr == subKey) {
		return false;
	}

	auto wSubKey = Utf8ToUnicode(subKey);
	auto redirectAccess = KEY_WOW64_32KEY;
	if(use64) {
		redirectAccess = KEY_WOW64_64KEY;
	}

	auto status = ::RegDeleteKeyExW(
		_key, wSubKey.c_str(), redirectAccess, 0);
	if(ERROR_SUCCESS == status) { //无子健
		return true;
	}

	Registry hkey(_key, subKey, KEY_ALL_ACCESS, kOpenExisting, use64);
	if(false == hkey) {
		return false;
	}

	do {
		DWORD keyNameSize = MAX_PATH;
		wchar keyName[MAX_PATH] = {0};
		status = ::RegEnumKeyExW(hkey, 0, keyName, 
			&keyNameSize, nullptr, nullptr, nullptr, nullptr);
		if(ERROR_SUCCESS == status) {
			if(!hkey.delKey(UnicodeToUtf8(keyName).c_str(), use64)) {
				break;
			}
		} else {
			SetLastError(status);
			break;
		}

	} while(true);

	status = ::RegDeleteKeyExW(_key, wSubKey.c_str(), redirectAccess, 0);
	return (ERROR_SUCCESS == status) ? true : (SetLastError(status), false);
}

bool Registry::delValue(const i8* valueName) {
	if(nullptr == valueName) {
		return false;
	}

	auto t = Utf8ToUnicode(valueName);
	auto code = ::RegDeleteValueW(_key, t.c_str());
	return (code == ERROR_SUCCESS) ? true : (SetLastError(code), false);
}

bool Registry::getSubKeyName(std::vector<ustring>& names) {
	auto index = 0;
	wchar keyName[MAX_PATH] = {0};
	
	do {
		DWORD keyNameSize = MAX_PATH;
		auto status = ::RegEnumKeyExW(_key, index++, keyName,
			&keyNameSize, nullptr, nullptr,nullptr, nullptr);
		if(ERROR_SUCCESS == status) {
			names.push_back(UnicodeToUtf8(keyName));
			::memset(keyName, 0, sizeof(wchar) * MAX_PATH);
		} else {
			if(ERROR_NO_MORE_ITEMS == status) {
				SetLastError(ERROR_SUCCESS);
			}
			break;
		}

	} while(true);

	return ERROR_SUCCESS == GetLastError();
}

bool Registry::getValuesKeyName(std::vector<ustring>& values) {
	static const i32 MAX_VALUE_NAME = 16383;
	
	i32 index = 0;
	wchar *keyName = new wchar[MAX_VALUE_NAME];

	do {
		DWORD keyNameSize = MAX_VALUE_NAME;
		::memset(keyName, 0, sizeof(wchar) * MAX_VALUE_NAME);
		auto status = RegEnumValueW(_key, index++, keyName,
			&keyNameSize, nullptr, nullptr, nullptr, nullptr);
		if(ERROR_SUCCESS == status) {
			values.push_back(UnicodeToUtf8(keyName));
		} else {
			if(ERROR_NO_MORE_ITEMS == status) {
				SetLastError(ERROR_SUCCESS);
			}
			break;
		}
	} while(true);

	if(nullptr != keyName) {
		delete[] keyName;
	}
	
	return ERROR_SUCCESS == GetLastError();
}

}