#include <foundation/include/curl/libcurl.h>

#include <helper/CommonError.h>
#include <foundation/include/base/File.h>
#include <foundation/include/base/System.h>
#include <foundation/include/base/Base64.h>
#include <foundation/include/curl/curl/curl.h>


namespace ws {

const static i32 __httpStatusOK = 200;

struct libcurlMailData {
	explicit libcurlMailData(std::vector<ws::ustring>& v) : 
		_vdata(v), _index(0) {
		
	}

	u32 _index;
	const std::vector<ws::ustring>& _vdata;
};

struct libcurlTask {
	explicit libcurlTask(void* arg) : _running(true), _arg(arg) {

	}

	void* _arg;
	std::atomic<bool> _running;
};

static i32 __onDebug(CURL*, curl_infotype, i8* pData, size_t size, void* arg) {
	auto v = static_cast<libcurl*>(arg);
	if(nullptr != v && size >= 1) {
		v->nprint(SimpleLogSink::kLevelTrace, pData, size);
	}

	return CURLE_OK;
}

static size_t __onWrite(void* buffer, size_t size, size_t nmemb, void* arg) {
	if(nullptr != arg && nullptr != buffer) {
		auto task = static_cast<libcurlTask*>(arg);
		auto str = static_cast<ustring*>(task->_arg);
		if(nullptr != str && task->_running) {
			i8* pData = (i8*)buffer;
			str->append(pData, size * nmemb);
			return size * nmemb;
		}
	}

	assert(false);
	return -1;
}

static size_t __onWriteFile(void* buffer, size_t size, size_t nmemb, void* arg) {
	if(nullptr != arg && nullptr != buffer) {
		auto task = static_cast<libcurlTask*>(arg);
		auto hfile = static_cast<File*>(task->_arg);
		if(task->_running && nullptr != hfile) {
			if(hfile->write((char*)buffer, size * nmemb)) {
				return size * nmemb;
			}
		}
	}

	assert(false);
	return -1;
}

static size_t __onPostMail(void *ptr, size_t size, size_t nmemb, void *arg) {
	if((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}

	auto task = static_cast<libcurlTask*>(arg);
	if(nullptr == task) {
		return -1;
	}

	auto d = static_cast<libcurlMailData*>(task->_arg);
	if(nullptr == d || nullptr == ptr || !task->_running) {
		return -1;
	}

	if(d->_index < d->_vdata.size()) {
		const auto& data = d->_vdata[d->_index++];
		memcpy(ptr, data.c_str(), data.size());
		return data.size();
	}

	return 0;
}

static bool __getHttpStatus(CURL* curl, i32& httpStatus) {
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
	return (__httpStatusOK == httpStatus) ? true : false;
}

static ustring __getError(CURLcode code, i32 httpstatus) {
	if(__httpStatusOK != httpstatus || CURLE_OK != code) {
		return fmt::format("<http : {} curl : {} >", httpstatus, code);
	}

	return __emptyString;
}

libcurl::libcurl() : _gzip(false), _debug(false), _timeout(1 * 60) {
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

libcurl::~libcurl() {
	curl_global_cleanup();
}

void* libcurl::getCurl(const ustring& url, const i8* pcapath) {
	auto* curl = curl_easy_init();
	if(nullptr == curl) {
		vprintError("getCurl失败 : {}", ws::System::syserr());
		SetLastError(Error::kCurlInitializeFailed);
		return nullptr;
	}

	if(_debug) {
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGDATA, this);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, __onDebug);
	}

	curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 30L);
	curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 15L);

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, _timeout);
	curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1); //支持重定向

	if(_gzip) {
		curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
	}

	if(nullptr == pcapath) {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
	} else {
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pcapath);
	}

	return curl;
}

bool libcurl::post(const ustring& url, const ustring& data, ustring& ret,
	u32& taskid, const i8* pCapath /*= nullptr*/) {
	ret.clear();
	taskid = 0;

	CURL* curl = getCurl(url, pCapath);
	if(nullptr == curl) {
		return false;
	}

	libcurlTask task(&ret);
	addTask(&task, taskid);

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __onWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &task);

	curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, 
		"Content-Type: application/json; charset=utf-8");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	bool success = true;
	i32 httpCode = __httpStatusOK;
	auto code = curl_easy_perform(curl);
	if( (CURLE_OK != code) ||
		(!__getHttpStatus(curl, httpCode))) {
		vprintError("post失败 {} : {}", url, __getError(code, httpCode));
		success = false;
	}

	if(nullptr != headers) {
		curl_slist_free_all(headers);
	}

	cancelTask(taskid);
	curl_easy_cleanup(curl);
	return success;
}

