#include <foundation/include/json/json.h>

#include <helper/Format.h>
#include <foundation/include/base/RC4.h>
#include <foundation/include/base/File.h>

namespace ws {

namespace JsonHelper {

const i8* _parseMoneys() {
	i8* p = nullptr;
	return "!@#%3456!@WJB";
}

bool parse(
	const ustring& data,
	FastJson& jroot) {
	return jroot.parse(data);
}

bool parseFile(
	const ustring& fpath,
	FastJson& jroot) {
	ustring fdata;
	File::load(fpath, fdata);
	return jroot.parse(fdata);
}

bool parseEncryptFile(
	const ustring& fpath,
	FastJson& jroot) {
	ustring fdata;
	File::load(fpath, fdata);
	RC4::decrypt(fdata, _parseMoneys());
	return jroot.parse(fdata);
}

void writeToConsole(
	const FastJson& jdata,
	bool prettyformat) {
	if(prettyformat) {
		ws::print(jdata.toPrettyString());
	} else {
		ws::print(jdata.toShortString());
	}
}

bool writeToFile(
	const ustring& fpath,
	const FastJson& jdata,
	bool prettyformat /* = true */) {

	ustring sdata;
	if(prettyformat) {
		sdata = jdata.toPrettyString();
	} else {
		sdata = jdata.toShortString();
	}

	return File::save(fpath, sdata, true, true);
}

bool writeToEncryptFile(
	const ustring& fpath,
	const FastJson& jdata,
	bool prettyformat /* = true */) {

	ustring sdata;
	if(prettyformat) {
		sdata = jdata.toPrettyString();
	} else {
		sdata = jdata.toShortString();
	}

	RC4::encrypt(sdata, _parseMoneys());
	return File::save(fpath, sdata, true, true);
}

}

}