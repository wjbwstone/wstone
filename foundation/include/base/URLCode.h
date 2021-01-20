#ifndef _LIBCOMMON_URLCODE_2020_01_15_12_08_05_H
#define _LIBCOMMON_URLCODE_2020_01_15_12_08_05_H

#include <helper/Types.h>

namespace ws {

/*******************************************************************
*@brief  : URL将会对特殊字符也进行编码（URIComponent模式）
********************************************************************/

namespace URL {

ustring encode(const i8* input);
ustring encode(const ustring& input);
ustring encode(const i8* input, u32 bytes);

ustring decode(const i8* input);
ustring decode(const ustring& in);
ustring decode(const i8* input, u32 bytes);

}

}

#endif //_LIBCOMMON_URLCODE_2020_01_15_12_08_05_H
