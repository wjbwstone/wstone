#ifndef _SQLITE_DB_2020_01_14_17_34_15_H
#define _SQLITE_DB_2020_01_14_17_34_15_H

#include <helper/Types.h>
#include <helper/FastMutex.h>
#include <helper/Uncopyable.h>
#include <foundation/include/sqlite/SqliteIDB.h>

#include "SqliteTable.h"
#include "easysqlite/sqlite3.h"

namespace ws {

namespace sqlite {

class DB : public IDB {
	kDisableCopyAssign(DB);

public:
	DB();
	~DB();

public:
	void close() override;
	bool transaction(fnTransaction fn) override;
	bool exec(const ustring& sql, i64* effectRow = nullptr) override;
	bool open(const ustring& dbpath, const ustring& password) override;
	TablePtr query(const ustring& sql, i64* totalRow = nullptr) override;

public:
	bool beginTrans();
	bool commmitTrans();
	bool rollbackTrans();

private:
	sqlite3* _hdb;
	FastMutex _fmDB;
};

}

}

#endif //_SQLITE_DB_2020_01_14_17_34_15_H