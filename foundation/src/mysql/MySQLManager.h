#ifndef _H_LIBCOMMON_MYSQLMANAGER_2020_09_29_14_27_05
#define _H_LIBCOMMON_MYSQLMANAGER_2020_09_29_14_27_05

#include <helper/All.h>
#include <foundation/include/mysql/IMySQLManager.h>

namespace ws {

namespace mysql {

class DB;
class DBHolder;

class DBManager : public IDBManager {
	kDisableCopyAssign(DBManager);

	friend class DBHolder;

public:
	DBManager();
	~DBManager();

public:
	ustring escape(const ustring& input) override;
	bool useDataBase(const ustring& dbname) override;
	bool createDataBase(const ustring& dbname) override;
	bool transaction(std::function<bool(void)> fn) override;
	TablePtr query(const i8* sql, i64* totals = nullptr) override;
	TablePtr query(const ustring& sql, i64* totalRow = nullptr) override;
	bool exec(const i8* sql, i64* affectedRow = nullptr) override;
	bool exec(const ustring& sql, i64* affectedRow = nullptr) override;
	bool conectDB(const i8* h, const i8* u, const i8* w, u16 p) override;
	bool getTableMetaData(tableMetaData&, const i8* table = nullptr) override;

private:
	DB* getDB();
	DB* createDB();
	void freeDB(DB* db);

private:
	u16 _port;
	ustring _pwd;
	ustring _host;
	ustring _user;
	ustring _dbname;
	FastMutex _fmDB;
	u16 _transactionDB;
	std::vector<DB*> _vdb;
	std::atomic<bool> _stoped;
};

}

}

#endif //_H_LIBCOMMON_MYSQLMANAGER_2020_09_29_14_27_05