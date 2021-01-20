#ifndef _LIBCOMMON_ROLLERFILESINK_2020_02_27_13_21_08_H
#define _LIBCOMMON_ROLLERFILESINK_2020_02_27_13_21_08_H

#include "FileSink.h"

namespace ws {

class RollerFileSink : public FileSink {
public:
	RollerFileSink(const std::string& filename, bool encrypt);
	~RollerFileSink();

protected:
	void write(LogDataPtr data) override;

protected:
	std::string constructNextFileName();

private:
	u32 _rollIndex;
	const static u32 s_filemaxsize = 20 * 1024 * 1024;
};

}

#endif //_LIBCOMMON_ROLLERFILESINK_2020_02_27_13_21_08_H