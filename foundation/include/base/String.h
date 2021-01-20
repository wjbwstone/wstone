#ifndef _LIBCOMMON_STRING_2020_05_02_09_14_14_H
#define _LIBCOMMON_STRING_2020_05_02_09_14_14_H

#include <helper/Types.h>
#include <helper/StringConvert.h>

namespace ws {

class String {
public:
	String();
	String(const i8* value);
	String(const wchar* value);
	String(const ustring& value);
	String(const wstring& value);
	String(const i8* value, u32 bytes);
	String(const wchar* value, u32 bytes);
	~String();

public:
	const ustring& get() const { return _value; }
	operator const ustring&() const  { return _value; }
	wstring toWString() const { return Utf8ToUnicode(_value); }

public:
	i32 toI32(i32 defaultValue = 0) const;
	u32 toU32(u32 defaultValue = 0) const;
	i64 toI64(i64 defaultValue = 0) const;
	u64 toU64(u64 defaultValue = 0) const;
	i32 hexToI32(i32 defaultValue = 0) const;
	double toDouble(double defaultValue = 0.00) const;
	ustring toLower() const;
	ustring toUpper() const;
	bool equalNoCase(const ustring& other) const;
	ustring replace(const ustring& oldStr, const ustring& newStr) const;
	void split(const ustring& pattern, std::vector<ustring>& vret) const;
	ustring addPrefix(const ustring& preFix, bool failedIfExist = true) const;

public:
	static i32 toI32(const i8* input, i32 defaultValue = 0);
	static u32 toU32(const i8* input, u32 defaultValue = 0);
	static i64 toI64(const i8* input, i64 defaultValue = 0);
	static u64 toU64(const i8* input, u64 defaultValue = 0);
	static i32 hexToI32(const i8* hexStr, i32 defaultValue = 0);
	static double toDouble(const i8* input, double defaultValue = 0.00);

	static ustring u32ToBinString(const u32& input);
	static u32 binStringToU32(const ustring& input);
	static ustring u64ToBinString(const u64& input);
	static u64 binStringToU64(const ustring& input);

	static void toLower(ustring& input);
	static ustring toLower(const i8* input);
	static void toUpper(ustring& input);
	static ustring toUpper(const i8* input);
	static bool equalNoCase(const i8* left, const i8* right);
	static bool equalNoCase(const ustring& left, const ustring& right);

	static bool endWith(const ustring& input, const ustring& substr);
	static bool startWith(const ustring& input, const ustring& substr);
	static void addPrefix(ustring& out, const ustring& prefix, 
		bool failedIfExist = true);
	static void split(const ustring& source, const ustring& pattern,
		std::vector<ustring>& ret);
	static void replace(ustring& source, const ustring& oldStr,
		const ustring& newStr);

private:
	ustring _value;
};

} //wstone

#endif //_LIBCOMMON_STRING_2020_05_02_09_14_14_H