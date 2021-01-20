#ifndef _H_LIBCOMMON_MYSQLQUERY_2020_09_29_14_27_58
#define _H_LIBCOMMON_MYSQLQUERY_2020_09_29_14_27_58

#include "MySQLRow.h"

#include <helper/All.h>
#include <foundation/include/mysql/MySQLField.h>

namespace ws {

namespace mysql {

enum Mode {
	kQueryNone = 0,
	kQueryCreate = 1,
	kQuerySelect = 2,
	kQueryUpdate = 3,
	kQueryDelete = 4,
	kQueryInsert = 5
};

class Query {
public:
	Query(Mode mode = kQuerySelect);
	~Query();

public:
	const ustring& toString() const { return _sql; }

public:
	Mode getMode() const { return _mode; }
	bool isMode(Mode mode) const { return _mode == mode; }

public: //select mode
	Query& from(const i8* name);
	Query& select(const i8* sql);
	Query& limit(u32 beginIndex, u32 endIndex);
	Query& orderBy(const i8* key, bool desc = true);

public: //insert mode
	

private:
	Mode _mode;
	ustring _sql;
};

}

}

#endif //_H_LIBCOMMON_MYSQLQUERY_2020_09_29_14_27_58