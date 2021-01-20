#include "SqliteDB.h"

#include "SqliteTable.h"

#include <helper/Safedelete.h>
#include <foundation/include/base/String.h>

namespace ws {

namespace sqlite {

ustring quote(const ustring& value) {
	ustring keyWord = value;
	ws::String::replace(keyWord, "'", "''");
	return keyWord;
}

ustring escape(const ustring& value) {
	ustring keyWord = value;
	ws::String::replace(keyWord, "/", "//");
	ws::String::replace(keyWord, "'", "''");
	ws::String::replace(keyWord, "[", "/[");
	ws::String::replace(keyWord, "]", "/]");
	ws::String::replace(keyWord, "%", "/%");
	ws::String::replace(keyWord, "&", "/&");
	ws::String::replace(keyWord, "_", "/_");
	ws::String::replace(keyWord, "(", "/(");
	ws::String::replace(keyWord, ")", "/)");
	return keyWord;
}

ustring unescape(const ustring& value) {
	ustring keyWord = value;
	ws::String::replace(keyWord, "//", "/");
	ws::String::replace(keyWord, "''", "'");
	ws::String::replace(keyWord, "/[", "[");
	ws::String::replace(keyWord, "/]", "]");
	ws::String::replace(keyWord, "/%", "%");
	ws::String::replace(keyWord, "/&", "&");
	ws::String::replace(keyWord, "/_", "_");
	ws::String::replace(keyWord, "/(", "(");
	ws::String::replace(keyWord, "/)", ")");
	return keyWord;
}

IDB* createDB() {
	return new DB();
}

void destroyDB(IDB* value) {
	auto v = dynamic_cast<DB*>(value);
	ws::Safedelete::checkedDelete(v);
}

DB::DB() : _hdb(nullptr) {
	assert(sqlite3_threadsafe());
}

DB::~DB() {
	close();
}

bool DB::open(const ustring& dbpath, const ustring& password) {
	assert(nullptr == _hdb);

	FastMutex::ScopedLock lock(_fmDB);

	sqlite3* pdb = nullptr;
	auto ecode = SQLITE_OK;
	if(SQLITE_OK != (ecode = sqlite3_open(dbpath.c_str(), &pdb))) {
		vprintError("连接数据库失败 ： {}", ecode);
		return false;
	}

	if(SQLITE_OK != (ecode = sqlite3_key(
		pdb, password.c_str(), password.size()))) {
		vprintError("数据库密码错误 ： {}", ecode);
		return false;
	}

	_hdb = pdb;
	return true;
}

void DB::close() {
	FastMutex::ScopedLock lock(_fmDB);

	if(nullptr != _hdb) {
		sqlite3_close(_hdb);
		_hdb = nullptr;
	}
}

bool DB::transaction(fnTransaction fn) {
	FastMutex::ScopedLock lock(_fmDB);

	if(nullptr == _hdb) {
		printError("数据库未连接");
		return false;
	}

	if(beginTrans()) {
		if(fn()) {
			return commmitTrans();
		} else {
			rollbackTrans();
		}
	}

	return false;
}

bool DB::exec(const ustring& sql, i64* effectrows /* = nullptr */) {
	FastMutex::ScopedLock lock(_fmDB);

	if(nullptr == _hdb) {
		printError("数据库未连接");
		return false;
	}

	sqlite3_stmt* stmt = nullptr;
	if(SQLITE_OK != sqlite3_prepare_v2(_hdb,
		sql.c_str(), sql.size(), &stmt, nullptr)) {
		vprintError("SQL执行错误码[{}] : {}", sqlite3_errmsg(_hdb), sql);
		sqlite3_finalize(stmt);
		return false;
	}

	while(SQLITE_ROW == sqlite3_step(stmt)) {

	}

	if(nullptr != effectrows) {
		*effectrows = sqlite3_changes(_hdb);
	}

	sqlite3_finalize(stmt);
	return true;
}

TablePtr DB::query(const ustring& sql, i64* totalRow /* = nullptr */) {
	FastMutex::ScopedLock lock(_fmDB);

	if(nullptr == _hdb) {
		printError("数据库未连接");
		return std::make_shared<ITable>();
	}

	auto ecode = SQLITE_OK;
	sqlite3_stmt* stmt = nullptr;
	if(SQLITE_OK != (ecode = sqlite3_prepare_v2(_hdb,
		sql.c_str(), sql.size(), &stmt, nullptr))) {
		vprintError("SQL查询错误码 {} : {}", sqlite3_errmsg(_hdb), sql);
		sqlite3_finalize(stmt);
		return std::make_shared<ITable>();
	}

	TablePtr table(new Table());

	auto columnCount = sqlite3_column_count(stmt);
	Field::Type type = Field::kTypeNull;
	for(auto i = 0; i < columnCount; ++i) {
		auto name = sqlite3_column_name(stmt, i);
		ustring dtype = sqlite3_column_decltype(stmt, i);
		String::toLower(dtype);
		if(ustring::npos != dtype.find("int")) {
			type = Field::kTypeI64;
		} else {
			type = Field::kTypeUTF8;
		}

		((Table*)(table.get()))->pushField(new Field(type, i, name));
	}

	if(nullptr != totalRow) {
		*totalRow = sqlite3_data_count(stmt);
	}

	while(SQLITE_ROW == sqlite3_step(stmt)) {
		auto row = new Row();
		for(auto i = 0; i < columnCount; ++i) {
			auto field = table->getField(i);
			switch(field->_type) {
				case Field::kTypeI64: {
					auto f = Cell::kFlagI64 | Cell::kFlagBool;
					auto v = sqlite3_column_int64(stmt, i);
					if((std::numeric_limits<i32>::max)() >= v &&
						(std::numeric_limits<i32>::min)() <= v) {
						f |= Cell::kFlagI32;
					}

					if(0 <= v) {
						f |= Cell::kFlagU64;
						if((std::numeric_limits<i32>::max)() >= v) {
							f |= Cell::kFlagU32;
						}
					}
					row->push(new Cell(v, f)); 
				} break;

				default: {
					auto t = (const i8*)sqlite3_column_text(stmt, i);
					row->push(new Cell(t));
				} break;
			}
		}
		
		((Table*)(table.get()))->pushRow(row);
	}

	sqlite3_finalize(stmt);
	return table;
}

bool DB::beginTrans() {
	return this->exec("BEGIN TRANSACTION");
}

bool DB::commmitTrans() {
	return this->exec("END TRANSACTION");
}

bool DB::rollbackTrans() {
	return this->exec("ROLLBACK");
}

}

}