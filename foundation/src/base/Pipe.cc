#include <foundation/include/base/Pipe.h>

namespace ws {

Pipe::Pipe() : _hRead(INVALID_HANDLE_VALUE), _hWrite(INVALID_HANDLE_VALUE) {
	SECURITY_ATTRIBUTES attr;
	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = nullptr;
	attr.bInheritHandle = false;
	CreatePipe(&_hRead, &_hWrite, &attr, 0);
}

Pipe::~Pipe() {
	if(INVALID_HANDLE_VALUE != _hWrite) {
		CloseHandle(_hWrite);
		_hWrite = INVALID_HANDLE_VALUE;
	}

	if(INVALID_HANDLE_VALUE != _hRead) {
		CloseHandle(_hRead);
		_hRead = INVALID_HANDLE_VALUE;
	}
}

bool Pipe::write(const ustring& data) {
	if(INVALID_HANDLE_VALUE == _hWrite) {
		SetLastError(ERROR_INVALID_HANDLE);
		return false;
	}

	DWORD w = 0;
	return (TRUE ==WriteFile(_hWrite, data.c_str(), data.size(), &w, nullptr));
}

bool Pipe::read(ustring& data, u32& bytes) {
	if(INVALID_HANDLE_VALUE == _hWrite) {
		SetLastError(ERROR_INVALID_HANDLE);
		return false;
	}

	data.clear();
	data.resize(bytes, '\0');

	DWORD w = 0;
	if(ReadFile(_hRead, (LPVOID)data.c_str(), bytes, &w, nullptr)) {
		data.resize(w);
		return true;
	}

	bytes = 0;
	data.clear();
	return false;
}

}