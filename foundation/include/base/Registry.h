#ifndef _LIBCOMMON_REGISTRY_2020_01_15_12_06_34_H
#define _LIBCOMMON_REGISTRY_2020_01_15_12_06_34_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>
#include <helper/CommonError.h>

namespace ws {

/*******************************************************************
*@brief  : 默认情况32位程序操作的注册表在64位系统下某些根键中的子健位置不一致
* Wow6432Node存储的是32位程序操作的注册表位置。
* 系统所有方法默认开启 KEY_WOW64_64KEY 标识，在32位系统自动忽略
* 键值名称勿超过2KB，键值内容勿超过1MB，键名称勿超过512byte
********************************************************************/
class Registry {
	kDisableCopyAssign(Registry);

public:
	/*******************************************************************
	*@brief  : 注意写入HKEY_CURRENT_USER使用下面方法获取根键
	********************************************************************/
	static HKEY getCurrentUserKey(u32 access, bool callerIsService = false);

public:
	enum CreateMode {
		kOpenExisting = 3, //打开已存在的
		kOpenAlways = 4, //新建或者打开
	};

public:
	Registry();
	Registry(HKEY rootkey, const i8* subkey, u32 access, 
		CreateMode mode = kOpenExisting, bool use64 = true);
	~Registry();

public:
	operator HKEY() const { return _key; }
	operator bool() const { return nullptr != _key; }

public:
	void close();
	/*******************************************************************
	*@brief  : accessFlag为访问权限(KEY_ALL_ACCESS...)
	*subKey : 输入 "" 打开指定的key
	********************************************************************/
	bool open(HKEY rootKey, const i8* subKey, u32 access,
		CreateMode mode = kOpenExisting, bool use64 = true);

public:
	void flush();
	bool setI32(const i8* valueName, i32 value);
	bool setI64(const i8* valueName, i64 value);
	bool setCString(const i8* valueName, const i8* value);
	bool setBin(const i8* valueName, const ustring& value);
	/*******************************************************************
	*@brief  : "" valuenName设置默认键值的值
	********************************************************************/
	bool setValue(const i8* valueName, i32 type, const i8* value, i32 bytes);

	bool getI32(const i8* valuename, i32& value);
	bool getI64(const i8* valueName, i64& value);
	bool getBin(const i8* valueName, ustring& value);
	bool getString(const i8* valueName, ustring& value);
	/*******************************************************************
	*@brief  : value为null返回所需bytes字节数, "" valuenName获取默认键值的值
	********************************************************************/
	bool getValue(const i8* valueName, wstring& value, i32 bytes);
	bool getValue(const i8* valueName, i32 type, i8* value, i32& bytes);

	/*******************************************************************
	*@brief  : 可递归删除键中的子健
	*subKey : 输入 "" 删除指定的key
	********************************************************************/
	bool delKey(const i8* subKey, bool use64 = true);

	/*******************************************************************
	*@brief  : 删除键中的值键
	********************************************************************/
	bool delValue(const i8* valueName);

	/*******************************************************************
	*@brief  : 返回键中所有直属子健的名称
	********************************************************************/
	bool getSubKeyName(std::vector<ustring>& names);

	/*******************************************************************
	*@brief  : 返回键中所有值键的名称
	********************************************************************/
	bool getValuesKeyName(std::vector<ustring>& values);

	/*******************************************************************
	*@brief  : 加载和保存reg文件
	********************************************************************/
	bool save(const ustring& fpath);
	bool load(const ustring& fpath);

private:
	HKEY _key;
};
	
}

#endif //_LIBCOMMON_REGISTRY_2020_01_15_12_06_34_H