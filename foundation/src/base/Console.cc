#include <foundation/include/base/Console.h>


#include <helper/Format.h>
#include <helper/StringConvert.h>

namespace ws {

namespace Console {

bool setTitle(const i8* title) {
	if(nullptr != title) {
		return TRUE == ::SetConsoleTitleW(Utf8ToUnicode(title).c_str());
	}

	return false;
}

bool setTitle(const ustring& title) {
	return setTitle(title.c_str());
}

void setCRTErrorMode(CRTErrorMode mode) {
	_set_error_mode(mode);
}

bool setCodePage(u32 input /*= 0*/, u32 output /*= 0*/) {
	bool success = true;
	if(0 != input) {
		success = (TRUE == ::SetConsoleCP(input));
	}

	if(success && (0 != output)) {
		success = (TRUE == ::SetConsoleOutputCP(output));
	}

	return success;
}

u32 setOutTextColor(u32 color) {
	auto h = ::GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO buffer;
	::GetConsoleScreenBufferInfo(h, &buffer);
	::SetConsoleTextAttribute(h, color | FOREGROUND_INTENSITY);
	return buffer.wAttributes;
}

void restoreOutTextColor(u32 color) {
	auto handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
	::SetConsoleTextAttribute(handle, color);
}

bool write(const i8* data, Color color, bool newline) {
	if(nullptr != data) {
		return write(data, strlen(data), color, newline);
	}

	return false;
}

bool write(const ustring& data, Color color, bool newline) {
	return write(data.c_str(), data.size(), color, newline);
}

bool write(const i8* data, u32 bytes, Color color, bool newline) {
	if(nullptr == data || 0 == bytes) {
		return false;
	}

	u32 oldColor = kNone;
	if(kNone != color) {
		oldColor = setOutTextColor(color);
	}

	u32 dIndex = 0;
	DWORD writed = 0;
	BOOL success = TRUE;
	auto h = ::GetStdHandle(STD_OUTPUT_HANDLE);
	auto d = ws::StringToWString<CP_UTF8>()(data, bytes);
	while(success) {
		auto minIndex = (std::min)(d.size() - dIndex, (u32)1024);
		if(0 == minIndex) {
			break;
		}

		success = ::WriteConsoleW(h, d.c_str() + dIndex, 
			minIndex, &writed, nullptr);
		dIndex += minIndex;
	}
	
	if(newline && success) {
		success = ::WriteConsoleW(h, L"\r\n", 2, &writed, nullptr);
	}

	if(kNone != oldColor) {
		restoreOutTextColor(oldColor);
	}

	return TRUE == success;
}

bool read(ustring& data, u32 character) {
	data.clear();
	
	BOOL success = TRUE;
	wchar* d = new wchar[character];
	if(0 != character) {
		auto h = ::GetStdHandle(STD_INPUT_HANDLE);
		success = ::ReadConsoleW(h, d, character, (DWORD*)&character, NULL);
	}

	if(success) {
		if(	2 <= character &&
			L'\r' == d[character - 2] &&
			L'\n' == d[character - 1]) {
			character -= 2;
		}

		data.append(ws::WStringToString<CP_UTF8>()(d, character));
	}

	ws::Safedelete::checkedDeleteArray(d);
	return TRUE == success;
}

bool disableSelectedMode() {
	auto h = GetStdHandle(STD_INPUT_HANDLE);
	DWORD o = 0;
	if(::GetConsoleMode(h, &o)) {
		o &= ~ENABLE_INSERT_MODE;
		o &= ~ENABLE_QUICK_EDIT_MODE;
		o |= ENABLE_EXTENDED_FLAGS;
		return TRUE == ::SetConsoleMode(h, o);
	}
	
	return false;
}

}

}