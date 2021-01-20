#include <foundation/include/logger/Logger.h>

#include "LogData.h"
#include "FileSink.h"
#include "OutputSink.h"
#include "ConsoleSink.h"
#include "AsyncFileSink.h"
#include "RollerFileSink.h"

namespace ws {

Logger::Logger(const ustring& name, 
	u32 sinktype /* = kLoggerSinkTypeDefault */,
	bool releaseEncrypt /*= true*/) {
	_level = kLoggerLevelAll;

	//加密导致原始字符串被加密，文件sink放于队列尾部
	if(kLoggerSinkTypeDefault == sinktype) {
		constructorDefault(name, releaseEncrypt);

	} else { 
		if(sinktype & kLoggerSinkTypeConsole) {
			addSink(new ConsoleSink());
		}

		if(sinktype & kLoggerSinkTypeOutputWindow) {
			addSink(new OutputSink());
		}

		if(sinktype & kLoggerSinkTypeFile) {
			addSink(new AsyncFileSink(name, releaseEncrypt));
		}
	}
}

Logger::~Logger() {
	for(const auto& it : _sinks) {
		it->flush();
		delete it;
	}

	_sinks.clear();
}

void Logger::write(LoggerLevel level, const i8* file, u32 line,
	const i8* pattern, const fmt::format_args& args) {
	
#ifdef _DEBUG
	bool print = true;
#else
	bool print = (level != kLoggerLevelDebug);
#endif

	if(print && (_level & level) && nullptr != pattern) {
		auto data = LogDataPtr(new LogData(level, file, line, pattern, args));
		for(auto it = _sinks.begin(); it != _sinks.end(); ++it) {
			(*it)->write(data);
		}
	}
}

void Logger::constructorDefault(const ustring& name, bool encrypt) {
#ifdef _DEBUG
	addSink(new OutputSink());
	addSink(new ConsoleSink());
	addSink(new AsyncFileSink(name, false));
#else
	addSink(new AsyncFileSink(name, encrypt));
#endif
}

void Logger::addSink(LogSink* sink) {
	for(const auto& it : _sinks) {
		if(it == sink) {
			return;
		}
	}

	_sinks.push_back(sink);
}

void Logger::eraseSink(const LogSink* sink) {
	for(auto it = _sinks.begin(); it != _sinks.end(); ++it) {
		if((*it) == sink) {
			_sinks.erase(it);
			break;
		}
	}
}

bool decryptLogFile(const ustring& from) {
	static ustring s_key = ("wlogbywstone");
	const static u32 s_magic = 0x12348765;

	File hfile(from);
	if(!hfile) {
		return false;
	}

	auto to = from;
	auto found = from.find_last_of('.');
	if(ustring::npos != found) {
		to.erase(found);
	}
	to.append("_decrypt.log");

	File hNewFile(to, File::kCreateAlways);
	if(INVALID_HANDLE_VALUE == (HANDLE)hNewFile) {
		return false;
	}

	u32 fileMagic = 0;
	u32 readMagicBytes = sizeof(fileMagic);
	if(!hfile.read((i8*)&fileMagic, readMagicBytes)) {
		return false;
	}

	if(fileMagic != s_magic) {
		return false;
	}

	while(true) {
		u32 header = 0;
		auto readBytes = sizeof(u32);
		if(!hfile.read((i8*)&header, readBytes)) {
			break;
		}

		if(sizeof(u32) != readBytes) {
			break;
		}

		ws::ustring fdata;
		readBytes = header;
		if(!hfile.read(fdata, readBytes)) {
			break;
		}

		RC4::decrypt(fdata, s_key.c_str());
		if(!hNewFile.write(fdata)) {
			break;
		}
	}

	return true;
}

}