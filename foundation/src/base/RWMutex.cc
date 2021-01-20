#include <foundation/include/base/RWMutex.h>

namespace ws {

RWMutex::RWMutex() : _readers(0), _writeSemaphore(nullptr) {
	_writeSemaphore = CreateSemaphoreW(nullptr, 1, 1, nullptr);
	assert(nullptr != _writeSemaphore);
	InitializeCriticalSection(&_readerLock);
}

RWMutex::~RWMutex() {
	_readers = 0;
	CloseHandle(_writeSemaphore);
	DeleteCriticalSection(&_readerLock);
}

void RWMutex::readLock() {
  EnterCriticalSection(&_readerLock);
  if(1 == ++_readers) {
      WaitForSingleObject(_writeSemaphore, INFINITE);
  }
  LeaveCriticalSection(&_readerLock);
}

void RWMutex::readUnlock() {
	EnterCriticalSection(&_readerLock);
	if(--_readers == 0) {
		ReleaseSemaphore(_writeSemaphore, 1, nullptr);
	}
	LeaveCriticalSection(&_readerLock);
}

bool RWMutex::readTrylock(u32 millseconds) {
    if(!TryEnterCriticalSection(&_readerLock)) {
        return false;
    }
    
    bool success = false;
    if(1 == ++_readers) {
        auto ret = WaitForSingleObject(_writeSemaphore, millseconds);
        if(WAIT_OBJECT_0 == ret) { 
            success = true;
        }
    }

    if(!success) {
        --_readers;
    }

     LeaveCriticalSection(&_readerLock);
     return success;
}

void RWMutex::writeLock() {
    WaitForSingleObject(_writeSemaphore, INFINITE);
}

void RWMutex::writeUnlock() {
	ReleaseSemaphore(_writeSemaphore, 1, nullptr);
}

bool RWMutex::writeTrylock(u32 millseconds) {
	auto ret = WaitForSingleObject(_writeSemaphore, millseconds);
	return (WAIT_OBJECT_0 == ret) ? true : false;
}

}
