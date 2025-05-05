#ifndef FIN_MD_NSETBT_CONSTANTS
#define FIN_MD_NSETBT_CONSTANTS
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <string>
#include <iostream>
#include "ConfigDetails.h"
#include "defines.h"
using namespace std;
namespace FIN
{
	namespace FIN_CONST
	{
		//! making some constants so that if there is any changes we need to change only at one place not everywhere.
		static const std::string pipe = "||";
		static const std::string EMPTY_STRING = "";
		static const int EMPTY_INTEGER = 0;

		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string FIN_CONFIG_FILE = "config.conf";
		static const std::string SNAPSHOT = "SNAPSHOT";
		static const std::string TOKEN = "Tokens.txt";

		static const std::string multicast_addr_A = "Multicast_Addr_A";
		static const std::string multicast_port_A = "Multicast_Port_A";
		static const std::string Interface_Addr = "Interface_Addr";
		static const std::string LOGGING = "LOGGING";
		static const std::string Enable_Log = "Enable_Log";
		static  const std::string Unicast_Addr = "Unicast_Addr";
		static const std::string Unicast_Port = "Unicast_Port";

		static const int DEPTH_SIZE = 5;
		static const std::string SEP_DOT = ".";
		static const UNSIGNED_INT_16 EXCHANGE_NUMBER = 34;

		static const double DEVIDEBY100 = 100.0;					//This is for Equity Derivatives

		//SERVICE MESSAGE
		const int32_t TIME_BROADCAST_MESSAGE = 2001;
		const int32_t AUCTION_KEEP_ALIVE_MESSAGE = 2030;
		//MARKET DATA MESSAGE
		const int32_t PRODUCT_STATE_CHANGE_MESSAGE = 2002;
		const int32_t SHORTAGE_AUCTION_SESSION_CHANGE_MESSAGE = 2003;
		const int32_t NEWS_HEADLINE = 2004;
		const int32_t MARKET_PICHTURE_BROADCAST = 2020;
		const int32_t MARKET_PICHTURE_BROADCAST_COMPLEX = 2021;
		const int32_t AUCTION_MARKET_PICTURE_BROADCAST = 2017;
		const int32_t ODD_LOT_MARKET_PICTURE_MESSAGE = 2027;
		const int32_t DEBT_MARKET_PICTURE = 2033;
		const int32_t INDEX_CHANGE_MESSAGE1 = 2011;
		const int32_t INDEX_CHANGE_MESSAGE2 = 2012;
		const int32_t LIMIT_PRICE_PROTECTION_RANGE = 2034;
		const int32_t CLOSE_PRICE = 2014;
		const int32_t OPEN_INTEREST_MESSAGE = 2015;
		const int32_t VAR_PERCENTAGE = 2016;
		const int32_t RBI_REFERENCE_RATE = 2022;
		const int32_t IMPLIED_VOLATILITY = 2028;
	}
}

#endif
