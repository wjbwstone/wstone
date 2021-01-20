#ifndef _HELPER_FASTMUTEX_2020_01_15_12_32_01_H
#define _HELPER_FASTMUTEX_2020_01_15_12_32_01_H

#include "Types.h"
#include "Uncopyable.h"

namespace ws {

template <class M>
class ScopedLock {
	kDisableCopyAssign(ScopedLock);

public:
	explicit ScopedLock(M& mutex) : _mutex(mutex) {
		_mutex.lock();
	}

	~ScopedLock() {
		_mutex.unlock();
	}

private:
	M& _mutex;
};

class FastMutex {
	kDisableCopyAssign(FastMutex);

public:
	typedef ScopedLock<FastMutex> ScopedLock;

public:
	FastMutex() {
		InitializeCriticalSectionAndSpinCount(&_cs, 4000);
	}

	~FastMutex() {
		DeleteCriticalSection(&_cs);
	}

public:
	void lock() {
		EnterCriticalSection(&_cs);
	}

	bool tryLock() {
		return TryEnterCriticalSection(&_cs) != 0;
	}

	bool tryLock(i32 milliseconds) {
		const i32 sleepMillis = 20;

		do {
			if(TryEnterCriticalSection(&_cs)) {
				return true;
			}

			Sleep(sleepMillis);
			milliseconds -= sleepMillis;
		} while(0 < milliseconds);

		return false;
	}

	void unlock() {
		LeaveCriticalSection(&_cs);
	}

private:
	CRITICAL_SECTION _cs;
};

}

#endif //_HELPER_FASTMUTEX_2020_01_15_12_32_01_H