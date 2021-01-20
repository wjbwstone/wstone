#ifndef _LIBCOMMON_MEMORYFILE_2020_01_15_12_05_49_H
#define _LIBCOMMON_MEMORYFILE_2020_01_15_12_05_49_H

/*******************************************************************
*@brief  : 
1> 被映射的文件不能过大
********************************************************************/

#include <helper/Types.h>
#include <helper/FastMutex.h>
#include <helper/Uncopyable.h>

#include "File.h"

namespace ws {

class MemoryFile {
	kDisableCopyAssign(MemoryFile);

public:
	MemoryFile();
	~MemoryFile();

public:
	const Path& getPath() const { return _file.getPath(); }

public:
	/*******************************************************************
	*@brief  : 新创建（磁盘文件将被覆盖）或者打开一个内存文件
	*@readOnly ： false表示创建一个bytes大小的内存映射文件
	* 关闭文件时候自动修正文件真实写入大小
	********************************************************************/
	void close();
	bool open(const Path& path, bool readOnly = true, u32 bytes = 0);

	u32 size();
	bool valid();
	void flush();
	bool readEOF();
	bool writeEOF();
	const i8* begin();
	bool seekRead(u32 offset);
	bool seekWrite(u32 offset);
	bool write(const i8* fdata);
	bool write(const bstring& fdata);
	bool write(const i8* fdata, u32 bytes);
	bool read(bstring& fdata, u32 readBytes);

private:
	File _file;
	i8* _fdata;
	u32 _fsize;
	bool _readOnly;
	HANDLE _hMapFile;
	u32 _readPointer;
	u32 _writePointer;
	FastMutex _fmFile;
};

}

#endif //_LIBCOMMON_MEMORYFILE_2020_01_15_12_05_49_H