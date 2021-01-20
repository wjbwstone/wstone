#ifndef _LIBCOMMON_CLOCK_2020_01_15_12_04_01_H
#define _LIBCOMMON_CLOCK_2020_01_15_12_04_01_H

/*******************************************************************
*@brief  : 时钟精度为 kMicroSecond
********************************************************************/

#include <helper/Types.h>

namespace ws {

class Clock {
public:
	const static u32 kMilliSecond = 1 * 1000;
	const static u32 kMicroSecond = 1000 * kMilliSecond;
	const static u32 kNanoSecond = 1000 * kMicroSecond;

public:
	Clock();

public:
	void start();
	u64 nowMS() const;
	u64 nowUS() const;
	u64 elapsedMS() const;
	u64 elapsedUS() const;
	u32 resolution() const;

protected:
	u64 update() const;
	u64 now(u32 accuracy) const;
	u64 elapsed(u32 accuracy) const;

private:
	u64 _frequency;
	u64 _startCount;
};

}

#endif //_LIBCOMMON_CLOCK_2020_01_15_12_04_01_H