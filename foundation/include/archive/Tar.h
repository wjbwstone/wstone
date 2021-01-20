#ifndef _LIBCOMMON_TAR_2020_04_16_15_20_45_H
#define _LIBCOMMON_TAR_2020_04_16_15_20_45_H

#include <helper/Types.h>

/*******************************************************************
*@brief  : 将文件夹或者文件内容打包为单一文件
********************************************************************/

namespace ws {

namespace tar {

bool compressFile(const ustring& path, const ustring& tarfile);
bool uncompressFile(const ustring& tarfile, const ustring& outDirectory);

}

}


#endif //_LIBCOMMON_TAR_2020_04_16_15_20_45_H