#include <foundation/include/language/Language.h>

#include <helper/StringConvert.h>
#include <foundation/include/base/RC4.h>
#include <foundation/include/base/File.h>
#include <foundation/include/base/Path.h>
#include <foundation/include/base/String.h>
#include <foundation/include/base/System.h>
#include <foundation/include/excel/libxl.h>

namespace ws {

static const i8* __key = "wscommon-!@#1988";

static bool __findKey(ws::Language* impl,
	libxl::ISheetT<ws::wchar>* client, ws::i32& keyIndex,
	ws::i32& enIndex, ws::i32& twIndex, ws::i32& cnIndex) {
	for(auto i = 0; i < client->lastCol(); ++i) {
		auto t = client->readStr(0, i);
		if(nullptr == t) {
			continue;
		}

		ws::wstring value = t;
		if(L"id" == value) {
			keyIndex = i;
		} else if(L"en" == value) {
			enIndex = i;
		} else if(L"cn" == value) {
			cnIndex = i;
		} else if(L"tw" == value) {
			twIndex = i;
		}
	}

	if(-1 == keyIndex) {
		impl->vprintWarn(
			"错误的多语言文本excel表单,无id列数据 ： {}",
			UnicodeToUtf8(client->name()));
		return false;
	}

	if( -1 == enIndex && -1 == cnIndex && -1 == twIndex) {
		impl->vprintWarn(
			"错误的多语言文本excel表单，无en,cn,tw列 ： {}", 
			UnicodeToUtf8(client->name()));
		return false;
	}

	return true;
}

static void __getSheet(ws::Language* impl, libxl::ISheetT<ws::wchar>* client, 
	FastJson& subSheet, ws::i32 keyIndex, ws::i32 valueIndex) {
	for(ws::i32 j = 0; j < client->lastRow(); ++j) {
		auto k = client->readStr(j, keyIndex);
		if(nullptr == k) {
			continue;
		}
		 
		ws::wstring vstr;
		ws::wstring kstr = k;
		auto celltype = client->cellType(j, valueIndex);
		switch(celltype) {
			case libxl::CELLTYPE_STRING: {
					auto v = client->readStr(j, valueIndex);
					if(nullptr != v) {
						vstr = v;
					}
				} break;

			case libxl::CELLTYPE_NUMBER: {
				auto number = client->readNum(j, valueIndex);
				vstr = fmt::format(L"{}", number);
			} break;

			case libxl::CELLTYPE_BOOLEAN: {
				auto number = client->readBool(j, valueIndex);
				vstr = fmt::format(L"{}", number ? L"true" : L"false");
			} break;

			default:
				impl->vprintInfo("暂不支持该数据格式 {}： {}", 
					UnicodeToUtf8(client->name()), 
					celltype);
				break;
		}

		auto key = UnicodeToUtf8(kstr);
		if(subSheet.hasMember(key)) {
			impl->vprintWarn("在 {} 中发现重复的ID : {}", 
				UnicodeToUtf8(client->name()), key);
			continue;
		}

		subSheet[key.c_str()] = UnicodeToUtf8(vstr);
	}
}

Language::Language() : _local("cn") {

}

Language::~Language() {

}

bool Language::makeBin(const ustring& fpath) {
	auto book = xlCreateBookW();
	if(!book->load(Utf8ToUnicode(fpath).c_str())) {
		vprintError("加载文件失败 {} : {}", fpath, book->errorMessage());
		book->release();
		return false;
	}

	if(0 == book->sheetCount()) {
		vprintError("excel文件无表单 : {}", fpath);
		book->release();
		return false;
	}

	FastJson root;
	for(auto i = 0; i < book->sheetCount(); ++i) {
		auto client = book->getSheet(i);
		auto jsheet = root[UnicodeToUtf8(client->name()).c_str()];

		ws::i32 enIndex = -1;
		ws::i32 twIndex = -1;
		ws::i32 cnIndex = -1;
		ws::i32 keyIndex = -1;
		if(!__findKey(this, client, keyIndex, enIndex, twIndex, cnIndex)) {
			continue;
		}

		if(-1 != enIndex) {
			auto jEN = jsheet["en"];
			__getSheet(this, client, jEN, keyIndex, enIndex);
		}

		if(-1 != twIndex) {
			auto jTW = jsheet["tw"];
			__getSheet(this, client, jTW, keyIndex, twIndex);
		}
		
		if(-1 != cnIndex) {
			auto jCN = jsheet["cn"];
			__getSheet(this, client, jCN, keyIndex, cnIndex);
		}
	}

	book->release();

	auto data = root.toShortString();
	ws::RC4::encrypt(data, __key);

	auto savePath = ws::System::getAppDirectory().append("language.bin");
	if(!File::save(savePath, data, true, true)) {
		vprintError("保存文件失败 {} : {}", GetLastError(), savePath);
		return false;
	}

	return true;
}

bool Language::makeJson(const ustring& fpath) {
	bstring fdata;
	if(!File::load(fpath, fdata)) {
		vprintError("加载文件失败 {} : {}", GetLastError(), fpath);
		return false;
	}
	ws::RC4::decrypt(fdata, __key);

	ws::FastJson jdata;
	if(!jdata.parse(fdata)) {
		printError("错误的json格式");
		return false;
	}

	auto sf = ws::System::getAppDirectory().append("language.json");
	if(!File::save(sf, jdata.toPrettyString(), true, true)) {
		vprintError("保存文件失败 {} : {}", GetLastError(), fpath);
		return false;
	}

	return true;
}

void Language::setLocal(const ustring& local) {
	assert(ws::String::equalNoCase(local, "cn") ||
		   ws::String::equalNoCase(local, "en") ||
		   ws::String::equalNoCase(local, "tw"));
	_local = local;
}

const ustring& Language::getLocal() const {
	return _local;
}

bool Language::loadFile(const ustring& fpath) {
	bstring fdata;
	if(!File::load(fpath, fdata)) {
		vprintError("加载文件失败 {} : {}", GetLastError(), fpath);
		return false;
	}

	RC4::decrypt(fdata, __key);
	if(!JsonHelper::parse(fdata, _root)) {
		vprintError("解密语言文本失败，文件格式有误？ ： {}", fpath);
		return false;
	}

	return true;
}

const i8* Language::text(const i8* sheetname, const i8* textid) {
	const auto& d = _root.document();
	if(!d.HasMember(sheetname) ||
		!d[sheetname].HasMember(_local.c_str()) ||
		!d[sheetname][_local.c_str()].HasMember(textid)) {
		vprintError("获取ID文本错误 {}->{} : {}", sheetname, _local, textid);
		return textid;
	}

	const auto& jitem = d[sheetname][_local.c_str()][textid];
	return jitem.GetString();
}

ustring Language::text(
	const ustring& sheetname,
	const ustring& textid) {
	const auto& d = _root.document();
	if( !d.HasMember(sheetname.c_str()) ||
		!d[sheetname.c_str()].HasMember(_local.c_str()) ||
		!d[sheetname.c_str()][_local.c_str()].HasMember(textid.c_str())) {
		vprintError("获取ID文本错误 {}->{} : {}", sheetname, _local, textid);
		return textid;
	}

	const auto& jitem = d[sheetname.c_str()][_local.c_str()][textid.c_str()];
	return ustring(jitem.GetString(), jitem.GetStringLength());
}

}