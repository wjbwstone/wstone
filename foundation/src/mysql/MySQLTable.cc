#include "MySQLTable.h"

#include <foundation/include/base/String.h>

namespace ws {

namespace mysql {

Table::Table() {
	
}

Table::~Table() {
	for(auto it : _fields) {
		delete it;
	}

	for(auto it = _rows.cbegin(); it != _rows.cend(); ++it) {
		delete (*it);
	}
}

bool Table::next() {
	return _rows.next();
}

void Table::move(u32 index) {
	_rows.move(index);
}

u32 Table::size() const {
	return _rows.size();
}

bool Table::empty() const {
	return _rows.empty();
}

bool Table::valid() const {
	return true;
}

bool Table::asBool(const i8* key) const {
	u32 index = 0;
	if(asValue(key, Field::kTypeBool, index)) {
		return _rows->get(index)->operator bool();
	}

	return __super::asBool(key);
}

i32 Table::asI32(const i8* key) const {
	u32 index = 0;
	if(asValue(key, Field::kTypeI32, index)) {
		return _rows->get(index)->operator i32();
	}

	return __super::asI32(key);
}

u32 Table::asU32(const i8* key) const {
	u32 index = 0;
	if(asValue(key, Field::kTypeU32, index)) {
		return _rows->get(index)->operator u32();
	}

	return __super::asU32(key);
}

i64 Table::asI64(const i8* key) const {
	u32 index = 0;
	if(asValue(key, Field::kTypeI64, index)) {
		return _rows->get(index)->operator i64();
	}

	return __super::asI64(key);
}

u64 Table::asU64(const i8* key) const {
	u32 index = 0;
	if(asValue(key, Field::kTypeU64, index)) {
		return _rows->get(index)->operator u64();
	}

	return __super::asU64(key);
}

ustring Table::asUTF8(const i8* key) const {
	u32 index = 0;
	if(asValue(key, Field::kTypeUTF8, index)) {
		return _rows->get(index)->operator ustring();
	}

	return __super::asUTF8(key);
}

const i8* Table::asCUTF8(const i8* key) const {
	u32 index = 0;
	if(asValue(key, Field::kTypeUTF8, index)) {
		return _rows->get(index)->operator const ws::i8 *();
	}

	return __super::asCUTF8(key);
}

bool Table::asValue(const i8* key, Field::Type type, u32& index) const {
	auto field = getField(key);
	if(nullptr == field || !_rows) {
		return false;
	}

	if(field->_index >= _rows->size()) {
		assert(false && "Out of vector ranges!!!");
		return false;
	}

	index = field->_index;
	if(field->_type != type) {
		if(0 == (_rows->get(index)->flag() & type)) {
			assert(false && "mysql data type error!!!");
			return false;
		}
	}

	return true;
}

const Field* Table::getField(u32 index) const {
	if(_fields.size() <= index) {
		assert(false && "The field was not found!!!");
		return nullptr;
	}

	return _fields[index];
}

const Field* Table::getField(const i8* key) const {
	if(nullptr != key) {
		for(const auto& it : _fields) {
			auto p = key;
			auto r = it->_name.c_str();
			while(::tolower(*p) == *r) {
				if(*p == '\0') {
					return it;
				}

				p++;
				r++;
			}
		}
	}

	assert(false && "The field was not found!!!");
	return nullptr;
}

void Table::pushRow(const Row* row) {
	_rows.push(row);
}

void Table::pushField(const Field* field) {
	_fields.push_back(field);
}

}

}