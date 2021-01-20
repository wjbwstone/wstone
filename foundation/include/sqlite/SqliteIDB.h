#ifndef _SQLITE_IDB_2020_01_14_17_27_28_H
#define _SQLITE_IDB_2020_01_14_17_27_28_H

#include <helper/Types.h>
#include <helper/SimpleLogSink.h>

#include "SqliteITable.h"

/*******************************************************************
*@brief  : 
*1> 单连接，支持线程安全
*2> 数据类型只支持 int（最大8字节），text，blob
*3> 一次性操作的SQL语句不要超过1MB字节
*4> 对特殊字符需要转义，比如字符结束符
********************************************************************/

namespace ws {

namespace sqlite { 

typedef std::function<bool(void)> fnTransaction;

/*******************************************************************
*@brief  : 转义字符 ' 为 '', 防止语法错误
********************************************************************/
ustring quote(const ustring& value);

/*******************************************************************
*@brief  : 用于like查询，基本上用不到这个函数，与mysql有差别
********************************************************************/
ustring escape(const ustring& value);
ustring unescape(const ustring& value);

class IDB : public SimpleLogSink {
public:
	virtual void close() = 0;
	virtual bool transaction(fnTransaction fn) = 0;
	virtual bool exec(const ustring& sql, i64* effectrows = nullptr) = 0;
	virtual bool open(const ustring& dbpath, const ustring& password) = 0;
	virtual TablePtr query(const ustring& sql, i64* totalcount = nullptr) = 0;
};

IDB* createDB();
void destroyDB(IDB*);

}

}

#ifdef _DEBUG
#pragma comment(lib, "libsqlited.lib")
#else
#pragma comment(lib, "libsqlite.lib")
#endif //_DEBUG

#endif //_SQLITE_IDB_2020_01_14_17_27_28_H