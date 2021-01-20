#ifndef _LIBMYSQL_FIELD_2020_01_15_11_14_06_H
#define _LIBMYSQL_FIELD_2020_01_15_11_14_06_H

#include <helper/Types.h>

namespace ws {

namespace mysql {

class Field {
public:
	enum Type {
		kTypeNull = 0,
		kTypeBool = 1,
		kTypeI32 = 2, 
		kTypeU32 = 4, 
		kTypeI64 = 8, 
		kTypeU64 = 16, 
		kTypeUTF8 = 32,
	};

	Field(Type type, u32 index, const ustring& name) :
		_type(type), _index(index), _name(name) {

	}

public:
	Type _type;
	u32 _index;
	ustring _name;
};

}

}

#endif //_LIBMYSQL_FIELD_2020_01_15_11_14_06_H