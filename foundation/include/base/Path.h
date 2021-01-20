#ifndef _LIBCOMMON_PATH_2020_04_30_16_54_19_H
#define _LIBCOMMON_PATH_2020_04_30_16_54_19_H

#include <helper/Types.h>

namespace ws {

/*******************************************************************
*@brief  : 
*1 所有涉及路径都将转换为标准的windows路径分隔符
*2 所有目录路径都将智能补齐 '\\' 字符
********************************************************************/

class Path {
public:
	Path();
	Path(const i8* path);
	Path(const ustring& path);
	Path(const ustring& path, bool appendBackslash);
	~Path();

public:
	operator const ustring&() const { return _path; }
	const ustring& getPath() const { return _path; }
	const wstring& apiPath() const { return _apiPath; }

public:
	void clear();
	ustring print() const;
	void setPath(const ustring& path, bool appendBackslash);

public:
	bool isFile() const;
	bool isExist() const;
	bool isDirectory() const;
	ustring fileExt() const;
	ustring fileName() const;
	ustring parentPath() const;
	ustring fileFullName() const;

public:
	static bool isExist(const Path& path);
	static bool isFile(const Path& path);
	static bool isDirectory(const Path& path);
	static ustring fileExt(const Path& path);
	static ustring fileName(const Path& path);
	static ustring fileFullName(const Path& path);
	static ustring parentPath(const Path& path);

	static void addBackslash(wstring& path);
	static void addBackslash(ustring& path);
	static ustring toBackslash(const ustring& path);
	static wstring toBackslash(const wstring& path);
	static ustring toAbsolutePath(const ustring& path);
	static wstring toAbsolutePath(const wstring& path);

private:
	ustring _path;
	wstring _apiPath;
};

}

#endif //_LIBCOMMON_PATH_2020_04_30_16_54_19_H