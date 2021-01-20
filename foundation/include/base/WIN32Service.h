#ifndef _LIBCOMMON_WIN32SERVICE_2020_01_15_12_08_14_H
#define _LIBCOMMON_WIN32SERVICE_2020_01_15_12_08_14_H

#include <helper/Types.h>
#include <helper/FastMutex.h>
#include <helper/Uncopyable.h>

/*******************************************************************
*@brief : 创建服务的时候，命令行参数如下 
* args : xxService.exe --install 服务名称 服务描述 参数1 参数N
* 使用 sc start 服务名称 参数1 参数N 输入服务需要的参数将不被该服务识别
* 该程序只支持安装服务，其他操作请使用sc管理工具
********************************************************************/

namespace ws {

class Win32Service {
	kDisableCopyAssign(Win32Service);

public:
	Win32Service();
	virtual ~Win32Service();

public:
	/*******************************************************************
	*@brief  : debug模式下直接调用run函数
	********************************************************************/
	void launch(i32 argc, const wchar* argv[]);

public:
	virtual void stop() = 0;
	virtual void run(const std::vector<ustring>& vcmd) = 0;

protected:
	void run(DWORD argc, LPWSTR* argv);
	void reportStatus(DWORD state, DWORD exitCode, DWORD waitHint);

protected:
	static void WINAPI serviceMain(DWORD, LPWSTR*);
	static DWORD WINAPI serviceHandler(DWORD, DWORD, LPVOID, LPVOID);

protected:
	SERVICE_STATUS _status;
	std::vector<ustring> _vcmd;
	SERVICE_STATUS_HANDLE _hStatus;
};

class ServiceHelper {
	struct Service {
		Service(ServiceHelper* scm, const ustring& name,
			u32 access = SERVICE_ALL_ACCESS);
		~Service();

		operator const SC_HANDLE() const { return _hService; }
		SC_HANDLE _hService;
	};

public:
	ServiceHelper();
	~ServiceHelper();

public:
	bool createService(
		const ustring& name,
		const ustring& exePath,
		const ustring& desc = __emptyString,
		const std::vector<ustring>& vcmd = std::vector<ustring>());

	bool isStoped(const ustring& name);
	bool isRunning(const ustring& name);
	bool stopService(const ustring& name);
	bool startService(const ustring& name);
	bool deleteService(const ustring& name);
	bool setServiceDescription(const ustring& name, const ustring& d);
	bool forceDeleteService(const ustring& name, const ustring& binname);

private:
	bool isServiceStatus(const ustring&, DWORD);
	bool queryServiceStatus(SC_HANDLE, SERVICE_STATUS_PROCESS&);
	bool waitServiceStatus(SC_HANDLE, i32 tryCount, DWORD status);
	bool setServiceDescription(SC_HANDLE, const ustring& description);

private:
	SC_HANDLE _hManger;
};

}

#endif //_LIBCOMMON_WIN32SERVICE_2020_01_15_12_08_14_H