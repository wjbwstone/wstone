#include "ConsoleSink.h"

namespace ws {

ConsoleSink::ConsoleSink() {

}

ConsoleSink::~ConsoleSink() {

}

void ConsoleSink::flush() {

}

void ConsoleSink::write(LogDataPtr data) {
	FastMutex::ScopedLock lock(_fmConsole);

	Console::Color color = Console::kNone;
	if(kLoggerLevelError == data->level()) {
		color = Console::kRed;
	} else if(kLoggerLevelWarn == data->level()) {
		color = Console::kBlue;
	} else if(kLoggerLevelTrace == data->level()) {
		color = Console::kGreen;
	}

	Console::write(data->data(), data->size(), color, false);
}

}