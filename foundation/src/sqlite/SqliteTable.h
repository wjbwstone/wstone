#ifndef _SQLITE_TABLE_2020_01_14_17_07_38_H
#define _SQLITE_TABLE_2020_01_14_17_07_38_H

#include "SqliteRow.h"

#include <helper/List.h>
#include <helper/Uncopyable.h>
#include <foundation/include/sqlite/SqliteITable.h>

namespace ws {

namespace sqlite {

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

#endif //_SQLITE_TABLE_2020_01_14_17_07_38_H