#include <foundation/include/archive/Zip.h>

#include <helper/StringConvert.h>
#include <foundation/include/base/File.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/System.h>
#include <foundation/include/base/String.h>
#include <foundation/include/base/Directory.h>

#include <foundation/src/archive/zlib/zlib.h>
#include <foundation/src/archive/zlib/minizip/zip.h>
#include <foundation/src/archive/zlib/minizip/unzip.h>

namespace ws {

namespace Zip {

static bool __addFile(zipFile zfile, const Path& path, 
	const i8* preDir /*= nullptr*/) {
	ustring fname;
	if(nullptr != preDir) {
		fname.append(preDir);
	}
	fname.append(path.fileFullName());

	zip_fileinfo zinfo = {0};
	auto ret = zipOpenNewFileInZip(zfile, Utf8ToAnsi(fname).c_str(),
		&zinfo, nullptr, 0, nullptr, 0, nullptr,
		Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	if(ZIP_OK != ret) {
		return false;
	}

	File file(path);
	if(!file) {
		zipCloseFileInZip(zfile);
		return false;
	}

	bool success = true;
	i8* buffer = (i8*)malloc(8192);
	do {
		u32 readBytes = 8192;
		if(file.read(buffer, readBytes)) {
			if(0 == readBytes) {
				break;
			}

			if(ZIP_OK != zipWriteInFileInZip(zfile, buffer, readBytes)) {
				success = false;
				break;
			}

		} else {
			success = false;
			break;
		}
	} while(true);
	::free(buffer);

	zipCloseFileInZip(zfile);
	return success;
}

static bool __addDir(zipFile zfile, const Path& path, const ustring& preDir,
	bool isTopDir = false) {
	if(!isTopDir) {
		zip_fileinfo zinfo = {0};
		zinfo.external_fa = FILE_ATTRIBUTE_DIRECTORY;
		zipOpenNewFileInZip(zfile, Utf8ToAnsi(preDir).c_str(),
			&zinfo, nullptr, 0, nullptr, 0, nullptr,
			Z_DEFLATED, Z_DEFAULT_COMPRESSION);
	}

	Directory d(path);
	bool success = d.valid();
	while(success && d.next()) {
		if(d.isDirectory()) {
			ustring nextDir = preDir + d.name() + "\\";
			success = __addDir(zfile, d.fullPath(), nextDir);
		} else {
			success = __addFile(zfile, d.fullPath(), preDir.c_str());
		}
	}
	
	return success;
}

static bool __uncompressFile(const zipFile& zfile, const ustring& path, 
	const ustring& outDir) {
	unz_global_info info;
	if(UNZ_OK != unzGetGlobalInfo(zfile, &info)) {
		return false;
	}

	unz_file_info fileInfo;
	i8 filename[1024] = {0};
	for(uLong i = 0; i < info.number_entry; ++i) {
		if(UNZ_OK != unzGetCurrentFileInfo(
			zfile, &fileInfo, filename,
			sizeof(filename), nullptr, 0, nullptr, 0)) {
			return false;
		}

		ustring ufilename = filename;
		if(!ISUTF8(filename)) {
			ufilename = AnsiToUtf8(ufilename);
		}

		auto filePath = Path(outDir + ufilename);

		if(!(fileInfo.external_fa & FILE_ATTRIBUTE_DIRECTORY)) {
			Directory::createDirectory(Path::parentPath(filePath));
			if(UNZ_OK != unzOpenCurrentFile(zfile)) {
				return false;
			}

			File file(filePath, File::kCreateAlways);
			if(!file) {
				return false;
			}

			i8* fbuffer = (i8*)malloc(8192 * sizeof(i8));
			do {
				auto readBytes = unzReadCurrentFile(zfile, fbuffer, 8192);
				if(0 >= readBytes) {
					free(fbuffer);
					break;
				}

				if(!file.write(fbuffer, readBytes)) {
					free(fbuffer);
					return false;
				}
			} while(true);
			unzCloseCurrentFile(zfile);

		} else {
			Directory::createDirectory(filePath);
		}

		auto code = unzGoToNextFile(zfile);
		if(UNZ_OK != code && UNZ_END_OF_LIST_OF_FILE != code) {
			return false;
		}
	}

	return true;
}

bool compress(const i8* input, ustring& output, bool gzip) {
	if(nullptr != input) {
		return compress(input, strlen(input), output, gzip);
	}

	output.clear();
	SetLastError(ERROR_INVALID_PARAMETER);
	return false;
}

bool compress(const ustring& input, ustring& output, bool gzip) {
	return compress(input.c_str(), input.size(), output, gzip);
}

bool compress(const i8* input, u32 bytes, ustring& output, bool gzip) {
	output.clear();

	if(nullptr == input) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	if(0 == bytes) {
		return true;
	}

	z_stream stream;
	memset(&stream, 0, sizeof(z_stream));

	auto ecode = Z_OK;
	if(gzip) {
		ecode = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
			MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY);
	} else {
		ecode = deflateInit(&stream, Z_DEFAULT_COMPRESSION);
	}
	if(Z_OK != ecode) {
		return false;
	}

	u32 buffersize = 4096;
	stream.avail_in = bytes;
	stream.next_in = (byte*)input;
	auto buffer = (i8*)malloc(buffersize);
	do {
		stream.avail_out = buffersize;
		stream.next_out = (byte*)buffer;
		ecode = deflate(&stream, stream.avail_in ? Z_NO_FLUSH : Z_FINISH);
		output.append(buffer, buffersize - stream.avail_out);
	} while(Z_OK == ecode);

	free(buffer);
	deflateEnd(&stream);
	return (Z_STREAM_END == ecode);
}

bool uncompress(const i8* input, ustring& output, bool gzip) {
	if(nullptr != input) {
		return uncompress(input, strlen(input), output, gzip);
	}

	output.clear();
	SetLastError(ERROR_INVALID_PARAMETER);
	return false;
}

bool uncompress(const ustring& input, ustring& output, bool gzip) {
	return uncompress(input.c_str(), input.size(), output, gzip);
}

bool uncompress(const i8* input, u32 bytes, ustring& output, bool gzip) {
	output.clear();

	if(nullptr == input) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	if(0 == bytes) {
		return true;
	}

	z_stream stream;
	memset(&stream, 0, sizeof(stream));

	auto ecode = Z_OK; 
	if(gzip) {
		ecode = inflateInit2(&stream, MAX_WBITS + 16);
	} else {
		ecode = inflateInit(&stream);
	}
	if(Z_OK != ecode) {
		return false;
	}

	u32 buffersize = 4096;
	i8* buffer = (i8*)malloc(buffersize);
	stream.avail_in = bytes;
	stream.next_in = (byte*)(input);
	do {
		stream.avail_out = buffersize;
		stream.next_out = (byte*)buffer;
		ecode = inflate(&stream, Z_NO_FLUSH);
		output.append(buffer, buffersize - stream.avail_out);
	} while(Z_OK == ecode);

	free(buffer);
	inflateEnd(&stream);
	return (Z_STREAM_END == ecode);
}

bool compressFile(const ustring& existPath, const ustring& outPath) {
	assert(!outPath.empty());

	Path ePath(existPath);
	if(!ePath.isExist()) {
		return false;
	}

	Path oPath(outPath);
	Directory::createDirectory(oPath.parentPath());

	auto zfile = zipOpen(Utf8ToAnsi(outPath).c_str(), APPEND_STATUS_CREATE);
	if(nullptr == zfile) {
		return false;
	}

	bool success = true;
	if(ePath.isDirectory()) {
		ustring preDir;
		success = __addDir((void*)zfile, ePath, preDir, true);
	} else {
		success = __addFile(zfile, ePath, nullptr);
	}

	if(!success) {
		File::del(oPath);
	}

	zipClose(zfile, nullptr);
	return success;
}

bool uncompressFile(const ustring& existFilePath, const ustring& outDirectory) {
	assert(!outDirectory.empty());
	
	Path oPath(outDirectory, true);
	if(!Directory::createDirectory(oPath)) {
		return false;
	}

	auto zfile = unzOpen(Utf8ToAnsi(existFilePath).c_str());
	if(nullptr == zfile) {
		SetLastError(ERROR_FILE_NOT_FOUND);
		return false;
	}

	auto ok = __uncompressFile(zfile, existFilePath, oPath.getPath());
	if(!ok) {
		Directory::deleteDirectory(oPath);
	}

	unzClose(zfile);
	return ok;
}

}

}