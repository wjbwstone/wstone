#ifndef _LIBCOMMON_LIBZXING_2020_02_27_16_20_58_H
#define _LIBCOMMON_LIBZXING_2020_02_27_16_20_58_H

#include <helper/Types.h>

namespace ws {

namespace libzxing {

/*******************************************************************
*@brief  : 默认文件保存为png。支持 bmp，jpeg，png
********************************************************************/
bool writeQRCode(const ustring& fpath, 
	const ustring& contents, u32 width, u32 height);

bool readQRCode(const ustring& fpath, ustring& contents);

}

}

#pragma comment(lib, "Gdiplus.lib")

#ifdef _DEBUG
#pragma comment(lib, "libzxingd.lib")
#else
#pragma comment(lib, "libzxing.lib")
#endif //_DEBUG

#endif //_LIBCOMMON_LIBZXING_2020_02_27_16_20_58_H