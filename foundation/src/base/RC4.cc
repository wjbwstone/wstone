#include <foundation/include/base/RC4.h>

namespace ws {

namespace RC4 {

static void __init(byte *randkey, byte *key, i32 keylen) {
	for(i32 i = 0; i < 256; ++i) {
		randkey[i] = i;
	}

	i32 j = 0;
	for(i32 i = 0; i < 256; ++i) {
		j = (j + randkey[i] + key[i % keylen]) & 0xff;
		byte temp = randkey[i];
		randkey[i] = randkey[j];
		randkey[j] = temp;
	}
}

static void __crypt(byte *key, byte *Data, i32 len_data) {
	i32 k = 0;
	i32 i = 0;
	i32 j = 0;
	while(k < len_data) {
		i = (i + 1) & 0xff;
		byte a = key[i];
		j = (j + a) & 0xff;
		byte b = key[j];
		key[i] = b;
		key[j] = a;
		Data[k++] ^= key[(a + b) & 0xff];
	}
}

void encrypt(ustring& inout, const i8* key) {
	encrypt(&inout[0], inout.size(), key);
}

void decrypt(ustring& inout, const i8* key) {
	decrypt(&inout[0], inout.size(), key);
}

void encrypt(i8* inout, u32 bytes, const i8* key) {
	if(nullptr != inout && nullptr != key && 0 != strlen(key)) {
		byte _key[256] = {0};
		__init(_key, (byte*)key, strlen(key));
		__crypt(_key, (byte*)inout, bytes);
	}
}

void decrypt(i8* inout, u32 bytes, const i8* key) {
	if(nullptr != inout && nullptr != key && 0 != strlen(key)) {
		byte _key[256] = {0};
		__init(_key, (byte*)key, strlen(key));
		__crypt(_key, (byte*)inout, bytes);
	}
}

}

}