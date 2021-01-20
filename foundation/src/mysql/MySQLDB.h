#ifndef _LIBMYSQL_DB_2020_01_15_11_14_41_H
#define _LIBMYSQL_DB_2020_01_15_11_14_41_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>
#include <foundation/src/mysql/inner/mysql.h>

namespace ws {

namespace mysql {

class Table;
class DBManager;

class DB {
	kDisableCopyAssign(DB);

public:
	explicit DB(DBManager* manger);
	~DB();

public:
	DBManager* getManager() { return _manger; }

	bool busy() const { return _busy; }
	void setBusy(bool val) { _busy = val; }
	void setTransaction(u32 tid) { _tid = tid; }
	bool transactioning(u32 tid) const { return _tid == tid; }

	bool beginTrans() { return exec("start transaction"); }
	bool commitTrans() { return 0 == mysql_commit(_mysql); }
	bool rollbackTrans() { return 0 == mysql_rollback(_mysql); }

public:
	void disconnect();
	bool connect(const i8* h, const i8* u, const i8* w, u16 p);

	ustring escape(const ustring& in);
	Table* query(const i8* sql, u32 = 0xffffffff, i64* totals = nullptr);
	bool exec(const i8* sql, u32 = 0xffffffff, i64* affectedRow = nullptr);

private:
	ustring err();
	i64 fetchTotals();
	Table* buildTable();
	void buildTableRow(Table* table, MYSQL_RES* result, u32 size);
	void buildTableHeader(Table* table, MYSQL_FIELD* fields, u32 size);

private:
	u32 _tid;
	bool _busy;
	MYSQL* _mysql;
	DBManager* _manger;
};

typedef std::shared_ptr<DB> DBPtr;

}

}

#endif //_LIBMYSQL_DB_2020_01_15_11_14_41_H