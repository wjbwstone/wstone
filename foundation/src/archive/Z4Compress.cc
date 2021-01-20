#include <foundation/include/archive/Z4Compress.h>

#include <helper/StringConvert.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/File.h>
#include <foundation/include/base/System.h>
#include <foundation/include/base/String.h>

#include <foundation/src/archive/lz4/lz4frame_static.h>
#include <foundation/src/archive/lz4/lz4hc.h>
#include <foundation/src/archive/lz4/lz4io.h>

namespace ws {

namespace lz4 {

bool compress(const i8* input, u32 bytes, ustring& output) {
	output.clear();

	if(nullptr == input) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	if(0 == bytes) {
		return true;
	}

	auto maxBytes = LZ4_compressBound(bytes);
	output.clear();
	output.resize(maxBytes);
	auto v = LZ4_compress_default(input, (i8*)output.c_str(), bytes, maxBytes);
	output.resize(v);

	return (0 != v);
}

bool compress(const ustring& input, ustring& output) {
	return compress(input.c_str(), input.size(), output);
}

bool uncompress(const i8* input, u32 bytes, ustring& output) {
	output.clear();

	if(nullptr == input) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	if(0 == bytes) {
		return true;
	}

	auto maxBytes = bytes + 1;
	output.clear();
	output.resize(maxBytes);

	auto v = LZ4_decompress_safe(input, (i8*)output.c_str(), bytes, maxBytes);
	output.resize(v);
	return (0 != v);
}

bool uncompress(const ustring& input, ustring& output) {
	return uncompress(input.c_str(), input.size(), output);
}

bool compressFile(const ustring& path, const ustring& outpath) {
	try {
		auto apath = Utf8ToAnsi(path);
		auto aoutpath = Utf8ToAnsi(outpath);
		auto prefs = LZ4IO_defaultPreferences();
		LZ4IO_setBlockSizeID(prefs, 7);
		auto result = LZ4IO_compressFilename(
			prefs, apath.c_str(), aoutpath.c_str(), 1);
		LZ4IO_freePreferences(prefs);
		return true;
	} catch(...) {

	}

	return false;
}

bool uncompressFile(const ustring& path, const ustring& outpath) {
	try {
		auto apath = Utf8ToAnsi(path);
		auto aoutpath = Utf8ToAnsi(outpath);
		auto prefs = LZ4IO_defaultPreferences();
		LZ4IO_setBlockSizeID(prefs, 7);
		auto result = LZ4IO_decompressFilename(
			prefs, apath.c_str(), aoutpath.c_str());
		LZ4IO_freePreferences(prefs);
		return true;
	} catch(...) {

	}

	return false;
}

}

}