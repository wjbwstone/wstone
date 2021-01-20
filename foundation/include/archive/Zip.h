#ifndef _LIBCOMMON_ZIP_2020_02_27_14_12_06_H
#define _LIBCOMMON_ZIP_2020_02_27_14_12_06_H

#include <helper/Types.h>

namespace ws {

namespace Zip {

bool compress(const i8* input, ustring& output, bool gzip = false);
bool compress(const ustring& input, ustring& output, bool gzip = false);
bool compress(const i8* input, u32 bytes, ustring& output, bool gzip = false);

bool uncompress(const i8* input, ustring& output, bool gzip = false);
bool uncompress(const ustring& input, ustring& output, bool gzip = false);
bool uncompress(const i8* input, u32 bytes, ustring& output, bool gzip = false);

bool compressFile(const ustring& existPath, const ustring& outPath);
bool uncompressFile(const ustring& existFilePath, const ustring& outDirectory);

}

}

#ifdef _DEBUG
#pragma comment(lib, "zlibd.lib")
#else
#pragma comment(lib, "zlib.lib")
#endif //_DEBUG

#endif //_LIBCOMMON_ZIP_2020_02_27_14_12_06_H