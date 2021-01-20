#ifndef _LIBCOMMON_THREAD_2020_01_15_12_07_18_H
#define _LIBCOMMON_THREAD_2020_01_15_12_07_18_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>

namespace ws {

typedef std::function<void()> threadCallback;

class Thread {
	kDisableCopyAssign(Thread);

public:
	Thread();
	~Thread();

public:
	void join();
	u32 getID();
	void wakeup();
	void suspend();
	bool joinable();
	void run(threadCallback fn);

public:
	static bool createKey(u32& key);
	static bool destroyKey(u32 key);
	static bool setKeyValue(u32 key, void* value);
	static bool getKeyValue(u32 key, void*& value);

private:
	void reset();
	static u32 __stdcall work(PVOID args);

private:
	u32 _tid;
	HANDLE _hthread;
	threadCallback _fn;
};

}

#endif //_LIBCOMMON_THREAD_2020_01_15_12_07_18_H