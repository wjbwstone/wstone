#include <foundation/include/base/Clock.h>

namespace ws {

Clock::Clock() : _startCount(0), _frequency(0) {
	LARGE_INTEGER v;
	if(QueryPerformanceFrequency(&v)) {
		_frequency = v.QuadPart;
	}

	start();
}

void Clock::start() {
	_startCount = update();
}

u64 Clock::elapsedMS() const {
	return elapsed(kMilliSecond);
}

u64 Clock::elapsedUS() const {
	return elapsed(kMicroSecond);
}

u64 Clock::nowMS() const {
	return now(kMilliSecond);
}

u64 Clock::nowUS() const {
	return now(kMicroSecond);
}

u32 Clock::resolution() const {
	return kMicroSecond;
}

u64 Clock::update() const {
	assert(0 != _frequency);
	LARGE_INTEGER v;
	if(QueryPerformanceCounter(&v)) {
		return v.QuadPart;
	}

	assert(false && "Get performance couner failed!!!");
	return 0;
}

u64 Clock::elapsed(u32 accuracy) const {
	assert(0 != _startCount);
	auto v = update() - _startCount;
	return (u64)(v * (double)(1.0 / _frequency) * accuracy);
}

u64 Clock::now(u32 accuracy) const {
	auto v = update();
	return (u64)(v * (double)(1.0 / _frequency) * accuracy);
}

}

