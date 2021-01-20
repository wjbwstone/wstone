
#include <foundation\include\base\Event.h>

namespace ws {

Event::Event(bool autoReset /*= true*/)  {
	_event = ::CreateEventW(nullptr, !autoReset, false, nullptr);
}

Event::~Event() {
	if(nullptr != _event) {
		::CloseHandle(_event);
		_event = nullptr;
	}
}

bool Event::set() {
	return ::SetEvent(_event) ? true : false;
}

bool Event::reset() {
	return ::ResetEvent(_event) ? true : false;
}

void Event::wait() {
	switch(WaitForSingleObject(_event, INFINITE)) {
		case WAIT_OBJECT_0: return;
		default: break;
	}
}

bool Event::tryWait(u32 milliseconds) {
	switch(WaitForSingleObject(_event, milliseconds)) {
		case WAIT_OBJECT_0: return true;
		case WAIT_TIMEOUT: return false;
		default: break;
	}

	return false;
}

}