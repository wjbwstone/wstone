#ifndef _HELPER_FORMAT_2020_01_15_12_32_23_H
#define _HELPER_FORMAT_2020_01_15_12_32_23_H

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#ifndef FMT_EXCEPTIONS
#define FMT_EXCEPTIONS 0
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Types.h"

namespace ws {
/*******************************************************************
*@brief  : 替换 std::copy
********************************************************************/
template<typename InputIterator, typename OutputIterator>
inline OutputIterator fmtcopy(InputIterator first,
	InputIterator last, OutputIterator result) {
	while(first != last) {
		*result++ = *first++;
	}

	return result;
}

}

#include "fmt/format.h"
#include "fmt/printf.h"
#include "fmt/ranges.h"
#include "fmt/ostream.h"

namespace ws {

template<typename... Args>
inline void vprint(const i8* pattern, const Args&... args) {
	assert(nullptr != pattern);
	if(nullptr != pattern) {
		fmt::memory_buffer data;
		fmt::format_to(data, pattern, args...);
		fmt::format_to(data, "\r\n");
		fwrite((void*)data.data(), 1, data.size(), stdout);
	}
}

inline void nprint(const i8* data, u32 bytes) {
	if(nullptr != data && 0 != bytes) {
		fwrite((void*)data, 1, bytes, stdout);
		fwrite("\r\n", 1, 2, stdout);
	}
}

inline void print(const i8* data) {
	if(nullptr != data) {
		nprint(data, strlen(data));
	}
}

inline void print(const ustring& data) {
	nprint(data.c_str(), data.size());
}

}

#endif //_HELPER_FORMAT_2020_01_15_12_32_23_H