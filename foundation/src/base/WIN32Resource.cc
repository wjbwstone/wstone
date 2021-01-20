#include <foundation/include/base/WIN32Resource.h>

#include <helper/StringConvert.h>

#include <foundation/include/base/Process.h>
#include <foundation/include/base/MemoryFile.h>

namespace ws {

bool Win32Resource::pack(const i8* moduleFile, 
	const i8* type, u32 id, const i8* fromFile) {
	if(nullptr == moduleFile || nullptr == fromFile || nullptr == type) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	ws::MemoryFile file;
	if(false == file.open(fromFile)) {
		return false;
	}

	auto wf = Utf8ToUnicode(moduleFile);
	auto hRes = ::BeginUpdateResourceW(wf.c_str(), FALSE);
	if(nullptr == hRes) {
		return false;
	}

	auto wtype = Utf8ToUnicode(type);
	if(!::UpdateResourceW(hRes, wtype.c_str(),
		MAKEINTRESOURCEW(id),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
		(LPVOID)file.begin(), file.size())) {
		return false;
	}

	return (TRUE == ::EndUpdateResourceW(hRes, FALSE));
}

bool Win32Resource::unpack(const i8* moduleFile, 
	const i8* type, u32 id, const i8* toFile) {
	if(nullptr == moduleFile || nullptr == toFile || nullptr == type) {
		SetLastError(ERROR_INVALID_PARAMETER);
		return false;
	}

	u32 fsize = 0;
	const i8* fdata = nullptr;

	auto wModuleFile = Utf8ToUnicode(moduleFile);
	auto hMou = ::LoadLibraryW(wModuleFile.c_str());
	if(nullptr != hMou) {

		auto w = Utf8ToUnicode(type);
		auto hRes = ::FindResourceW(hMou, MAKEINTRESOURCEW(id), w.c_str());
		if(nullptr != hRes) {
			auto hResData = ::LoadResource(hMou, hRes);
			if(nullptr != hResData) {
				fdata = (const i8*)::LockResource(hResData);
				if(nullptr != fdata) {
					fsize = ::SizeofResource(hMou, hRes);
				}
			}
		}
	}

	bool success = false;
	if(nullptr != fdata) {
		ws::File file(toFile, ws::File::kCreateAlways);
		if(!(success = file.write(fdata, fsize))) {
			ws::File::del(file.getPath());
		}
	}

	if(nullptr != hMou) {
		::FreeLibrary(hMou);
	}

	return success;
}

}