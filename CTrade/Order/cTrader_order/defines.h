#ifndef FIN_COMMON_DEFINES_H
#define FIN_COMMON_DEFINES_H

#include <thread>
#include <chrono>
#include <time.h>
#include "common.h"
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
	typedef double DOUBLE;	           // 8 Byte

	typedef  SIGNED_INT_8 INT_8;
	typedef  SIGNED_INT_16 INT_16;
	typedef  SIGNED_INT_32 INT_32;
	typedef  UNSIGNED_INT_8 UINT_8;
	typedef  UNSIGNED_INT_16 UINT_16;
	typedef  UNSIGNED_INT_32 UINT_32;
	typedef  UNSIGNED_INT_64 UINT_64;
	typedef DOUBLE  PRICE;
	typedef SIGNED_INT_64 SIZE;

	typedef SIGNED_INT_64 INT_64;

constexpr auto RESET = "\033[0m";
constexpr auto BLACK =  "\033[30m";      /* Black */
constexpr auto RED   =  "\033[31m";     /* Red */
constexpr auto GREEN =  "\033[32m";     /* Green */

static inline std::string log_getCurrentTime()
{
	time_t result = time(NULL);
	char str[26];
	ctime_s(str, sizeof str, &result);
	std::string s(str);
	s.pop_back();
	s += " | ";
	return s;
}


#define FIN_DEVLOG( ARG )\
	std::cout << CON_BLUE << log_getCurrentTime() << ARG<< std::endl

#define FIN_INFO( ARG )\
	std::cout << CON_GREEN << log_getCurrentTime() << ARG<< CON_WHITE <<std::endl

#define FIN_WARN( ARG )\
	std::cout <<  CON_YELLOW << log_getCurrentTime() << ARG <<  CON_WHITE << std::endl

#define FIN_ERROR( ARG )\
	std::cerr << CON_RED << log_getCurrentTime() << ARG<< CON_WHITE <<std::endl

}

#endif
