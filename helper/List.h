#ifndef _HELPER_LIST_2020_01_15_12_33_09_H
#define _HELPER_LIST_2020_01_15_12_33_09_H

#include "Types.h"

namespace ws {

template<typename T>
class List : protected std::list<T> {
public:
	List() : _curIndex(cend()), _nextIndex(cend()) {

	}

	~List() {
		clearAll();
	}

	inline u32 size() const { return __super::size(); }
	inline bool empty() const { return __super::empty(); }
	inline operator bool() const { return  cend() != _curIndex; }
	inline const_iterator cend() const { return __super::cend(); }
	inline const_iterator cbegin() const { return __super::cbegin(); }

 	T operator->() const {
		if(operator bool()) {
			return *_curIndex;
		}

		assert(false && "List Out of range!!!");
		return T();
	}

	bool next() {
		if(_nextIndex != cend()) {
			_curIndex = _nextIndex++;
			return true;
		}

		_curIndex = cend();
		return false;
	}

	void push(const T& val) {
		push_back(val);
		if(1 == size()) {
			_curIndex = cbegin();
			_nextIndex = _curIndex;
		}
	}

	void move(u32 index) {
		auto v = find(index);
		if(cend() != v) {
			_curIndex = v;
			_nextIndex = _curIndex;
		}
	}

	T get(u32 index) const {
		auto v = find(index);
		if(cend() != v) {
			return (*v);
		}

		return T();
	}

	void clear(u32 index) {
		auto v = find(index);
		if(cend() != v) {
			if(_curIndex == v) {
				++_curIndex;
			}

			if(_nextIndex == v) {
				++_nextIndex;
			}

			erase(v);
		}
	}

	void clearAll() {
		__super::clear();
		_curIndex = cend();
		_nextIndex = cend();
	}

	const_iterator find(u32 index) const {
		if(index < size()) {
			auto v = cbegin();
			while(index--) {
				v++;
			}

			return v;
		}

		assert(false && "List Out of range!!!");
		return cend();
	}

protected:
	const_iterator _curIndex;
	const_iterator _nextIndex;
};

}

#endif //_HELPER_LIST_2020_01_15_12_33_09_H