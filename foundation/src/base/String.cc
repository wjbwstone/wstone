#include <foundation/include/base/String.h>

namespace ws {

String::String() {

}

String::String(const i8* value) {
	if(nullptr != value) {
		_value = value;
	}
}

String::String(const wchar* value) : _value(UnicodeToUtf8(value)) {

}


String::String(const ustring& value) : _value(value) {

}

String::String(const wstring& value) : _value(UnicodeToUtf8(value)) {

}

String::String(const i8* value, u32 bytes) {
	if(nullptr != value) {
		_value.assign(value, bytes);
	}
}

String::String(const wchar* value, u32 bytes) : 
	_value(ws::WStringToString<CP_UTF8>()(value, bytes)) {

}

String::~String() {

}

i32 String::toI32(i32 defaultValue /* = 0 */) const {
	return String::toI32(_value.c_str(), defaultValue);
}

u32 String::toU32(u32 defaultValue /* = 0 */) const {
	return String::toU32(_value.c_str(), defaultValue);
}

i64 String::toI64(i64 defaultValue /* = 0 */) const {
	return String::toI64(_value.c_str(), defaultValue);
}

u64 String::toU64(u64 defaultValue /* = 0 */) const {
	return String::toU64(_value.c_str(), defaultValue);
}

double String::toDouble(double defaultValue /* = 0.00 */) const {
	return String::toDouble(_value.c_str(), defaultValue);
}

i32 String::hexToI32(i32 defaultValue /* = 0 */) const {
	return String::hexToI32(_value.c_str(), defaultValue);
}

ustring String::toLower() const {
	auto v = _value;
	String::toLower(v);
	return v;
}

ustring String::toUpper() const {
	auto v = _value;
	String::toUpper(v);
	return v;
}

bool String::equalNoCase(const ustring& other) const {
	return String::equalNoCase(_value, other);
}

ustring String::replace(const ustring& oldStr, 
	const ustring& newStr) const {
	auto v = _value;
	String::replace(v, oldStr, newStr);
	return v;
}

void String::split(const ustring& pattern, 
	std::vector<ustring>& vret) const {
	return String::split(_value, pattern, vret);
}

ustring String::addPrefix(const ustring& preFix,
	bool failedIfExist /*= true*/) const {
	auto v = _value;
	String::addPrefix(v, preFix, failedIfExist);
	return v;
}

double String::toDouble(const i8* input, double defaultValue /* = 0.00 */) {
	if(nullptr != input) {
		double ret = atof(input);
		assert(ERANGE != errno && EINVAL != errno);
		return ret;
	}
	
	return defaultValue;
}

i32 String::toI32(const i8* input, i32 defaultValue /* = 0 */) {
	if(nullptr != input) {
		i32 ret = atoi(input);
		assert(ERANGE != errno && EINVAL != errno);
		return ret;
	}
	
	return defaultValue;
}

u32 String::toU32(const i8* input, u32 defaultValue /* = 0 */) {
	if(nullptr != input) {
		i64 ret = _atoi64(input);
		assert(ERANGE != errno && EINVAL != errno);
		assert(ret <= UINT_MAX);
		return static_cast<u32>(ret);
	}
	
	return defaultValue;
}

i64 String::toI64(const i8* input, i64 defaultValue /* = 0 */) {
	if(nullptr != input) {
		i64 ret = _atoi64(input);
		assert(ERANGE != errno && EINVAL != errno);
		return ret;
	}
	
	return defaultValue;
}

u64 String::toU64(const i8* input, u64 defaultValue /* = 0 */) {
	if(nullptr != input) {
		u64 ret = _strtoui64(input, nullptr, 10);
		assert(ERANGE != errno && EINVAL != errno);
		return ret;
	}

	return defaultValue;
}

i32 String::hexToI32(const i8* hexStr, i32 defaultValue /* = 0 */) {
	if(nullptr != hexStr) {
		i8* offset;
		return strtol(hexStr, &offset, 16);
	}
	
	return defaultValue;
}

ustring String::u32ToBinString(const u32& input) {
	if(0 == input) {
		return "0";
	}

	ustring ret;
	bool run = false;
	for(auto i = 31; i >= 0; --i) {
		if((input >> i) & 1) {
			ret.push_back('1');
			run = true;
		} else {
			if(run) {
				ret.push_back('0');
			}
		}
	}

	return ret;
}

u32 String::binStringToU32(const ustring& input) {
	u32 ret = 0;
	u32 weight = 1;
	i32 index = input.size();
	while(index--) {
		ret += (input[index] - '0') * weight;
		weight *= 2;
	}

	return ret;
}

ustring String::u64ToBinString(const u64& input) {
	if(0 == input) {
		return "0";
	}

	ustring ret;
	bool run = false;
	for(auto i = 63; i >= 0; --i) {
		if((input >> i) & 1) {
			ret.push_back('1');
			run = true;
		} else {
			if(run) {
				ret.push_back('0');
			}
		}
	}

	return ret;
}

u64 String::binStringToU64(const ustring& input) {
	u64 ret = 0;
	u64 weight = 1;
	i32 index = input.size();
	while(index--) {
		ret += (input[index] - '0') * weight;
		weight *= 2;
	}

	return ret;
}

void String::toLower(ustring& input) {
	std::transform(input.begin(), input.end(), input.begin(), ::tolower);
}

ustring String::toLower(const i8* input) {
	ustring ret;
	while(nullptr != input && '\0' != *input) {
		ret.append(1, ::tolower(*input++));
	}

	return ret;
}

void String::toUpper(ustring& input) {
	std::transform(input.begin(), input.end(), input.begin(), ::toupper);
}

ustring String::toUpper(const i8* input) {
	ustring ret;
	while(nullptr != input && '\0' != *input) {
		ret.append(1, ::toupper(*input++));
	}

	return ret;
}

bool String::equalNoCase(const i8* left, const i8* right) {
	if(nullptr != left && nullptr != right) {
		while(::tolower(*left) == ::tolower(*right)) {
			if('\0' == *left) {
				return true;
			}

			left++;
			right++;
		}
	}

	return false;
}

bool String::equalNoCase(const ustring& left, const ustring& right) {
	if(left.size() == right.size()) {
		for(u32 i = 0; i < right.size(); ++i) {
			if(::toupper(left[i]) != ::toupper(right[i])) {
				return false;
			}
		}

		return true;
	}

	return false;
}

void String::addPrefix(ustring& out, const ustring& prefix, 
	bool failedIfExist /* = true */) {
	bool needAdd = true;
	auto v = prefix.size();
	if(failedIfExist) {
		needAdd = (0 != out.compare(0, v, prefix.c_str(), v));
	}

	if(needAdd) {
		out.insert(out.begin(), prefix.begin(), prefix.end());
	}
}

bool String::startWith(const ustring& input, const ustring& substr) {
	return input.find(substr) == 0;
}

bool String::endWith(const ustring& input, const ustring& substr) {
	return input.rfind(substr) == (input.length() - substr.length());
}

void String::split(
	const ustring& source,
	const ustring& pattern,
	std::vector<ustring>& ret) {
	auto target = source + pattern;
	u32 size = target.size();
	for(u32 pos = 0; pos < size; ++pos) {
		auto found = target.find(pattern, pos);
		if(target.npos != found) {
			ret.push_back(target.substr(pos, found - pos));
			pos = found + (pattern.size() - 1);
		}
	}
}

void String::replace(
	ustring& source,
	const ustring& oldStr,
	const ustring& newStr) {
	if(!oldStr.empty()) {
		ustring::size_type pos(0);
		for(; pos != ustring::npos; pos += newStr.length()) {
			if(ustring::npos != (pos = source.find(oldStr, pos))) {
				source.replace(pos, oldStr.length(), newStr);
			} else {
				break;
			}
		}
	}
}

} //wstone