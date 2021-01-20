#ifndef _LIBMYSQL_TABLE_2020_03_19_13_16_31_H
#define _LIBMYSQL_TABLE_2020_03_19_13_16_31_H

#include "MySQLRow.h"

#include <helper/All.h>
#include <foundation/include/mysql/MySQLField.h>
#include <foundation/include/mysql/IMySQLTable.h>

namespace ws {

namespace mysql {

class Table : public ITable {
	kDisableCopyAssign(Table);

public:
	Table();
	~Table();

public:
	bool next() override;
	u32 size() const override;
	bool valid() const override;
	bool empty() const override;
	void move(u32 index) override;
	const Field* getField(u32 index) const override;
	const Field* getField(const i8* key) const override;

public:
	i32 asI32(const i8* key) const override;
	u32 asU32(const i8* key) const override;
	i64 asI64(const i8* key) const override;
	u64 asU64(const i8* key) const override;
	bool asBool(const i8* key) const override;
	ustring asUTF8(const i8* key) const override;
	const i8* asCUTF8(const i8* key) const override;

public:
	void pushRow(const Row* row);
	void pushField(const Field* field);

private:
	bool asValue(const i8* key, Field::Type type, u32& index) const;

private:
	List<const Row*> _rows;
	std::vector<const Field*> _fields;
};

}

}

#endif //_LIBMYSQL_TABLE_2020_03_19_13_16_31_H