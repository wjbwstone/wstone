#include <foundation/include/base/Thread.h>

namespace ws {

Thread::Thread() {
	reset();
}

Thread::~Thread() {
	if(joinable()) {
		assert(false && "Warnning, the thead is running!!!");
		TerminateThread(_hthread, 0);
		reset();
	}
}

void Thread::run(threadCallback fn) {
	_fn = fn;
	assert(nullptr == _hthread && "Oops!!! Don't do this");
	_hthread = (HANDLE)_beginthreadex(nullptr, 0, work, this, 0, &_tid);
}

bool Thread::joinable() {
	return _hthread != nullptr;
}

void Thread::join() {
	assert(_tid != GetCurrentThreadId() && "Don't join yourself!!!");

	if(joinable()) {
		WaitForSingleObject(_hthread, INFINITE);
		CloseHandle(_hthread);
		reset();
	}
}

ws::u32 Thread::getID() {
	return _tid;
}

void Thread::suspend() {
	if(0 != _tid) {
		SuspendThread(_hthread);
	}
}

void Thread::wakeup() {
	if(0 != _tid) {
		while(0 != ResumeThread(_hthread)) {

		}
	}
}

bool Thread::createKey(u32& key) {
	auto v = ::TlsAlloc();
	if(TLS_OUT_OF_INDEXES != v) {
		key = (ws::u32)v;
		return true;
	}

	return false;
}

bool Thread::destroyKey(u32 key) {
	return (TRUE == ::TlsFree((DWORD)key));
}

bool Thread::setKeyValue(u32 key, void* value) {
	return (TRUE == ::TlsSetValue((DWORD)key, (LPVOID)value));
}

bool Thread::getKeyValue(u32 key, void*& value) {
	value = nullptr;
	auto t = ::TlsGetValue((DWORD)key);
	if(NULL != t) {
		value = t;
		return true;
	}

	return false;
}

void Thread::reset() {
	_tid = 0;
	_fn = nullptr;
	_hthread = nullptr;
}

u32 __stdcall Thread::work(PVOID args) {
	auto impl = reinterpret_cast<Thread*>(args);
	if(nullptr == impl || nullptr == impl->_fn) {
		return 0;
	}

	{
		impl->_fn();
	}

	_endthreadex(0);
	return 0;
}

}