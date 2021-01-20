#include <foundation\include\base\MemoryPool.h>

namespace ws {

#pragma warning(disable:4200)
struct MemoryPool::Chunk {
	Chunk* _pre;
	Chunk* _next;
	u32 _freeUnitIndex;
	u32 _freeUnitCount;
	i8 _data[0];
};

MemoryPool::MemoryPool(ElementByte v, ChunkElement c, u32 maxChunk, 
	bool multThread) : 
	_elementByte(v), 
	_chunkElement(c),
	_currentChunk(0),
	_maxChunk(maxChunk),
	_multThread(multThread) {
	assert(0 != c && 0 != v && 0 != maxChunk);
	_headChunk = createChunk();
}

MemoryPool::~MemoryPool() {
	auto pChunk = _headChunk;
	while(nullptr != pChunk) {
		auto pNext = pChunk->_next;
		free(pChunk);
		pChunk = pNext;
	}
}

void* MemoryPool::alloc() {
	if(_multThread) {
		_fmPool.lock();
	}

	void* pValue = nullptr;
	auto pChunk = findFreeChunk();
	if(nullptr == pChunk) {
		pChunk = createChunk();
		if(nullptr != pChunk) {
			if(nullptr != _headChunk) {
				_headChunk->_pre = pChunk;
			}
			pChunk->_next = _headChunk;
			_headChunk = pChunk;
		}
	}

	if(nullptr != pChunk) {
		pValue = getChunkFreeUnit(pChunk);
	}

	if(_multThread) {
		_fmPool.unlock();
	}

	assert(nullptr != pValue && "Not enough memory!!!");
	return pValue;
}

void MemoryPool::release(void* p) {
	if(_multThread) {
		_fmPool.lock();
	}

	auto pChunk = pointerInChunk(p);
	if(nullptr != pChunk) {
		releaseChunkUnit(pChunk, p);
		deleteChunk(pChunk);
	}

	if(_multThread) {
		_fmPool.unlock();
	}
}

MemoryPool::Chunk* MemoryPool::createChunk() {
	auto cap = _chunkElement * _elementByte;
	auto pChunk = (Chunk*)malloc(sizeof(Chunk) + cap);
	if(nullptr != pChunk) {
		pChunk->_pre = nullptr;
		pChunk->_next = nullptr;
		pChunk->_freeUnitIndex = 0;
		pChunk->_freeUnitCount = _chunkElement;
		for(u32 i = 1; i < _chunkElement; ++i) {
			*(u32*)(pChunk->_data + ((i - 1) * _elementByte)) = i;
		}

		_currentChunk++;
	}

	return pChunk;
}

MemoryPool::Chunk* MemoryPool::findFreeChunk() {
	auto pChunk = _headChunk;
	while(nullptr != pChunk) {
		if(0 != pChunk->_freeUnitCount) {
			return pChunk;
		}

		pChunk = pChunk->_next;
	}

	return nullptr;
}

MemoryPool::Chunk* MemoryPool::pointerInChunk(void* p) {
	if(nullptr != p) {
		auto pChunk = _headChunk;
		while(nullptr != pChunk && (0 < _chunkElement)) {
			auto pBegin = pChunk->_data;
			auto pEnd = pBegin + (_elementByte * (_chunkElement - 1));
			if((u32)p >= (u32)pBegin && (u32)p <= (u32)pEnd) {
				return pChunk;
			} else {
				pChunk = pChunk->_next;
			}
		}
	}

	return nullptr;
}

void MemoryPool::deleteChunk(Chunk* pChunk) {
	assert(nullptr != pChunk);
	if( (_currentChunk <= _maxChunk) || 
		(_chunkElement != pChunk->_freeUnitCount) ) {
		return;
	}

	auto pre = pChunk->_pre;
	auto next = pChunk->_next;

	if(nullptr != pre) {
		pre->_next = next;
	} else {
		_headChunk = next;
	}

	if(nullptr != next) {
		next->_pre = pre;
	}

	free(pChunk);
	_currentChunk--;
}

void* MemoryPool::getChunkFreeUnit(Chunk* pChunk) {
	assert(nullptr != pChunk);
	assert(0 < pChunk->_freeUnitCount);
	pChunk->_freeUnitCount--;
	auto pFree = pChunk->_data + (pChunk->_freeUnitIndex * _elementByte);
	pChunk->_freeUnitIndex = *(u32*)(pFree);
	return (void*)pFree;
}

void MemoryPool::releaseChunkUnit(Chunk* pChunk, void* p) {
	assert(nullptr != pChunk && nullptr != p);
	pChunk->_freeUnitCount++;
	*((u32*)p) = pChunk->_freeUnitIndex;
	pChunk->_freeUnitIndex = ((u32)p - (u32)(pChunk->_data)) / _elementByte;
	assert(_chunkElement >= pChunk->_freeUnitCount);
}

}