#ifndef _HELPER_SINGLETON_2019_12_18_13_25_26_H
#define _HELPER_SINGLETON_2019_12_18_13_25_26_H

#include "FastMutex.h"

namespace ws {

template<class Son>
class Singleton {
public:
	static Son* getInstance() {
		if(nullptr == s_sun) {
			FastMutex::ScopedLock lock(s_fmSun);
			if(nullptr == s_sun)
				s_sun = new Son();
		}

		return s_sun;
	}

	static void destroyInstance() {
		if(nullptr != s_sun) {
			FastMutex::ScopedLock lock(s_fmSun);
			if(nullptr != s_sun) {
				delete s_sun;
				s_sun = nullptr;
			}
		}
	}

private:
	static Son* s_sun;
	static FastMutex s_fmSun;
};

template<class Son>
FastMutex Singleton<Son>::s_fmSun;

template<class Son>
Son* Singleton<Son>::s_sun = nullptr;

}

#endif //_HELPER_SINGLETON_2019_12_18_13_25_26_H