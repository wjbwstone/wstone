#ifndef _LIBCOMMON_RC4_2020_01_15_12_06_23_H
#define _LIBCOMMON_RC4_2020_01_15_12_06_23_H

#include <helper/Types.h>

namespace ws {

namespace RC4 {

/*******************************************************************
*@brief  : key长度[0-256]字节
********************************************************************/
void encrypt(ustring& inout, const i8* key);
void decrypt(ustring& inout, const i8* key);

void encrypt(i8* inout, u32 bytes, const i8* key);
void decrypt(i8* inout, u32 bytes, const i8* key);

}

}

#endif //_LIBCOMMON_RC4_2020_01_15_12_06_23_H