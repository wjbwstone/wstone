#ifndef _LIBCOMMON_DIRECTORY_2020_04_29_12_20_48_H
#define _LIBCOMMON_DIRECTORY_2020_04_29_12_20_48_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>

#include "Path.h"

namespace ws {

/*******************************************************************
*@brief  : 获取当前文件夹中所有文件信息，不包含子目录文件信息
*note ： 空目录作失败处理
********************************************************************/
class Directory {
	kDisableCopyAssign(Directory);

public:
	explicit Directory(const Path& path);
	~Directory();

public:
	bool next();
	bool valid() const;
	bool empty() const;
	const Path& currentPath() const;

public: //文件夹中文件信息
	bool isFile() const;
	ustring name() const;
	i64 fileSize() const;
	ustring print() const;
	Path fullPath() const;
	i64 createTime() const;
	bool isDirectory() const;
	i64 lastModifyTime() const;

	bool copyTo(const Path& to) const;

public:
	static bool deleteDirectory(const Path& target);
	static bool createDirectory(const Path& target);
	static bool walkDirectory(const Path&, std::vector<ustring>&);
	static bool copyDirectory(const Path& from, const Path& to);

private:
	bool _next();

private:
	Path _path;
	bool _inited;
	HANDLE _hfile;
	WIN32_FIND_DATAW _fileInfo;
};

}

#endif //_LIBCOMMON_DIRECTORY_2020_04_29_12_20_48_H