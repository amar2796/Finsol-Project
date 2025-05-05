#ifndef FIN_COMMON_DEFINES_H
#define FIN_COMMON_DEFINES_H

#include <thread>
namespace FIN
{

	// TODO : Make this platform independent


	typedef int8_t    SIGNED_INT_8;    // 1 Byte
	typedef uint8_t   UNSIGNED_INT_8;  // 1 Byte
	typedef uint16_t  UNSIGNED_INT_16; // 2 Byte
	typedef int16_t   SIGNED_INT_16;   // 2 Byte
	typedef int32_t   SIGNED_INT_32;   // 4 Byte
	typedef uint32_t  UNSIGNED_INT_32; // 4 Byte
	typedef int64_t   SIGNED_INT_64;   // 8 Byte
	typedef uint64_t  UNSIGNED_INT_64; // 8 Byte
	typedef double 		DOUBLE;	           // 8 Byte

	typedef  SIGNED_INT_8 INT_8;
	typedef  SIGNED_INT_16 INT_16;
	typedef  SIGNED_INT_32 INT_32;
	typedef  UNSIGNED_INT_8 UINT_8;
	typedef  UNSIGNED_INT_16 UINT_16;
	typedef  UNSIGNED_INT_32 UINT_32;
	typedef  UNSIGNED_INT_64 UINT_64;
	typedef SIGNED_INT_64 SIZE;

	typedef SIGNED_INT_64 INT_64;
	typedef SIGNED_INT_32 SIGNED_LONG;
	typedef uint_least8_t  SIGNED_SUB_SHORT;
	typedef int16_t SIGNED_SHORT;
	typedef int32_t SIGNED_LONG;
	typedef DOUBLE  PRICE;
	typedef UINT_32  QUANTITY;
	typedef SIGNED_INT_64 SIZE;
	typedef SIGNED_INT_64 VOLUME;


#define FIN_DEVLOG( ARG )\
	std::cout << ARG<< std::endl

#define FIN_INFO( ARG )\
	std::cout << ARG<< std::endl

#define FIN_WARN( ARG )\
	std::cout <<  "WARN: " <<  ARG<< std::endl

#define FIN_ERROR( ARG )\
	std::cerr << "ERROR:"<< ARG<< std::endl

#define DEVLOG( ARG )\
	std::cout << ARG<< std::endl

#define WARN( ARG )\
	std::cout <<  "WARN: " <<  ARG<< std::endl

#define ERROR( ARG )\
	std::cout << "ERROR:"<< ARG<< std::endl

}

#endif
