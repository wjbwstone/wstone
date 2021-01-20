#ifndef _HELPER_ANY_2020_01_15_12_28_48_H
#define _HELPER_ANY_2020_01_15_12_28_48_H

#include "Uncopyable.h"

namespace ws {

class Any {
	struct IHolder {
		virtual ~IHolder() {};
		virtual const type_info& type() const = 0;
		virtual IHolder* clone() const = 0;
	};

	template<typename valueType>
	class Holder : public IHolder {
		kDisableCopyAssign(Holder);

		friend class Any;

	public:
		Holder(const valueType& value) : _value(value) {}

	public:
		virtual const std::type_info& type() const {
			return typeid(valueType);
		}

		virtual IHolder* clone() const {
			return new Holder(_value);
		}

	private:
		valueType _value;
	};

public:
	Any() : _holder(nullptr) {
	
	}

	Any(const Any& other) {
		if(this == &other) {
			return;
		}
		
		auto temp = dynamic_cast<IHolder*>(other._holder);
		_holder = temp ? temp->clone() : nullptr;
	}

	template<typename valueType>
	Any(const valueType& value) : 
		_holder(new Holder<
			std::remove_cv<std::decay<const valueType>::type>::type>(value)) {

	}

	~Any() { clear(); }

public:
	Any& operator=(const Any& rhs) {
		if(this == &rhs) {
			return *this;
		}
		
		clear();
		auto temp = dynamic_cast<IHolder*>(rhs._holder);
		this->_holder = temp ? temp->clone() : nullptr;
		return *this;
	}

	template<typename valueType>
	Any& operator=(const valueType& value) {
		clear();
		this->_holder = new Holder<valueType>(value);
		return *this;
	}

	void clear() {
		if(nullptr != _holder) {
			delete _holder;
			_holder = nullptr;
		}
	}

	bool empty() const {
		return (nullptr == _holder);
	}

public:
	template<typename valueType>
	static valueType cast(Any& any) {
		auto pHolder = static_cast<Holder<
			std::remove_reference<valueType>::type>*>(any._holder);
		if(any.empty() || pHolder->type() != typeid(valueType)) {
			assert(false && "bad cast!!!");
			return valueType();
		}

		return pHolder->_value;
	}

private:
	IHolder* _holder;
};

}

#endif //_HELPER_ANY_2020_01_15_12_28_48_H