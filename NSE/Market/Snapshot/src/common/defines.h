#ifndef FIN_DEFINES_H
#define FIN_DEFINES_H

#include <thread>
namespace FIN
{

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
	typedef DOUBLE  PRICE_D;
	typedef INT_32  PRICE;
	typedef UNSIGNED_INT_64 QUANTITY;
	typedef SIGNED_INT_64 FIN_SIZE;

	typedef SIGNED_INT_64 INT_64;

#define DEVLOG( ARG )\
	std::cout /*<< "[" <<std::this_thread::get_id() << "]" "("<< __FILE__<<":"<<__LINE__ << ") "*/ << ARG<< std::endl

}

#endif
