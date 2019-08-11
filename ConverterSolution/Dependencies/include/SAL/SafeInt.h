
#pragma once

#define INDEX_NONE -1
#define INDEX_EMPTY 0

// Found at website below
/* https://www.tutorialspoint.com/cplusplus/cpp_data_types.htm */
#define M_INT8 127
#define M_UINT8 255
#define M_INT16 32767
#define M_UINT16 65535
#define M_INT32 2147483647
#define M_UINT32 4294967295

namespace sal
{
	typedef char int8;
	typedef unsigned char uint8;
	typedef short int int16;
	typedef unsigned short int uint16;
	typedef long int int32;
	typedef unsigned long int uint32;

#ifdef _WIN64
	typedef unsigned __int64    size_t;
#else  /* _WIN64 */
	typedef unsigned int        size_t;
#endif
}