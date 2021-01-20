#ifndef _H_LOG_2018_02_27_11_14_00_
#define _H_LOG_2018_02_27_11_14_00_

class Log : public ws::Singleton<Log> {
public:
	Log() {
		_logger.reset(new ws::Logger("test", 0, false));
	}

	~Log() {
		_logger = nullptr;
	}

public:
	ws::LoggerPtr _logger;
};

#define MLOG(level, p, ...) \
	ws::writeLogger(Log::getInstance()->_logger, level, \
		__FILE__, __LINE__, p, ##__VA_ARGS__)

#define MLOGD(p, ...) MLOG(ws::kLoggerLevelDebug, p, ##__VA_ARGS__)
#define MLOGT(p, ...) MLOG(ws::kLoggerLevelTrace, p, ##__VA_ARGS__)
#define MLOGI(p, ...) MLOG(ws::kLoggerLevelInfo, p, ##__VA_ARGS__)
#define MLOGW(p, ...) MLOG(ws::kLoggerLevelWarn, p, ##__VA_ARGS__)
#define MLOGE(p, ...) MLOG(ws::kLoggerLevelError, p, ##__VA_ARGS__)

#endif//_H_LOG_2018_02_27_11_14_00_