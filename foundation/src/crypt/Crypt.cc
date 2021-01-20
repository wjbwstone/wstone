#include <foundation/include/crypt/Crypt.h>

#include <helper/Format.h>
#include <helper/StringConvert.h>
#include <foundation/include/base/String.h>

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include <foundation/src/crypt/cryptopp/cryptlib.h>
#include <foundation/src/crypt/cryptopp/md5.h>
#include <foundation/src/crypt/cryptopp/files.h>
#include <foundation/src/crypt/cryptopp/hex.h>
#include <foundation/src/crypt/cryptopp/base64.h>
#include <foundation/src/crypt/cryptopp/crc.h>
#include <foundation/src/crypt/cryptopp/sha.h>
#include <foundation/src/crypt/cryptopp/hmac.h>
#include <foundation/src/crypt/cryptopp/arc4.h>
#include <foundation/src/crypt/cryptopp/modes.h>
#include <foundation/src/crypt/cryptopp/aes.h>
#include <foundation/src/crypt/cryptopp/gcm.h>
#include <foundation/src/crypt/cryptopp/chacha.h>
#include <foundation/src/crypt/cryptopp/osrng.h>
#include <foundation/src/crypt/cryptopp/rsa.h>
#include <foundation/src/crypt/cryptopp/chachapoly.h>
#include <foundation/src/crypt/cryptopp/chachapoly.h>

namespace ws {

namespace Crypt {

#define CTRY try {
#define CCATCH } catch(const CryptoPP::Exception& e) { ws::print(e.GetWhat()); }

ws::ustring toHex(const ustring& source, bool toupper /*= true*/) {
	return toHex(source.c_str(), source.size(), toupper);
}

ws::ustring toHex(const i8* source, u32 bytes, bool toupper /*= true*/) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HexEncoder(
			new CryptoPP::StringSink(digest), toupper));
	return digest;
	CCATCH

	return __emptyString;
}

ustring toBase64(const ustring& source, bool urlMode /*= false*/) {
	return toBase64(source.c_str(), source.size(), urlMode);
}

ustring toBase64(const i8* source, u32 bytes, bool urlMode /*= false*/) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	if(!urlMode) {
		CryptoPP::StringSource((const byte*)source, bytes, true,
			new CryptoPP::Base64Encoder(
				new CryptoPP::StringSink(digest), false));
	} else {
		CryptoPP::StringSource((const byte*)source, bytes, true,
			new CryptoPP::Base64URLEncoder(
				new CryptoPP::StringSink(digest), false));
	}
	return digest;
	CCATCH

	return __emptyString;
}

ustring fromBase64(const ustring& source, bool urlMode /*= false*/) {
	return fromBase64(source.c_str(), source.size(), urlMode);
}

ustring fromBase64(const i8* source, u32 bytes, bool urlMode /*= false*/) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	if(!urlMode) {
		CryptoPP::StringSource((const byte*)source, bytes, true,
			new CryptoPP::Base64Decoder(
				new CryptoPP::StringSink(digest)));
	} else {
		CryptoPP::StringSource((const byte*)source, bytes, true,
			new CryptoPP::Base64URLDecoder(
				new CryptoPP::StringSink(digest)));
	}
	return digest;
	CCATCH

	return __emptyString;
}

ustring md5(const ustring& source, bool toupper /* = true */) {
	return md5(source.c_str(), source.size(), toupper);
}

ustring md5(const i8* source, u32 bytes, bool toupper /* = true */) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::Weak::MD5 hash;
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

ustring md5File(const i8* fpath, bool toupper /*= true*/) {
	if(nullptr == fpath) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	return md5File(ws::ustring(fpath, strlen(fpath)), toupper);
}

ustring md5File(const ustring& fpath, bool toupper /* = true */) {
	CTRY
	ustring digest;
	CryptoPP::Weak::MD5 hash;
	auto wpath = Utf8ToUnicode(fpath);
	CryptoPP::FileSource(wpath.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

bool md5Verify(const ustring& md5Value, const i8* source) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return false;
	}

	return md5Verify(md5Value, source, strlen(source));
}

bool md5Verify(const ustring& md5Value, const ustring& source) {
	return md5Verify(md5Value, source.c_str(), source.size());
}

bool md5Verify(const ustring& md5Value, const i8* source, u32 bytes) {
	auto newValue = md5(source, bytes);
	return ws::String::equalNoCase(md5Value, newValue);
}

