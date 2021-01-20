#include <foundation/include/base/ThreadPool.h>

#include <helper/Format.h>

namespace ws {

ThreadPool::ThreadPool(u32 threads /* = 0 */) : _running(false) {
	start(threads);
}

ThreadPool::~ThreadPool() {
	stop();
}

void ThreadPool::post(const Job& task) {
	FastMutex::ScopedLock lock(_fmJobs);
	if(_running) {
		_jobs.push(task);
		_condition.wake();
	}
}

ThreadPool::Job ThreadPool::getJob() {
	FastMutex::ScopedLock lock(_fmJobs);
	if(_running) {
		if(_jobs.empty()) {
			_condition.wait(_fmJobs);
		}

		if(!_jobs.empty()) {
			Job job(_jobs.front());
			_jobs.pop();
			return job;
		}
	}

	return Job(nullptr);
}

void ThreadPool::stop() {
	{
		FastMutex::ScopedLock lock(_fmJobs);
		_running = false;
		_jobs.swap(std::queue<Job>());
	}

	_condition.wakeAll();

	{
		FastMutex::ScopedLock lock(_fmThreads);
		for(auto& it : _threads) {
			it->join();
			delete it;
		}
		_threads.clear();
	}
}

void ThreadPool::clear() {
	FastMutex::ScopedLock lock(_fmJobs);
	_jobs.swap(std::queue<Job>());
}

u32 ThreadPool::size() {
	FastMutex::ScopedLock lock(_fmJobs);
	return _jobs.size();
}

void ThreadPool::suspend() {
	FastMutex::ScopedLock lock(_fmThreads);
	for(auto it : _threads) {
		it->suspend();
	}
}

void ThreadPool::wakeup() {
	FastMutex::ScopedLock lock(_fmThreads);
	for(auto it : _threads) {
		it->wakeup();
	}
}

void ThreadPool::start(u32 threads) {
	if(0 != threads) {
		bool needSetRunning = false; {
			FastMutex::ScopedLock lock(_fmThreads);
			if(_threads.empty()) {
				needSetRunning = true;
			}
		}

		if(needSetRunning) {
			FastMutex::ScopedLock lock(_fmJobs);
			_running = true;
		}

		if(needSetRunning) {
			FastMutex::ScopedLock lock(_fmThreads);
			for(u32 i = 0; i < threads; ++i) {
				auto t = new ws::Thread();
				t->run(std::bind(&ThreadPool::run, this));
				_threads.push_back(t);
			}
		}
	}
}

void ThreadPool::run() {
	while(true) {
		Job job = getJob();
		if(nullptr != job) {
			job();
		} else {
			return;
		}
	}
}

}