#ifndef _LIBCOMMON_CONSOLESINK_2020_02_27_13_19_49_H
#define _LIBCOMMON_CONSOLESINK_2020_02_27_13_19_49_H

#include "LogSink.h"

namespace ws {

class ConsoleSink : public LogSink {
public:
	ConsoleSink();
	~ConsoleSink();

public:
	void flush() override;
	void write(LogDataPtr data) override;

private:
	FastMutex _fmConsole;
};

}

#endif //_LIBCOMMON_CONSOLESINK_2020_02_27_13_19_49_H