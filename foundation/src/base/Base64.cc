
#include <foundation/include/base/Base64.h>

namespace ws {

namespace Base64 {

static const ustring __c64 =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static bool __isValidCh(byte c) {
	return (isalnum(c) || (c == '+') || (c == '/'));
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

ustring encode(const i8* input, u32 bytes) {
	if(nullptr == input || 0 == bytes) {
		return __emptyString;
	}

	byte c3[3];
	byte c4[4];
	auto i = 0;
	ustring ret;
	auto source = reinterpret_cast<const byte*>(input);
	while(bytes--) {
		c3[i++] = *(source++);
		if(i == 3) {
			ret.append(1, __c64[(c3[0] & 0xfc) >> 2]);
			ret.append(1, __c64[((c3[0] & 0x03) << 4) + ((c3[1] & 0xf0) >> 4)]);
			ret.append(1, __c64[((c3[1] & 0x0f) << 2) + ((c3[2] & 0xc0) >> 6)]);
			ret.append(1, __c64[c3[2] & 0x3f]);
			i = 0;
		}
	}

	if(i) {
		for(auto j = i; j < 3; ++j) {
			c3[j] = '\0';
		}

		c4[0] = (c3[0] & 0xfc) >> 2;
		c4[1] = ((c3[0] & 0x03) << 4) + ((c3[1] & 0xf0) >> 4);
		c4[2] = ((c3[1] & 0x0f) << 2) + ((c3[2] & 0xc0) >> 6);
		c4[3] = c3[2] & 0x3f;

		for(auto j = 0; j < i + 1; ++j) {
			ret.append(1, __c64[c4[j]]);
		}

		while(i++ < 3) {
			ret += '=';
		}
	}

	return ret;
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

	auto i = 0;
	auto in_ = 0;
	byte c3[3];
	byte c4[4];
	ustring ret;

	auto source = reinterpret_cast<const byte*>(input);
	while(bytes-- && (source[in_] != '=') && __isValidCh(source[in_])) {
		c4[i++] = source[in_]; 
		in_++;
		if(i == 4) {
			for(i = 0; i < 4; i++) {
				c4[i] = static_cast<byte>(__c64.find(c4[i]));
			}

			c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
			c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
			c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

			for(i = 0; i < 3; i++) {
				ret.append(1, c3[i]);
			}
			i = 0;
		}
	}

	if(i) {
		for(auto j = i; j < 4; j++) {
			c4[j] = 0;
		}

		for(auto j = 0; j < 4; j++) {
			c4[j] = (byte)__c64.find(c4[j]);
		}

		c3[0] = (c4[0] << 2) + ((c4[1] & 0x30) >> 4);
		c3[1] = ((c4[1] & 0xf) << 4) + ((c4[2] & 0x3c) >> 2);
		c3[2] = ((c4[2] & 0x3) << 6) + c4[3];

		for(auto j = 0; j < (i - 1); j++) {
			ret.append(1, c3[j]);
		}
	}

	return ret;
}

}

}