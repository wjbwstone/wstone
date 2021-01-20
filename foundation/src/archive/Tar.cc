#include <foundation/include/archive/tar.h>

#include <helper/Format.h>
#include <helper/Uncopyable.h>
#include <helper/StringConvert.h>
#include <foundation/include/base/File.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/String.h>
#include <foundation/include/base/Directory.h>
#include <foundation/include/base/System.h>

namespace ws {

namespace tar {

class TarFile {
public:
	struct Header {
		i8 filename[100];
		i8 filemode[8];
		i8 owerid[8]; //八进制数字
		i8 groupid[8]; //八进制数字
		i8 filesize[12]; //八进制数字
		i8 unixtime[12]; //八进制数字
		i8 checksum[8]; //八进制数字
		i8 filetype;
		i8 linkedfilename[100];
		i8 magic[6];
		i8 version[2];
		i8 ownername[32];
		i8 ownergroup[32];
		i8 devicemajor[8]; //八进制数字
		i8 deviceminor[8]; //八进制数字
		i8 filenameprefix[155];
		i8 padding[12];
	};

public:
	TarFile(const ustring& path) : _path(path) {
		if(_path.isFile()) {
			_file.open(_path);
		}
	}

	bool compress(const ustring& tarfile) {
		if(!_path.isExist()) {
			return false;
		}

		std::vector<ustring> paths;
		if(INVALID_HANDLE_VALUE == (HANDLE)_file) {
			paths.push_back(_path);
			Directory::walkDirectory(_path, paths);
		} else {
			paths.push_back(_path);
		}

		File hTarFile(tarfile, File::kCreateAlways);
		if(!hTarFile) {
			return false;
		}

		Header block;
		ustring longPath;
		bool success = true;
		auto found = _path.parentPath().find_last_of('\\');

		for(u32 i = 0; i < paths.size() && success;) {
			const auto& subPath = paths[i];
			auto fname = subPath.substr(found + 1);
			if(100 <= fname.size() && longPath.empty()) {
				longPath = fname;
			} else {
				if(!longPath.empty()) {
					fname = fname.substr(0, 99);
					longPath.clear();
				}

				++i;
			}

			if(!writeHeader(block, hTarFile, subPath, fname)) {
				success = false;
				break;
			}

			switch(block.filetype) {
				case '5': {	 } break;
				case '0': success = writeFile(hTarFile, subPath); break;
				case 'L': success = writeLongPath(hTarFile, fname); break;
				default: break;
			}
		}

		if(success) {
			success = writeTarFileEnd(hTarFile);
		}

		return success;
	}

	bool uncompress(const ustring& outDirectory) {
		if(!_file) {
			SetLastError(ERROR_FILE_NOT_FOUND);
			return false;
		}

		Header block;
		ustring fpath;
		ustring longpath;
		u64 totalReadBytes = 0;
		Directory::createDirectory(outDirectory);
		do {
			u32 readByets = __chunkBytes;
			if(!_file.read((char*)&block, readByets)) {
				return false;
			}

			if(0 == readByets) {
				break;
			}

			auto newChecksum = calcChecksum(block);
			auto oldChecksum = oct2Numeric(block.checksum, 8);
			if(newChecksum != oldChecksum && (0 != strlen(block.magic))) {
				SetLastError(ERROR_INVALID_FLAGS);
				return false;
			}

			totalReadBytes += readByets;
			if(!longpath.empty()) {
				fpath = outDirectory + "\\" + longpath;
				longpath.clear();
			} else {
				if(!ISUTF8(block.filename)) {
					fpath = outDirectory + "\\" + AnsiToUtf8(block.filename);
				} else {
					fpath = outDirectory + "\\" + (block.filename);
				}
			}

			switch(block.filetype) {
				case '5': {
					if(!Directory::createDirectory(fpath)) {
						return false;
					}
				} break;

				case 'L': {//超长文件路径
					auto fsize = oct2Numeric(block.filesize, 12);
					if(!extracLongPath(longpath, fsize)) {
						return false;
					}
				} break;

				default: {
					auto fsize = oct2Numeric(block.filesize, 12);
					if(!extractFile(fpath, fsize)) {
						File::del(fpath);
						return false;
					}
				} break;
			}
		} while(true);

		return true;
	}

protected:
	bool writeHeader(Header& block, File& hTarFile, const ustring& path,
		const ustring& fname) {
		memset(&block, 0, sizeof(Header));

		if(100 > fname.size()) {
			auto v = fname;
			String::replace(v, "\\", "/");
			memcpy_s(block.filename, 99, v.c_str(), v.size());
		} else {
			memcpy_s(block.filename, 99, "././@LongLink", 13);
		}

		memcpy_s(block.filemode, 7, "0000755", 7);
		memcpy_s(block.owerid, 7, "0000000", 7);
		memcpy_s(block.groupid, 7, "0000000", 7);
		memcpy_s(block.filesize, 11, "00000000000", 11);

		auto octstr = u32ToOct((u32)time(nullptr), 11);
		memcpy_s(block.unixtime, 11, octstr.c_str(), octstr.size());
		memcpy_s(block.magic, 5, "ustar", 5);
		memcpy_s(block.version, 2, "00", 2);

		if(100 <= fname.size()) {
			block.filetype = 'L';
			octstr = u32ToOct(fname.size(), 11);
			memcpy_s(block.filesize, 11, octstr.c_str(), octstr.size());
		} else {
			if(Path::isDirectory(path)) {
				block.filetype = '5';
			} else {
				block.filetype = '0';
				auto fsize = File::size(path);
				octstr = u32ToOct((u32)fsize, 11);
				memcpy_s(block.filesize, 11, octstr.c_str(), octstr.size());
			}
		}

		octstr = u32ToOct(calcChecksum(block), 7);
		memcpy_s(block.checksum, 7, octstr.c_str(), octstr.size());
		return hTarFile.write((i8*)&block, sizeof(Header));
	}

