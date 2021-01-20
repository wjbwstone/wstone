#ifndef _HELPER_PROFILE_2020_04_09_16_38_40_H
#define _HELPER_PROFILE_2020_04_09_16_38_40_H

#include "Types.h"
#include "SimpleLogSink.h"

namespace ws {

class Profile {
	kDisableCopyAssign(Profile);

public:
	explicit Profile(const i8* func, ws::SimpleLogSink* logger = nullptr) :
		_func(func),
		_btime(millisecond()),
		_simplelogger(logger) {
		SMLOGT("开始执行函数 : {}->{}", _func, _btime);
	}

	~Profile() {
		auto e = millisecond();
		SMLOGT("结束执行函数 : {}->{}({}毫秒)", _func, e, e - _btime);
	}

private:
	inline i64 millisecond() {
		return (microsecond() / 1000);
	}

	inline i64 microsecond() {
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		auto v = ((ws::i64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
		v -= 116444736000000000;
		return v / 10;
	}

private:
	ws::u64 _btime;
	ws::ustring _func;
	ws::SimpleLogSink* _simplelogger;
};

}

#define kProfileLogger(loggerptr) \
	ws::Profile _dummyProfile(__FUNCTION__, loggerptr)

#define kProfile() \
	ws::SimpleLogSink __dummyLoggerPtr; \
	ws::Profile _dummyProfile(__FUNCTION__, &__dummyLoggerPtr)

#endif //_HELPER_PROFILE_2020_04_09_16_38_40_H