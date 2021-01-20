#ifndef _HELPER_DEADLOCK_2020_01_15_12_31_00_H
#define _HELPER_DEADLOCK_2020_01_15_12_31_00_H

#include "Types.h"
#include "SimpleLogSink.h"

namespace ws {

template<class Guard>
class DeadLock {
	kDisableCopyAssign(DeadLock);

public:
	DeadLock(Guard& guard, const i8* file, u32 line, 
		ws::SimpleLogSink* logger = nullptr) :
		_guard(guard),
		_simplelogger(logger),
		_tid(GetCurrentThreadId()),
		_pos(fmt::format("[{}-{}]", file, line)) {
		SMLOGT("请求上锁：{}->{:p} {}", _tid, (void*)&_guard, _pos);
		_guard.lock();
		SMLOGT("请求上锁成功：{}->{:p} {}", _tid, (void*)&_guard, _pos);
	}

	~DeadLock() {
		SMLOGT("请求解锁：{}->{:p} {}", _tid, (void*)&_guard, _pos);
		_guard.unlock();
		SMLOGT("请求解锁成功：{}->{:p} {}", _tid, (void*)&_guard, _pos);
	}

private:
	ws::u32 _tid;
	Guard& _guard;
	ws::ustring _pos;
	ws::SimpleLogSink* _simplelogger;
};

}

#define kDeadLock(lock, logger) \
	ws::DeadLock _dummyDeadlock(lock, __FILE__, __LINE__, logger)

#endif //_HELPER_DEADLOCK_2020_01_15_12_31_00_H