#include "LogData.h"

namespace ws {

LogData::LogData(LoggerLevel level, const i8* file, u32 line,
	const i8* pattern, const fmt::format_args& args) : _level(level) {
	assert(nullptr != pattern && nullptr != file);

	SYSTEMTIME st;
	GetLocalTime(&st);
	fmt::format_to(_buffer,
		"[{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0>3}] ",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
		st.wSecond, st.wMilliseconds);

	const i8* vlevel = "[trace] ";
	switch(level) {
		case kLoggerLevelTrace: vlevel = "[trace] "; break;
		case kLoggerLevelDebug: vlevel = "[debug] "; break;
		case kLoggerLevelInfo:  vlevel = "[info] "; break;
		case kLoggerLevelWarn:  vlevel = "[warn] "; break;
		case kLoggerLevelError: vlevel = "[error] "; break;
		default: assert(false && "incorrect level format"); break;
	}

	fmt::format_to(_buffer, vlevel);
	fmt::format_to(_buffer, "[{}:{}] ", fileName(file), line);
	fmt::format_to(_buffer, "[{}] ", ::GetCurrentThreadId());
	fmt::vformat_to(_buffer, pattern, args);
	fmt::format_to(_buffer, "\r\n");
}

LogData::~LogData() {
	
}

const i8* LogData::fileName(const i8* file) const {
	if(nullptr != file) {
		auto bytes = strlen(file);
		while(--bytes) {
			if('\\' == file[bytes]) {
				return file + bytes + 1;
			}
		}

		return file;
	}

	return ws::__emptyString.c_str();
}

}