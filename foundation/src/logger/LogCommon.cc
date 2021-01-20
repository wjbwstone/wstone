#include "LogCommon.h"

namespace ws {

namespace LogCommon {

ustring getLogDir() {
	return ws::System::getAppDirectory().append("logs\\");
}

const char* getLogExt() {
	return "wlog";
}

bool canDeleteFiles() {
	static u32 s_startDay = 0;
	auto today = Time::toDays();
	if(s_startDay != today) {
		s_startDay = today;
		return true;
	}

	return false;
}

void fileDeletePolicy() {
	Directory d(getLogDir());
	while(d.next()) {
		if(d.isFile()) {
			if((d.lastModifyTime() + (7 * 24 * 60 * 60)) <= _time64(nullptr)) {
				File::del(d.fullPath());
			}
		}
	}
};

}

}