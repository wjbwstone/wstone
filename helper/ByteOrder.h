#ifndef _H_HELPER_BYTEORDER_2020_10_28_15_17_28
#define _H_HELPER_BYTEORDER_2020_10_28_15_17_28

#include "Arch.h"

namespace ws {

#if defined(MACHINE_LITTLE_ENDIAN)
	#define htob16(v) htons(v)
	#define htob32(v) htonl(v)
	#define htob64(v) _byteswap_uint64(v)

	#define btoh16(v) ntohs(v)
	#define btoh32(v) ntohl(v)
	#define btoh64(v) _byteswap_uint64(v)

	#define htol16(v) (v)
	#define htol32(v) (v)
	#define htol64(v) (v)

	#define ltoh16(v) (v)
	#define ltoh32(v) (v)
	#define ltoh64(v) (v)

#elif defined(MACHINE_BIG_ENDIAN)
	#define htob16(v) (v)
	#define htob32(v) (v)
	#define htob64(v) (v)

	#define btoh16(v) (v)
	#define btoh32(v) (v)
	#define btoh64(v) (v)

	#define htol16(v) _byteswap_ushort(v)
	#define htol32(v) _byteswap_ulong(v)
	#define htol64(v) _byteswap_uint64(v)

	#define ltoh16(v) _byteswap_ushort(v)
	#define ltoh32(v) _byteswap_ulong(v)
	#define ltoh64(v) _byteswap_uint64(v)

#else
	#error Define either MACHINE_LITTLE_ENDIAN or MACHINE_BIG_ENDIAN
#endif

}

#endif //_H_HELPER_BYTEORDER_2020_10_28_15_17_28