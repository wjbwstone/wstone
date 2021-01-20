
#include <foundation\include\base\Directory.h>

#include <helper\Format.h>
#include <helper\StringConvert.h>
#include <foundation\include\json\json.h>
#include <foundation\include\base\File.h>
#include <foundation\include\base\Path.h>
#include <foundation\include\base\Time.h>

namespace ws {

Directory::Directory(const Path& path) :
	_path(path, true),
	_inited(false),
	_hfile(INVALID_HANDLE_VALUE) {
	memset(&_fileInfo, 0, sizeof(WIN32_FIND_DATAW));

	auto v = _path.apiPath() + L"*.*";
	_hfile = FindFirstFileW(v.c_str(), &_fileInfo);

	if("." == name() || ".." == name()) {
		_next();
	}
}

Directory::~Directory() {
	if(nullptr != _hfile) {
		::FindClose(_hfile);
		_hfile = nullptr;
	}
}

bool Directory::valid() const {
	return INVALID_HANDLE_VALUE != _hfile;
}

bool Directory::empty() const {
	return 0 == _fileInfo.dwFileAttributes;
}

bool Directory::next() {
	if(_inited) {
		return _next();
	}

	_inited = true;
	return !empty();
}

bool Directory::_next() {
	do {
		if(!FindNextFileW(_hfile, &_fileInfo)) {
			memset(&_fileInfo, 0, sizeof(WIN32_FIND_DATAW));
			return false;
		}
	} while("." == name() || ".." == name());

	return true;
}

const Path& Directory::currentPath() const {
	return _path;
}

bool Directory::isFile() const {
	return (!empty() && !isDirectory());
}

bool Directory::isDirectory() const {
	assert(!empty());
	return 0 != (_fileInfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

ustring Directory::name() const {
	assert(!empty());
	return UnicodeToUtf8(_fileInfo.cFileName);
}

Path Directory::fullPath() const {
	if(isDirectory()) {
		return (_path.getPath() + name() + "\\");
	}
	
	return (_path.getPath() + name());
}

i64 Directory::fileSize() const {
	if(isFile()) {
		LARGE_INTEGER li;
		li.LowPart = _fileInfo.nFileSizeLow;
		li.HighPart = _fileInfo.nFileSizeHigh;
		return li.QuadPart;
	}
	
	return 0;
}

i64 Directory::createTime() const {
	assert(!empty());
	auto ctime = _fileInfo.ftLastWriteTime;
	FileTimeToLocalFileTime(&ctime, &ctime);
	return Time::toUnixTime(ctime);
}

i64 Directory::lastModifyTime() const {
	assert(!empty());
	auto ctime = _fileInfo.ftLastWriteTime;
	FileTimeToLocalFileTime(&ctime, &ctime);
	return Time::toUnixTime(ctime);
}

bool Directory::copyTo(const Path& to) const {
	return Directory::copyDirectory(_path, to);
}

ustring Directory::print() const {
	assert(!empty());
	FastJson jroot;
	jroot["name"] = name();
	jroot["size"] = fileSize();
	jroot["isfile"] = isFile();
	jroot["path"] = fullPath().getPath();
	jroot["parentpath"] = currentPath().getPath();
	jroot["createtime"] = Time::toDatetimeString(createTime());
	jroot["modifytime"] = Time::toDatetimeString(lastModifyTime());
	auto v = jroot.toPrettyString();
	ws::print(v);
	return v;
}

bool Directory::walkDirectory(const Path& target, std::vector<ustring>& paths) {
	Directory d(target);
	bool success = d.valid();
	while(success && d.next()) {
		paths.push_back(d.fullPath());
		if(d.isDirectory()) {
			success = Directory::walkDirectory(d.fullPath(), paths);
		}
	}

	return success;
}

bool Directory::deleteDirectory(const Path& target) {
	Directory d(target);
	bool success = d.valid();
	while(success && d.next()) {
		if(d.isDirectory()) {
			success = Directory::deleteDirectory(d.fullPath());
		} else {
			success = File::del(d.fullPath());
		}
	}

	return (success && 
		(TRUE == RemoveDirectoryW(d.currentPath().apiPath().c_str())));
}

bool Directory::createDirectory(const Path& target) {
	auto out = target.toBackslash(target);
	ustring::size_type index = 0;
	if(0 == out.compare(0, 4, "\\\\?\\", 4)) {
		index = 4;
	}

	while(index++ < out.size()) {
		if('\\' == out[index - 1]) {
			auto v = out.substr(0, index);
			if(!Path::isDirectory(v)) {
				if(!CreateDirectoryW(Utf8ToUnicode(v).c_str(), nullptr)) {
					return false;
				}
			}
		}
	}

	return true;
}

bool Directory::copyDirectory(const Path& from, const Path& to) {
	if(!Directory::createDirectory(to)) {
		return false;
	}

	Directory d(from);
	bool success = d.valid();
	while(success && d.next()) {
		auto t = Path::toBackslash(to) + d.name();
		if(d.isDirectory()) {
			success = Directory::copyDirectory(d.fullPath(), t);
		} else {
			success = File::copy(d.fullPath(), t);
		}
	}

	return success;
}

}