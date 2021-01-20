#include "FileSink.h"

namespace ws {

FileSink::FileSink(const ustring& filename, bool encrypt) : 
	_writeBytes(0),
	_encrypt(encrypt),
	_fileName(filename) {
	createLogFileName();
	createLogFile();
}

FileSink::~FileSink() {
	closeLogFile();
}

void FileSink::flush() {
	_file.flush();
}

void FileSink::write(LogDataPtr data) {
	if(LogCommon::canDeleteFiles()) {
		LogCommon::fileDeletePolicy();
	}

	assert(nullptr != data);
	FastMutex::ScopedLock lock(_fmFile);

#ifdef _DEBUG
	if(_file.write(data->data(), data->size())) {
		_writeBytes += data->size();
	}
#else
	if(_encrypt) {
		const static i8* s_key = "wlogbywstone";
		RC4::encrypt(data->mutableData(), data->size(), s_key);
		u32 head = data->size();
		if(_file.write((char*)&head, sizeof(u32))) {
			_writeBytes += sizeof(u32);
		} else {
			return;
		}

		if(_file.write(data->data(), data->size())) {
			_writeBytes += data->size();
		}

	} else {
		if(_file.write(data->data(), data->size())) {
			_writeBytes += data->size();
		}
	}
#endif

}

void FileSink::createLogFileName() {
	auto t = ws::Time::toTmLocalTime();
	auto v = fmt::format(".{:0>4}{:0>2}{:0>2}.{:0>2}{:0>2}{:0>2}.{}.{}",
		t.tm_year + 1900, t.tm_mon + 1, t.tm_mday,
		t.tm_hour, t.tm_min, t.tm_sec,
		::GetCurrentProcessId(), LogCommon::getLogExt());
	_fileName.append(v);

	Directory::createDirectory(LogCommon::getLogDir());
}

void FileSink::closeLogFile() {
	_file.close();
	_writeBytes = 0;
}

void FileSink::createLogFile() {
	_file.open(LogCommon::getLogDir().append(_fileName), File::kOpenAlways);
	
#ifdef _DEBUG

#else
	if(_encrypt) {
		_file.write((const i8*)&s_magic, sizeof(u32));
	}
#endif
}

}