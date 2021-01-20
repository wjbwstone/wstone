#ifndef _LIBJSON_FASTJSON_2020_01_15_13_07_41_H
#define _LIBJSON_FASTJSON_2020_01_15_13_07_41_H

#include <helper/Types.h>
#include <helper/Format.h>
#include <helper/Uncopyable.h>

#include "rapidjson/rapidjson.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

namespace ws {

class FastJson;
class JsonWriter;

class JsonWriter {
	kDisableCopyAssign(JsonWriter);

public:
	JsonWriter()  : _writer(_buffer) { }
	operator bool() const { return _writer.IsComplete(); }

public:
	void startObject(const i8* key = nullptr);
	void endObject() { _writer.EndObject(); }
	void startArray(const i8* key = nullptr);
	void endArray() { _writer.EndArray(); }

public:
	bool write(const i8* jdata, const i8* key = nullptr);
	bool write(const FastJson& value, const i8* key = nullptr);

public: //object
	void add(const i8* key) { setKey(key); _writer.Null(); }
	void add(const i8* key, bool v) { setKey(key); _writer.Bool(v); }
	void add(const i8* key, i32 v) { setKey(key); _writer.Int(v); }
	void add(const i8* key, u32 v) { setKey(key); _writer.Uint(v); }
	void add(const i8* key, i64 v) { setKey(key); _writer.Int64(v); }
	void add(const i8* key, u64 v) { setKey(key); _writer.Uint64(v); }
	void add(const i8* key, const i8* v) { setKey(key); append(v); }
	void add(const i8* key, const ustring& v) { setKey(key); append(v); }

public: //array
	void append() { _writer.Null(); }
	void append(bool v) { _writer.Bool(v); }
	void append(i32 v) { _writer.Int(v); }
	void append(u32 v) { _writer.Uint(v); }
	void append(i64 v) { _writer.Int64(v); }
	void append(u64 v) { _writer.Uint64(v); }
	void append(const i8* v) { if(nullptr != v) _writer.String(v); }
	void append(const ustring& v) { _writer.String(v.c_str(), v.size()); }

public:
	ustring toString() const;

protected:
	void setKey(const i8* key) { if(nullptr != key) _writer.Key(key); }

private:
	rapidjson::StringBuffer _buffer;
	rapidjson::Writer<rapidjson::StringBuffer> _writer;
};

class FastJson {
public:
	FastJson();
	~FastJson();

	/*******************************************************************
	*@brief  : 使用者慎用
	********************************************************************/
	FastJson(const FastJson& other);

protected:
	FastJson(const FastJson* parent, rapidjson::Value* value);

public:
	void clear();
	bool parse(const ustring& data);
	bool parse(const i8* data, u32 bytes = 0xffffffff);
	
public:
	u32 memberSize() const;
	ustring memberName(u32 index) const;
	bool hasMember(const i8* key) const;
	bool hasMember(const ustring& key) const;
	
	FastJson operator[](const i8* key);
	FastJson operator[](const i8* key) const;

	FastJson& operator=(bool value);
	FastJson& operator=(i32 value);
	FastJson& operator=(u32 value);
	FastJson& operator=(i64 value);
	FastJson& operator=(u64 value);
	FastJson& operator=(double value);
	FastJson& operator=(const i8* value);
	FastJson& operator=(const ustring& value);

	FastJson& operator=(FastJson&& other);
	FastJson& operator=(const FastJson& other);
	
public: //array
	bool empty() const;
	u32 arraySize() const;

	FastJson operator[](i32 index);
	FastJson operator[](i32 index) const;

	FastJson& append(bool value);
	FastJson& append(i32 value);
	FastJson& append(u32 value);
	FastJson& append(i64 value);
	FastJson& append(u64 value);
	FastJson& append(double value);
	FastJson& append(const ustring& value);
	FastJson& append(const i8* value, u32 bytes = 0xffffffff);
	
	FastJson& append(FastJson&& other);
	FastJson& append(const FastJson& value);

public:
	ustring asString() const;
	const i8* asCString() const;
	i32 asI32(i32 defaultValue = 0) const;
	u32 asU32(u32 defaultValue = 0) const;
	i64 asI64(i64 defaultValue = 0) const;
	u64 asU64(u64 defaultValue = 0) const;
	bool asBool(bool defaultValue = false) const;
	double asDouble(double defaultValue = 0.00) const;

public:
	ustring toShortString() const;
	ustring toPrettyString() const;

public:
	using Allocator = rapidjson::Document::AllocatorType;
	Allocator& allocator() { return _doc->GetAllocator(); }
	rapidjson::Document& document() { return *(_doc.get()); }
	rapidjson::Document& document() const { return *(_doc.get()); }

private:
	rapidjson::Value* getValue() { return _value ? _value : _doc.get(); }
	rapidjson::Value* getValue() const { return _value ? _value : _doc.get(); }
	
	rapidjson::Value* getArray() {
		auto v = getValue();
		if(v->IsNull()) {
			v->SetArray();
		}
		return v;
	}

private:
	rapidjson::Value* _value;
	std::shared_ptr<rapidjson::Document> _doc;
};

}

#endif //_LIBJSON_FASTJSON_2020_01_15_13_07_41_H