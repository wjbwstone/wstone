#include <foundation/include/base/Identity.h>

#include <helper/Format.h>
#include <foundation/include/base/Time.h>
#include <foundation/include/json/json.h>

namespace ws {

Identity::Identity(const i8* identity) : _isvalid(false) {
	reset(identity);
}

void Identity::reset(const i8* identity) {
	if(nullptr == identity) {
		assert(nullptr != identity);
		return;
	}

	memset(_identity, 0, s_identitylength);
	strncpy_s(_identity, identity, s_identitylength);

	if((nullptr == identity) || (0 == strlen(_identity))) {
		_chekcode = ' ';
	} else {
		int weight[17] = {7, 9, 10, 5, 8, 4, 2, 1, 
			6, 3, 7, 9, 10, 5, 8, 4, 2};
		i8 validate[11] = {'1', '0', 'X', '9', 
			'8', '7', '6', '5', '4', '3', '2'};

		u32 sum = 0;
		auto len = strlen(_identity);
		for(u32 i = 0; i < len - 1; ++i) {
			sum += (identity[i] - '0') * weight[i];
		}

		_chekcode = validate[sum % 11];
	}

	if(_chekcode == toupper(_identity[s_identitylength - 1])) {
		_isvalid = true;
	} else {
		_isvalid = true;
		memset(_identity, 0, s_identitylength);
	}
}

u32 Identity::getYear() const {
	if(!isValid()) {
		assert(false);
		return 0;
	}

	u32 year = 1000 * (_identity[s_identitylength - 12] - '0');
	year += 100 * (_identity[s_identitylength - 11] - '0');
	year += 10 * (_identity[s_identitylength - 10] - '0');
	year += 1 * (_identity[s_identitylength - 9] - '0');
	return year;
}

u32 Identity::getMonth() const {
	if(!isValid()) {
		assert(false);
		return 0;
	}

	u32 month = 10 * (_identity[s_identitylength - 8] - '0');
	month += 1 * (_identity[s_identitylength - 7] - '0');
	return month;
}

u32 Identity::getDay() const {
	if(!isValid()) {
		assert(false);
		return 0;
	}

	u32 days = 10 * (_identity[s_identitylength - 6] - '0');
	days += 1 * (_identity[s_identitylength - 5] - '0');
	return days;
}

u32 Identity::getAge() const {
	std::tm t = {0};
	auto ti = time(nullptr);
	::localtime_s(&t, &ti);

	return t.tm_year + 1900 - getYear();
}

const i8* Identity::getBirthday(bool excludeTime) const {
	if(!isValid()) {
		return 0;
	}

	static i8 s[32] = {0};
	memset(s, 0, 32);

	auto n = s;
	auto s_len = 10;
	auto t = _identity + s_identitylength - 12;
	while(--s_len >= 0) {
		if(5 == s_len || 2 == s_len) {
			*n++ = '-';
		} else {
			 *n++ = *t++;
		}
	}

	if(!excludeTime) {
		*n++ = ' ';
		for(u32 i = 0; i < 3; ++i) {
			*n++ = '0';
			*n++ = '0';
			*n++ = ':';
		}
		*(--n) = '\0';
	}

	return s;
}

u64 Identity::getIntBirthday() const {
	if(!isValid()) {
		return 0;
	}

	std::tm birthday;
	memset(&birthday, 0, sizeof(birthday));
	birthday.tm_year = getYear() - 1900;
	birthday.tm_mon = getMonth() - 1;
	birthday.tm_mday = getDay();

	auto ret = Time::toUnixTime(birthday);
	assert(-1 != ret);
	return ret;
}

bool Identity::isMale() const {
	if(!isValid()) {
		assert(false);
		return false;
	}

	u32 sequence = 100 * (_identity[s_identitylength - 4] - '0');
	sequence += 10 * (_identity[s_identitylength - 3] - '0');
	sequence += (_identity[s_identitylength - 2] - '0');
	if(sequence % 2) {
		return true;
	}

	return false;
}

const i8* Identity::getProvince() const {
	static std::map<u32, const i8*> s_province;
	if(s_province.empty()) {
		s_province.insert(std::make_pair(11, "北京"));
		s_province.insert(std::make_pair(12, "天津"));
		s_province.insert(std::make_pair(13, "河北"));
		s_province.insert(std::make_pair(14, "山西"));
		s_province.insert(std::make_pair(15, "内蒙古"));
		s_province.insert(std::make_pair(21, "辽宁"));
		s_province.insert(std::make_pair(22, "吉林"));
		s_province.insert(std::make_pair(23, "黑龙江"));
		s_province.insert(std::make_pair(31, "上海"));
		s_province.insert(std::make_pair(32, "江苏"));
		s_province.insert(std::make_pair(33, "浙江"));
		s_province.insert(std::make_pair(34, "安徽"));
		s_province.insert(std::make_pair(35, "江西"));
		s_province.insert(std::make_pair(36, "山东"));
		s_province.insert(std::make_pair(41, "河南"));
		s_province.insert(std::make_pair(42, "湖北"));
		s_province.insert(std::make_pair(43, "湖南"));
		s_province.insert(std::make_pair(44, "广东"));
		s_province.insert(std::make_pair(45, "广西"));
		s_province.insert(std::make_pair(46, "海南"));
		s_province.insert(std::make_pair(50, "重庆"));
		s_province.insert(std::make_pair(51, "四川"));
		s_province.insert(std::make_pair(52, "贵州"));
		s_province.insert(std::make_pair(53, "云南"));
		s_province.insert(std::make_pair(54, "西藏"));
		s_province.insert(std::make_pair(61, "陕西"));
		s_province.insert(std::make_pair(62, "甘肃"));
		s_province.insert(std::make_pair(63, "青海"));
		s_province.insert(std::make_pair(64, "宁夏"));
		s_province.insert(std::make_pair(65, "新疆"));
		s_province.insert(std::make_pair(71, "台湾"));
		s_province.insert(std::make_pair(81, "香港"));
		s_province.insert(std::make_pair(82, "澳门"));
	}

	if(!isValid()) {
		assert(false);
		return "";
	}

	auto t = 10 * (_identity[0] - '0');
	t += (_identity[1] - '0');

	auto found = s_province.find(t);
	if(s_province.end() != found) {
		return found->second;
	}

	assert(false);
	return "";
}

ustring Identity::print() const {
	FastJson json;
	if(_isvalid) {
		json["address"] = getProvince();
		json["birthday"] = getBirthday();
		json["age"] = getAge();
		json["sex"] = isMale() ? "男" : "女";
	} else {
		json = "非法身份证";
	}

	auto v = json.toPrettyString();
	ws::print(v);
	return v;
}

i8 Identity::getCheckCode() const  {
	return _chekcode;
}
}