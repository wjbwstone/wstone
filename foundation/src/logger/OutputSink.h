#ifndef _LIBCOMMON_OUTPUTSINK_2020_02_27_13_20_43_H
#define _LIBCOMMON_OUTPUTSINK_2020_02_27_13_20_43_H

#include "LogSink.h"

namespace ws {

class OutputSink : public LogSink {
public:
	OutputSink();
	~OutputSink();

public:
	void flush() override;
	void write(LogDataPtr data) override;
};

}

#endif //_LIBCOMMON_OUTPUTSINK_2020_02_27_13_20_43_H