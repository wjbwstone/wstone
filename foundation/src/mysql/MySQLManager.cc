#include "MySQLManager.h"

#include <foundation/include/base/String.h>
#include <foundation/include/base/System.h>
#include <foundation/src/mysql/inner/mysql.h>

#include "MySQLDB.h"
#include "MySQLQuery.h"
#include "MySQLTable.h"

namespace ws {

namespace mysql { 

IDBManager* createDBManager() {
	return new DBManager();
}

void destroyDBManager(IDBManager* val) {
	auto p = dynamic_cast<DBManager*>(val);
	ws::Safedelete::checkedDelete(p);
}

class DBHolder {
public:
	DBHolder(DB* db) : _db(db) { 
	
	}

	~DBHolder() {
		if(nullptr != _db) {
			_db->getManager()->freeDB(_db);
		}
	}

public:
	DB* operator->() { return _db; }
	operator bool() { return nullptr != _db; }

private:
	DB* _db;
};

DBManager::DBManager() : _port(0), _stoped(false), _transactionDB(0) {

}

DBManager::~DBManager() {
	FastMutex::ScopedLock lock(_fmDB);
	_stoped = true;

	for(auto it : _vdb) {
		if(it->busy()) {
			auto tryWaitCount = 20;
			while(tryWaitCount-- && it->busy()) {
				Sleep(100);
			}

			assert(!it->busy());
		}

		it->disconnect();
		delete it;
	}
}

bool DBManager::conectDB(const i8* h, const i8* u, const i8* w, u16 p) {
	if(nullptr == h || nullptr == u || nullptr == w) {
		printError("连接数据库参数错误");
		return false;
	}

	_host = h; _user = u; _pwd = w; _port = p;

	FastMutex::ScopedLock lock(_fmDB);
	for(auto i = 0; i < 2; ++i) {
		auto db = createDB();
		if(nullptr != db) {
			_vdb.push_back(db);
		} else {
			_vdb.clear();
			return false;
		}
	}

	return true;
}

ustring DBManager::escape(const ustring& input) {
	DBHolder db(getDB());
	if(db) {
		return db->escape(input);
	}

	return __emptyString;
}

bool DBManager::useDataBase(const ustring& dbname) {
	_dbname = dbname;

	FastMutex::ScopedLock lock(_fmDB);
	auto sql = fmt::format("use {}", _dbname);
	for(auto& it : _vdb) {
		if(!it->exec(sql.c_str())) {
			vprintTrace("选择数据库失败 ： {}", _dbname);
			return false;
		}
	}

	return true;
}

bool DBManager::createDataBase(const ustring& dbname) {
	DBHolder db(getDB());
	if(db) {
		auto sql = fmt::format("create database if not exists {}", dbname);
		return db->exec(sql.c_str());
	}

	return false;
}

bool DBManager::transaction(std::function<bool()> fn) {
	auto success = false;
	DBHolder db(getDB());
	if(db) {
		if(db->beginTrans()) {
			{
				FastMutex::ScopedLock lock(_fmDB);
				++_transactionDB;
				db->setTransaction(GetCurrentThreadId());
			}

			if(fn()) {
				success = db->commitTrans();
			} else {
				db->rollbackTrans();
			}

			{
				FastMutex::ScopedLock lock(_fmDB);
				--_transactionDB;
				db->setTransaction(0);
			}
		}
	}

	return success;
}

TablePtr DBManager::query(const i8* sql, i64* totals /* = nullptr */) {
	DBHolder db(getDB());
	if(db) {
		auto table = db->query(sql, 0xffffffff, totals);
		if(nullptr != table) {
			return TablePtr(table);
		}
	}

	return std::make_shared<ITable>();
}

TablePtr DBManager::query(const ustring& sql, i64* totals /* = nullptr */) {
	DBHolder db(getDB());
	if(db) {
		auto table = db->query(sql.c_str(), sql.size(), totals);
		if(nullptr != table) {
			return TablePtr(table);
		}
	}

	return std::make_shared<ITable>();
}

bool DBManager::exec(const i8* sql, i64* affectedRow /* = nullptr */) {
	DBHolder db(getDB());
	if(db) {
		return db->exec(sql, 0xffffffff, affectedRow);
	}

	return false;
}

bool DBManager::exec(const ustring& sql, i64* affectedRow /* = nullptr */) {
	DBHolder db(getDB());
	if(db) {
		return db->exec(sql.c_str(), sql.size(), affectedRow);
	}

	return false;
}

bool DBManager::getTableMetaData(tableMetaData& mdatas,
	const i8* tablename /* = nullptr */) {
	ustring wsql = fmt::format(" where table_schema = '{}' ", _dbname);
	if(nullptr != tablename) {
		wsql.append(fmt::format(" and table_name = '{}' ", tablename));
	}

	auto sql = fmt::format(
		"select table_name, COLUMN_NAME as c, "
		"DATA_TYPE as dt, COLUMN_TYPE as ct, "
		"ORDINAL_POSITION as pos, COLUMN_KEY as _key "
		"from information_schema.COLUMNS "
		"{} order by pos asc", wsql);

	auto table = query(sql);
	if(!table->valid()) {
		mdatas.clear();
		return false;
	}

	while(table->next()) {
		auto pos = table->asU64("pos");
		auto dtype = table->asUTF8("dt");
		auto ctype = table->asUTF8("ct");
		auto cname = table->asUTF8("c");
		auto tname = table->asUTF8("table_name");

		bool pkey = false;
		if(String::equalNoCase(table->asCUTF8("_key"), "PRI")) {
			pkey = true;
		}

		auto type = Field::kTypeUTF8;
		if(dtype == "int" || dtype == "tinyint" || dtype == "smallint") {
			if(std::string::npos != ctype.find("unsigned")) {
				type = Field::kTypeU32;
			} else {
				type = Field::kTypeI32;
			}

		} else if(dtype == "bigint") {
			if(std::string::npos != ctype.find("unsigned")) {
				type = Field::kTypeU64;
			} else {
				type = Field::kTypeI64;
			}
		}

		auto found = mdatas.find(tname);
		if(mdatas.end() != found) {
			found->second.push_back(Field(type, (u32)pos, cname));
		} else {
			std::vector<Field> vField;
			vField.push_back(Field(type, (u32)pos, cname));
			mdatas.emplace(tname, vField);
		}
	}

	return true;
}

DB* DBManager::getDB() {
	FastMutex::ScopedLock lock(_fmDB);
	if(_stoped) {
		return nullptr;
	}

	if(0 != _transactionDB) {
		for(auto& it : _vdb) {
			if(it->transactioning(GetCurrentThreadId())) {
				return it;
			}
		}
	}

	for(auto& it : _vdb) {
		if(!it->busy()) {
			it->setBusy(true);
			return it;
		}
	}

	auto newdb = createDB();
	if(nullptr != newdb) {
		_vdb.push_back(newdb);
		newdb->setBusy(true);
		if(20 < _vdb.size()) {
			printWarn("系统已经产生了20个数据库连接池，注意性能");
		}

		return newdb;
	}

	return nullptr;
}

DB* DBManager::createDB() {
	auto db = new DB(this);
	if(db->connect(_host.c_str(), _user.c_str(), _pwd.c_str(), _port)) {
		if(!_dbname.empty()) {
			db->exec(fmt::format("use {}", _dbname).c_str());
		}

		return db;
	} 
	
	delete db;
	printError("产生空闲的DB数据连接失败");
	return nullptr;
}

void DBManager::freeDB(DB* db) {
	assert(nullptr != db);

	FastMutex::ScopedLock lock(_fmDB);
	if(db->transactioning(GetCurrentThreadId())) {
		return;
	}

	db->setBusy(false);
	auto maxSize = 2 * System::getCPUCount();
	if(maxSize >= _vdb.size()) {
		return;
	}

	for(auto it = _vdb.begin(); it != _vdb.end(); ++it) {
		if(db == (*it)) {
			delete (*it);
			_vdb.erase(it);
			break;
		}
	}
}

}

}