#include <foundation/include/base/Timer.h>

#include <foundation/include/base/System.h>

namespace ws {

struct Timer::Task {
	Timer::Task() :
		_timerid(0),
		_timeOut(0),
		_interval(0),
		_caller(nullptr) {

	}

	friend bool operator<(const Timer::TaskPtr& left,
		const Timer::TaskPtr& right) {
		return left->_timeOut < right->_timeOut;
	}

	friend bool operator>(const Timer::TaskPtr& left,
		const Timer::TaskPtr& right) {
		return left->_timeOut > right->_timeOut;
	}

	u64 _timeOut;
	u32 _timerid;
	u32 _interval;
	functorTimer _caller;
};

Timer::Timer() : _stoped(true) {
	
}

Timer::~Timer() { 

}

void Timer::add(u32 tid, u32 interval, const functorTimer& fn) {
	if(exist(tid)) {
		assert(false && "Don't add the same timer id!!!");
		return;
	}

	if(_stoped) {
		assert(false && "You need call start before add!!!");
		return;
	}

	FastMutex::ScopedLock lock(_fmData);

	auto task = new Task();
	task->_caller = fn;
	task->_timerid = tid;
	task->_interval = interval;
	task->_timeOut = _clock.nowMS() + task->_interval;
	_datas.push(std::shared_ptr<Task>(task));
	
	_evt.set();
}

bool Timer::exist(u32 tid) {
	FastMutex::ScopedLock lock(_fmData);
	for(u32 i = 0; i < _datas.size(); ++i) {
		auto item = _datas.get(i);
		if(item->_timerid == tid) {
			return true;
		}
	}

	return false;
}

void Timer::erase(u32 tid) {
	FastMutex::ScopedLock lock(_fmData);
	for(u32 i = 0; i < _datas.size(); ++i) {
		auto item = _datas.get(i);
		if(item->_timerid == tid) {
			_datas.erase(i);
			_evt.set();
			break;
		}
	}
}

void Timer::suspend(u32 tid) {
	FastMutex::ScopedLock lock(_fmData);
	for(u32 i = 0; i < _datas.size(); ++i) {
		auto item = _datas.get(i);
		if(item->_timerid == tid) {
			item->_timeOut = 0xffffffffffffffff;
			_datas.buildHeap();
			_evt.set();
			break;
		}
	}
}

void Timer::wakeup(u32 tid) {
	FastMutex::ScopedLock lock(_fmData);
	for(u32 i = 0; i < _datas.size(); ++i) { 
		auto item = _datas.get(i);
		if(item->_timerid == tid) {
			item->_timeOut = _clock.nowMS() + item->_interval;
			_datas.buildHeap();
			_evt.set();
			break;
		}
	}
}

void Timer::clear() {
	_fmData.lock();
	_datas.clear();
	_fmData.unlock();
	_evt.set();
}

void Timer::start() {
	assert(!_thread.joinable() && "Oops, you need call stop!!!");
	if(!_thread.joinable()) {
		_stoped = false;
		_thread.run(std::bind(&Timer::run, this));
	}
}

void Timer::stop() {
	_stoped = true;
	clear();
	_thread.join();
}

std::shared_ptr<Timer::Task> Timer::nextTask() {
	FastMutex::ScopedLock lock(_fmData);

	auto task = _datas.top(true);
	if(nullptr != task) {
		task = _datas.top();
		task->_timeOut = _clock.nowMS() + task->_interval;
		_datas.push(task);
		return task;
	}

	return nullptr;
}

u64 Timer::nextTaskSleepTime() {
	FastMutex::ScopedLock lock(_fmData);

	auto task = _datas.top(true);
	if(nullptr != task) {
		if(0xffffffffffffffff == task->_timeOut) {
			return INFINITE;
		}

		auto now = _clock.nowMS();
		if(task->_timeOut >= now) {
			return task->_timeOut - now;
		}

		return 0;
	}

	return INFINITE;
}

void Timer::run() {
	while(!_stoped) {
		auto wait = nextTaskSleepTime();
		if(_evt.tryWait((u32)wait)) {
			continue;
		}

		auto t = nextTask();
		if(nullptr != t) {
			t->_caller(t->_timerid);
		}
	}
}

}