bool libcurl::post(const ustring& cookiefilepath, const ustring& url,
	const ustring& postdata, ustring& ret, ws::u32& taskid,
	bool getCookie /*= false*/, const i8* pCapath /*= nullptr*/) {
	ret.clear();
	taskid = 0;

	CURL* curl = getCurl(url, pCapath);
	if(nullptr == curl) {
		return false;
	}

	libcurlTask task(&ret);
	addTask(&task, taskid);

	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postdata.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postdata.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __onWrite);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &task);

	if(getCookie) {
		curl_easy_setopt(curl, CURLOPT_COOKIEJAR, cookiefilepath.c_str());
	} else {
		curl_easy_setopt(curl, CURLOPT_COOKIEFILE, cookiefilepath.c_str());
	}

	curl_slist *header = nullptr;
	auto hvalue = "Content-Type: application/json; charset=utf-8";
	header = curl_slist_append(header, hvalue);

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);

	auto success = true;
	i32 httpCode = __httpStatusOK;
	auto code = curl_easy_perform(curl);
	if( (CURLE_OK != code) || (!__getHttpStatus(curl, httpCode))) {
		vprintError("post失败 {} : {}", url, __getError(code, httpCode));
		success = false;
	}

	if(nullptr != header) {
		curl_slist_free_all(header);
	}

	cancelTask(taskid);
	curl_easy_cleanup(curl);
	return success;
}

bool libcurl::get(const ustring& url, ustring& ret, ws::u32& taskid,
	const i8* pCapath /*= nullptr*/) {
	ret.clear();
	taskid = 0;

	CURL* curl = getCurl(url, pCapath);
	if(nullptr == curl) {
		return false;
	}

	libcurlTask task(&ret);
	addTask(&task, taskid);

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &task);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __onWrite);

	bool success = true;
	i32 httpCode = __httpStatusOK;
	auto code = curl_easy_perform(curl);
	if( (CURLE_OK != code) ||
		(!__getHttpStatus(curl, httpCode))) {
		vprintError("get失败 {} : {}", url, __getError(code, httpCode));
		success = false;
	}

	cancelTask(taskid);
	curl_easy_cleanup(curl);
	return success;
}

bool libcurl::get(const ustring& url, const std::vector<ustring>& headers,
	ustring& ret, ws::u32& taskid, const i8* pCapath /*= nullptr*/) {
	ret.clear();
	taskid = 0;

	CURL* curl = getCurl(url, pCapath);
	if(nullptr == curl) {
		return false;
	}

	libcurlTask task(&ret);
	addTask(&task, taskid);

	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &task);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __onWrite);

	struct curl_slist *urlheaders = NULL;
	for(const auto& it : headers) {
		urlheaders = curl_slist_append(urlheaders, it.c_str());
	}

	if(!headers.empty()) {
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, urlheaders);
	}

	bool success = true;
	i32 httpCode = __httpStatusOK;
	auto code = curl_easy_perform(curl);
	if( (CURLE_OK != code) ||
		(!__getHttpStatus(curl, httpCode))) {
		vprintError("get失败 {} : {}", url, __getError(code, httpCode));
		success = false;
	}

	if(nullptr != urlheaders) {
		curl_slist_free_all(urlheaders);
	}

	cancelTask(taskid);
	curl_easy_cleanup(curl);
	return success;
}

bool libcurl::getFile(const ustring& url, const ustring& fpath,
	ws::u32& taskid, const i8* pCapath /*= nullptr*/) {
	taskid = 0;

	CURL* curl = getCurl(url, pCapath);
	if(nullptr == curl) {
		return false;
	}

	File file(fpath, File::kCreateAlways);
	if(!file) {
		vprintError("创建文件失败 {} : {}", fpath, System::syserr());
		curl_easy_cleanup(curl);
		return false;
	}

	libcurlTask task((File*)&file);
	addTask(&task, taskid);

	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __onWriteFile);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &task);

	bool success = true;
	i32 httpCode = __httpStatusOK;
	auto code = curl_easy_perform(curl);
	if( (CURLE_OK != code) ||
		(!__getHttpStatus(curl, httpCode))) {
		vprintError("getFile失败 {} : {}", url, __getError(code, httpCode));
		File::del(fpath);
		success = false;
	}

	cancelTask(taskid);
	curl_easy_cleanup(curl);
	return success;
}

