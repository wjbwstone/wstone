
#include <foundation/include/base/Time.h>

namespace ws {

Time::Time() : _unixTime(0) {

}

Time::Time(i64 unixTime) : _unixTime(unixTime) {

}

Time::~Time() {
	reset(0);
}

i64 Time::seconds() const {
	return Time::now();
}

i64 Time::milliSeconds() const {
	return Time::millisecond();
}

i64 Time::microSeconds() const {
	return Time::microsecond();
}

void Time::reset(i64 unixTime /* = _time64(nullptr) */) {
	_unixTime = unixTime;
}

i64 Time::now() {
	return ::_time64(nullptr);
}

i64 Time::millisecond() {
	return (microsecond() / 1000);
}

i64 Time::microsecond() {
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);

	/*
    * A file time is a 64-bit value that represents the number
    * of 100-nanosecond intervals that have elapsed since
    * January 1, 1601 12:00 A.M. UTC.
    *
    * Between January 1, 1970 (Epoch) and January 1, 1601 there were
    * 134774 days,
    * 11644473600 seconds or
    * 11644473600,000,000,0 100-nanosecond intervals.
    */
    i64 intervals = ((i64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    intervals -= 116444736000000000;

	/*timeval ret;
    ret.tv_sec = (long) (intervals / 10000000);
    ret.tv_usec = (long) ((intervals % 10000000) / 10);*/
	return intervals / 10;
}

i32 Time::localTimeBias() {
	TIME_ZONE_INFORMATION t;
	GetTimeZoneInformation(&t);
	return t.Bias * 60;
}

i64 Time::toUnixTime(const std::tm& tms) {
	return ::_mktime64(const_cast<std::tm*>(&tms));
}

std::tm Time::toTmLocalTime(i64 val) {
	assert(0 != val);
	std::tm tms;
	auto v = _localtime64_s(&tms, &val);
	assert(0 == v);
	return tms;
}

i64 Time::toUnixTime(const FILETIME& ft) {
	SYSTEMTIME stime;
	FileTimeToSystemTime(&ft, &stime);
	return toUnixTime(stime);
}

i64 Time::toUnixTime(const SYSTEMTIME& st) {
	std::tm gm = {st.wSecond, st.wMinute, st.wHour, st.wDay, 
		st.wMonth - 1, st.wYear - 1900, st.wDayOfWeek, 0, 0};
	return ::_mktime64(&gm);
}

SYSTEMTIME Time::toSystemLocalTime(i64 val) {
	auto t = toTmLocalTime(val);
	SYSTEMTIME st;
	st.wYear = 1900 + (WORD)t.tm_year;
	st.wMonth = 1 + (WORD)t.tm_mon;
	st.wDayOfWeek = (WORD)t.tm_wday;
	st.wDay = (WORD)t.tm_mday;
	st.wHour = (WORD)t.tm_hour;
	st.wMinute = (WORD)t.tm_min;
	st.wSecond = (WORD)t.tm_sec; 
	st.wMilliseconds = 0;
	return st;
}

i64 Time::toUnixTime(const ustring& strtime) {
	assert(!strtime.empty());

	std::tm tms = {0};
	i32 y = 0;
	i32 m = 0;
	i32 d = 0;
	i32 h = 0;
	i32 mi = 0;
	i32 s = 0;

	sscanf_s(strtime.c_str(), "%d-%d-%d %d:%d:%d", &y, &m, &d, &h, &mi, &s);
	assert(y >= 1900);
	assert(m >= 1);

	tms.tm_year = y - 1900;
	tms.tm_mon = m - 1;
	tms.tm_mday = d;
	tms.tm_hour = h;
	tms.tm_min = mi;
	tms.tm_sec = s;
	tms.tm_isdst = 0;

	return static_cast<i64>(_mktime64(&tms));
}

ustring Time::toDatetimeString(i64 val) {
	std::tm tms = toTmLocalTime(val);
	char s[32] = {0};
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tms);
	return s;
}

ustring Time::toDateString(i64 val) {
	std::tm tms = toTmLocalTime(val);
	char s[32] = {0};
	strftime(s, sizeof(s), "%Y-%m-%d", &tms);
	return s;
}

ustring Time::toTimeString(i64 val) {
	auto tms = toTmLocalTime(val);
	char s[32] = {0};
	strftime(s, sizeof(s), "%H:%M:%S", &tms);
	return s;
}

u32 Time::toDayPastTime(i64 val) {
	auto tms = toTmLocalTime(val);
	return((tms.tm_hour * 3600) + (tms.tm_min * 60) + tms.tm_sec);
}

i64 Time::toDayStartTime(i64 val) {
	return i64(val - toDayPastTime(val));
}

u64 Time::toWeekStartTime(i64 val) {
	auto tms = toTmLocalTime(val);
	auto wday = tms.tm_wday;
	if(0 == wday) {
		wday = 7;
	}

	return val - ((wday - 1) * 3600 * 24 + toDayPastTime(val));
}

u32 Time::toWeek(i64 val) {
	auto tms = toTmLocalTime(val);
	return (0 != tms.tm_wday) ? tms.tm_wday : 7;
}

i64 Time::toMinutes(i64 val) {
	return (val / 60);
}

u32 Time::toDays(i64 val) {
	auto tms = toTmLocalTime(val);
	u32 year = tms.tm_year + 1900;
	u32 ryear = year / 4 - year / 100 + year / 400;
	return (year * 365 + tms.tm_yday + ryear + 1);
}

u64 Time::toMonthStartTime(i64 val) {
	auto tms = toTmLocalTime(val);
	return val - ((tms.tm_mday - 1) * 3600 * 24 + toDayPastTime(val));
}

} //ws