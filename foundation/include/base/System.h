#ifndef _LIBCOMMON_SYSTEM_2020_01_15_12_07_06_H
#define _LIBCOMMON_SYSTEM_2020_01_15_12_07_06_H

#include <helper/Types.h>

namespace ws {

namespace System {

//获取模块版本信息
struct ModuleVersion {
	ModuleVersion(const i8* fpath);
	ModuleVersion(const ustring& fpath);

	void build(const i8* fpath);
	ustring print(bool printToStdout = true) const;

	ustring _companyname; //公司名称
	ustring _filedesc; //文件描述
	ustring _fileversion; //文件版本
	ustring _internalname; //文件内部名称
	ustring _leagalcopyright; //版权
	ustring _originalfilename; //原始文件名称
	ustring _productname; //产品名称
	ustring _productversion; //产品版本
};

/*******************************************************************
*@brief  : 获取唯一ID
********************************************************************/
u64 getUniqueID();

/*******************************************************************
*@brief  : 内存返回值MB单位
********************************************************************/
u64 getPhysicsMemory();
u64 getPhysicsmemoryUsed();
u64 getVirtualMemory();
u64 getVirtualMemoryUsed();

/*******************************************************************
*@brief  : 注意，该函数将挂起调用者1秒
********************************************************************/
u32 getCPUUsage();
u32 getCPUCount();

/*******************************************************************
*@brief  : 获取[0-maxvalue)间等概率随机数
********************************************************************/
u32 getEqualProbability(u32 maxvalue);
void getEqualProbability(u32 maxvalue, u32 count, std::vector<u32>& ret);

ustring getRandomString(u32 minCount, u32 maxCount);

/*******************************************************************
*@brief  : 在[0-maxVlaue)半区间中获取不重复的count个数,maxValue勿太大
********************************************************************/
std::vector<u32> getUniqueRandomNumber(u32 maxValue, u32 count);

/*******************************************************************
*@brief  : 返回值以 '\\'字符结尾
********************************************************************/
ustring getAppName();
ustring getAppFullPath();
ustring getAppDirectory();

ustring getSysTempDir();
ustring getComputerName();

bool isX86System();
bool isWindwsXP();
bool isWindows7();

bool getHostIP(ustring& ip);
bool getHostMac(ustring& mac);
bool getHostIP(std::vector<ustring>& ips);
bool getHostMacIP(std::vector<std::pair<ustring, std::vector<ustring>>>& infos);

bool execBat(const ustring& batcmd);
bool execBin(const ustring& fpath,
	const std::vector<ustring>& vcmd = std::vector<ustring>());
bool restartSelf(const std::vector<ustring>& vcmd = std::vector<ustring>());

ustring syserr(u32 errid = GetLastError());

/*******************************************************************
*@brief  : 注意格式化内容，格式化参数个数，容易引起crash，尽量使用fmt格式库
********************************************************************/
void debugLog(const i8* pattern, ...);
ustring format(const i8* pattern, ...);
ustring vformat(const i8* pattern, va_list args);

/*******************************************************************
*@brief  :  debugViewLog日志前缀为 "WJB: "
********************************************************************/
void outputWindowLog(const i8* pattern, ...);
}

}

#pragma comment(lib, "Psapi.lib")

#endif //_LIBCOMMON_SYSTEM_2020_01_15_12_07_06_H