	bool writeLongPath(File& hTarFile, const ustring& fname) {
		if(hTarFile.write(fname)) {
			return paddingFile(hTarFile, fname.size());
		}

		return false;
	}

	bool writeFile(File& hTarFile, const ustring& fpath) {
		File hfile(fpath);
		if(!hfile) {
			return false;
		}

		i8 buffer[4096];
		bool success = true;
		u32 totalWrited = 0;
		do {
			u32 readBytes = 4096;
			if(!hfile.read(buffer, readBytes)) {
				success = false;
				break;
			}

			if(0 != readBytes) {
				totalWrited += readBytes;
				if(!hTarFile.write(buffer, readBytes)) {
					success = false;
					break;
				}
			} else {
				break;
			}
			
		} while(true);

		if(success) {
			success = paddingFile(hTarFile, totalWrited);
		}

		return success;
	}

	bool writeTarFileEnd(File& hfile) {
		i8 f[__chunkBytes] = {0};
		if(hfile.write(f, __chunkBytes)) {
			return hfile.write(f, __chunkBytes);
		}

		return false;
	}

	bool extracLongPath(ustring& path, u32 fsize) {
		path.clear();
		
		auto buffer = (i8*)malloc(fsize);
		if(!_file.read(buffer, fsize)) {
			free(buffer);
			return false;
		}

		path.assign(buffer, fsize);
		if(!ISUTF8(path)) {
			path = AnsiToUtf8(path);
		}

		auto success = seekFile(fsize);
		free(buffer);
		return success;
	}

	bool extractFile(const ustring& path, u32 fsize) {
		File hfile(path, File::kCreateAlways);
		if(!hfile) {
			return false;
		}

		i8 buffer[4096];
		auto success = true;
		u32 totoalWriteBytes = 0;
		do {
			u32 readBytes = (std::min)((u32)4096, fsize - totoalWriteBytes);
			if(!_file.read(buffer, readBytes)) {
				success = false;
				break;
			}

			if(!hfile.write(buffer, readBytes)) {
				success = false;
				break;
			}

			totoalWriteBytes += readBytes;
		} while(totoalWriteBytes < fsize);

		if(success) {
			success = seekFile(fsize);
		}

		return success;
	}

	bool paddingFile(File& hfile, u32 writedBytes) {
		i8 _fixEmpty[__chunkBytes] = {0};
		auto pad = __chunkBytes - (writedBytes % __chunkBytes);
		if(__chunkBytes != pad) {
			return hfile.write(_fixEmpty, pad);
		}

		return true;
	}

	bool seekFile(u32 singleFileBytes) {
		auto fillBytes = __chunkBytes - (singleFileBytes % __chunkBytes);
		if(__chunkBytes != fillBytes) {
			return _file.move(fillBytes, FILE_CURRENT);
		}

		return true;
	}

private:
	static u32 oct2Numeric(const char* oct, u32 size) {
		auto ret = 0;
		while(0 < size--) {
			if(!(' ' == *oct || '\0' == *oct)) {
				ret = ret * 8 + *oct - '0';
			}
			oct++;
		}

		return ret;
	}

	static ustring u32ToOct(u32 value, u32 fixLen = 0) {
		if(0 == value) {
			return ustring(fixLen, '0');
		}

		i8 buffer[65];
		_ltoa_s(value, buffer, 8);
		auto len = strlen(buffer);
		ustring ret = buffer;
		if(fixLen > len) {
			ret.insert(0, fixLen - len, '0');
		}

		return ret;
	}

	static i32 calcChecksum(const Header& header) {
		auto crc = 0;
		const auto& p = (byte*)&header;
		for(auto i = 0; i < 148; ++i) {
			crc += p[i];
		}

		for(int i = 156; i < __chunkBytes; ++i) {
			crc += p[i];
		}

		return (crc + 256);
	}

private:
	File _file;
	Path _path;
	const static u32 __chunkBytes = 512;
};

bool compressFile(const ustring& path, const ustring& tarfile) {
	TarFile v(path);
	return v.compress(tarfile);
}

bool uncompressFile(const ustring& tarfile, const ustring& outDirectory) {
	TarFile v(tarfile);
	return v.uncompress(outDirectory);
}

}

}