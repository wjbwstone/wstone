#ifndef _WSCOMMON_LANGUAGE_2020_03_20_10_55_55_H
#define _WSCOMMON_LANGUAGE_2020_03_20_10_55_55_H

/*******************************************************************
*@brief  : 根据Excel固定格式生成多语言bin文件
*1> 'cn'表示简体中文， 'en'英文，'tw'繁体中文。默认area为 'cn'
********************************************************************/

#include <helper/Types.h>
#include <helper/Uncopyable.h>
#include <helper/SimpleLogSink.h>
#include <foundation/include/json/json.h>

namespace ws {

class Language : public SimpleLogSink {
	kDisableCopyAssign(Language);

public:
	Language();
	~Language();

public:
	/*******************************************************************
	*@brief  : 产生文件至exe模块路径中language.bin
	********************************************************************/
	bool makeBin(const ustring& fpath);

	/*******************************************************************
	*@brief  : 将bin文件生成为可读的language.json文件
	********************************************************************/
	bool makeJson(const ustring& fpath);

	/*******************************************************************
	*@brief  : 配置地区
	*'cn'表示简体中文， 'en'英文，'tw'繁体中文。默认area为 'cn'
	********************************************************************/
	const ustring& getLocal() const;
	void setLocal(const ustring& local);

	/*******************************************************************
	*@brief  : 加载bin文件
	********************************************************************/
	bool loadFile(const ustring& fpath);

	/**********************************************************
	*@brief  : 获取指定区域（英文，中文，繁体）文本
	*@sheetname : 表单名称
	*@textid : 文本ID
	*@return : 成功返回对应的文本，失败返回自己
	***********************************************************/
	const i8* text(const i8* sheetname, const i8* textid);
	ustring text(const ustring& sheetname, const ustring& textid);

private:
	ustring _local;
	FastJson _root;
};

}

#endif //_WSCOMMON_LANGUAGE_2020_03_20_10_55_55_H