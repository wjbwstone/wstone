#include <foundation/include/base/FolderMonitor.h>

#include <helper/StringConvert.h>

#include <foundation/include/base/Path.h>
#include <foundation/include/base/File.h>
#include <foundation/include/base/Thread.h>
#include <foundation/include/base/Process.h>
#include <foundation/include/base/Condition.h>

namespace ws {

enum MonitorState {
	Inactive,
	Starting,
	Monitoring,
	Shutdown,
};

struct FolderWatch {
	FolderWatch(
		const ustring& folder, 
		HANDLE hDir, 
		bool moniterSubdir,
		DWORD monitorFlags) :
		_folder(folder), 
		_hDir(hDir),
		_state(MonitorState::Inactive), 
		_bufferSize(0),
		_monitorSubdir(moniterSubdir),
		_monitorFlags(monitorFlags), _error(0) {
		memset(&_overlapped, 0, sizeof(_overlapped));
	}

	~FolderWatch() {
		assert(_state == MonitorState::Inactive);
		stopMonitor(0);
	}

	void startMonitor(HANDLE iocp) {
		if(_state != MonitorState::Inactive) {
			return;
		}

		{
			FastMutex::ScopedLock lock(_fmStatus);
			_state = MonitorState::Starting;
			PostQueuedCompletionStatus(
				iocp, sizeof(this), 
				(ULONG_PTR)this, &_overlapped);
			while(MonitorState::Monitoring != _state) {
				_signal.wait(_fmStatus);
			}
		}

		if(ERROR_SUCCESS != _error) {
			FastMutex::ScopedLock lock(_fmStatus);
			_state = MonitorState::Inactive;
		}
	}

	void stopMonitor(HANDLE iocp) {
		if(_state != MonitorState::Inactive) {
			FastMutex::ScopedLock lock(_fmStatus);
			_state = MonitorState::Shutdown;

			PostQueuedCompletionStatus(
				iocp, sizeof(this),
				(ULONG_PTR)this, &_overlapped);
			while(MonitorState::Inactive != _state) {
				_signal.wait(_fmStatus);
			}
		}

		if(_hDir != INVALID_HANDLE_VALUE) {
			CloseHandle(_hDir);
			_hDir = INVALID_HANDLE_VALUE;
		}
	}

	static const UINT32 BUFFERSIZE = 65536; //DWORD字节对齐

	HANDLE _hDir;
	DWORD _error;
	ustring _folder;
	DWORD _bufferSize;
	Condition _signal;
	MonitorState _state;
	DWORD _monitorFlags;
	FastMutex _fmStatus;
	bool _monitorSubdir;
	ustring _cacheOldName;
	OVERLAPPED _overlapped;
	UINT8 _buffer[BUFFERSIZE];
};

struct FileNotify {
public:
	FileNotify(UINT8* buffer, DWORD size) :
		_buffer(buffer), 
		_bufferSize(size) {
		_curRecord = (PFILE_NOTIFY_INFORMATION)_buffer;
	}

	bool getNext() {
		if(_curRecord && 
		   _curRecord->NextEntryOffset != 0) {
			PFILE_NOTIFY_INFORMATION oldRecord = _curRecord;
			_curRecord = (PFILE_NOTIFY_INFORMATION)(
				(UINT8*)_curRecord + 
				_curRecord->NextEntryOffset);

			if((DWORD)((UINT8*)_curRecord - _buffer) > _bufferSize) {
				assert(false);
				_curRecord = oldRecord;
			}

			return (_curRecord != oldRecord);
		}

		return false;
	}

	DWORD getAction() const {
		assert(_curRecord != nullptr);
		if(_curRecord) {
			return _curRecord->Action;
		}

		return 0;
	}

	wstring getFileName() const {
		if(nullptr !=  _curRecord) {
			return wstring(
				_curRecord->FileName,
				_curRecord->FileNameLength / sizeof(wchar));
		}

		return L"";
	}

	ustring getFileFullName(
		const ustring& rootPath) const {
		ustring fullPath = rootPath;
		fullPath.append("\\").append(UnicodeToUtf8(getFileName()));
		return fullPath;
	}

protected:
	UINT8* _buffer;
	DWORD _bufferSize;
	PFILE_NOTIFY_INFORMATION _curRecord;
};

struct FileAction {
	static FileAction* createAdded(
		const ustring& fileName) {
		FileAction* action = new FileAction();
		action->lastSize = 0;
		action->newName = fileName;
		action->type = FolderMonitor::Added;
		action->checkForWriteStarted = false;
		return action;
	}

