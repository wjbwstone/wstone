#ifndef _HELPER_STRINGCONVERT_2019_06_19_15_33_33_H
#define _HELPER_STRINGCONVERT_2019_06_19_15_33_33_H

#include "Types.h"
#include "Uncopyable.h"
#include "Safedelete.h"

#define AnsiToUnicode(str)	ws::StringToWString<>()(str)
#define UnicodeToAnsi(str)	ws::WStringToString<>()(str)
#define Utf8ToUnicode(str)	ws::StringToWString<CP_UTF8>()(str)
#define UnicodeToUtf8(str)	ws::WStringToString<CP_UTF8>()(str)

#define Utf8ToAnsi(str) UnicodeToAnsi(Utf8ToUnicode(str))
#define AnsiToUtf8(str) UnicodeToUtf8(AnsiToUnicode(str))

#define UCS2ToAnsi(str) ws::UCSToString<>()(str)
#define UCS2ToUtf8(str) ws::UCSToString<3>()(str)

#define ISUTF8(str) ws::Utf8Validator::validate(str)

namespace ws {

template<i32 code = CP_ACP, i32 nbytes = 512>
class StringToWString {
	kDisableCopyAssign(StringToWString);

public:
	StringToWString() : _buffer(nullptr) {}
	~StringToWString() { if(nullptr != _buffer) delete[] _buffer; }

public:
	wstring operator()(const i8* p, i32 bytes = -1) {
		if(nullptr == p) {
			return L"";
		}

		wchar* store = _array;
		auto n = MultiByteToWideChar(code, 0, p, bytes, nullptr, 0);
		if(0 == n) {
			return L"";
		} else if(nbytes < n) {
			_buffer = new wchar[n];
			store = _buffer;
		}

		MultiByteToWideChar(code, 0, p, bytes, store, n);
		return wstring(store, (-1 == bytes) ? (n - 1) : n);
	}

	wstring operator()(const ustring& str) {
		return operator()(str.c_str(), str.size());
	}

private:
	wchar* _buffer;
	wchar _array[nbytes];
};

template<i32 code = CP_ACP, i32 nbytes = 512>
class WStringToString {
	kDisableCopyAssign(WStringToString);

public:
	WStringToString() : _buffer(nullptr) {}
	~WStringToString() { if(nullptr != _buffer) delete[] _buffer; }

public:
	ustring operator()(const wchar* p, i32 wbytes = -1) {
		if(nullptr == p) {
			return "";
		}

		i8* store = _array;
		auto n = WideCharToMultiByte(code, 0, p, wbytes, NULL, 0, NULL, NULL);
		if(0 == n) {
			return "";
		} else if(nbytes < n) {
			_buffer = new i8[n];
			store = _buffer;
		}

		WideCharToMultiByte(code, 0, p, wbytes, store, n, NULL, NULL);
		return ustring(store, (-1 == wbytes) ? (n - 1) : n);
	}

	ustring operator()(const wstring& wstr) {
		return operator()(wstr.c_str(), wstr.size());
	}

private:
	i8* _buffer;
	i8 _array[nbytes];
};

/**********************************************************
*@brief  :
* 模板参数2表示将ucs2转换为ansi编码（2将"\\u5f6d\\u65af\\u9676"字符转换为可显示字符）
*@return : 返回ansi编码格式的字符
***********************************************************/
template<i32 code = 2>
class UCSToString {
public:
	ustring operator()(const i8* p, i32 size = -1) {
		if(nullptr == p) {
			return "";
		}

		size = (-1 != size) ? size : strlen(p);

		wstring rst;
		auto escape = false;
		for(auto i = 0; i < size; i++) {
			char c = p[i];
			switch(c) {
				case '/':
				case '\\':
				case '%': escape = true; break;
				case 'u':
				case 'U':
					if(escape) {
						byte temp[5] = {0};
						memcpy(temp, p + i + 1, 4);
						u32 hex = 0;
						sscanf_s((i8*)temp, "%x", &hex);
						rst.push_back(hex);
						i += 4;
						escape = false;

					} else {
						rst.push_back(c);
					} break;

				default: rst.push_back(c); break;
			}
		}

		if(3 == code) {
			return UnicodeToUtf8(rst);
		}

		return UnicodeToAnsi(rst);
	}

	ustring operator()(const ustring& str) {
		return operator()(str.c_str(), str.size());
	}
};

namespace Utf8Validator {

static const u32 _accept = 0;
static const u32 _reject = 1;

static const byte utf8d[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
  8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
  0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
  0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
  0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
  1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
  1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
  1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

inline uint32_t decode(u32* state, u32* codep, byte val) {
	u32 type = utf8d[val];
	
	*codep = (*state != _accept) ?
		(val & 0x3fu) | (*codep << 6) :
		(0xff >> type)& (val);

	*state = utf8d[256 + *state * 16 + type];
	return *state;
}

class validator {
public:
	validator() : _state(_accept), _codepoint(0) {

	}

	template <typename iterator_type>
	bool decode(iterator_type begin, iterator_type end) {
		for(iterator_type it = begin; it != end; ++it) {
			u32 result = Utf8Validator::decode(&_state,
				&_codepoint, static_cast<byte>(*it));
			if(result == _reject) {
				return false;
			}
		}

		return true;
	}

	bool complete() {
		return _state == _accept;
	}

private:
	u32 _state;
	u32 _codepoint;
};

inline bool validate(const ustring& s) {
	validator v;
	if(!v.decode(s.begin(), s.end())) {
		return false;
	}

	return v.complete();
}

}

}

#endif//_HELPER_STRINGCONVERT_2019_06_19_15_33_33_H