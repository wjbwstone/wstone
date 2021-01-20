#ifndef _HELPER_RUNONCE_2020_01_15_12_34_10_H
#define _HELPER_RUNONCE_2020_01_15_12_34_10_H

/*******************************************************************
*@brief  : 使用方法
*@ex :
	static ws::RunOnce::Guard s_guard;
	RunOnce::runOnce(&s_guard, initOnce);
********************************************************************/

namespace ws {

typedef std::function<void()> fnRunOnce;

class RunOnce {
public:
	struct Guard {
		Guard() : _inited(false), _event(nullptr) {

		}

		bool _inited;
		HANDLE _event;
	};
	
public:
	static void runOnce(Guard* guard, fnRunOnce callback) {
		if(guard->_inited) {
			return;
		}

		auto cevent = CreateEventW(nullptr, 1, 0, nullptr);
		auto eevent = InterlockedCompareExchangePointer(
			&guard->_event, cevent, nullptr);
		if(nullptr == eevent) {
			callback();
			SetEvent(cevent);
			guard->_inited = true;
		} else {
			CloseHandle(cevent);
			WaitForSingleObject(eevent, INFINITE);
		}
	}
};

}

#endif //_HELPER_RUNONCE_2020_01_15_12_34_10_H