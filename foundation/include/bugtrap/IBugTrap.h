#ifndef _BUGTRAP_VS2015_IBUGTRAP_2020_01_15_12_15_10_H
#define _BUGTRAP_VS2015_IBUGTRAP_2020_01_15_12_15_10_H

#ifdef IBUGTRAP_EXPORTS
#define IBUGTRAP_API __declspec(dllexport)
#else
#define IBUGTRAP_API __declspec(dllimport)
#endif

#include <helper/Types.h>

namespace ws {

namespace IBugTrap {

/*******************************************************************
*@brief  : serverip默认值表示存储在本地
********************************************************************/

extern "C" {

IBUGTRAP_API void install(const wchar* serverip = nullptr, u16 port = 80);

}
	
}

}

#ifndef IBUGTRAP_EXPORTS
#ifdef _DEBUG
#pragma comment(lib, "bugtrapd.lib")
#else
#pragma comment(lib, "bugtrap.lib")
#endif
#endif

#endif //_BUGTRAP_VS2015_IBUGTRAP_2020_01_15_12_15_10_H