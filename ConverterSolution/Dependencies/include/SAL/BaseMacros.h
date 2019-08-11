
#pragma once

#ifndef FORCEINLINE
	#ifdef COMPILER_MSVC
		#define FORCEINLINE __forceinline
	#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
		#define FORCEINLINE inline __attribute__ ((always_inline))
	#else
		#define FORCEINLINE inline
	#endif
#endif

#ifndef NULL
	#ifdef __cplusplus
		#define NULL 0
	#else
		#define NULL ((void *)0)
	#endif
#endif