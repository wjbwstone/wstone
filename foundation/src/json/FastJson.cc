#include <foundation/include/json/FastJson.h>

#include <helper/Safedelete.h>

namespace ws {

using namespace rapidjson;

void JsonWriter::startObject(const i8* key /*= nullptr*/) {
	setKey(key);
	_writer.StartObject();
}

void JsonWriter::startArray(const i8* key /*= nullptr*/) {
	setKey(key);
	_writer.StartArray();
}

bool JsonWriter::write(const FastJson& value, const i8* key /*= nullptr*/) {
	setKey(key);
	return value.document().Accept(_writer);
}

bool JsonWriter::write(const i8* jdata, const i8* key /*= nullptr*/) {
	if(nullptr != jdata) {
		setKey(key);
		rapidjson::Reader v;
		return !v.Parse(rapidjson::StringStream(jdata), _writer).IsError();
	}
	
	return false;
}

ustring JsonWriter::toString() const {
	return _writer.IsComplete() ?
		ustring(_buffer.GetString(), _buffer.GetSize()) : __emptyString;
}

FastJson::FastJson() : _value(nullptr) {
	_doc = std::make_shared<rapidjson::Document>();
}

FastJson::~FastJson() {
	
}

FastJson::FastJson(const FastJson* parent, rapidjson::Value* value) :
	_doc(parent->_doc), _value(value) {
	assert(_doc);
}

FastJson::FastJson(const FastJson& other) : 
	_doc(other._doc), _value(other._value) {
	assert(_doc);
}

FastJson& FastJson::operator=(bool value) {
	getValue()->SetBool(value);
	return *this;
}

FastJson& FastJson::operator=(i32 value) {
	getValue()->SetInt(value);
	return *this;
}

FastJson& FastJson::operator=(u32 value) {
	getValue()->SetUint(value);
	return *this;
}

FastJson& FastJson::operator=(i64 value) {
	getValue()->SetInt64(value);
	return *this;
}

FastJson& FastJson::operator=(u64 value) {
	getValue()->SetUint64(value);
	return *this;
}

FastJson& FastJson::operator=(double value) {
	getValue()->SetDouble(value);
	return *this;
}

FastJson& FastJson::operator=(const i8* value) {
	if(nullptr != value) {
		getValue()->SetString(value, strlen(value), allocator());
	}

	return *this;
}

FastJson& FastJson::operator=(const ustring& value) {
	getValue()->SetString(value.c_str(), value.size(), allocator());
	return *this;
}

FastJson& FastJson::operator=(const FastJson& other) {
	auto v = getValue();
	v->CopyFrom((*other.getValue()), allocator());
	return *this;
}

FastJson& FastJson::operator=(FastJson&& other) {
	if(this != &other) {
		auto v = getValue();
		*v = std::move(*other.getValue());
	}

	return *this;
}

void FastJson::clear() {
	getValue()->SetNull();
}

bool FastJson::parse(const ustring& data) {
	return parse(data.c_str(), data.size());
}

bool FastJson::parse(const i8* data, u32 bytes) {
	if(nullptr == _value && nullptr != data) {
		bytes = (0xffffffff != bytes) ? bytes : strlen(data);
		_doc->Parse<rapidjson::kParseDefaultFlags |
			rapidjson::kParseCommentsFlag>(data, bytes);
		return !_doc->HasParseError();
	}

	assert(false && "Its not document!!!");
	return false;
}

u32 FastJson::memberSize() const {
	const auto& d = getValue();
	if(d->IsObject()) {
		return d->MemberCount();
	}

	return 0;
}

ustring FastJson::memberName(u32 index) const {
	if(index < memberSize()) {
		auto m = getValue()->MemberBegin() + index;
		return ustring(m->name.GetString(), m->name.GetStringLength());
	}

	assert(false && "Boom, Maybe out of range!!!");
	return __emptyString;
}

bool FastJson::hasMember(const ustring& key) const {
	return hasMember(key.c_str());
}

bool FastJson::hasMember(const i8* key) const {
	const auto& d = getValue();
	if(nullptr != key && d->IsObject()) {
		return (d->FindMember(key) != d->MemberEnd());
	}

	return false;
}

FastJson FastJson::operator[](const i8* key) {
	if(nullptr != key) {
		auto v = getValue();
		if(v->IsNull()) {
			v->SetObject();
		}

		if(v->IsObject()) {
			auto found = v->FindMember(key);
			if(v->MemberEnd() != found) {
				return FastJson(this, &found->value);
			}

			v->AddMember(rapidjson::Value(key, allocator()),
				rapidjson::Value(kNullType), allocator());
			found = --v->MemberEnd();
			return FastJson(this, &found->value);
		}
	}

	assert(getValue()->IsObject() && "Json data type incorrent!!!");
	static rapidjson::Value __value(kNullType);
	return FastJson(this, &__value);
}

FastJson FastJson::operator[](const i8* key) const {
	if(nullptr != key) {
		const auto& v = getValue();
		if(v->IsObject()) {
			auto found = v->FindMember(key);
			if(v->MemberEnd() != found) {
				return FastJson(this, &found->value);
			}
		}
	}

	assert(getValue()->IsObject() && "Json data type incorrent!!!");
	static rapidjson::Value __value(kNullType);
	return FastJson(this, &__value);
}

u32 FastJson::arraySize() const {
	const auto& v = getValue();
	if(v->IsArray()) {
		return v->Size();
	}

	assert(false && "Json data type incorrent!!!");
	return 0;
}

bool FastJson::empty() const {
	const auto& v = getValue();
	if(v->IsArray()) {
		return v->Empty();
	}

	assert(false && "Json data type incorrent!!!");
	return true;
}

FastJson FastJson::operator[](i32 index) {
	auto v = getArray();
	if(v->IsArray()) {
		if((u32)index < v->Size()) {
			return FastJson(this, v->Begin() + index);
		} else {
			v->PushBack(Value(kNullType), allocator());
			return FastJson(this, v->Begin() + (v->Size() - 1));
		}
	}

	assert(false && "Json data type incorrent!!!");
	static rapidjson::Value __value(kNullType);
	return FastJson(this, &__value);
}

FastJson FastJson::operator[](i32 index) const {
	const auto& v = getValue();
	if(v->IsArray()) {
		if((u32)index < v->Size()) {
			return FastJson(this, v->Begin() + index);
		}
	}

	assert(false && "Json data type incorrent!!!");
	static rapidjson::Value __value(kNullType);
	return FastJson(this, &__value);
}

FastJson& FastJson::append(bool value) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(value ? Value(kTrueType) : Value(kFalseType), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(i32 value) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(Value(value), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(u32 value) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(Value(value), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(i64 value) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(Value(value), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(u64 value) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(Value(value), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(double value) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(Value(value), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(const ustring& value) {
	return append(value.c_str(), value.size());
}

FastJson& FastJson::append(const i8* value, u32 bytes) {
	auto v = getArray();
	if(v->IsArray()) {
		bytes = (0xffffffff != bytes) ? bytes : strlen(value);
		v->PushBack(Value(value, bytes, allocator()), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(const FastJson& other) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(Value(*other.getValue(), allocator()), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

FastJson& FastJson::append(FastJson&& other) {
	auto v = getArray();
	if(v->IsArray()) {
		v->PushBack(std::move(*other.getValue()), allocator());
		return *this;
	}

	assert(false && "Json data type incorrent!!!");
	return *this;
}

bool FastJson::asBool(bool defaultValue /* = false */) const {
	const auto& v = getValue();
	if(v->IsBool()) {
		return v->GetBool();
	} else if(v->IsNumber()) {
		return (0 != v->GetInt64());
	}

	assert(false && "Json data type incorrent!!!");
	return defaultValue;
}

i32 FastJson::asI32(i32 defaultValue /* = 0 */) const {
	const auto& v = getValue();
	if(v->IsInt()) {
		return v->GetInt();
	}

	assert(false && "Json data type incorrent!!!");
	return defaultValue;
}

u32 FastJson::asU32(u32 defaultValue /* = 0 */) const {
	const auto& v = getValue();
	if(v->IsUint()) {
		return v->GetUint();
	}

	assert(false && "Json data type incorrent!!!");
	return defaultValue;
}

i64 FastJson::asI64(i64 defaultValue /* = 0 */) const {
	const auto& v = getValue();
	if(v->IsInt64()) {
		return v->GetInt64();
	}

	assert(false && "Json data type incorrent!!!");
	return defaultValue;
}

u64 FastJson::asU64(u64 defaultValue /* = 0 */) const {
	const auto& v = getValue();
	if(v->IsUint64()) {
		return v->GetUint64();
	}

	assert(false && "Json data type incorrent!!!");
	return defaultValue;
}

double FastJson::asDouble(double defaultValue /* = 0.00 */) const {
	const auto& v = getValue();
	if(v->IsDouble()) {
		return v->GetDouble();
	} else if(v->IsNumber()) {
		return static_cast<double>(v->GetInt64());
	}

	assert(false && "Json data type incorrent!!!");
	return defaultValue;
}

ustring FastJson::asString() const {
	const auto& v = getValue();
	if(v->IsString()) {
		return ustring(v->GetString(), v->GetStringLength());
	}

	assert(false && "Json data type incorrent!!!");
	return __emptyString;
}

const i8* FastJson::asCString() const {
	const auto& v = getValue();
	if(v->IsString()) {
		return v->GetString();
	}

	assert(false && "Json data type incorrent!!!");
	return __emptyString.c_str();
}

ws::ustring FastJson::toShortString() const {
	if(!_doc->HasParseError()) {
		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> v(buffer);
		getValue()->Accept(v);
		if(v.IsComplete()) {
			return ustring(buffer.GetString(), buffer.GetSize());
		}
	}

	return __emptyString;
}

ws::ustring FastJson::toPrettyString() const {
	if(!_doc->HasParseError()) {
		rapidjson::StringBuffer buffer;
		rapidjson::PrettyWriter<rapidjson::StringBuffer> v(buffer);
		getValue()->Accept(v);
		if(v.IsComplete()) {
			return ustring(buffer.GetString(), buffer.GetSize());
		}
	}

	return __emptyString;
}

}