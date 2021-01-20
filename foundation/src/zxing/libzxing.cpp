#include <foundation/include/zxing/libzxing.h>

#include <gdiplus.h>

#include <helper/StringConvert.h>
#include <foundation/include/base/Path.h>

#include "wrappers/gdiplus/BarcodeGenerator.h"
#include "wrappers/gdiplus/BarcodeReader.h"
#include "wrappers/gdiplus/GdiplusInit.h"

namespace ws {

namespace libzxing {

static ws::ustring __bartype = "QR_CODE";

static ZXing::GdiplusInit __gdipulusinit;

static bool __getEncoderClsid(const wchar *format, CLSID *pClsid) {
	u32 num = 0;
	u32 size = 0;     
	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0) {
		return false;
	}

	auto codecs = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if(nullptr == codecs) {
		return false; 
	}

	GetImageEncoders(num, size, codecs);
	for(u32 j = 0; j < num; ++j) {
		if(0 == wcscmp(codecs[j].MimeType, format)) {
			*pClsid = codecs[j].Clsid;
			free(codecs);
			return true;   
		}
	}

	free(codecs);
	return false;    
}

bool writeQRCode(const ustring& fpath, const ustring& contents,
	u32 width, u32 height) {
	
	ustring filetype = "png";
	auto ext = Path(fpath).fileExt();
	if(!ext.empty()) {
		filetype = ext;
	}

	ZXing::BarcodeGenerator work(__bartype);
	work.setMargin(2);
	work.setEncoding("UTF8");
	auto ret = work.generate(
		Utf8ToUnicode(contents), width, height);
	if(nullptr == ret) {
		return false;
	}

	CLSID gid;
	wstring type(L"image/");
	type.append(Utf8ToUnicode(filetype));
	__getEncoderClsid(type.c_str(), &gid);
	return Gdiplus::Status::Ok == 
		ret->Save(Utf8ToUnicode(fpath).c_str(), &gid, nullptr);
}

bool readQRCode(const ustring& fpath, ustring& contents) {
	contents.clear();

	ZXing::BarcodeReader work(false, true, __bartype);
	Gdiplus::Bitmap file(Utf8ToUnicode(fpath).c_str());
	if(Gdiplus::Status::Ok != file.GetLastStatus()) {
		return false;
	}

	auto d = work.scan(file);
	if(d.format == __bartype) {
		contents = d.text;
		return true;
	}
	
	return false;
}

}

}