#ifndef _LIBCOMMON_THREADPOOL_2020_01_15_12_07_28_H
#define _LIBCOMMON_THREADPOOL_2020_01_15_12_07_28_H

#include <helper/Types.h>
#include <helper/FastMutex.h>
#include <helper/Uncopyable.h>

#include "Thread.h"
#include "Condition.h"

namespace ws {

class ThreadPool {
	typedef std::function<void()> Job;

	kDisableCopyAssign(ThreadPool);

public:
	explicit ThreadPool(u32 threads = 0);
	~ThreadPool();

public:
	u32 size();
	void stop();
	void clear();
	void wakeup();
	void suspend();
	void start(u32 threads);
	void post(const Job& task);

private:
	void run();
	Job getJob();

private:
	FastMutex _fmJobs;
	FastMutex _fmThreads;
	Condition _condition;
	std::queue<Job> _jobs;
	volatile bool _running;
	std::vector<Thread*> _threads;
};

}

#endif //_LIBCOMMON_THREADPOOL_2020_01_15_12_07_28_H