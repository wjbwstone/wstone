#ifndef _LIBCOMMON_SINK_2020_02_27_13_17_44_H
#define _LIBCOMMON_SINK_2020_02_27_13_17_44_H

#include "LogData.h"

namespace ws {

class LogSink {
	kDisableCopyAssign(LogSink);

public:
	LogSink() {}
	virtual ~LogSink() {}

public:
	virtual void flush() = 0;
	virtual void write(LogDataPtr data) = 0;
};

}

#endif //_LIBCOMMON_SINK_2020_02_27_13_17_44_H