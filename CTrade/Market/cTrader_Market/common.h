#ifndef FIN_COMMON_H
#define FIN_COMMON_H
#include <string>
#include "defines.h"

#ifdef _WIN32
#include <Time.h> // WINDOWS
#else
#include<sys/time.h> // LINUX
#endif
#include <chrono>

namespace FIN 
{
	namespace FIN_CONST
	{
		//! making some constants so that if there is any changes we need to only change in one place not everywhere.
		static const std::string EMPTY_STRING = "";
		//static std::string FIN_CONFIG_FILE = "";
		static const std::string QUICKFIX_CONFIG_FILE = "conf\\exchange_details.conf";
		static const std::string FIN_CONFIG_FILE = "conf\\broadcast_details.conf";
		static const std::string FILE_NAME = "FileName";
		static const std::string  TOKEN_NAME= "conf\\subscribedSymbols.txt";
		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string SEP_DOT = ".";
		static const std::string CONTRACT_DOWNLOAD = "CONTRACT_DOWNLOAD";

		const int PORT = 8090 ;

		const uint16_t Exchange_CME = 0;
		const uint16_t Exchange_LME = 1;
		const uint16_t Exchange_NSECD = 7;
		const uint16_t Exchange_NSEFO = 8;
		const uint16_t Exchange_NSECM = 9;
		const uint16_t Exchange_INTEGRAL = 18;

		const uint16_t BroadCastMsgType_Invalid = 0;
		const uint16_t BroadCastMsgType_MarketData = 1;
		
	}

}
#endif
