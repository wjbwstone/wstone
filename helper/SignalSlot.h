#ifndef _H__SIGSLOT_2020_10_26_15_46_49
#define _H__SIGSLOT_2020_10_26_15_46_49

#include "Types.h"
#include "FastMutex.h"

namespace ws {

class SlotObject;

class SignalBase {
public:
	virtual void disconnectSlot(SlotObject*) = 0;
	virtual void duplicateSlot(const SlotObject* pOld, SlotObject* pNew) = 0;
};

class SlotObject {
public:
	SlotObject() {  }

	SlotObject(const SlotObject& other) {
		FastMutex::ScopedLock lock(_fmSignal);
		for(const auto& it : _signals) {
			it->duplicateSlot(&other, this);
			_signals.insert(it);
		}
	}

	virtual ~SlotObject() {
		disconnectAll();
	}

	bool connected(SignalBase* value) {
		FastMutex::ScopedLock lock(_fmSignal);
		for(const auto it : _signals) {
			if(value == it) {
				return true;
			}
		}

		return false;
	}

	void connect(SignalBase* value) {
		FastMutex::ScopedLock lock(_fmSignal);
		_signals.insert(value);
	}

	void disconnect(SignalBase* value) {
		FastMutex::ScopedLock lock(_fmSignal);
		_signals.erase(value);
	}

	void disconnectAll() {
		FastMutex::ScopedLock lock(_fmSignal);
		for(const auto& it : _signals) {
			it->disconnectSlot(this);
		}

		_signals.clear();
	}

private:
	FastMutex _fmSignal;
	std::set<SignalBase*> _signals;
};

template<typename... Args>
class ConnectionBase {
public:
	virtual void emit(Args...) = 0;
	virtual ConnectionBase* clone() = 0;
	virtual SlotObject* getSlot() const = 0;
	virtual ConnectionBase* duplicateSlot(SlotObject*) = 0;
};

template<class tSlotObject, typename... Args>
class Connection : public ConnectionBase<Args...> {
	typedef void (tSlotObject::*pFunHandler)(Args...);

public:
	Connection() : _handler(nullptr), _pSlot(nullptr) {

	}

	Connection(tSlotObject* pSlot, void (tSlotObject::*pFun)(Args...)) {
		_pSlot = pSlot;
		_handler = pFun;
	}

	SlotObject* getSlot() const override {
		return _pSlot;
	}

	void emit(Args... args) override {
		if(nullptr != _pSlot && nullptr != _handler) {
			(_pSlot->*_handler)(args...);
		}
	}

	ConnectionBase<Args...>* clone() override {
		return new Connection<tSlotObject, Args...>(*this);
	}

	ConnectionBase<Args...>* duplicateSlot(SlotObject* pNew) override {
		return new Connection<tSlotObject, Args...>(
			dynamic_cast<tSlotObject*>(pNew), _handler);
	}
	
private:
	tSlotObject* _pSlot;
	pFunHandler _handler;
};

template<typename... Args>
class Signal : public SignalBase {
public:
	Signal() {

	}

	Signal(const Signal<Args...>& other) {
		FastMutex::ScopedLock lock(_fmSlots);
		for(const auto it : other._slots) {
			it->getSlot()->connect(this);
			_slots.push_back(it->clone());
		}
	}

	~Signal() {
		disconnectAll();
	}

	void operator()(Args... args) {
		emit(args...);
	}

	void duplicateSlot(const SlotObject* pOld, SlotObject* pNew) override {
		FastMutex::ScopedLock lock(_fmSlots);
		for(const auto& it : _slots) {
			if(pOld == it->getSlot()) {
				_slots.push_back(it->duplicateSlot(pNew));
			}
		}
	}

	void disconnectSlot(SlotObject* pSlot) override {
		FastMutex::ScopedLock lock(_fmSlots);
		for(auto it = _slots.begin(); it != _slots.end(); ) {
			if(pSlot == (*it)->getSlot()) {
				delete* it;
				it = _slots.erase(it);
			} else {
				++it;
			}
		}
	}

	template<class tSlotObject>
	void connect(tSlotObject* pSlot, void(tSlotObject::* pFun)(Args...)) {
		FastMutex::ScopedLock lock(_fmSlots);
		_slots.push_back(new Connection<tSlotObject, Args...>(pSlot, pFun));
		pSlot->connect(this);
	}

	void emit(Args... args) {
		FastMutex::ScopedLock lock(_fmSlots);
		for(const auto& it : _slots) {
			it->emit(args...);
		}
	}

	bool empty() const {
		FastMutex::ScopedLock lock(_fmSlots);
		return _slots.empty();
	}

	void disconnectAll() {
		FastMutex::ScopedLock lock(_fmSlots);
		for(const auto& it : _slots) {
			it->getSlot()->disconnect(this);
			delete it;
		}

		_slots.clear();
	}

	void disconnect(SlotObject* pSlot) {
		FastMutex::ScopedLock lock(_fmSlots);
		for(auto it = _slots.begin(); it != _slots.end(); ) {
			if(pSlot == (*it)->getSlot()) {
				delete* it;
				it = _slots.erase(it);
				pSlot->disconnect(this);
			} else {
				++it;
			}
		}
	}

private:
	FastMutex _fmSlots;
	std::list<ConnectionBase<Args...>* >  _slots;
};

}

#endif //_H__SIGSLOT_2020_10_26_15_46_49