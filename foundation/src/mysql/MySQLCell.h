#ifndef _LIBCOMMON_MYSQLCELL_2020_05_18_09_47_37_H
#define _LIBCOMMON_MYSQLCELL_2020_05_18_09_47_37_H

#include <helper/Types.h>

namespace ws {

namespace mysql {

class Cell {
public:
	static const i8 kFlagBool = 1;
	static const i8 kFlagI32 = 2;
	static const i8 kFlagU32 = 4;
	static const i8 kFlagI64 = 8;
	static const i8 kFlagU64 = 16;
	static const i8 kFlagString = 32;

public:
	Cell(i32 v, i8 f) : _f(f) { _v._i64 = v; }
	Cell(i64 v, i8 f) : _f(f) { _v._i64 = v; }
	Cell(u32 v, i8 f) : _f(f) { _v._u64 = v; }
	Cell(u64 v, i8 f) : _f(f) { _v._u64 = v; }
	Cell(const i8* v, u32 n) : _f(kFlagString) { _v._str = new String(v, n); }

	~Cell() { if(kFlagString == _f) { delete _v._str; } }

	operator i32() const { return _v._i32; }
	operator u32() const { return _v._u32; }
	operator i64() const { return _v._i64; }
	operator u64() const { return _v._u64; }
	operator bool() const { return 0 != _v._u64; }
	operator ustring() const { return _v._str->toString(); }

	u32 size() const { return _v._str->_bytes; }
	operator const i8* () const { return _v._str->_data; }

public:
	u32 flag() const { return _f; }

private:
	struct String {
		String(const i8* data, u32 bytes) : _bytes(bytes) {
			assert(nullptr != data);
			_data = new i8[_bytes + 1];
			memcpy_s(_data, _bytes, data, _bytes);
			_data[_bytes] = '\0';
		}

		~String() {
			delete[] _data;
		}

		ustring toString() { return ustring(_data, _bytes); }

		i8* _data;
		u32 _bytes;
	};

private:
	u32 _f;
	union {
		i32 _i32;
		u32 _u32;
		i64 _i64;
		u64 _u64;
		String* _str;
	} _v;
};

}

}

#endif //_LIBCOMMON_MYSQLCELL_2020_05_18_09_47_37_H