#ifndef _LIBCOMMON_IDENTITY_2020_01_15_12_05_23_H
#define _LIBCOMMON_IDENTITY_2020_01_15_12_05_23_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>

/*******************************************************************
*@brief  : 支持18位标准身份证号识别，X符号统一识别为大写
********************************************************************/

namespace ws {

class Identity {
	kDisableCopyAssign(Identity);

public:
	explicit Identity(const i8* identity);

public:
	u32 getAge() const;
	u32 getDay() const;
	bool isMale() const;
	u32 getYear() const;
	u32 getMonth() const;
	ustring print() const;
	i8 getCheckCode() const;
	const i8* getProvince() const;
	void reset(const i8* identity);
	bool isValid() const { return _isvalid; }

	/*******************************************************************
	*@brief  : 返回生日unix时间戳
	********************************************************************/
	u64 getIntBirthday() const;

	/**********************************************************
	*@brief  : 身份证有效的年龄
	*@excludeTime : 默认返回标准的日期时间格式
	***********************************************************/
	const i8* getBirthday(bool excludeTime = false) const;

private:
	i8 _chekcode;
	bool _isvalid;
	const static u32 s_identitylength = 18;
	i8 _identity[s_identitylength + 1];
};

}

#endif //_LIBCOMMON_IDENTITY_2020_01_15_12_05_23_H