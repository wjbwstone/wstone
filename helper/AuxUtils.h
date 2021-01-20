#ifndef _HELPER_AUXT_2020_03_30_16_28_06_H
#define _HELPER_AUXT_2020_03_30_16_28_06_H

namespace ws {

/*******************************************************************
*@brief  : 步进target至boundary的最小倍数
********************************************************************/
inline u32 roundUp(u32 target, u32 boundary) {
	if(target > boundary) {
		if(0 == (boundary % 2)) {
			return (target + boundary - 1) & ~(boundary - 1);
		}

		return target + (boundary - (target % boundary));
	}
	
	return boundary;
}

/*******************************************************************
*@brief  : 步进target至2的最小幂
********************************************************************/
inline i32 next2Pow(i32 target) {
	auto imax = (std::numeric_limits<i32>::max)();
	target -= 1;
	target |= target >> 1;
	target |= target >> 2;
	target |= target >> 4;
	target |= target >> 8;
	target |= target >> 16;
	return (target < 0) ? 1 : ((target >= imax) ? imax : (target + 1));
}

inline bool isPower2(u32 value) {
	return value && !(value & (value - 1));
}

inline bool equalString(const ustring& left, const ustring& right) {
	if(left.size() == right.size()) {
		for(u32 i = 0; i < left.size(); ++i) {
			if(::toupper(left[i]) != ::toupper(right[i])) {
				return false;
			}
		}

		return true;
	}

	return false;
}

}

#endif //_HELPER_AUXT_2020_03_30_16_28_06_H