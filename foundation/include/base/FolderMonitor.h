#ifndef _LIBCOMMON_FOLDERMONITOR_2020_01_15_12_05_05_H
#define _LIBCOMMON_FOLDERMONITOR_2020_01_15_12_05_05_H

#include <helper/Types.h>
#include <helper/FastMutex.h>
#include <helper/Uncopyable.h>

namespace ws {

/*******************************************************************
*@brief  : 别在回调事件里面做太多事情
********************************************************************/

class Thread;
struct FileAction;
struct FileNotify;
struct FolderWatch;

class FolderMonitor {
	kDisableCopyAssign(FolderMonitor);

public:
	enum FileActionType {
		Added,
		Removed,
		Modified,
		Renamed
	};

	typedef std::function<
		void(FileActionType,
			 const ustring& path,
			 const ustring& oldpath)> callbackFileEvent;

public:
	FolderMonitor();
	~FolderMonitor();
	
public:
	void stopMonitorAll();
	void stopMonitor(const ustring& folderPath);
	bool startMonitor(const ustring& dir, bool subdir);
	void setFileEventCallback(callbackFileEvent callback);

private:
	void update();
	void workerThreadMain();
	void handleNotifications(FileNotify* notify, FolderWatch* watcher);

private:
	HANDLE _hIOCP;
	Thread* _thread;
	FastMutex _fmMoniter;
	callbackFileEvent _callbackFileEvent;
	std::queue<FileAction*> _fileactions;
	std::list<FileAction*> _activeFileActions;
	std::vector<FolderWatch*> _foldersWatch;
};

}

#endif //_LIBCOMMON_FOLDERMONITOR_2020_01_15_12_05_05_H