#ifndef _LIBCOMMON_BASE64_2020_01_15_12_03_51_H
#define _LIBCOMMON_BASE64_2020_01_15_12_03_51_H

#include <helper/Types.h>

namespace ws {

namespace Base64 {

ustring encode(const i8* input);
ustring encode(const ustring& input);
ustring encode(const i8* input, u32 bytes);

ustring decode(const i8* input);
ustring decode(const ustring& input);
ustring decode(const i8* input, u32 bytes);

}

}

#endif //_LIBCOMMON_BASE64_2020_01_15_12_03_51_H