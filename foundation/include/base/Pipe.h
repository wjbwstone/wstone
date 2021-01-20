#ifndef _LIBCOMMON_PIPE_2020_01_15_12_06_13_H
#define _LIBCOMMON_PIPE_2020_01_15_12_06_13_H

#include <helper/Types.h>
#include <helper/Uncopyable.h>

namespace ws {

class Pipe {
	kDisableCopyAssign(Pipe);

public:
	Pipe();
	~Pipe();

public:
	bool write(const ustring&);
	bool read(ustring&, u32& bytes);

public:
	HANDLE _hRead;
	HANDLE _hWrite;
};

}

#endif //_LIBCOMMON_PIPE_2020_01_15_12_06_13_H