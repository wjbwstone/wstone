#ifndef __COMMANDLINE_2020_06_12_16_33_01_H
#define __COMMANDLINE_2020_06_12_16_33_01_H

#include "Types.h"
#include "AuxUtils.h"
#include "StringConvert.h"

/*******************************************************************
*@brief  : 大小写无关的命令判断
********************************************************************/

namespace ws {

class CommandLine {
public:
static std::vector<ustring> parse(i32 argc, 
	const wchar* argv[], i32 startIndex = 1) {
	std::vector<ustring> vcmd;
	for(auto i = startIndex; i < argc; ++i) {
		ustring cmd = UnicodeToUtf8(argv[i]);
		vcmd.push_back(cmd);
	}

	return vcmd;
}

static std::vector<ustring> parse(i32 argc, 
	const i8* argv[], i32 startIndex = 1) {
	std::vector<ustring> vcmd;
	for(auto i = startIndex; i < argc; ++i) {
		ustring cmd = AnsiToUtf8(argv[i]);
		vcmd.push_back(cmd);
	}

	return vcmd;
}

static ustring parse(const std::vector<ustring>& vcmd) {
	ustring cmds;
	for(const auto& cmd : vcmd) {
		cmds.append(1, ' ');
		if(cmd.npos != cmd.find(' ')) {
			cmds.append(1, '\"');
			cmds.append(cmd);
			cmds.append(1, '\"');
		} else {
			cmds.append(cmd);
		}
	}

	return cmds;
}

static ustring quoteString(const ustring& value, bool forceQuote = false) {
	bool needQuote = forceQuote;
	if(!needQuote) {
		if(!value.empty() && ('\"' != value[0])) {
			needQuote = true;
		}
	}
	
	if(needQuote) {
		ustring newValue;
		newValue.append(1, '\"');
		newValue.append(value);
		newValue.append(1, '\"');
		return newValue;
	}

	return value;
}

static bool command(const std::vector<ustring>& vcmd,
	const ustring& cmd, u32 index) {
	if(!(vcmd.size() > (index))) {
		return false;
	}

	return ws::equalString(cmd, vcmd[index]);
}

static bool findCommand(const std::vector<ustring>& vcmd, const ustring& cmd) {
	for(const auto& it : vcmd) {
		if(ws::equalString(cmd, it)) {
			return true;
		}
	}

	return false;
}

};

}

#endif //__COMMANDLINE_2020_06_12_16_33_01_H