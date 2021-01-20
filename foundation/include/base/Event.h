#ifndef _LIBCOMMON_EVENT_2020_01_15_12_04_41_H
#define _LIBCOMMON_EVENT_2020_01_15_12_04_41_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>

namespace ws {

class Event {
	kDisableCopyAssign(Event);

public:
	explicit Event(bool autoReset = true);
	~Event();

public:
	operator HANDLE() const { return _event; }

public:
	bool set();
	bool reset();
	void wait();
	bool tryWait(u32 milliseconds);

private:
	HANDLE _event;
};

}

#endif //_LIBCOMMON_EVENT_2020_01_15_12_04_41_H