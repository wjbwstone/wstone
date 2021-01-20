#ifndef _H_HELPER_ARCH_2020_10_28_14_59_59
#define _H_HELPER_ARCH_2020_10_28_14_59_59

#if defined(_M_X64) || defined(__x86_64__)
#define ARCH_X86_FAMILY
#define ARCH_X86_64
#define ARCH_64_BITS
#define MACHINE_LITTLE_ENDIAN
#elif defined(_M_ARM64) || defined(__aarch64__)
#define ARCH_ARM_FAMILY
#define ARCH_64_BITS
#define MACHINE_LITTLE_ENDIAN
#elif defined(_M_IX86) || defined(__i386__)
#define ARCH_X86_FAMILY
#define ARCH_X86
#define ARCH_32_BITS
#define MACHINE_LITTLE_ENDIAN
#elif defined(__ARMEL__)
#define ARCH_ARM_FAMILY
#define ARCH_32_BITS
#define MACHINE_LITTLE_ENDIAN
#elif defined(__MIPSEL__)
#define ARCH_MIPS_FAMILY
#if defined(__LP64__)
#define ARCH_64_BITS
#else
#define ARCH_32_BITS
#endif
#define MACHINE_LITTLE_ENDIAN
#elif defined(__pnacl__)
#define ARCH_32_BITS
#define MACHINE_LITTLE_ENDIAN
#elif defined(__EMSCRIPTEN__)
#define ARCH_32_BITS
#define MACHINE_LITTLE_ENDIAN
#else
	#error Please add support for your architecture!!!
#endif

#if !(defined(MACHINE_LITTLE_ENDIAN) ^ defined(MACHINE_BIG_ENDIAN))
	#error Define either MACHINE_LITTLE_ENDIAN or MACHINE_BIG_ENDIAN
#endif

#endif //_H_HELPER_ARCH_2020_10_28_14_59_59
