#ifndef _LIBCOMMON_RWMUTEX_2020_04_07_13_57_45_H
#define _LIBCOMMON_RWMUTEX_2020_04_07_13_57_45_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>
#include <helper/FastMutex.h>

namespace ws {

class RWMutex {
	kDisableCopyAssign(RWMutex);

public:
	RWMutex();
	~RWMutex();

public:
	void readLock();
	void readUnlock();
	bool readTrylock(u32 millseconds);

	void writeLock();
	void writeUnlock();
	bool writeTrylock(u32 millseconds);

private:
	u32 _readers;
	HANDLE _writeSemaphore;
	CRITICAL_SECTION _readerLock;
};

}

#endif //_LIBCOMMON_RWMUTEX_2020_04_07_13_57_45_H