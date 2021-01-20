#ifndef _LIBCOMMON_LOGDATA_2020_02_27_13_18_44_H
#define _LIBCOMMON_LOGDATA_2020_02_27_13_18_44_H

#include "LogCommon.h"

namespace ws {

class LogData {
public:
	LogData(LoggerLevel, const i8*, u32,const i8*, const fmt::format_args&);
	~LogData();

public:
	inline u32 size() const { return _buffer.size(); }
	inline LoggerLevel level() const { return _level; }
	inline i8* mutableData() { return _buffer.data(); }
	inline const i8* data() const { return _buffer.data(); }
	inline ustring toString() const { return fmt::to_string(_buffer); }
	
private:
	const i8* fileName(const i8* file) const;

private:
	LoggerLevel _level;
	fmt::memory_buffer _buffer;
};

typedef std::shared_ptr<LogData> LogDataPtr;

}

#endif //_LIBCOMMON_LOGDATA_2020_02_27_13_18_44_H