#ifndef _LIBCOMMON_LIBCURL_2020_02_27_15_40_12_H
#define _LIBCOMMON_LIBCURL_2020_02_27_15_40_12_H

/*******************************************************************
*@brief  : 
*1> URL地址中参数部分请注意使用http编码，无包含http无法解析的符号
*2> post的数据ContentType为utf8编码的json
********************************************************************/

#include <helper/Types.h>
#include <helper/Uncopyable.h>
#include <helper/FastMutex.h>
#include <helper/SimpleLogSink.h>

namespace ws {

class libcurl : public SimpleLogSink {
	kDisableCopyAssign(libcurl);

public:
	libcurl();
	~libcurl();

public:
	/*******************************************************************
	*@brief  : debug模式打印详细的curl操作日志，默认禁用
	********************************************************************/
	void setDebug(bool debug) { _debug = debug; }

	/*******************************************************************
	*@brief  : 设置gzip数据压缩支持，默认禁用
	********************************************************************/
	void setGZip(bool enbale) { _gzip = enbale; }

	/*******************************************************************
	*@brief  : 命令超时时间，获取文件命令无超时间（有低速断开机制）。
	* 该方法把默认超时时间1分钟修改为最新的时间
	********************************************************************/
	void setHandleTimeOout(u32 seconds) { _timeout = seconds; }

	bool post(const ustring& url, const ustring& postdata,
		ustring& ret, u32& taskid, const i8* pCapath = nullptr);

	/*******************************************************************
	*@brief  : 使用cookie方式post数据，
	*cookiefilepath ： cookie文件地址
	*getCookie ： 默认获取cookie信息存储到指定文件中，true表示传入cookie信息
	********************************************************************/
	bool post(const ustring& cookiefilepath, const ustring& url,
		const ustring& postdata, ustring& ret, u32& taskid,
		bool getCookie = false, const i8* pCapath = nullptr);

	bool get(const ustring& url, ustring& ret, u32& taskid,
		const i8* pCapath = nullptr);

	bool get(const ustring& url, const std::vector<ustring>& headers,
		ustring& ret, u32& taskid, const i8* pCapath = nullptr);

	/*******************************************************************
	*@brief  : 获取文件将无超时机制，有网络速度过低失败功能
	********************************************************************/
	bool getFile(const ustring& url, const ustring& fpath,
		u32& taskid, const i8* pCapath = nullptr);

	bool getFtpFile(const ustring& ftpaddr, const ustring& username,
		const ustring& password, const ustring& fpath, u32& taskid);

	bool sendSmtpMail(const ustring& username, const ustring& password,
		const ustring& mailserver, u16 port,
		const ustring& mailFrom, const std::vector<ustring>& mailTo,
		const ustring& subject, const ustring& content, u32& taskid);

	bool cancelTask(u32 taskid);

private:
	void addTask(void* task, u32& taskid);
	void* getCurl(const ustring& url, const i8* pcapath);

private:
	bool _gzip;
	bool _debug;
	u32 _timeout;
	FastMutex _fmTask;
	std::vector<void*> _vTask;
};

}

#ifdef _DEBUG
#pragma comment(lib, "curld.lib")
#else
#pragma comment(lib, "curl.lib")
#endif //_DEBUG

#endif //_LIBCOMMON_LIBCURL_2020_02_27_15_40_12_H