#ifndef FIN_COMMON_H
#define FIN_COMMON_H
#define WIN32_LEAN_AND_MEAN
#include <string>
#include "defines.h"

#ifdef _WIN32
#include <Time.h> // WINDOWS
#else
#include<sys/time.h> // LINUX
#endif
#include <chrono>

#include <iostream>
#include <windows.h>
namespace FIN 
{
	namespace FIN_CONST
	{
		//! making some constants so that if there is any changes we need to only change in one place not everywhere.
		static const std::string EMPTY_STRING = "";
		static const std::string FIN_CONFIG_FILE = "conf\\broadcast_details.conf";
		static const std::string FILE_NAME = "FileName";
		static const std::string TARGET_COMP_ID = "CTRADER_ORDER";
		static const std::string MD_SUBSCRIPTION = "MD_SUBSCRIPTION";
		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string SEP_DOT = ".";

		//static const std::string  TOKEN_NAME= "conf\\subscribedSymbols.txt";
		//static const std::string CMEepochFile = "conf\\epoch.txt";
		//static const std::string CMESymbolVolume = "conf\\SymbolVolumeFile.txt";
		//static const std::string CMETradeSeqFile = "conf\\TradeSeriesAndSeqNo.txt";
		//static std::string FIN_CONFIG_FILE = "";
		//const int PORT = 8090 ;

		/*const uint16_t Exchange_CME = 0;
		const uint16_t Exchange_LME = 1;
		const uint16_t Exchange_NSECD = 7;
		const uint16_t Exchange_NSEFO = 8;
		const uint16_t Exchange_NSECM = 9;*/

		const uint16_t BroadCastMsgType_Invalid = 0;
		const uint16_t BroadCastMsgType_MarketData = 1;
		
	}


	inline std::ostream& CON_BLUE(std::ostream &s)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, FOREGROUND_BLUE
			| FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		return s;
	}

	inline std::ostream& CON_RED(std::ostream &s)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_RED | FOREGROUND_INTENSITY);
		return s;
	}

	inline std::ostream& CON_GREEN(std::ostream &s)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		return s;
	}

	inline std::ostream& CON_YELLOW(std::ostream &s)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		return s;
	}

	inline std::ostream& CON_WHITE(std::ostream &s)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout,
			FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
		return s;
	}

	struct color {
		color(WORD attribute) :m_color(attribute) {};
		WORD m_color;
	};

	template <class _Elem, class _Traits>
	std::basic_ostream<_Elem, _Traits>&
		operator<<(std::basic_ostream<_Elem, _Traits>& i, color& c)
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hStdout, c.m_color);
		return i;
	}	
}
#endif
