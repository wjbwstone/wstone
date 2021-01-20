#include <foundation/include/redis/Redis.h>

#include <foundation/src/redis/hiredis/hiredis.h>

#include <helper/fmt/format.h>
#include <helper/StringConvert.h>

namespace ws {

namespace libredis {

struct CommandHolder {
	CommandHolder(Redis* redis, const i8 *format, ...) : _reply(nullptr) {
		va_list ap;
		va_start(ap, format);

		_result = true;
		const i8* err = "unknown";
		auto pRedis = (redisContext*)redis->getImpl();
		if(nullptr == pRedis) {
			assert(false && "Redis not establish");
			_result = false;

		} else {
			_reply = (redisReply*)redisvCommand(pRedis, format, ap);
			if(0 != pRedis->err || nullptr == _reply) {
				err = pRedis->errstr;
				_result = false;

			} else {
				if(REDIS_REPLY_ERROR == _reply->type) {
					err = _reply->str;
					_result = false;
				}
			}
		}

		if(!_result) {
			redis->printError(err);
		}

		va_end(ap);
	}

	~CommandHolder() {
		if(nullptr != _reply) {
			freeReplyObject((void*)_reply);
			_reply = nullptr;
		}
	}

	operator bool() const { return _result; }
	const redisReply* operator->() { return _reply; }

	bool _result;
	redisReply* _reply;
};

Redis::Redis() : _impl(nullptr) {
	
}

Redis::~Redis() {
	if(nullptr != _impl) {
		redisFree((redisContext*)_impl);
		_impl = nullptr;
	}
}

bool Redis::isValid() {
	return nullptr != _impl;
}

void* Redis::getImpl() {
	return _impl;
}

bool Redis::selectDB(u32 dbindex) {
	FastMutex::ScopedLock lock(_mtRedis);
	return CommandHolder(this, "select %d", dbindex);
}

bool Redis::connect(const ustring& ip, u32 port, const i8* pwd) {
	timeval timeout = {10, 0};
	auto pImpl = redisConnectWithTimeout(ip.c_str(), port, timeout);
	if(nullptr == pImpl) {
		printError("创建redis实列失败");
		return false;
	}

	if(0 != pImpl->err) {
		vprintError("连接redis服务器失败 : {}", pImpl->errstr);
		redisFree(pImpl);
		return false;
	}
	_impl = pImpl;

	if(nullptr != pwd && !CommandHolder(this, "auth %s", pwd)) {
		redisFree(pImpl);
		_impl = nullptr;
		return false;
	}

	redisEnableKeepAlive(pImpl);
	redisSetTimeout(pImpl, timeout);
	return true;
}

bool Redis::getKeys(std::vector<ustring>& keys,const i8* pattern) {
	ustring f("*");
	if(nullptr != pattern) {
		f = fmt::format("*{}*", pattern);
	}

	FastMutex::ScopedLock lock(_mtRedis);
	CommandHolder ret(this, "keys %s", f.c_str());
	if(ret) {
		keys.clear();
		keys.reserve(ret->elements);
		for(u32 i = 0; i < ret->elements; ++i) {
			auto next = ret->element[i];
			keys.push_back(ustring(next->str, next->len));
		}
	}

	return ret;
}

bool Redis::keepQueueSize(const i8* key, i32 maxSize, bool saveTail) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	return saveTail ? 
		CommandHolder(this, "ltrim %s %d -1", key, -(maxSize - 1)) :
		CommandHolder(this, "ltrim %s 0 %d", key, maxSize - 1);
}

bool Redis::getQueueData(const i8* key, i32 begin, i32 end,
	std::vector<ustring>& values) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	CommandHolder ret(this, "lrange %s %d %d", key, begin, end);
	if(ret) {
		values.clear();
		values.reserve(ret->elements);
		for(u32 i = 0; i < ret->elements; ++i) {
			auto next = ret->element[i];
			values.push_back(ustring(next->str, next->len));
		}
	}

	return ret;
}

bool Redis::getQueueData(const i8* key, i32 index, ustring& value) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	CommandHolder ret(this, "lindex %s %d", key, index);
	if(ret) {
		value.assign(ret->str, ret->len);
	}

	return ret;
}

bool Redis::setQueueData(const i8* key, const ustring& value, bool saveToTail) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	auto n = value.size();
	const auto d = value.c_str();
	return saveToTail ? 
		CommandHolder(this, "rpush %s %b", key, d, n) :
		CommandHolder(this, "lpush %s %b", key, d, n);
}

bool Redis::delQueueData(const i8* key, ustring& value, bool delTail) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	if(delTail) {
		CommandHolder ret(this, "rpop %s", key);
		if(ret) {
			value = ustring(ret->str, ret->len);
		}

		return ret;
	}

	CommandHolder ret(this, "lpop %s", key);
	if(ret) {
		value.assign(ret->str, ret->len);
	}

	return ret;
}

bool Redis::delQueueData(const i8* key, i32 begin, i32 end) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	return CommandHolder(this, "ltrim %s %d %d", key, begin, end);
}

bool Redis::getData(const i8* key, ustring& value) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	value.clear();
	CommandHolder ret(this, "get %s", key);
	if(ret) {
		value.assign(ret->str, ret->len);
	}

	return ret;
}

bool Redis::delKey(const i8* key) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	return CommandHolder(this, "del %s", key);
}

bool Redis::exist(const i8* key) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	CommandHolder ret(this, "exists %s", key);
	if(ret) {
		return (REDIS_REPLY_INTEGER == ret->type) && (1 == ret->integer);
	}

	return false;
}

bool Redis::rename(const i8* oldkey, const i8* newkey) {
	if(nullptr == oldkey || nullptr == newkey) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	return CommandHolder(this, "rename %s %s", oldkey, newkey);
}

bool Redis::setData(const i8* key, const ustring& value, u32 live) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	auto n = value.size();
	const auto d = value.c_str();
	return (0 == live) ?
		CommandHolder(this, "set %s %b", key, d, n) :
		CommandHolder(this, "setex %s %d %b", key, live, d, n);
}

bool Redis::getSetData(const i8* key, std::vector<ustring>& sets) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	CommandHolder ret(this, "smembers %s", key);
	if(ret) {
		u32 index = 0;
		sets.clear();
		sets.reserve(ret->elements);
		while(index < ret->elements) {
			auto next = ret->element[index];
			sets.push_back(ustring(next->str, next->len));
			index++;
		}
	}

	return ret;
}

bool Redis::addSetData(const i8* key, const std::vector<ustring>& sets) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	for(const auto& it : sets) {
		if(!CommandHolder(this, "sadd %s %b", key, it.c_str(), it.size())) {
			return false;
		}
	}

	return true;
}

bool Redis::addSetData(const i8* key, const ustring& member) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	if(!CommandHolder(this, "sadd %s %b", key, member.c_str(), member.size())) {
		return false;
	}

	return true;
}

bool Redis::delSetData(const i8* key, const ustring& member) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	return CommandHolder(this, "srem %s %b", key, member.c_str(), member.size());
}

bool Redis::delSetData(const i8* key, const std::vector<ustring>& sets) {
	if(nullptr == key) {
		printError("Invalid Arguments");
		return false;
	}

	FastMutex::ScopedLock lock(_mtRedis);
	for(const auto& it : sets) {
		if(!CommandHolder(this, "srem %s %b", key, it.c_str(), it.size())) {
			return false;
		}
	}

	return true;
}

bool Redis::save(bool sync /*= false*/) {
	FastMutex::ScopedLock lock(_mtRedis);
	return CommandHolder(this, sync ? "save" : "bgsave");
}

}

}