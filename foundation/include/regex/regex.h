#ifndef _LIBREGEX_REGEX_2020_01_15_13_49_59_H
#define _LIBREGEX_REGEX_2020_01_15_13_49_59_H

#include <helper/Types.h>

namespace ws {

namespace regex {

bool match(
	const ustring& pattern,
	const ustring& in,
	bool fullMatch = false);

bool match(
	const ustring& pattern, 
	const ustring& in,
	std::vector<ustring>& subMatchs,
	bool fullMatch = false);

}

}

#ifdef _DEBUG
#pragma comment(lib, "libregexd.lib")
#else
#pragma comment(lib, "libregex.lib")
#endif //_DEBUG

#endif //_LIBREGEX_REGEX_2020_01_15_13_49_59_H