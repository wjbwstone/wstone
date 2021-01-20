#ifndef _LIBCOMMON_LOGGER_2020_02_27_13_12_46_H
#define _LIBCOMMON_LOGGER_2020_02_27_13_12_46_H

#include <helper/Types.h>
#include <helper/Format.h>
#include <helper/Uncopyable.h>

namespace ws {

enum LoggerLevel {
	kLoggerLevelDebug = 1,  //debug模式有效
	kLoggerLevelTrace = 2,
	kLoggerLevelInfo = 4,
	kLoggerLevelWarn = 8,
	kLoggerLevelError = 16,
	kLoggerLevelAll = 32 - 1,
};

enum LoggerSinkType {
	kLoggerSinkTypeDefault = 0,
	kLoggerSinkTypeFile = 1,
	kLoggerSinkTypeConsole = 2,
	kLoggerSinkTypeOutputWindow = 4,
};

class LogSink;

class Logger {
	kDisableCopyAssign(Logger);

public:
	/*******************************************************************
	*@brief  : level默认为kLoggerLevelAll
	********************************************************************/
	explicit Logger(const ustring& name, 
		u32 sinkType = kLoggerSinkTypeDefault, bool releaseEncrypt = true);
	~Logger();

public:
	inline u32 getLevel() const { return _level; }
	inline void addLevel(LoggerLevel level) { _level |= level; }
	inline void deleteLevel(LoggerLevel level) { _level &= ~level; }

public:
	void write(LoggerLevel level, const i8* filename, u32 line, 
		const i8* pattern, const fmt::format_args& args);

private:
	void addSink(LogSink* sink);
	void eraseSink(const LogSink* sink);
	void constructorDefault(const ustring& name, bool encrypt);

private:
	u32 _level;
	std::vector<LogSink*> _sinks;
};
typedef std::shared_ptr<Logger> LoggerPtr;

bool decryptLogFile(const ustring& from);

template<typename... Args>
void writeLogger(LoggerPtr& logger, LoggerLevel level,
	const i8* file, u32 line, const i8* pattern, const Args&... args) {
	bool print = true;
#ifndef _DEBUG
	print = (kLoggerLevelDebug != level);
#endif
	if(print && nullptr != logger && nullptr != pattern) {
		logger->write(level, file, line, pattern,
			fmt::make_format_args(args...));
	}
}

}

//调试选择Zi宏__FILE__为相对路径，ZI为绝对路径
#define WLOG(g, level, p, ...) \
	ws::writeLogger(g, level, __FILE__, __LINE__, p, ##__VA_ARGS__)

#define WLOGD(g, p, ...) WLOG(g, ws::kLoggerLevelDebug, p, ##__VA_ARGS__)
#define WLOGT(g, p, ...) WLOG(g, ws::kLoggerLevelTrace, p, ##__VA_ARGS__)
#define WLOGI(g, p, ...) WLOG(g, ws::kLoggerLevelInfo, p, ##__VA_ARGS__)
#define WLOGW(g, p, ...) WLOG(g, ws::kLoggerLevelWarn, p, ##__VA_ARGS__)
#define WLOGE(g, p, ...) WLOG(g, ws::kLoggerLevelError, p, ##__VA_ARGS__)

#endif //_LIBCOMMON_LOGGER_2020_02_27_13_12_46_H