#ifndef _LIBCOMMON_CONDITION_2020_01_15_12_04_30_H
#define _LIBCOMMON_CONDITION_2020_01_15_12_04_30_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>
#include <helper/FastMutex.h>

#include "Event.h"

namespace ws {

class Condition {
	kDisableCopyAssign(Condition);

public:
	Condition();
	~Condition();

public:
	void wake();
	void wakeAll();
	void wait(FastMutex& mutex);
	bool tryWait(FastMutex& mutex, u32 milliseconds);

private:
	FastMutex _fmQueue;
	std::deque<Event*> _waitQueue;
};

}

#endif //_LIBCOMMON_CONDITION_2020_01_15_12_04_30_H