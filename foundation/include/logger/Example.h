#pragma once

/*******************************************************************
*@brief  : 该头文件请拷贝至工作项目中，注意修改日志名 Example
********************************************************************/

#include <helper/Singleton.h>
#include <foundation/include/logger/Logger.h>

class Log : public ws::Singleton<Log> {
public:
	Log() {
		_logger.reset(new ws::Logger("Example"));
	}

	~Log() {
		_logger = nullptr;
	}

public:
	ws::LoggerPtr _logger;
};

#define MLOG(l, p, ...) \
	ws::writeLogger(Log::getInstance()->_logger, l, \
		__FILE__, __LINE__, p, ##__VA_ARGS__)

#ifdef _DEBUG
#define MLOGD(p, ...) MLOG(ws::kLoggerLevelDebug, p, ##__VA_ARGS__)
#else
#define MLOGD(p, ...) ((void)0)
#endif

#define MLOGT(p, ...) MLOG(ws::kLoggerLevelTrace, p, ##__VA_ARGS__)
#define MLOGI(p, ...) MLOG(ws::kLoggerLevelInfo, p, ##__VA_ARGS__)
#define MLOGW(p, ...) MLOG(ws::kLoggerLevelWarn, p, ##__VA_ARGS__)
#define MLOGE(p, ...) MLOG(ws::kLoggerLevelError, p, ##__VA_ARGS__)