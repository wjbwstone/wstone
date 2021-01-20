#include "AsyncFileSink.h"

namespace ws {

AsyncFileSink::AsyncFileSink(const ustring& filename, bool encrypt) :
	RollerFileSink(filename, encrypt) {
	_threadpool.start(1);
}

AsyncFileSink::~AsyncFileSink() {
	_threadpool.stop();
}

void AsyncFileSink::write(LogDataPtr data) {
	_threadpool.post(std::bind(&AsyncFileSink::postWrite, this, data));
}

void AsyncFileSink::postWrite(LogDataPtr data) {
	__super::write(data);
}

}