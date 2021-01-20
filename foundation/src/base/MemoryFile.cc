#include <foundation/include/base/MemoryFile.h>

#include <helper/StringConvert.h>
#include <foundation/include/base/File.h>

namespace ws {

MemoryFile::MemoryFile() :
	_fsize(0),
	_file(nullptr),
	_fdata(nullptr),
	_readOnly(true),
	_readPointer(0),
	_writePointer(0),
	_hMapFile(nullptr) {
	
}

MemoryFile::~MemoryFile() {
	close();
}

bool MemoryFile::open(const Path& path, bool readOnly, u32 fsize) {
	FastMutex::ScopedLock lock(_fmFile);

	assert(!valid());
	assert(nullptr == _fdata);
	assert(0 == _fsize || 0 == _readPointer || 0 == _writePointer);

	_readOnly = readOnly;
	auto mode = File::kOpenExisting;
	if(_readOnly) {
		_fsize = (u32)File::size(path);
	} else {
		_fsize = fsize;
		mode = File::kCreateAlways;
	}

	if(_file.open(path, mode)) {
		if(nullptr != (_hMapFile = ::CreateFileMappingW(_file, nullptr,
			_readOnly ? PAGE_READONLY : PAGE_READWRITE, 
			0, _fsize, nullptr))) {
			if(nullptr != (_fdata = (i8*)::MapViewOfFile(_hMapFile,
				_readOnly ? FILE_MAP_READ : FILE_MAP_ALL_ACCESS, 0, 0, 0))) {
				return true;
			}
		}
	}

	close();
	return false;
}

void MemoryFile::close() {
	FastMutex::ScopedLock lock(_fmFile);

	if(nullptr != _fdata) {
		::UnmapViewOfFile(_fdata);
		_fdata = nullptr;
	}

	if(nullptr != _hMapFile) {
		::CloseHandle(_hMapFile);
		_hMapFile = nullptr;
	}

	if(!_readOnly) {
		_file.move(_writePointer);
		::SetEndOfFile(_file);
	}

	_fsize = 0;
	_readOnly = true;
	_readPointer = 0;
	_writePointer = 0;

	_file.close();
}

u32 MemoryFile::size() {
	FastMutex::ScopedLock lock(_fmFile);
	return _fsize;
}

void MemoryFile::flush() {
	FastMutex::ScopedLock lock(_fmFile);
	if(valid()) {
		::FlushViewOfFile(_fdata, 0);
	}
}

bool MemoryFile::valid() {
	FastMutex::ScopedLock lock(_fmFile);
	if((nullptr != _hMapFile) && (_file)) {
		return true;
	}

	SetLastError(ERROR_INVALID_HANDLE);
	return false;
}

bool MemoryFile::readEOF() {
	FastMutex::ScopedLock lock(_fmFile);
	return (_readPointer == _fsize);
}

bool MemoryFile::writeEOF() {
	FastMutex::ScopedLock lock(_fmFile);
	return (_writePointer == _fsize);
}

const i8* MemoryFile::begin() {
	FastMutex::ScopedLock lock(_fmFile);
	if(valid()) {
		return _fdata;
	}

	return __emptyString.c_str();
}

bool MemoryFile::seekRead(u32 offset) {
	FastMutex::ScopedLock lock(_fmFile);

	if(valid()) {
		if(offset <= _fsize) {
			_readPointer = offset;
			return true;
		}

		SetLastError(ERROR_INVALID_PARAMETER);
	}

	return false;
}

bool MemoryFile::seekWrite(u32 offset) {
	FastMutex::ScopedLock lock(_fmFile);

	if(valid()) {
		if(offset <= _fsize) {
			_writePointer = offset;
			return true;
		}

		SetLastError(ERROR_INVALID_PARAMETER);
	}

	return false;
}

bool MemoryFile::write(const i8* fdata) {
	if(nullptr != fdata) {
		return write(fdata, strlen(fdata));
	}

	SetLastError(ERROR_INVALID_PARAMETER);
	return false;
}

bool MemoryFile::write(const bstring& fdata) {
	return write(fdata.c_str(), fdata.size());
}

bool MemoryFile::write(const i8* fdata, u32 bytes) {
	if(nullptr == fdata) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	FastMutex::ScopedLock lock(_fmFile);

	if(_readOnly) {
		SetLastError(ERROR_NOACCESS);
		return false;
	}

	if(!valid()) {
		return false;
	}

	if(0 != bytes) {
		auto needWritedBytes = (std::min)(bytes, _fsize - _writePointer);
		if(needWritedBytes != bytes) {
			SetLastError(ERROR_NOT_ENOUGH_MEMORY);
			return false;
		}

		auto fbegin = _fdata + _writePointer;
		memcpy_s(fbegin, bytes, fdata, bytes);
		_writePointer += bytes;
	}
	
	return true;
}

bool MemoryFile::read(bstring& fdata, u32 readBytes) {
	fdata.clear();
	
	FastMutex::ScopedLock lock(_fmFile);
	if(!valid()) {
		return false;
	}

	auto fbegin = _fdata + _readPointer;
	auto realSize = (std::min)(readBytes, _fsize - _readPointer);
	fdata.assign(fbegin, fbegin + realSize);
	_readPointer += realSize;
	return true;
}

}