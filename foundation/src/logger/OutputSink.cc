#include "OutputSink.h"

namespace ws {

OutputSink::OutputSink() {

}

OutputSink::~OutputSink() {

}

void OutputSink::flush() {

}

void OutputSink::write(LogDataPtr data) {
	auto d = ws::StringToWString<CP_UTF8>()(data->data(), data->size());
	OutputDebugStringW(d.c_str());
}

}