#ifndef _LIBCOMMON_JSON_2020_02_27_13_50_06_H
#define _LIBCOMMON_JSON_2020_02_27_13_50_06_H

#include <helper/Types.h>

#include "FastJson.h"

namespace ws {

namespace JsonHelper {

bool parse(
	const ustring& data,
	FastJson& jdata);

bool parseFile(
	const ustring& fpath,
	FastJson& jdata);

bool parseEncryptFile(
	const ustring& fpath,
	FastJson& jdata);

void writeToConsole(
	const FastJson& jdata,
	bool prettyformat = true);

bool writeToFile(
	const ustring& fpath,
	const FastJson& jdata,
	bool prettyformat = true);

bool writeToEncryptFile(
	const ustring& fpath,
	const FastJson& jdata,
	bool prettyformat = true);
}

}

#endif //_LIBCOMMON_JSON_2020_02_27_13_50_06_H