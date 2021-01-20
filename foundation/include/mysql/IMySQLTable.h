#ifndef _LIBMYSQL_ITABLE_2020_01_15_11_14_29_H
#define _LIBMYSQL_ITABLE_2020_01_15_11_14_29_H

#include <helper/Types.h>

#include "MySQLField.h"

namespace ws {

namespace mysql {

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
	virtual const i8* asCUTF8(const i8* key) const { return __emptyString.c_str(); }
};

typedef std::shared_ptr<ITable> TablePtr;

}

}

#endif //_LIBMYSQL_ITABLE_2020_01_15_11_14_29_H