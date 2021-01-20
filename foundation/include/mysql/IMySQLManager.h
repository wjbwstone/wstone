#ifndef _LIBMYSQL_IDBMANAGER_2020_01_15_11_14_17_H
#define _LIBMYSQL_IDBMANAGER_2020_01_15_11_14_17_H

#include <helper/All.h>

#include "MySQLField.h"
#include "IMySQLTable.h"

/*******************************************************************
*@brief  : 
1> 查询分页的sql执行语句需包含 SQL_CALC_FOUND_ROWS 修饰来获取总条数
2> 整数在合理范围可为i32,u32,i64,bool,u64类型
3> 不支持double类型，日期类型可转换为string或者u64
********************************************************************/

namespace ws {

namespace mysql {

//table名称-表数据结构
typedef std::map<ws::ustring, std::vector<ws::mysql::Field>> tableMetaData;

class IDBManager : public SimpleLogSink {
public:
	virtual ustring escape(const ustring& input) = 0;
	virtual bool useDataBase(const ustring& dbname) = 0;
	virtual bool createDataBase(const ustring& dbname) = 0;
	virtual bool transaction(std::function<bool(void)> fn) = 0;
	virtual TablePtr query(const i8* sql, i64* totals = nullptr) = 0;
	virtual TablePtr query(const ustring& sql, i64* totalRow = nullptr) = 0;
	virtual bool exec(const i8* sql, i64* affectedRow = nullptr) = 0;
	virtual bool exec(const ustring& sql, i64* affectedRow = nullptr) = 0;
	virtual bool conectDB(const i8* h, const i8* u, const i8* w, u16 p) = 0;

	/*******************************************************************
	*@brief  : 获取数据表元数据信息
	*table : 默认访问当前数据库所有表元数据
	********************************************************************/
	virtual bool getTableMetaData(tableMetaData&, const i8* table = nullptr) = 0;
};

IDBManager* createDBManager();
void destroyDBManager(IDBManager*);

}

}

#pragma comment(lib, "libmysql.lib")

#endif //_LIBMYSQL_IDBMANAGER_2020_01_15_11_14_17_H