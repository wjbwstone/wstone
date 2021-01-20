#ifndef _LIBCOMMON_FILE_2020_04_30_11_20_05_H
#define _LIBCOMMON_FILE_2020_04_30_11_20_05_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>

#include "Path.h"

namespace ws {

class File {
	kDisableCopyAssign(File);

public:
	enum CreateMode {
		kCreateNew = 1, //新建，存在返回失败
		kCreateAlways = 2, //新建，存在覆盖
		kOpenExisting = 3, //打开已存在的, 只能读取
		kOpenAlways = 4, //新建或者打开，可读写
	};

public:
	File();
	File(const Path& path, CreateMode mode = kOpenExisting);
	~File();

public:
	operator HANDLE() const { return _file; }
	const Path& getPath() const { return _path; }
	operator bool() const { return INVALID_HANDLE_VALUE != _file; }

public:
	void close();
	bool open(const Path& path, CreateMode mode = kOpenExisting);

public:
	i64 size() const;
	bool flush() const;
	i64 createTime() const;
	i64 lastModifyTime() const;
	bool write(const i8* data);
	bool write(const bstring& data);
	bool copyTo(const Path& path) const;
	bool read(i8* data, u32& readBytes);
	bool write(const i8* data, u32 bytes);
	bool read(bstring& data, u32& bytes);
	bool move(i64 indexBytes, u32 method = FILE_BEGIN);

public:
	static i64 size(const Path& path);
	static bool del(const Path& path);
	static i64 createTime(const Path& path);
	static i64 lastModifyTime(const Path& path);
	static bool load(const Path& path, bstring& data);
	static bool copy(const Path& from, const Path& to);
	static bool rename(const Path& from, const Path& to);
	static bool save(const Path& path, const bstring& data, 
		bool createDirectory = false, bool destroyIfExist = false);

private:
	Path _path;
	HANDLE _file;
};

}

#endif //_LIBCOMMON_FILE_2020_04_30_11_20_05_H