bool md5VerifyFile(const ustring& md5Value, const ustring& fpath) {
	auto newValue = md5File(fpath);
	return ws::String::equalNoCase(md5Value, newValue);
}

u32 crc32(const ustring& source) {
	return crc32(source.c_str(), source.size());
}

u32 crc32(const i8* source, u32 bytes) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return 0;
	}

	CTRY
	u32 ret = 0;
	CryptoPP::CRC32 hash;
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::ArraySink((byte*)&ret, sizeof(ret))));
	return ret;
	CCATCH

	return 0;
}

u32 crc32File(const ustring& fpath) {
	CTRY
	u32 ret = 0;
	CryptoPP::CRC32 hash;
	auto wpath = Utf8ToUnicode(fpath);
	CryptoPP::FileSource(wpath.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::ArraySink((byte*)&ret, sizeof(ret))));
	return ret;
	CCATCH

	return 0;
}

bool crc32Verify(u32 crc32Value, const ustring& source) {
	return crc32Verify(crc32Value, source.c_str(), source.size());
}

bool crc32Verify(u32 crc32Value, const i8* source, u32 bytes) {
	return (crc32Value == crc32(source, bytes));
}

bool crc32VerifyFile(u32 crc32Value, const ustring& fpath) {
	return (crc32Value == crc32File(fpath));
}

ustring sha1(const ustring& source, bool toupper /* = true */) {
	return sha1(source.c_str(), source.size(), toupper);
}

ustring sha1(const i8* source, u32 bytes, bool toupper /* = true */) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::SHA1 hash;
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

