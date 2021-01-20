#ifndef _LIBCOMMON_ASYNCFILESINK_2020_02_27_13_19_10_H
#define _LIBCOMMON_ASYNCFILESINK_2020_02_27_13_19_10_H

#include "RollerFileSink.h"

namespace ws {

class AsyncFileSink : public RollerFileSink {
public:
	AsyncFileSink(const ustring& filename, bool encrypt);
	~AsyncFileSink();

protected:
	void write(LogDataPtr data) override;

protected:
	void postWrite(LogDataPtr data);

private:
	ThreadPool _threadpool;
};

}

#endif //_LIBCOMMON_ASYNCFILESINK_2020_02_27_13_19_10_H