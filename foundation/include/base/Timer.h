#ifndef _LIBCOMMON_TIMER_2020_01_15_12_07_42_H
#define _LIBCOMMON_TIMER_2020_01_15_12_07_42_H

#include <helper/Types.h>
#include <helper/Heap.h>
#include <helper/FastMutex.h>
#include <helper/Uncopyable.h>

#include "Event.h"
#include "Thread.h"
#include "Clock.h"

namespace ws {

typedef std::function<void(u32 timerid)> functorTimer;

class Timer {
	kDisableCopyAssign(Timer);

	struct Task;
	typedef std::shared_ptr<Task> TaskPtr;

public:
	Timer();
	~Timer();

public:
	void start();
	void stop();
	void clear();
	bool exist(u32 tid);
	void erase(u32 tid);
	void wakeup(u32 tid);
	void suspend(u32 tid);
	void add(u32 tid, u32 intervalMilliSecond, const functorTimer& fn);
	
private:
	void run();
	TaskPtr nextTask();
	u64 nextTaskSleepTime();

private:
	Event _evt;
	Clock _clock;
	Thread _thread;
	FastMutex _fmData;
	std::atomic<bool> _stoped;
	Heap<TaskPtr, std::greater<TaskPtr>> _datas;
};

}

#endif //_LIBCOMMON_TIMER_2020_01_15_12_07_42_H