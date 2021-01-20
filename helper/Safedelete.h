#ifndef _HELPER_SAFEDELETE_2019_11_28_13_28_23_H
#define _HELPER_SAFEDELETE_2019_11_28_13_28_23_H

namespace ws {

namespace Safedelete {

template<class T>
inline void checkedDelete(T*& x) {
	typedef char typeMustBeComplete[sizeof(T) ? 1 : -1];
	(void)sizeof(typeMustBeComplete);

	if(nullptr != x) {
		delete x;
		x = nullptr;
	}
}

template<class T>
inline void checkedDeleteArray(T*& x) {
	typedef char typeMustBeComplete[sizeof(T) ? 1 : -1];
	(void)sizeof(typeMustBeComplete);

	if(nullptr != x) {
		delete[] x;
		x = nullptr;
	}
}

}

}

#endif //_HELPER_SAFEDELETE_2019_11_28_13_28_23_H