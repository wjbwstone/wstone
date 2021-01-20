#ifndef _HELPER_HEAP_2020_01_15_12_32_45_H
#define _HELPER_HEAP_2020_01_15_12_32_45_H

#include "Types.h"
#include "Uncopyable.h"

namespace ws {

/*******************************************************************
*@brief  : 默认最小堆
********************************************************************/

template<class T, class functorCompare = std::greater<T> >
class Heap {
	kDisableCopyAssign(Heap);

public:
	Heap() : _funCompare(functorCompare()) {
		clear();
	}

	~Heap() {
		clear();
	}

public:
	u32 size() const {
		return _data.size();
	}

	bool empty() const {
		return _data.empty();
	}

	T top(bool peek = false) {
		if(_data.empty()) {
			return T();
		}

		auto t = *_data.cbegin();
		if(peek) {
			return t;
		}

		std::pop_heap(_data.begin(), _data.end(), _funCompare);
		_data.pop_back();
		return t;
	}

	void push(const T& value) {
		_data.push_back(value);
		std::push_heap(_data.begin(), _data.end(), _funCompare);
	}

	T get(u32 index) const {
		auto v = find(index);
		if(_data.cend() != v) {
			return *v;
		}

		return T();
	}

	void erase(u32 index) {
		auto v = find(index);
		if(_data.cend() != v) {
			_data.erase(v);
			buildHeap();
		}
	}

	void clear() {
		_data.clear();
		buildHeap();
	}

	void buildHeap() {
		std::make_heap(_data.begin(), _data.end(), _funCompare);
	}

private:
	typedef typename std::vector<T>::const_iterator citer;
	citer find(u32 index) const {
		if(index >= size()) {
			assert(false && "Out of range!!!");
			return _data.cend();
		}

		return _data.cbegin() + index;
	}
	
 private:
	std::vector<T> _data;
	functorCompare _funCompare;
};

}

#endif //_HELPER_HEAP_2020_01_15_12_32_45_H