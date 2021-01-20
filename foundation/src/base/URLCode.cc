#include <foundation/include/base/URLCode.h>

namespace ws {

namespace URL {

static byte __toHex(byte x) {
	return  (x > 9) ? (x + 55) : (x + 48);
}

static byte __fromHex(byte x) {
	return isdigit(x) ? x - '0' : tolower(x) - 'a' + 10;
}

ustring encode(const i8* input) {
	if(nullptr != input) {
		return encode(input, strlen(input));
	}

	return __emptyString;
}

ustring encode(const ustring& input) {
	return encode(input.c_str(), input.size());
}

ws::ustring encode(const i8* input, u32 bytes) {
	if(nullptr == input || 0 == bytes) {
		return __emptyString;
	}

	ustring t;
	auto d = reinterpret_cast<const byte*>(input);
	for(u32 i = 0; i < bytes; i++) {
		const auto& v = d[i];
		if( isalnum(v) || (nullptr != strchr(".-_~", v))) {
			t.append(1, v);
		} else {
			t.append(1, '%');
			t.append(1, __toHex(v >> 4));
			t.append(1, __toHex(v % 16));
		}
	}

	return t;
}

ustring decode(const i8* input) {
	if(nullptr != input) {
		return decode(input, strlen(input));
	}

	return __emptyString;
}

ustring decode(const ustring& input) {
	return decode(input.c_str(), input.size());
}

ustring decode(const i8* input, u32 bytes) {
	if(nullptr == input || 0 == bytes) {
		return __emptyString;
	}

	ustring t;
	auto d = reinterpret_cast<const byte*>(input);
	for(u32 i = 0; i < bytes; i++) {
		const auto& v = d[i];
		if('%' == v) {
			assert((i + 2) < bytes);
			if((i + 2) < bytes) {
				auto high = __fromHex(d[++i]);
				auto low = __fromHex(d[++i]);
				t.append(1, high * 16 + low);
			} 
		} else {
			t.append(1, v);
		}
	}

	return t;
}

}

}