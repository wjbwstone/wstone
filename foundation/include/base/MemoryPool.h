#ifndef _H_LIBCOMMON_MEMORYPOOL_2020_10_15_10_25_05
#define _H_LIBCOMMON_MEMORYPOOL_2020_10_15_10_25_05

#include <helper/Types.h>
#include <helper/FastMutex.h>
#include <helper/Uncopyable.h>

/*******************************************************************
*@brief  : 固定大小的内存块管理器
*1> 非内置数据类型使用内存池在释放对象时候注意调用析构函数
* ex：
*	auto p = new(MemoryPool.alloc()) DataType();
*	p->~DataType();
********************************************************************/

namespace ws {

class MemoryPool {
	kDisableCopyAssign(MemoryPool);

public:
	typedef u32 ElementByte;
	typedef u32 ChunkElement;

public:
	/*******************************************************************
	* ElementByte 对象占用的内存大小
	* ChunkElement 内存块对象个数
	* maxChunk 最大内存块，超过之后在创建超标内存块，超标内存块未使用时将被释放
	* multThread 默认支持线程安全
	********************************************************************/
	MemoryPool(ElementByte, ChunkElement, u32 maxChunk = 1024,
		bool multThread = true);
	~MemoryPool();

public:
	void* alloc();
	void release(void* p);

	template<typename T>
	T* create() {
		return new(alloc()) T();
	}

	template<typename T>
	void destroy(T*& p) {
		if(nullptr != p) {
			p->~T();
			release(p);
			p = nullptr;
		}
	}

private:
	struct Chunk;
	Chunk* createChunk();
	Chunk* findFreeChunk();
	Chunk* pointerInChunk(void* p);
	void deleteChunk(Chunk* pChunk);
	void* getChunkFreeUnit(Chunk* pChunk);
	void releaseChunkUnit(Chunk* pChunk, void* p);

private:
	u32 _maxChunk;
	bool _multThread;
	u32 _elementByte;
	u32 _currentChunk;
	u32 _chunkElement;
	FastMutex _fmPool;
	Chunk* _headChunk;
};

}

#endif //_H_LIBCOMMON_MEMORYPOOL_2020_10_15_10_25_05