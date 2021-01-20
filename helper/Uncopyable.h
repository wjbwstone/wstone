#ifndef _HELPER_UNCOPYABLE_2020_01_10_14_52_21_H
#define _HELPER_UNCOPYABLE_2020_01_10_14_52_21_H

/*******************************************************************
*@brief  : 运行类进行拷贝，注意指针拷贝问题和拷贝自己
********************************************************************/

namespace ws {

namespace UnCopyable {

#define kDisableAssign(classname) \
private: \
	classname& operator=(const classname&)

#define kDisableCopy(classname) \
private: \
	classname(const classname&)

#define kDisableCopyAssign(classname) \
private: \
	classname(const classname&); \
	classname& operator=(const classname&)
}

}

#endif //_HELPER_UNCOPYABLE_2020_01_10_14_52_21_H