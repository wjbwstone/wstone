#ifndef _LIBCOMMON_PROCESS_2020_06_09_14_51_43_H
#define _LIBCOMMON_PROCESS_2020_06_09_14_51_43_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>

#include "Path.h"

/*******************************************************************
*@brief  : 32位进程与64位进程之间某些数据无法获取，比如 getPath 方式
********************************************************************/

namespace ws {

class Process;
typedef std::shared_ptr<Process> ProcessPtr;

class Process {
	kDisableCopyAssign(Process);
	
public:
	explicit Process(u32 pid, const ustring& executename = __emptyString, 
		u32 access = PROCESS_ALL_ACCESS);
	~Process();

public:
	void close();
	bool open(u32 pid, const ustring& executename = __emptyString,
		u32 access = PROCESS_ALL_ACCESS);

public:
	u32 getID() const;
	bool valid() const;
	bool terminate() const;
	HANDLE getHandle() const;
	const Path& getPath() const;
	bool emptyWorkingSet() const;
	ustring getExecuteName() const;
	bool wait(u32 milliseconds = INFINITE) const;

public:
	static ProcessPtr currentProcess();

	/*******************************************************************
	*@brief  : 默认获取当前进程执行文件路径
	********************************************************************/
	static Path getProcessPath(const ProcessPtr& process = nullptr);

	/*******************************************************************
	*@brief  : 默认清理当前进程内存碎片
	********************************************************************/
	static bool emptyProcessWorkingSet(const ProcessPtr& process = nullptr);

	static bool walkProcess(std::vector<ProcessPtr>& vprocess,
		u32 access = PROCESS_ALL_ACCESS);
	static ProcessPtr findProcess(const ustring& executename,
		u32 access = PROCESS_ALL_ACCESS);
	static ProcessPtr openProcess(u32 pid, 
		const ustring& executename = __emptyString, 
		u32 access = PROCESS_ALL_ACCESS);

	/*******************************************************************
	*@brief  : 注意：如果去启动一个带盾牌的程序，将调用者也设置为高权限
	* 当时服务调用者时候，withServierPrivilege可能需为真。
	* 但不同权限的程序之间的通信可能有问题，目前已知的是文件拖拽无法执行
	* 非服务可忽略withServierPrivilege(服务进程具有较高权限，普通进程右击管理员方式运行即可）
	********************************************************************/
	static ProcessPtr createProcess(const Path& fpath,
		const std::vector<ustring>& vcmd = std::vector<ustring>(),
		bool hideWindow = false,
		bool callerIsServiceAndNeedInteractive = false,
		bool useServicePrivilege = false);

	static bool getLoginUserToken(HANDLE& token);
	static bool getLoginUserSessionID(u32& sessionid);
	static bool killProcess(const ustring& executename);

	/*******************************************************************
	*@brief  : 默认提升当前进程 SeDebugPrivilege 权限
	********************************************************************/
	static bool upPrivilege(const ProcessPtr& process = nullptr, 
		const i8* privilege = nullptr, bool promotion = true);

private:
	u32 _id;
	Path _path;
	u32 _access;
	HANDLE _handle;
	ustring _executename;

public:
	static const u32 _invalidID = (std::numeric_limits<u32>::max)();
};

}

#endif //_LIBCOMMON_PROCESS_2020_06_09_14_51_43_H