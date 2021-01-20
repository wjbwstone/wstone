#ifndef _LIBCOMMON_CRYPT_2020_04_26_17_26_52_H
#define _LIBCOMMON_CRYPT_2020_04_26_17_26_52_H

#include <helper/Types.h>

/*******************************************************************
*@brief  : 所有的key都有最大长度，不足补零。超出截断
*1> 注意所有C字符串长度
********************************************************************/

namespace ws {

namespace Crypt {

ustring toHex(const ustring& source, bool toupper = true);
ustring toHex(const i8* source, u32 bytes, bool toupper = true);

ustring toBase64(const ustring& source, bool urlMode = false);
ustring toBase64(const i8* source, u32 bytes, bool urlMode = false);
ustring fromBase64(const ustring& source, bool urlMode = false);
ustring fromBase64(const i8* source, u32 bytes, bool urlMode = false);

ustring md5(const ustring& source, bool toupper = true);
ustring md5(const i8* source, u32 bytes, bool toupper = true);
ustring md5File(const i8* fpath, bool toupper = true);
ustring md5File(const ustring& fpath, bool toupper = true);
bool md5Verify(const ustring& md5Value, const ustring& source);
bool md5Verify(const ustring& md5Value, const i8* source, u32 bytes);
bool md5VerifyFile(const ustring& md5Value, const ustring& fpath);

u32 crc32(const ustring& source);
u32 crc32(const i8* source, u32 bytes);
u32 crc32File(const ustring& fpath);
bool crc32Verify(u32 crc32Value, const i8* source, u32 bytes);
bool crc32Verify(u32 crc32Value, const ustring& source);
bool crc32VerifyFile(u32 crc32Value, const ustring& fpath);

ustring sha1(const ustring& source, bool toupper = true);
ustring sha1(const i8* source, u32 bytes, bool toupper = true);
ustring sha1File(const ustring& fpath, bool toupper = true);
bool sha1Verify(const ustring& sha1Value, const ustring& source);
bool sha1Verify(const ustring& sha1Value, const i8* source, u32 bytes);
bool sha1VerifyFile(const ustring& sha1Value, const ustring& fpath);

ustring sha256(const ustring& source, bool toupper = true);
ustring sha256(const i8* source, u32 bytes, bool toupper = true);
ustring sha256File(const ustring& fpath, bool toupper = true);
bool sha256Verify(const ustring& sha1Value, const ustring& source);
bool sha256Verify(const ustring& sha1Value, const i8* source, u32 bytes);
bool sha256VerifyFile(const ustring& sha1Value, const ustring& fpath);

ustring sha512(const ustring& source, bool toupper = true);
ustring sha512(const i8* source, u32 bytes, bool toupper = true);
ustring sha512File(const ustring& fpath, bool toupper = true);
bool sha512Verify(const ustring& sha1Value, const ustring& source);
bool sha512Verify(const ustring& sha1Value, const i8* source, u32 bytes);
bool sha512VerifyFile(const ustring& sha1Value, const ustring& fpath);

/*******************************************************************
*@brief  : hmac消息认证码key长度[0-INTMAX]字节
********************************************************************/
ustring hmacSha1(const ustring& source, const i8* key, bool toupper = true);
ustring hmacSha1(const i8* source, u32 bytes, const i8* key, bool toupper = true);
ustring hmacSha256(const ustring& source, const i8* key, bool toupper = true);
ustring hmacSha256(const i8* source, u32 bytes, const i8* key, bool toupper = true);
ustring hmacSha512(const ustring& source, const i8* key, bool toupper = true);
ustring hmacSha512(const i8* source, u32 bytes, const i8* key, bool toupper = true);
ustring hmacMD5(const ustring& source, const i8* key, bool toupper = true);
ustring hmacMD5(const i8* source, u32 bytes, const i8* key, bool toupper = true);

/*******************************************************************
*@brief  : key长度最大256字节
********************************************************************/
void rc4Encrypt(ustring& inout, const i8* key);
void rc4Encrypt(i8* inout, u32 bytes, const i8* key);
void rc4Decrypt(ustring& inout, const i8* key);
void rc4Decrypt(i8* inout, u32 bytes, const i8* key);

/*******************************************************************
*@brief  : key长度恒定16字节，128位，数据块填充方式为PKCS5
********************************************************************/
ustring aesEcb128Encrypt(const ustring& source, const i8* key);
ustring aesEcb128Encrypt(const i8* source, u32 bytes, const i8* key);
ustring aesEcb128Decrypt(const ustring& source, const i8* key);
ustring aesEcb128Decrypt(const i8* source, u32 bytes, const i8* key);

/*******************************************************************
*@brief  : key长度恒定32字节，256位，数据块填充方式为PKCS5
********************************************************************/
ustring aesEcb256Encrypt(const ustring& source, const i8* key);
ustring aesEcb256Encrypt(const i8* source, u32 bytes, const i8* key);
ustring aesEcb256Decrypt(const ustring& source, const i8* key);
ustring aesEcb256Decrypt(const i8* source, u32 bytes, const i8* key);

/*******************************************************************
*@brief  : key长度恒定16字节，128位，iv长度恒定16字节，数据块填充方式为PKCS5
********************************************************************/
ustring aesCbc128Encrypt(const ustring& source, const i8* key, const i8* iv);
ustring aesCbc128Encrypt(const i8* source, u32 bytes, const i8* key, const i8* iv);
ustring aesCbc128Decrypt(const ustring& source, const i8* key, const i8* iv);
ustring aesCbc128Decrypt(const i8* source, u32 bytes, const i8* key, const i8* iv);

/*******************************************************************
*@brief  : 带消息完整性校验的加密。
* key长度恒定16字节，128位，iv长度[1-UINTMAX]字节
* aad 数据校验消息完整性
********************************************************************/
ustring aesGcm128Encrypt(const ustring& source, const i8* key, 
	const i8* iv, const i8* aad = "");
ustring aesGcm128Encrypt(const i8* source, u32 bytes, const i8* key, 
	const i8* iv, const i8* aad = "");
ustring aesGcm128Decrypt(const ustring& source, const i8* key,
	const i8* iv, const i8* aad = "");
ustring aesGcm128Decrypt(const i8* source, u32 bytes, const i8* key,
	const i8* iv, const i8* aad = "");

/*******************************************************************
*@brief  : key长度恒定32字节，256位，iv长度恒定8字节
********************************************************************/
ustring chaCha20Encrypt(const ustring& source, const i8* key, const i8* iv);
ustring chaCha20Encrypt(const i8* source, u32 bytes, const i8* key, const i8* iv);
ustring chaCha20Decrypt(const ustring& source, const i8* key, const i8* iv);
ustring chaCha20Decrypt(const i8* source, u32 bytes, const i8* key, const i8* iv);

/*******************************************************************
*@brief  : key恒定32字节，iv恒定12字节
* mac 获取加密的消息认证码
* aad(AEAD)数据用于产生MAC（消息认证码）与校验消息完整性
********************************************************************/
ustring chaCha20EncryptPoly1305(const ustring& source, const i8* key, 
	const i8* iv, ustring& mac, const i8* aad = "");
ustring chaCha20DecryptPoly1305(const ustring& source, const i8* key,
	const i8* iv, const ustring& mac, const i8* aad = "");

/*******************************************************************
*@brief  : 非对称加密,使用OAEP填充模式,密钥对第一个为公钥，第二个为私钥
********************************************************************/
bool generatePubPriKey(std::pair<ws::ustring, ws::ustring>& keys);
ustring rsaEncrypt(const ustring& source, const ustring& publickey);
ustring rsaEncrypt(const i8* source, u32 bytes, const ustring& publickey);
ustring rsaDecrypt(const ustring& source, const ustring& privatekey);
ustring rsaDecrypt(const i8* source, u32 bytes, const ustring& privatekey);

}

}

#ifdef _DEBUG
#pragma comment(lib, "libcryptoppd.lib")
#else
#pragma comment(lib, "libcryptopp.lib")
#endif //_DEBUG

#endif //_LIBCOMMON_CRYPT_2020_04_26_17_26_52_H