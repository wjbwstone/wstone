#ifndef _HELPER_TYPES_2020_01_15_12_26_41_H
#define _HELPER_TYPES_2020_01_15_12_26_41_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/*
						_ooOoo_
					   o8888888o
					   88" . "88
					   (| -_- |)
						O\ = /O
					____/`---'\____
				  .   ' \\| |// `.
				   / \\||| : |||// \
				 / _||||| -:- |||||- \
				   | | \\\ - /// | |
				 | \_| ''\---/'' | |
				  \ .-\__ `-` ___/-. /
			   ___`. .' /--.--\ `. . __
			."" '< `.___\_<|>_/___.' >'"".
		   | | : `- \`.;`\ _ /`;.`/ - ` : | |
			 \ \ `-. \_ __\ /__ _/ .-` / /
	 ======`-.____`-.___\_____/___.-`____.-'======
						`=---='

	 .............................................
			  佛祖保佑             永无BUG
*/

/*******************************************************************
*@brief  : 解决max冲突问题，将函数max使用()包裹
*1> (std::max)(value1, value2) or (std::numeric_limits<Foo>::max)();
*2> 定义 NOMINMAX
********************************************************************/

#include <io.h>
#include <ctime>
#include <limits>
#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <cassert>

#include <set>
#include <map>
#include <deque>
#include <queue>
#include <list>
#include <stack>
#include <vector>
#include <string>
#include <atomic>
#include <memory>
#include <limits>
#include <random>
#include <iostream>
#include <algorithm>
#include <functional>
#include <sys/timeb.h>
#include <unordered_set>
#include <unordered_map>

#include <windows.h>
#include <ws2tcpip.h>
#include <process.h>
#include <Iphlpapi.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include <ShellAPI.h>
#include <Wtsapi32.h>
#include <userenv.h>
#include <tlhelp32.h>

namespace ws {
	typedef char i8;
	typedef unsigned char byte;
	typedef wchar_t wchar;
	typedef short i16;
	typedef unsigned short u16;
	typedef int i32;
	typedef unsigned int u32;
	typedef __int64 i64;
	typedef unsigned __int64 u64;
	typedef std::string ustring; //存储UTF8编码
	typedef std::wstring wstring;
	typedef std::string bstring; //二进制内容
	const static ustring __emptyString;
}

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Userenv.lib")
#pragma comment(lib, "Wtsapi32.lib")
#pragma comment(lib, "Ws2_32.lib")

#endif //_HELPER_TYPES_2020_01_15_12_26_41_H