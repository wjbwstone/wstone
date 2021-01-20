#include <foundation/include/base/Condition.h>

namespace ws {

struct ConditionScopedLock {
	explicit ConditionScopedLock(FastMutex& mutex) : _mutex(mutex) { }
	~ConditionScopedLock() { _mutex.lock(); }
	FastMutex& _mutex;
};

Condition::Condition() {

}

Condition::~Condition() {
	wakeAll();
}

void Condition::wait(FastMutex& mutex) {
	ConditionScopedLock unlock(mutex);
	Event evt; {
		FastMutex::ScopedLock lock(_fmQueue);
		mutex.unlock();
		_waitQueue.push_back(&evt);
	} evt.wait();
}

bool Condition::tryWait(FastMutex& mutex, u32 milliseconds) {
	ConditionScopedLock unlock(mutex);
	Event evt; {
		FastMutex::ScopedLock lock(_fmQueue);
		mutex.unlock();
		_waitQueue.push_back(&evt);
	}

	if(!evt.tryWait(milliseconds)) {
		FastMutex::ScopedLock lock(_fmQueue);
		for(auto it = _waitQueue.begin(); it != _waitQueue.end(); ++it) {
			if(*it == &evt) {
				_waitQueue.erase(it);
				break;
			}
		}

		return false;
	}

	return true;
}

void Condition::wake() {
	FastMutex::ScopedLock lock(_fmQueue);
	if(!_waitQueue.empty()) {
		_waitQueue.front()->set();
		_waitQueue.pop_front();
	}
}

void Condition::wakeAll() {
	FastMutex::ScopedLock lock(_fmQueue);
	for(auto& it : _waitQueue) {
		it->set();
	}
	_waitQueue.clear();
}

}