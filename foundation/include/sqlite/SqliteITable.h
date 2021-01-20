#ifndef _SQLITE_ITABLE_2020_01_14_17_11_56_H
#define _SQLITE_ITABLE_2020_01_14_17_11_56_H

#include <helper/Types.h>

#include "SqliteField.h"

namespace ws {

namespace sqlite {

class ITable {
public:
	ITable() {};
	virtual ~ITable() {};

public:
	virtual void move(u32 index) {}
	virtual bool next() { return false; }
	virtual u32 size() const { return 0; }
	virtual bool empty() const { return true; }
	virtual bool valid() const { return false; }
	virtual const Field* getField(u32 index) const { return nullptr; }
	virtual const Field* getField(const i8* key) const { return nullptr; }

public:
	virtual i32 asI32(const i8* key) const { return 0; }
	virtual u32 asU32(const i8* key) const { return 0; }
	virtual i64 asI64(const i8* key) const { return 0; }
	virtual u64 asU64(const i8* key) const { return 0; }
	virtual bool asBool(const i8* key) const { return false; }
	virtual ustring asUTF8(const i8* key) const { return __emptyString; }
};

typedef std::shared_ptr<ITable> TablePtr;

}

}

#endif //_SQLITE_ITABLE_2020_01_14_17_11_56_H