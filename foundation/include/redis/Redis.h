#ifndef _LIBREDIS_REDIS_2020_01_15_13_48_23_H
#define _LIBREDIS_REDIS_2020_01_15_13_48_23_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>
#include <helper/FastMutex.h>
#include <helper/SimpleLogSink.h>

namespace ws {

namespace libredis { 

class Redis : public SimpleLogSink {
	kDisableCopyAssign(Redis);

public:
	Redis();
	~Redis();

public:
	bool connect(const ustring& ip, u32 port, const i8* pwd = nullptr);
	bool isValid();
	void* getImpl();
	bool selectDB(u32 dbindex);
	bool save(bool sync = false);
	bool exist(const i8* key);
	bool delKey(const i8* key);
	bool rename(const i8* oldkey, const i8* newkey);
	bool getKeys(std::vector<ustring>& keys, const i8* pattern = nullptr);
	bool getData(const i8* key, ustring& value);
	/*******************************************************************
	*@brief  : 过期时间设置0表示不过期
	********************************************************************/
	bool setData(const i8* key, const ustring& value, u32 liveSec = 0);

	bool getSetData(const i8* key, std::vector<ustring>& sets);
	bool addSetData(const i8* key, const ustring& member);
	bool addSetData(const i8* key, const std::vector<ustring>& sets);
	bool delSetData(const i8* key, const ustring& member);
	bool delSetData(const i8* key, const std::vector<ustring>& sets);
	
	bool getQueueData(const i8* key, i32 index, ustring& value);
	bool delQueueData(const i8* key, ustring& value, bool delTail = true);
	/*******************************************************************
	*@brief  : 保留being和end范围内的数据，包含begin，end索引数据
	********************************************************************/
	bool delQueueData(const i8* key, i32 begin, i32 end);
	bool keepQueueSize(const i8* key, i32 maxSize = 200, bool saveTail = true);
	bool getQueueData(const i8* key, i32 begin, i32 end,
		std::vector<ustring>& values);
	bool setQueueData(const i8* key, const ustring& value,
		bool saveToTail = true);

private:
	void* _impl;
	FastMutex _mtRedis;
};

}

}

#ifdef _DEBUG
#pragma comment(lib, "libredisd.lib")
#else
#pragma comment(lib, "libredis.lib")
#endif

#endif //_LIBREDIS_REDIS_2020_01_15_13_48_23_H