bool libcurl::getFtpFile(const ustring& ftpaddr, const ustring& username,
	const ustring& password, const ustring& fpath, ws::u32& taskid) {
	taskid = 0;

	auto curl = getCurl(ftpaddr, nullptr);
	if(nullptr == curl) {
		return false;
	}

	File file(fpath, File::kCreateAlways);
	if(!file) {
		vprintError("创建文件失败 {} : {}", fpath, System::syserr());
		curl_easy_cleanup(curl);
		return false;
	}

	libcurlTask task((File*)&file);
	addTask(&task, taskid);

	curl_easy_setopt(curl, CURLOPT_PORT, 21);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 0);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 5);
	curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&task);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, __onWriteFile);
	curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());

	bool success = true;
	i32 httpCode = __httpStatusOK;
	auto code = curl_easy_perform(curl);
	if( (CURLE_OK != code) ||
		(!__getHttpStatus(curl, httpCode))) {
		vprintError("getFtpFile失败 {} : {}", ftpaddr,
			__getError(code, httpCode));
		File::del(fpath);
		success = false;
	}

	cancelTask(taskid);
	curl_easy_cleanup(curl);
	return success;
}

bool libcurl::sendSmtpMail(const ustring& username, const ustring& password,
	const ustring& mailserver, u16 port, 
	const ustring& mailFrom, const std::vector<ustring>& mailTo, 
	const ustring& subject, const ustring& content, ws::u32& taskid) {
	taskid = 0;

	CURL* curl = getCurl(fmt::format(
		"smtp://{}:{}", mailserver, port), nullptr);
	if(nullptr == curl) {
		return false;
	}

	ustring formatToList;
	curl_slist* tolist = nullptr;
	for(const auto& it : mailTo) {
		tolist = curl_slist_append(tolist, it.c_str());
		formatToList.append(it).append(",");
	}

	if(!formatToList.empty()) {
		formatToList.erase(--formatToList.end());
	}
	
	std::vector<ws::ustring> vPostData;
	vPostData.push_back(fmt::format("From: {}\r\n", mailFrom));
	vPostData.push_back(fmt::format("To: {}\r\n", formatToList));
	vPostData.push_back(fmt::format("Subject: {}\r\n", subject));
	vPostData.push_back(fmt::format("Mime-version: 1.0\r\n"));
	vPostData.push_back("Content-type: text/html;Charset=\"UTF-8\"\r\n");
	vPostData.push_back("Content-transfer-encoding: 8bit\r\n\r\n");
	vPostData.push_back(content);

	libcurlMailData mailData(vPostData);
	libcurlTask task((void*)&mailData);
	addTask(&task, taskid);

	curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
	curl_easy_setopt(curl, CURLOPT_READDATA, &task);
	curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, tolist);
	curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
	curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
	curl_easy_setopt(curl, CURLOPT_MAIL_FROM, mailFrom.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, __onPostMail);

	bool success = true;
	i32 httpCode = __httpStatusOK;
	auto code = curl_easy_perform(curl);
	if( (CURLE_OK != code) ||
		(!__getHttpStatus(curl, httpCode))) {
		vprintError("邮件发送失败 : {}", __getError(code, httpCode));
		success = false;
	}

	if(nullptr != tolist) {
		curl_slist_free_all(tolist);
	}

	cancelTask(taskid);
	curl_easy_cleanup(curl);
	return success;
}

void libcurl::addTask(void* task, u32& taskid) {
	taskid = reinterpret_cast<u32>(task);
	FastMutex::ScopedLock lock(_fmTask);
	_vTask.push_back(task);
}

bool libcurl::cancelTask(ws::u32 taskid) {
	auto task = reinterpret_cast<libcurlTask*>(taskid);

	FastMutex::ScopedLock lock(_fmTask);
	for(auto it = _vTask.begin(); it != _vTask.end(); ++it) {
		if((*it) == task) {
			task->_running = false;
			_vTask.erase(it);
			return true;
		}
	}

	return false;
}

}