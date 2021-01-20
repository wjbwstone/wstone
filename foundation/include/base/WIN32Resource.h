#ifndef _H_LIBCOMMON_WIN32RESOURCE_2020_10_10_14_33_38
#define _H_LIBCOMMON_WIN32RESOURCE_2020_10_10_14_33_38

#include <helper/Types.h>
#include <helper/Uncopyable.h>

namespace ws {

namespace Win32Resource {

/*******************************************************************
*@brief  : 打包或者解包自定义资源数据，预定义资源使用系统提供的API
********************************************************************/
bool pack(const i8* moduleFile, const i8* type, u32 id, const i8* fromFile);
bool unpack(const i8* moduleFile, const i8* type, u32 id, const i8* toFile);

};

}

#endif //_H_LIBCOMMON_WIN32RESOURCE_2020_10_10_14_33_38