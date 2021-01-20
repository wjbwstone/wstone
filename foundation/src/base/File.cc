#include <foundation/include/base/File.h>

#include <helper/CommonError.h>
#include <helper/StringConvert.h>

#include <foundation/include/base/Time.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/Directory.h>

namespace ws {

File::File() : _file(INVALID_HANDLE_VALUE) {

}

File::File(const Path& path, CreateMode mode /* = kOpenExisting */) :
	_file(INVALID_HANDLE_VALUE) {
	open(path, mode);
}

File::~File() {
	close();
}

bool File::open(const Path& path, CreateMode mode /*= kOpenExisting*/) {
	if(INVALID_HANDLE_VALUE != _file) {
		SetLastError(Error::kStatusNotMatch);
		assert(INVALID_HANDLE_VALUE == _file);
		return false;
	}

	_path = path;

	if(kCreateNew == mode || kCreateAlways == mode) {
		Directory::createDirectory(Path::parentPath(_path));
	}

	_file = CreateFileW(_path.apiPath().c_str(),
		(mode == kOpenExisting) ? GENERIC_READ : GENERIC_ALL,
		FILE_SHARE_READ, nullptr, mode, FILE_ATTRIBUTE_ARCHIVE, nullptr);

	return operator bool();
}

void File::close() {
	_path.clear();
	if(INVALID_HANDLE_VALUE != _file) {
		::CloseHandle(_file);
		_file = INVALID_HANDLE_VALUE;
	}
}

bool File::flush() const {
	return ::FlushFileBuffers(_file) ? true : false;
}

i64 File::size() const {
	return File::size(_path);
}

i64 File::createTime() const {
	return File::createTime(_path);
}

i64 File::lastModifyTime() const {
	return File::lastModifyTime(_path);
}

bool File::copyTo(const Path& path) const {
	return File::copy(_path, path);
}

bool File::write(const i8* data) {
	if(nullptr != data) {
		return write(data, strlen(data));
	}

	SetLastError(ERROR_INVALID_PARAMETER);
	return false;
}

bool File::write(const bstring& data) {
	return write(data.c_str(), data.size());
}

bool File::write(const i8* data, u32 bytes) {
	if(nullptr == data) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	return TRUE == WriteFile(_file, data, bytes, (LPDWORD)&bytes, nullptr);
}

bool File::read(bstring& data, u32& bytes) {
	data.resize(bytes, '\0');

	auto success = read(&data[0], bytes);
	if(success) {
		data.resize(bytes);
	} else {
		data.clear();
	}

	return success;
}

bool File::read(i8* data, u32& bytes) {
	if(nullptr == data) {
		bytes = 0;
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	return TRUE == ReadFile(_file, data, bytes, (LPDWORD)&bytes, nullptr);
}

bool File::move(i64 indexBytes, u32 method /*= FILE_BEGIN*/) {
	LARGE_INTEGER v;
	v.QuadPart = indexBytes;
	return (TRUE == SetFilePointerEx(_file, v, nullptr, method));
}

i64 File::size(const Path& path) {
	WIN32_FILE_ATTRIBUTE_DATA ffd;
	if(!GetFileAttributesExW(path.apiPath().c_str(),
		GetFileExInfoStandard, &ffd)) {
		return 0;
	}

	LARGE_INTEGER li;
	li.LowPart = ffd.nFileSizeLow;
	li.HighPart = ffd.nFileSizeHigh;
	return li.QuadPart;
}

bool File::del(const Path& path) {
	return (TRUE == DeleteFileW(path.apiPath().c_str()));
}

i64 File::createTime(const Path& path) {
	WIN32_FILE_ATTRIBUTE_DATA ffd;
	if(!GetFileAttributesExW(path.apiPath().c_str(),
		GetFileExInfoStandard, &ffd)) {
		return 0;
	}

	auto ctime = ffd.ftCreationTime;
	FileTimeToLocalFileTime(&ctime, &ctime);
	return Time::toUnixTime(ctime);
}

i64 File::lastModifyTime(const Path& path) {
	WIN32_FILE_ATTRIBUTE_DATA ffd;
	if(!GetFileAttributesExW(
		path.apiPath().c_str(),
		GetFileExInfoStandard, &ffd)) {
		return 0;
	}

	auto ctime = ffd.ftLastWriteTime;
	FileTimeToLocalFileTime(&ctime, &ctime);
	return Time::toUnixTime(ctime);
}

bool File::copy(const Path& from, const Path& to) {
	return TRUE == CopyFileW(from.apiPath().c_str(), 
		to.apiPath().c_str(), FALSE);
}

bool File::rename(const Path& from, const Path& to) {
	return (TRUE == MoveFileExW(from.apiPath().c_str(),
		to.apiPath().c_str(),
		MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED));
}

bool File::load(const Path& path, bstring& data) {
	File f(path);
	if(f) {
		auto readBytes = (u32)f.size();
		return f.read(data, readBytes);
	}

	return false;
}

bool File::save(const Path& path, const bstring& data,
	bool createDirectory /* = false */,
	bool destroyIfExist /* = false */) {
	if(createDirectory) {
		Directory::createDirectory(path.parentPath());
	}
	
	auto mode = File::kCreateNew;
	if(destroyIfExist) {
		mode = File::kCreateAlways;
	}

	return File(path, mode).write(data);
}

}