ustring sha1File(const ustring& fpath, bool toupper /* = true */) {
	CTRY
	ustring digest;
	CryptoPP::SHA1 hash;
	wstring wpath = Utf8ToUnicode(fpath);
	CryptoPP::FileSource(wpath.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH
	
	return __emptyString;
}

bool sha1Verify(const ustring& sha1Value, const i8* source, u32 bytes) {
	auto newValue = sha1(source, bytes);
	return ws::String::equalNoCase(sha1Value, newValue);
}

bool sha1Verify(const ustring& sha1Value, const ustring& source) {
	return sha1Verify(sha1Value, source.c_str(), source.size());
}

bool sha1VerifyFile(const ustring& sha1Value, const ustring& fpath) {
	auto newValue = sha1File(fpath);
	return ws::String::equalNoCase(sha1Value, newValue);
}

ustring sha256(const ustring& source, bool toupper /* = true */) {
	return sha256(source.c_str(), source.size(), toupper);
}

ustring sha256(const i8* source, u32 bytes, bool toupper /* = true */) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::SHA256 hash;
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

ustring sha256File(const ustring& fpath, bool toupper /* = true */) {
	CTRY
	ustring digest;
	CryptoPP::SHA256 hash;
	wstring wpath = Utf8ToUnicode(fpath);
	CryptoPP::FileSource(wpath.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

bool sha256Verify(const ustring& md5Value, const i8* source, u32 bytes) {
	auto newValue = sha256(source, bytes);
	return ws::String::equalNoCase(md5Value, newValue);
}

bool sha256Verify(const ustring& md5Value, const ustring& source) {
	return sha256Verify(md5Value, source.c_str(), source.size());
}

bool sha256VerifyFile(const ustring& md5Value, const ustring& fpath) {
	auto newValue = sha256File(fpath);
	return ws::String::equalNoCase(md5Value, newValue);
}

ustring sha512(const ustring& source, bool toupper /* = true */) {
	return sha512(source.c_str(), source.size(), toupper);
}

ustring sha512(const i8* source, u32 bytes, bool toupper /* = true */) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::SHA512 hash;
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

ustring sha512File(const ustring& fpath, bool toupper /* = true */) {
	CTRY
	ustring digest;
	CryptoPP::SHA512 hash;
	wstring wpath = Utf8ToUnicode(fpath);
	CryptoPP::FileSource(wpath.c_str(), true,
		new CryptoPP::HashFilter(hash,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

bool sha512Verify(const ustring& md5Value, const i8* source, u32 bytes) {
	auto newValue = sha512(source, bytes);
	return ws::String::equalNoCase(md5Value, newValue);
}

bool sha512Verify(const ustring& md5Value, const ustring& source) {
	return sha512Verify(md5Value, source.c_str(), source.size());
}

bool sha512VerifyFile(const ustring& md5Value, const ustring& fpath) {
	auto newValue = sha512File(fpath);
	return ws::String::equalNoCase(md5Value, newValue);
}

ustring hmacSha1(const ustring& source, const i8* key, bool toupper) {
	return hmacSha1(source.c_str(), source.size(), key, toupper);
}

ustring hmacSha1(const i8* source, u32 bytes,
	const i8* key, bool toupper /* = true */) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::HMAC<CryptoPP::SHA1> hmac((byte*)key, strlen(key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hmac,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

ustring hmacSha256(const ustring& source, const i8* key, bool toupper) {
	return hmacSha256(source.c_str(), source.size(), key, toupper);
}

ustring hmacSha256(const i8* source, u32 bytes,
	const i8* key, bool toupper /* = true */) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::HMAC<CryptoPP::SHA256> hmac((byte*)key, strlen(key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hmac,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

ustring hmacSha512(const ustring& source, const i8* key, bool toupper) {
	return hmacSha512(source.c_str(), source.size(), key, toupper);
}

ustring hmacSha512(const i8* source, u32 bytes,
	const i8* key, bool toupper /* = true */) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::HMAC<CryptoPP::SHA512> hmac((byte*)key, strlen(key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hmac,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

ustring hmacMD5(const ustring& source, const i8* key, bool toupper) {
	return hmacMD5(source.c_str(), source.size(), key, toupper);
}

ustring hmacMD5(const i8* source, u32 bytes,
	const i8* key, bool toupper /* = true */) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring digest;
	CryptoPP::HMAC<CryptoPP::Weak::MD5> hmac((byte*)key, strlen(key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::HashFilter(hmac,
			new CryptoPP::HexEncoder(
				new CryptoPP::StringSink(digest), toupper)));
	return digest;
	CCATCH

	return __emptyString;
}

void rc4Encrypt(ustring& inout, const i8* key) {
	rc4Encrypt((i8*)inout.c_str(), inout.size(), key);
}

void rc4Encrypt(i8* inout, u32 bytes, const i8* key) {
	if(nullptr == inout || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return;
	}

	CTRY
	byte _key[CryptoPP::Weak::ARC4::MAX_KEYLENGTH] = {0};
	u32 keybytes = (std::min)(_countof(_key), strlen(key));
	if(0 == keybytes) {
		return;
	}
	memcpy_s(_key, _countof(_key), key, keybytes);

	CryptoPP::Weak::ARC4 coder(_key, keybytes);
	coder.ProcessString((byte*)inout, bytes);
	CCATCH
}

void rc4Decrypt(ustring& inout, const i8* key) {
	return rc4Decrypt((i8*)inout.c_str(), inout.size(), key);
}

void rc4Decrypt(i8* inout, u32 bytes, const i8* key) {
	if(nullptr == inout || 0 == bytes || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return;
	}

	CTRY
	byte _key[CryptoPP::Weak::ARC4::MAX_KEYLENGTH] = {0};
	u32 keybytes = (std::min)(_countof(_key), strlen(key));
	if(0 == keybytes) {
		return;
	}
	memcpy_s(_key, _countof(_key), key, keybytes);

	CryptoPP::Weak::ARC4 coder(_key, keybytes);
	coder.ProcessString((byte*)inout, bytes);
	CCATCH
}

ustring aesEcb128Encrypt(const ustring& source, const i8* key) {
	return aesEcb128Encrypt(source.c_str(), source.size(), key);
}

ustring aesEcb128Encrypt(const i8* source, u32 bytes, const i8* key) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring cipher;
	byte _key[CryptoPP::AES::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key, 
		(std::min)(_countof(_key), strlen(key)));

	CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption coder;
	coder.SetKey(_key, _countof(_key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(cipher), 
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
	return cipher;
	CCATCH

	return __emptyString;
}

ustring aesEcb128Decrypt(const ustring& source, const i8* key) {
	return aesEcb128Decrypt(source.c_str(), source.size(), key);
}

ustring aesEcb128Decrypt(const i8* source, u32 bytes, const i8* key) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring plain;
	byte _key[CryptoPP::AES::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption coder;
	coder.SetKey(_key, _countof(_key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(plain),
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
	return plain;
	CCATCH

	return __emptyString;
}

ustring aesEcb256Encrypt(const ustring& source, const i8* key) {
	return aesEcb256Encrypt(source.c_str(), source.size(), key);
}

ustring aesEcb256Encrypt(const i8* source, u32 bytes, const i8* key) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring cipher;
	byte _key[CryptoPP::AES::MAX_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption coder;
	coder.SetKey(_key, _countof(_key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(cipher),
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
	return cipher;
	CCATCH

	return __emptyString;
}

ustring aesEcb256Decrypt(const ustring& source, const i8* key) {
	return aesEcb256Decrypt(source.c_str(), source.size(), key);
}

ustring aesEcb256Decrypt(const i8* source, u32 bytes, const i8* key) {
	if(nullptr == source || nullptr == key) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring plain;
	byte _key[CryptoPP::AES::MAX_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption coder;
	coder.SetKey(_key, _countof(_key));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(plain),
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
	return plain;
	CCATCH

	return __emptyString;
}

ustring aesCbc128Encrypt(const ustring& source,
	const i8* key, const i8* iv) {
	return aesCbc128Encrypt(source.c_str(), source.size(), key, iv);
}

ustring aesCbc128Encrypt(const i8* source, u32 bytes, 
	const i8* key, const i8* iv) {
	if(nullptr == source || nullptr == key || nullptr == iv) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring cipher;
	byte _key[CryptoPP::AES::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	byte _iv[CryptoPP::AES::BLOCKSIZE] = {0};
	memcpy_s(_iv, _countof(_iv), iv,
		(std::min)(_countof(_iv), strlen(iv)));

	CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption coder;
	coder.SetKeyWithIV(_key, _countof(_key), _iv, _countof(_iv));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(cipher),
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
	return cipher;
	CCATCH

	return __emptyString;
}

ustring aesCbc128Decrypt(const ustring& source, 
	const i8* key, const i8* iv) {
	return aesCbc128Decrypt(source.c_str(), source.size(), key, iv);
}

ustring aesCbc128Decrypt(const i8* source, u32 bytes, 
	const i8* key, const i8* iv) {
	if(nullptr == source || nullptr == key || nullptr == iv) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring plain;
	byte _key[CryptoPP::AES::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	byte _iv[CryptoPP::AES::BLOCKSIZE] = {0};
	memcpy_s(_iv, _countof(_iv), iv,
		(std::min)(_countof(_iv), strlen(iv)));

	CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption coder;
	coder.SetKeyWithIV(_key, _countof(_key), _iv, _countof(_iv));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(plain),
			CryptoPP::BlockPaddingSchemeDef::PKCS_PADDING));
	return plain;
	CCATCH

	return __emptyString;
}

ws::ustring aesGcm128Encrypt(const ustring& source, const i8* key,
	const i8* iv, const i8* aad) {
	return aesGcm128Encrypt(source.c_str(), source.size(), key, iv, aad);
}

ws::ustring aesGcm128Encrypt(const i8* source, u32 bytes, 
	const i8* key, const i8* iv, const i8* aad) {
	if(nullptr == source || nullptr == key || 
		nullptr == iv || nullptr == aad || 0 == strlen(iv)) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	byte _key[CryptoPP::AES::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	CryptoPP::GCM<CryptoPP::AES>::Encryption coder;
	coder.SetKeyWithIV(_key, _countof(_key), (byte*)iv, strlen(iv));

	ustring cipher;
	const u32 tagSize = 16;
	CryptoPP::AuthenticatedEncryptionFilter af(coder, 
		new CryptoPP::StringSink(cipher), false, tagSize);

	af.ChannelPut(CryptoPP::AAD_CHANNEL, (const byte*)aad, strlen(aad));
	af.ChannelMessageEnd(CryptoPP::AAD_CHANNEL);

	af.ChannelPut(CryptoPP::DEFAULT_CHANNEL, (const byte*)source, bytes);
	af.ChannelMessageEnd(CryptoPP::DEFAULT_CHANNEL);
	return cipher;
	CCATCH
	
	return __emptyString;
}

ws::ustring aesGcm128Decrypt(const ustring& source, const i8* key,
	const i8* iv, const i8* add) {
	return aesGcm128Decrypt(source.c_str(), source.size(), key, iv, add);
}

ws::ustring aesGcm128Decrypt(const i8* source, u32 bytes, 
	const i8* key, const i8* iv, const i8* aad) {
	const i32 tagSize = 16;
	if(nullptr == source || nullptr == key || nullptr == iv || 
		nullptr == aad || tagSize > bytes || 0 == strlen(iv)) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	byte _key[CryptoPP::AES::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	CryptoPP::GCM<CryptoPP::AES>::Decryption coder;
	coder.SetKeyWithIV(_key, _countof(_key), (byte*)iv, strlen(iv));

	ustring plain;
	CryptoPP::AuthenticatedDecryptionFilter df(coder, 
		new CryptoPP::StringSink(plain),
		CryptoPP::AuthenticatedDecryptionFilter::MAC_AT_BEGIN, tagSize);

	df.ChannelPut(CryptoPP::DEFAULT_CHANNEL, 
		(const byte*)(source + bytes - tagSize), tagSize);
	df.ChannelPut(CryptoPP::AAD_CHANNEL, 
		(const byte*)aad, strlen(aad));
	df.ChannelPut(CryptoPP::DEFAULT_CHANNEL,
		(const byte*)(source), bytes - tagSize);

	df.ChannelMessageEnd(CryptoPP::AAD_CHANNEL);
	df.ChannelMessageEnd(CryptoPP::DEFAULT_CHANNEL);

	return (df.GetLastResult()) ? plain : __emptyString;
	CCATCH

	return __emptyString;
}

ustring chaCha20Encrypt(const ustring& source,
	const i8* key, const i8* iv) {
	return chaCha20Encrypt(source.c_str(), source.size(), key, iv);
}

ustring chaCha20Encrypt(const i8* source, u32 bytes,
	const i8* key, const i8* iv) {
	if(nullptr == source || nullptr == key || nullptr == iv) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring cipher;
	byte _key[CryptoPP::ChaCha::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	byte _iv[CryptoPP::ChaCha::IV_LENGTH] = {0};
	memcpy_s(_iv, _countof(_iv), iv,
		(std::min)(_countof(_iv), strlen(iv)));

	CryptoPP::ChaCha::Encryption coder;
	coder.SetKeyWithIV(_key, _countof(_key), _iv, _countof(_iv));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(cipher)));
	return cipher;
	CCATCH

	return __emptyString;
}

ustring chaCha20Decrypt(const ustring& source,
	const i8* key, const i8* iv) {
	return chaCha20Decrypt(source.c_str(), source.size(), key, iv);
}

ustring chaCha20Decrypt(const i8* source, u32 bytes,
	const i8* key, const i8* iv) {
	if(nullptr == source || nullptr == key || nullptr == iv) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	ustring plain;
	byte _key[CryptoPP::ChaCha::DEFAULT_KEYLENGTH] = {0};
	memcpy_s(_key, _countof(_key), key,
		(std::min)(_countof(_key), strlen(key)));

	byte _iv[CryptoPP::ChaCha::IV_LENGTH] = {0};
	memcpy_s(_iv, _countof(_iv), iv,
		(std::min)(_countof(_iv), strlen(iv)));

	CryptoPP::ChaCha::Decryption coder;
	coder.SetKeyWithIV(_key, _countof(_key), _iv, _countof(_iv));
	CryptoPP::StringSource((const byte*)source, bytes, true,
		new CryptoPP::StreamTransformationFilter(coder,
			new CryptoPP::StringSink(plain)));
	return plain;
	CCATCH

	return __emptyString;
}

ustring chaCha20EncryptPoly1305(const ustring& source,
	const i8* key, const i8* iv, ustring& mac, const i8* aad /*= ""*/) {
	if(nullptr == key || nullptr == iv || nullptr == aad) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	mac.clear();

	CryptoPP::ChaCha20Poly1305::Encryption enc;

	auto eKeyByte = enc.DefaultKeyLength();
	auto* _key = new byte[eKeyByte];
	memset(_key, 0, eKeyByte);
	memcpy_s(_key, eKeyByte, key, (std::min)(eKeyByte, strlen(key)));
	auto eIvByte = enc.DefaultIVLength();
	auto* _iv = new byte[eIvByte];
	memset(_iv, 0, eIvByte);
	memcpy_s(_iv, eIvByte, iv, (std::min)(eIvByte, strlen(iv)));

	CTRY
	ustring cipher;
	enc.SetKeyWithIV(_key, eKeyByte, _iv, eIvByte);
	cipher.resize(source.size());
	mac.resize(enc.DigestSize());
	enc.EncryptAndAuthenticate((byte*)(cipher.c_str()), 
		(byte*)mac.c_str(), mac.size(), 
		_iv, eIvByte, (const byte*)aad, strlen(aad), 
		(const byte*)source.c_str(), source.size());
	ws::Safedelete::checkedDelete(_key);
	ws::Safedelete::checkedDelete(_iv);
	return cipher;
	CCATCH

	ws::Safedelete::checkedDelete(_key);
	ws::Safedelete::checkedDelete(_iv);
	return __emptyString;
}

ustring chaCha20DecryptPoly1305(const ustring& source, const i8* key,
	const i8* iv, const ustring& mac, const i8* aad /* = "" */) {
	if(nullptr == key || nullptr == iv || nullptr == aad) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CryptoPP::ChaCha20Poly1305::Encryption enc;
	auto eKeyByte = enc.DefaultKeyLength();
	auto* _key = new byte[eKeyByte];
	memset(_key, 0, eKeyByte);
	memcpy_s(_key, eKeyByte, key, (std::min)(eKeyByte, strlen(key)));
	auto eIvByte = enc.DefaultIVLength();
	auto* _iv = new byte[eIvByte];
	memset(_iv, 0, eIvByte);
	memcpy_s(_iv, eIvByte, iv, (std::min)(eIvByte, strlen(iv)));
	enc.SetKeyWithIV(_key, eKeyByte, _iv, eIvByte);

	CTRY
	ustring plain;
	plain.resize(source.size());
	enc.DecryptAndVerify((byte*)(plain.c_str()),
		(const byte*)mac.c_str(), mac.size(),
		_iv, eIvByte, (byte*)aad, strlen(aad),
		(const byte*)source.c_str(), source.size());
	ws::Safedelete::checkedDelete(_key);
	ws::Safedelete::checkedDelete(_iv);
	return plain;
	CCATCH

	ws::Safedelete::checkedDelete(_key);
	ws::Safedelete::checkedDelete(_iv);
	return __emptyString;
}

bool generatePubPriKey(std::pair<ws::ustring, ws::ustring>& keys) {
	CTRY
	CryptoPP::AutoSeededRandomPool rnd;
	CryptoPP::InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rnd, 1024);

	CryptoPP::RSA::PublicKey rsaPublic(params);
	CryptoPP::RSA::PrivateKey rsaPrivate(params);

	CryptoPP::StringSink first(keys.first);
	rsaPublic.Save(first);
	CryptoPP::StringSink second(keys.second);
	rsaPrivate.Save(second);
	return true;
	CCATCH

	return false;
}

ws::ustring rsaEncrypt(const ustring& source, const ustring& publickey) {
	return rsaEncrypt(source.c_str(), source.size(), publickey);
}

ws::ustring rsaEncrypt(const i8* source, u32 bytes, const ustring& publickey) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	CryptoPP::RSA::PublicKey pubkey;
	pubkey.Load(CryptoPP::StringSource(publickey, true));

	CryptoPP::AutoSeededRandomPool rng;
	CryptoPP::RSAES_OAEP_SHA_Encryptor encode(pubkey);
	
	ws::ustring cipher;
	u32 chunkByte = encode.FixedMaxPlaintextLength();
	for(u32 i = 0; i < bytes; i += chunkByte) {
		u32 len = (std::min)(chunkByte,  (bytes - i));
		CryptoPP::ArraySource ss1((byte*)(source + i), len, true, 
			new CryptoPP::PK_EncryptorFilter(rng, encode,
				new CryptoPP::StringSink(cipher)));
	}

	return cipher;
	CCATCH

	return __emptyString;
}

ws::ustring rsaDecrypt(const ustring& source, const ustring& privatekey) {
	return rsaDecrypt(source.c_str(), source.size(), privatekey);
}

ws::ustring rsaDecrypt(const i8* source, u32 bytes, const ustring& privatekey) {
	if(nullptr == source) {
		assert(false && "Invalid arguments!!!");
		return __emptyString;
	}

	CTRY
	CryptoPP::RSA::PrivateKey prikey;
	prikey.Load(CryptoPP::StringSource(privatekey, true));

	CryptoPP::AutoSeededRandomPool rng;
	CryptoPP::RSAES_OAEP_SHA_Decryptor decode(prikey);

	ws::ustring plain;
	u32 chunkByte = decode.FixedCiphertextLength();
	for(u32 i = 0; i < bytes; i += chunkByte) {
		u32 len = (std::min)(chunkByte, (bytes - i));
		CryptoPP::ArraySource ss1((byte*)(source + i), len, true,
			new CryptoPP::PK_DecryptorFilter(rng, decode,
				new CryptoPP::StringSink(plain)));
	}

	return plain;
	CCATCH

	return __emptyString;
}

}

}