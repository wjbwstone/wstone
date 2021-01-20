#include "MySQLDB.h"

#include <helper/SimpleLogSink.h>
#include <foundation/include/base/String.h>

#include "MySQLTable.h"
#include "MySQLManager.h"

namespace ws {

namespace mysql {

DB::DB(DBManager* manger) : _tid(0), _busy(false), _manger(manger), 
	_mysql(nullptr) {

}

DB::~DB() {
	disconnect();
}

bool DB::connect(const i8* h, const i8* u, const i8* w, u16 p) {
	assert(nullptr != h && nullptr != u && nullptr != w);
	if(nullptr != _mysql) {
		return true;
	}

	_mysql = mysql_init(nullptr);
	if(nullptr == _mysql) {
		_manger->printError("数据库初始化失败");
		return false;
	}

	u32 val = 10;
	mysql_options(_mysql, MYSQL_OPT_CONNECT_TIMEOUT, &val);
	bool reconnect = true;
	mysql_options(_mysql, MYSQL_OPT_RECONNECT, &reconnect);

	if(nullptr == mysql_real_connect(_mysql, h, u, w, 0, p, 0, 0)) {
		_manger->vprintError("数据库连接错误 ： {}", err());
		disconnect();
		return false;
	}

	if(0 != mysql_set_character_set(_mysql, "utf8mb4")) {
		if(0 != mysql_set_character_set(_mysql, "utf8")) {
			_manger->vprintError("数据库连接失败 ： {}", err());
			disconnect();
			return false;
		}
	}

	return true;
}

void DB::disconnect() {
	if(nullptr != _mysql) {
		mysql_close(_mysql);
		_mysql = nullptr;
	}
}

ustring DB::err() {
	if(nullptr != _mysql) {
		return fmt::format("{}-{}", mysql_errno(_mysql), mysql_error(_mysql));
	}

	assert(nullptr != _mysql);
	return "Unknown";
}

ws::ustring DB::escape(const ustring& in) {
	assert(nullptr != _mysql);

	ustring ret;
	ret.resize((in.size() * 2) + 1);
	auto v = mysql_real_escape_string(_mysql, &ret[0], in.data(), in.size());
	ret.resize(v);
	return ret;
}

Table* DB::query(const i8* sql, u32 bytes, i64* totals) {
	assert(nullptr != _mysql);
	if(nullptr == sql || 0 == bytes) {
		_manger->printError("查询参数错误");
		return false;
	}

	bytes = (0xffffffff != bytes) ? bytes : strlen(sql);
	auto ret = mysql_real_query(_mysql, sql, bytes);
	if(0 == ret) {
		auto table = buildTable();
		if(nullptr != table) {
			if(nullptr != totals) {
				*totals = fetchTotals();
			}
			return table;
		}
	}

	_manger->vprintError("mysql查询失败 {} : {}", err(), sql);
	return nullptr;
}

bool DB::exec(const i8* sql, u32 bytes, i64* affectedRow) {
	assert(nullptr != _mysql);
	if(nullptr == sql || 0 == bytes) {
		_manger->printError("查询参数错误");
		return false;
	}

	bytes = (0xffffffff != bytes) ? bytes : strlen(sql);
	if(0 == mysql_real_query(_mysql, sql, bytes)) {
		auto result = mysql_store_result(_mysql);
		if(0 != mysql_errno(_mysql)) {
			_manger->vprintError("mysql获取结果集失败 {} : {}", err(), sql);
			return false;
		}

		if(nullptr != affectedRow) {
			*affectedRow = mysql_affected_rows(_mysql);
		}

		if(nullptr != result) {
			mysql_free_result(result);
		}

		return true;
	}

	_manger->vprintError("mysql执行失败 {} : {}", err(), sql);
	return false;
}

i64 DB::fetchTotals() {
	i64 ret = 0;
	auto table = query("select found_rows() as t");
	if(nullptr != table && table->next()) {
		ret = table->asI64("t");
	}

	ws::Safedelete::checkedDelete(table);
	return ret;
}

Table* DB::buildTable() {
	assert(nullptr != _mysql);
	auto result = mysql_store_result(_mysql);
	if(nullptr == result) {
		_manger->vprintError("获取结果集失败: {}", err());
		return nullptr;
	}

	auto table = new Table();
	auto fieldsnum = mysql_num_fields(result);
	auto* fields = mysql_fetch_fields(result);
	buildTableHeader(table, fields, fieldsnum);
	buildTableRow(table, result, fieldsnum);

	mysql_free_result(result);
	return table;
}

void DB::buildTableHeader(Table* table, MYSQL_FIELD* fields, u32 size) {
	assert(nullptr != fields);

	for(u32 i = 0; i < size; ++i) {
		auto type = Field::kTypeNull;
		switch(fields[i].type) {
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_SHORT:
			case MYSQL_TYPE_LONG:
			case MYSQL_TYPE_INT24:
				if(fields[i].flags & UNSIGNED_FLAG) {
					type = Field::kTypeU32;
				} else {
					type = Field::kTypeI32;
				} break;

			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_NEWDECIMAL:
				if(fields[i].flags & UNSIGNED_FLAG) {
					type = Field::kTypeU64;
				} else {
					type = Field::kTypeI64;
				} break;

			default: type = Field::kTypeUTF8; break;
		}
		
		table->pushField(new Field(type, i, String::toLower(fields[i].name)));
	}
}

void DB::buildTableRow(Table* table, MYSQL_RES* result, u32 size) {
	MYSQL_ROW row;
	while(row = mysql_fetch_row(result)) {
		auto it = new Row(); 
		auto lengths = (u32*)mysql_fetch_lengths(result);
		for(u32 i = 0; i < size; ++i) {
			switch(table->getField(i)->_type) {
				case Field::kTypeUTF8:
					if(nullptr != row[i]) {
						it->push(new Cell(row[i], lengths[i])); 
					} else { //Null
						it->push(new Cell(__emptyString.c_str(), 0));
					}
					break;

				case Field::kTypeI32: {
					auto t = String::toI32(row[i]);
					auto f = Cell::kFlagI32 | Cell::kFlagI64 | Cell::kFlagBool;
					if(0 <= t) {
						f |= Cell::kFlagU32 | Cell::kFlagU64;
					}
					it->push(new Cell(t, f));
				} break;

				case Field::kTypeU32: {
					auto t = String::toU32(row[i]);
					auto f = Cell::kFlagU32 | Cell::kFlagI64 | 
						Cell::kFlagU64 | Cell::kFlagBool;
					if(u32((std::numeric_limits<i32>::max)()) >= t) {
						f |= Cell::kFlagI32;
					}
					it->push(new Cell(t, f));
				} break;

				case Field::kTypeI64: {
					auto t = String::toI64(row[i]);
					auto f = Cell::kFlagI64 | Cell::kFlagBool;

					if(0 <= t) {
						f |= Cell::kFlagU64;
						if(INT_MAX >= t) {
							f |= Cell::kFlagI32 | Cell::kFlagU32;
						} else if(UINT_MAX >= t) {
							f |= Cell::kFlagU32;
						}
					} else {
						if(INT_MIN <= t) {
							f |= Cell::kFlagI32;
						}
					}

					it->push(new Cell(t, f));
				} break;

				case Field::kTypeU64: {
					auto t = String::toU64(row[i]);

					auto f = Cell::kFlagU64 | Cell::kFlagBool;
					if(INT_MAX >= t) {
						f |= Cell::kFlagI32 | Cell::kFlagU32 | Cell::kFlagI64;
					} else if(UINT_MAX >= t) {
						f |= Cell::kFlagU32 | Cell::kFlagI64;
					} else if(LLONG_MAX >= t) {
						f |= Cell::kFlagI64;
					}

					it->push(new Cell(t, f));
				} break;

				default: assert(false && "Unknown data type!!!"); break;
			}
		}

		table->pushRow(it);
	}
}

}

}