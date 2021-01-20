#ifndef _H_LIBCOMMON_CONSOLE_2020_09_22_10_17_54
#define _H_LIBCOMMON_CONSOLE_2020_09_22_10_17_54

#include <helper/Types.h>

namespace ws {

namespace Console {

enum Color { //勿更改值数据
	kNone = 0,
	kBlue = 1,
	kGreen = 2,
	kRed = 4,
};

enum CRTErrorMode {
	kDefualt = 0,
	kStdError = 1,
	kMessageBox = 2
};

bool setTitle(const i8* title);
bool setTitle(const ustring& title);

u32 setOutTextColor(u32 color);
void restoreOutTextColor(u32 color);

void setCRTErrorMode(CRTErrorMode mode);

/*******************************************************************
*@brief  : 设置输入和输出字符集，默认值使用系统默认字符集
********************************************************************/
bool setCodePage(u32 input = 0, u32 output = 0);

bool write(const i8* data, Color = kNone, bool newline = true);
bool write(const ustring& data, Color = kNone, bool newline = true);
bool write(const i8* data, u32 bytes, Color = kNone, bool newline = true);

/*******************************************************************
*@brief  : 读取数据，函数需要等到回车结束。character 读取指定字符数
********************************************************************/
bool read(ustring& data, u32 character = 1024);

/*******************************************************************
*@brief  : 禁止控制台 "选定模式"，防止控制台挂起
********************************************************************/
bool disableSelectedMode();

}

}

#endif //_H_LIBCOMMON_CONSOLE_2020_09_22_10_17_54