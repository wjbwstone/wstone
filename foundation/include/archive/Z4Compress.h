#ifndef _LIBCOMMON_LZ4_2020_04_16_09_54_27_H
#define _LIBCOMMON_LZ4_2020_04_16_09_54_27_H

#include <helper/Types.h>

namespace ws {

namespace lz4 {

bool compress(const ustring& input, ustring& output);
bool compress(const i8* input, u32 bytes, ustring& output);

bool uncompress(const ustring& input, ustring& output);
bool uncompress(const i8* input, u32 bytes, ustring& output);

/*******************************************************************
*@brief  : 压缩文件
*path : 文件路径，不支持目录
*outpath ： 文件绝对路径
********************************************************************/
bool compressFile(const ustring& path, const ustring& outpath);

/*******************************************************************
*@brief  : 解压文件
********************************************************************/
bool uncompressFile(const ustring& path, const ustring& outpath);

}

}

#endif //_LIBCOMMON_LZ4_2020_04_16_09_54_27_H