#include <foundation/include/base/Path.h>

#include <helper/StringConvert.h>
#include <foundation/include/json/json.h>
#include <foundation/include/base/String.h>

namespace ws {

Path::Path() {

}

Path::Path(const i8* path) {
	if(nullptr != path) {
		setPath(path, false);
	}
}

Path::Path(const ustring& path) {
	setPath(path, false);
}

Path::Path(const ustring& path, bool appendBackslash) {
	setPath(path, appendBackslash);
}

Path::~Path() {
	clear();
}

void Path::clear() {
	_path.clear();
	_apiPath.clear();
}

ustring Path::print() const {
	FastJson jroot;

	auto jpath = jroot["Path"];
	jpath["path"] = getPath();
	jpath["apiPath"] = UnicodeToUtf8(apiPath());
	jpath["parent"] = parentPath();
	jpath["filefullname"] = fileFullName();
	jpath["fileName"] = fileName();
	jpath["fileExt"] = fileExt();
	jpath["isDirectory"] = isDirectory();
	jpath["isFile"] = isFile();
	jpath["isExist"] = isExist();

	auto v = jroot.toPrettyString();
	ws::print(v);
	return v;
}

void Path::setPath(const ustring& path, bool appendBackslash) {
	_path = Path::toAbsolutePath(path);
	_apiPath = Utf8ToUnicode(_path);

	if((MAX_PATH - 12) < _path.size()) {//8.3文件命令格式
		if(0 != _path.compare(0, 4, "\\\\?\\", 4)) {
			_apiPath.insert(0, L"\\\\?\\");
		}
	}

	if(isDirectory() || appendBackslash) {
		Path::addBackslash(_path);
		Path::addBackslash(_apiPath);
	}
}

bool Path::isExist() const {
	return Path::isExist(*this);
}

bool Path::isDirectory() const {
	return Path::isDirectory(*this);
}

bool Path::isFile() const {
	return Path::isFile(*this);
}

ustring Path::fileExt() const {
	return Path::fileExt(*this);
}

ustring Path::fileName() const {
	return Path::fileName(this->getPath());
}

ustring Path::fileFullName() const {
	return Path::fileFullName(*this);
}

ustring Path::parentPath() const {
	return Path::parentPath(*this);
}

bool Path::isExist(const Path& path) {
	return (TRUE == PathFileExistsW(path.apiPath().c_str()));
}

bool Path::isFile(const Path& path) {
	WIN32_FILE_ATTRIBUTE_DATA ffd;
	if(!GetFileAttributesExW(path.apiPath().c_str(), 
		GetFileExInfoStandard, &ffd)) {
		return false;
	}

	return (0 == (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool Path::isDirectory(const Path& path) {
	WIN32_FILE_ATTRIBUTE_DATA ffd;
	if(!GetFileAttributesExW(path.apiPath().c_str(), 
		GetFileExInfoStandard, &ffd)) {
		return false;
	}

	return (0 != (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

ustring Path::fileExt(const Path& path) {
	auto v = fileFullName(path);
	auto found = v.find_last_of('.');
	if(v.npos != found) {
		return v.substr(found + 1);
	}

	return __emptyString;
}

ustring Path::fileName(const Path& path) {
	auto v = fileFullName(path);
	auto found = v.find_last_of('.');
	if(v.npos != found) {
		v.erase(found);
	}

	return v;
}

ustring Path::fileFullName(const Path& path) {
	const auto& v = path.getPath();
	auto found = v.find_last_of('\\');
	if(v.npos != found) {
		return v.substr(found + 1);;
	}

	return path;
}

ustring Path::parentPath(const Path& path) {
	auto v = path.getPath();
	if(!v.empty()) {
		if(('\\') == *v.rbegin()) {
			v.erase(v.size() - 1);
		}

		auto f = v.find_last_of('\\');
		if(v.npos != f) {
			return v.substr(0, f + 1);
		}
	}

	return v;
}

void Path::addBackslash(ustring& path) {
	if(!path.empty()) {
		if('\\' != *path.rbegin()) {
			path.append(1, '\\');
		}
	}
}

void Path::addBackslash(wstring& path) {
	if(!path.empty()) {
		if(L'\\' != *path.rbegin()) {
			path.append(1, L'\\');
		}
	}
}

ustring Path::toBackslash(const ustring& path) {
	auto v = path;
	addBackslash(v);
	return v;
}

wstring Path::toBackslash(const wstring& path) {
	auto v = path;
	addBackslash(v);
	return v;
}

ustring Path::toAbsolutePath(const ustring& path) {
	auto wpath = Utf8ToUnicode(path);
	return UnicodeToUtf8(toAbsolutePath(wpath));
}

wstring Path::toAbsolutePath(const wstring& path) {//API自动转换分隔符为 '\\'
	auto len = GetFullPathNameW(path.c_str(), 0, nullptr, nullptr);
	if(0 != len) {
		wstring v(len, L'\0');
		len = GetFullPathNameW(path.c_str(), len, &v[0], nullptr);
		v.resize(len);
		return v;
	}

	return path;
}

}