	static FileAction* createRemoved(
		const ustring& fileName) {
		FileAction* action = new FileAction();
		action->lastSize = 0;
		action->newName = fileName;
		action->checkForWriteStarted = false;
		action->type = FolderMonitor::Removed;
		return action;
	}

	static FileAction* createModified(
		const ustring& fileName) {
		FileAction* action = new FileAction();
		action->lastSize = 0;
		action->newName = fileName;
		action->checkForWriteStarted = false;
		action->type = FolderMonitor::Modified;
		return action;
	}

	static FileAction* createRenamed(
		const ustring& oldFilename,
		const ustring& newFileName) {
		FileAction* action = new FileAction();
		action->lastSize = 0;
		action->newName = newFileName;
		action->oldName = oldFilename;
		action->checkForWriteStarted = false;
		action->type = FolderMonitor::Renamed;
		return action;
	}

	static void destroy(FileAction* a) {
		delete a;
	}

	UINT64 lastSize;
	ustring oldName; 
	ustring newName;
	bool checkForWriteStarted;
	FolderMonitor::FileActionType type;
};

FolderMonitor::FolderMonitor() : 
	_hIOCP(nullptr),
	_thread(nullptr),
	_callbackFileEvent(nullptr) {
	
}

FolderMonitor::~FolderMonitor() {
	stopMonitorAll();

	{
		FastMutex::ScopedLock lock(_fmMoniter);
		while(!_fileactions.empty()) {
			FileAction* action = _fileactions.front();
			_fileactions.pop();
			FileAction::destroy(action);
		}
	}
	
}

void FolderMonitor::setFileEventCallback(
	callbackFileEvent callback) {
	_callbackFileEvent = callback;
}

bool FolderMonitor::startMonitor(const ustring& dir, bool subdir) {
	Path _path(dir);
	if(!_path.isDirectory()) {
		return false;
	}

	ws::Process::upPrivilege(nullptr, "SeBackupPrivilege");
	ws::Process::upPrivilege(nullptr, "SeRestorePrivilege");
	ws::Process::upPrivilege(nullptr, "SeChangeNotifyPrivilege");

	auto hDir = CreateFileW(_path.apiPath().c_str(), FILE_LIST_DIRECTORY,
		FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
	if(INVALID_HANDLE_VALUE == hDir) {
		return false;
	}

	auto flags = 
		FILE_NOTIFY_CHANGE_FILE_NAME | 
		FILE_NOTIFY_CHANGE_DIR_NAME | 
		FILE_NOTIFY_CHANGE_LAST_WRITE;
	auto w = new FolderWatch(dir, hDir, subdir, flags);
	_hIOCP = CreateIoCompletionPort(hDir, _hIOCP, (ULONG_PTR)w, 0);
	if(nullptr == _hIOCP) {
		delete w;
		return false;
	}

	_foldersWatch.push_back(w);

	if(nullptr == _thread) {
		_thread = new Thread();
		_thread->run(std::bind(&FolderMonitor::workerThreadMain, this));
	}
	
	w->startMonitor(_hIOCP);
	return true;
}

void FolderMonitor::stopMonitor(
	const ustring& folderPath) {
	auto findIter = std::find_if(
		_foldersWatch.begin(), 
		_foldersWatch.end(), 
		[&](const FolderWatch* x) { 
		return x->_folder == folderPath; 
	});

	if(findIter != _foldersWatch.end()) {
		FolderWatch* watchInfo = *findIter;
		watchInfo->stopMonitor(_hIOCP);
		delete watchInfo;
		_foldersWatch.erase(findIter);
	}

	if(_foldersWatch.size() == 0) {
		stopMonitorAll();
	}
}

void FolderMonitor::stopMonitorAll() {
	for(auto& watchInfo : _foldersWatch) {
		watchInfo->stopMonitor(_hIOCP); {
			FastMutex::ScopedLock lock(_fmMoniter);
			delete watchInfo;
		}
	}

	_foldersWatch.clear();

	if(nullptr != _thread) {
		PostQueuedCompletionStatus(_hIOCP, 0, 0, nullptr);
		_thread->join();
		delete _thread;
		_thread = nullptr;
	}

	if(nullptr != _hIOCP) {
		CloseHandle(_hIOCP);
		_hIOCP = nullptr;
	}
}

void FolderMonitor::workerThreadMain() {
	FolderWatch* watcher = nullptr;

	do {
		DWORD numBytes;
		LPOVERLAPPED overlapped;
		if(!GetQueuedCompletionStatus(_hIOCP, &numBytes,
			(PULONG_PTR)&watcher, &overlapped, INFINITE)) {
			assert(false);
		}

		if(nullptr != watcher) {
			MonitorState state; {
				FastMutex::ScopedLock lock(watcher->_fmStatus);
				state = watcher->_state;
			}

			switch(state) {
			case MonitorState::Starting:
				if(!ReadDirectoryChangesW(
					watcher->_hDir,
					watcher->_buffer,
					FolderWatch::BUFFERSIZE,
					watcher->_monitorSubdir,
					watcher->_monitorFlags, 
					&watcher->_bufferSize,
					&watcher->_overlapped, nullptr)) {
					assert(false);
					watcher->_error = GetLastError();
				} else {
					watcher->_error = ERROR_SUCCESS; {
						FastMutex::ScopedLock lock(watcher->_fmStatus);
						watcher->_state = MonitorState::Monitoring;
					}
				}

				watcher->_signal.wake();
				break;

			case MonitorState::Monitoring: {
					FileNotify info(
						watcher->_buffer,
						FolderWatch::BUFFERSIZE);
					handleNotifications(&info, watcher);

					if(!ReadDirectoryChangesW(
						watcher->_hDir, 
						watcher->_buffer, 
						FolderWatch::BUFFERSIZE,
						watcher->_monitorSubdir, 
						watcher->_monitorFlags, 
						&watcher->_bufferSize,
						&watcher->_overlapped, nullptr)) {
						assert(false);
						watcher->_error = GetLastError();
					} else {
						watcher->_error = ERROR_SUCCESS;
					}
				} break;

			case MonitorState::Shutdown:
				if(INVALID_HANDLE_VALUE != watcher->_hDir) {
					CloseHandle(watcher->_hDir);
					watcher->_hDir = INVALID_HANDLE_VALUE;

					{
						FastMutex::ScopedLock lock(watcher->_fmStatus);
						watcher->_state = MonitorState::Inactive;
					}

					{
						FastMutex::ScopedLock lock(_fmMoniter);
						watcher->_signal.wake();
					}
				} break;

			default: break;
			}
		}

	} while(nullptr != watcher);
}

void FolderMonitor::handleNotifications(
	FileNotify* notify,
	FolderWatch* watch) {
	std::vector<FileAction*> a;

	do {
		auto p = notify->getFileFullName(watch->_folder);
		switch(notify->getAction()) {
			case FILE_ACTION_ADDED:
				a.push_back(FileAction::createAdded(p)); break;
			case FILE_ACTION_REMOVED:
				a.push_back(FileAction::createRemoved(p)); break;
			case FILE_ACTION_MODIFIED:
				a.push_back(FileAction::createModified(p)); break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				watch->_cacheOldName = p; break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				a.push_back(FileAction::createRenamed(
					watch->_cacheOldName, p)); break;
			default: break;
		}

	} while(notify->getNext());

	{
		FastMutex::ScopedLock lock(_fmMoniter);
		for(auto& t : a) {
			_fileactions.push(t);
		}
	}

	update();
}

void FolderMonitor::update() {
	{
		FastMutex::ScopedLock lock(_fmMoniter);
		while(!_fileactions.empty()) {
			FileAction* action = _fileactions.front();
			_fileactions.pop();
			_activeFileActions.push_back(action);
		}
	}

	for(auto iter = _activeFileActions.begin(); 
		iter != _activeFileActions.end();) {
		FileAction* action = *iter;
		if(Path::isExist(action->newName)) {
			UINT64 size = File::size(action->newName);
			if(!action->checkForWriteStarted) {
				action->checkForWriteStarted = true;
				action->lastSize = size;
				++iter;
				continue;
			} else {
				if(action->lastSize != size) {
					action->lastSize = size;
					++iter;
					continue;
				}
			}
		}

		if(nullptr != _callbackFileEvent) {
			_callbackFileEvent(
				action->type, 
				action->newName, 
				action->oldName);
		}

		_activeFileActions.erase(iter++);
		FileAction::destroy(action);
	}
}

}