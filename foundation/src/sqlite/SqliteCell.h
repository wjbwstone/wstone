#ifndef _SQLITE_CELL_2020_01_14_17_00_03_H
#define _SQLITE_CELL_2020_01_14_17_00_03_H

#include <helper/Types.h>

namespace ws {

namespace sqlite {

class Cell {
public:
	static const u32 kFlagBool = 1;
	static const u32 kFlagI32 = 2;
	static const u32 kFlagU32 = 4;
	static const u32 kFlagI64 = 8;
	static const u32 kFlagU64 = 16;
	static const u32 kFlagString = 32;

public:
	Cell(i32 v, u32 f) : _f(f) { _v._i64 = v; }
	Cell(i64 v, u32 f) : _f(f) { _v._i64 = v; }
	Cell(u32 v, u32 f) : _f(f) { _v._u64 = v; }
	Cell(u64 v, u32 f) : _f(f) { _v._u64 = v; }
	Cell(const ustring& v) : _f(kFlagString) { _v._str = new ustring(v); }

	~Cell() { if(kFlagString == _f) { delete _v._str; } }

	operator i32() const { return _v._i32; }
	operator u32() const { return _v._u32; }
	operator i64() const { return _v._i64; }
	operator u64() const { return _v._u64; }
	operator bool() const { return 0 != _v._u64; }
	operator ustring() const { return *_v._str; }

public:
	u32 flag() const { return _f; }

private:
	u32 _f;
	union {
		i32 _i32;
		u32 _u32;
		i64 _i64;
		u64 _u64;
		ustring* _str;
	} _v;
};

#pragma pack()

}

}

#endif //_SQLITE_CELL_2020_01_14_17_00_03_H