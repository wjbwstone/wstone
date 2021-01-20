#include "MySQLQuery.h"

#include <foundation/include/base/String.h>

namespace ws {

namespace mysql {

Query::Query(Mode mode /* = kQuerySelect */) : _mode(mode) {
	
}

Query::~Query() {

}

Query& Query::select(const i8* sql) {
	if(nullptr == sql || !isMode(kQuerySelect)) {
		assert(false && "Invalid SQL statement!!!");
		return *this;
	}

	_sql.append(fmt::format("select {} ", sql));
	return *this;
}

Query& Query::from(const i8* name) {
	if(nullptr == name || !isMode(kQuerySelect)) {
		assert(false && "Invalid SQL statement!!!");
		return *this;
	}

	_sql.append(fmt::format("from {} ", name));
	return *this;
}

Query& Query::limit(u32 beginIndex, u32 endIndex) {
	assert(beginIndex <= endIndex || isMode(kQuerySelect));
	if(beginIndex <= endIndex && isMode(kQuerySelect)) {
		_sql.append(fmt::format("limit {}, {}", beginIndex, endIndex));
	}

	return *this;
}

Query& Query::orderBy(const i8* key, bool desc /* = true */) {
	assert(nullptr != key && 0 != strlen(key));
	if(nullptr != key && isMode(kQuerySelect)) {
		_sql.append(fmt::format("order by {} {} ", key, 
			desc ? "desc" : "asc"));
	}

	return *this;
}

}

}