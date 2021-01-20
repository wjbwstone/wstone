#ifndef _LIBCOMMON_MD5_2020_01_15_12_05_38_H
#define _LIBCOMMON_MD5_2020_01_15_12_05_38_H

#include <helper/Types.h>

namespace ws {

namespace MD5 {

ustring encode(const i8* input);
ustring encode(const ustring& input);
ustring encode(const i8* input, u32 bytes);

ustring encodeFile(const i8* fpath);
ustring encodeFile(const ustring& fpath);

}

}

#endif //_LIBCOMMON_MD5_2020_01_15_12_05_38_H
