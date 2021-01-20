#ifndef _HELPER_SIMPLELOGSINK_2019_11_02_15_26_32_H
#define _HELPER_SIMPLELOGSINK_2019_11_02_15_26_32_H

#include "Types.h"
#include "Format.h"

namespace ws {

class SimpleLogSink {

#define vprintMethod(method) \
template<typename... Args> \
void vprint##method(const i8 * pattern, const Args&... args) const { \
	vprint(kLevel##method, pattern, args...); \
}

#define printMethod(method) \
void print##method(const i8* data) const { \
	if(nullptr != data) { \
		nprint(kLevel##method, data, strlen(data)); \
	} \
} \
void print##method(const ws::ustring& data) const { \
	nprint(kLevel##method, data.c_str(), data.size()); \
}

public:
	enum Level {
		kLevelDebug = 1, //debug模式有效
		kLevelTrace = 2,
		kLevelInfo = 4,
		kLevelWarn = 8,
		kLevelError = 16
	};

	typedef std::function<void(Level, const i8*, u32 bytes)> functorPrint;

public:
	SimpleLogSink() : 
		_fnPrint(std::bind(
			&SimpleLogSink::defualtPrinter, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3)) {

	}

	virtual ~SimpleLogSink() {

	}

public:
	void setLogPrinter(functorPrint fn) { _fnPrint = fn; }

public:
	vprintMethod(Debug);
	vprintMethod(Trace);
	vprintMethod(Info);
	vprintMethod(Warn);
	vprintMethod(Error);

	printMethod(Debug);
	printMethod(Trace);
	printMethod(Info);
	printMethod(Warn);
	printMethod(Error);

public:
	template<typename... Args>
	void vprint(Level v, const i8* pattern, const Args& ...args) const {
		assert(nullptr != pattern);
		if(nullptr != pattern) {
			fmt::memory_buffer buf;
			fmt::format_to(buf, pattern, args...);
			nprint(v, buf.data(), buf.size());
		}
	}

	void nprint(Level v, const i8* data, u32 bytes) const {
		bool printV = true;
	#ifndef _DEBUG
		printV = (kLevelDebug != v);
	#endif
		if(printV && nullptr != data && 0 != bytes) {
			_fnPrint(v, data, bytes);
		}
	}

	static const i8* baseFile(const i8* file) {
		if(nullptr != file) {
			u32 bytes = strlen(file);
			while(--bytes) {
				if('\\' == file[bytes]) {
					return file + bytes + 1;
				}
			}

			return file;
		}
		
		return ws::__emptyString.c_str();
	}

private:
	void defualtPrinter(Level level, const i8* data, u32 bytes) {
		ws::nprint(data, bytes);
	}

protected:
	functorPrint _fnPrint;
};

template<typename... Args>
void simpleLogWrite(SimpleLogSink* logger, SimpleLogSink::Level level,
	const i8* file, u32 line, const i8* pattern, const Args&... args) {
	assert(nullptr != pattern);

	bool printV = true;
#ifndef _DEBUG
	printV = (ws::SimpleLogSink::kLevelDebug != level);
#endif

	if(printV && nullptr != logger && nullptr != pattern) {
		fmt::memory_buffer buf;
		fmt::format_to(buf, "[{}-{}] ", SimpleLogSink::baseFile(file), line);
		fmt::format_to(buf, pattern, args...);
		logger->nprint(level, buf.data(), buf.size());
	}
}

//调试选择Zi宏__FILE__为相对路径，ZI为绝对路径
#define SMLOG(level, p, ...) \
	ws::simpleLogWrite(_simplelogger, level, __FILE__, __LINE__, p, ##__VA_ARGS__);

#define SMLOGD(p, ...) SMLOG(ws::SimpleLogSink::kLevelDebug, p, ##__VA_ARGS__)
#define SMLOGT(p, ...) SMLOG(ws::SimpleLogSink::kLevelTrace, p, ##__VA_ARGS__)
#define SMLOGI(p, ...) SMLOG(ws::SimpleLogSink::kLevelInfo, p, ##__VA_ARGS__)
#define SMLOGW(p, ...) SMLOG(ws::SimpleLogSink::kLevelWarn, p, ##__VA_ARGS__)
#define SMLOGE(p, ...) SMLOG(ws::SimpleLogSink::kLevelError, p, ##__VA_ARGS__)

}

#endif//_HELPER_SIMPLELOGSINK_2019_11_02_15_26_32_H