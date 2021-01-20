#ifndef _HELPER_COMMONERROR_2019_01_19_10_50_17_H
#define _HELPER_COMMONERROR_2019_01_19_10_50_17_H

/*******************************************************************
*@brief  : 通过系统函数SetLastError设置自定义错误码
********************************************************************/

namespace ws {

namespace Error {
	
	enum Code {
		kSuccess = 0,
		kPlace = 0x20000000, //自定义错误码初始值

		kCurlInitializeFailed = kPlace + 1, //curl对象初始化失败
		kStatusNotMatch = kPlace + 2, //函数调用状态不正确，没有成对调用
	};

}

}

#endif //_HELPER_COMMONERROR_2019_01_19_10_50_17_H