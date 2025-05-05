#pragma once
#ifndef FIN_SYMBOL_PROCESS_H
#define FIN_SYMBOL_PROCESS_H
#include "publisher.h"
#include "defines.h"
#include "Nanolog.hpp"
namespace FIN {
	class SymbolProcess
	{
	public:
		SymbolProcess(const uint16_t exchangeId, std::string secId);
		~SymbolProcess();

		FIN::MarketData market_data;
		FIN::OHLC ohlc;
		FIN::BEST_ASK_BID _bab;
		FIN::DPR dpr;
		FIN::CONTRACTACTION contract_action;
		FIN::MARKETRESUMEACTION market_action;

		void sendMarketData();
		void sendOHLC();
		void sendBAB();
		void sendDPR();
		void sendCONTRACTACTION();
		void sendMARKETACTION();

		std::vector<std::string> seprateString(std::string msg, char checkVal);
		void processMarketPicture(std::vector<std::string>& vect);
		void processDPRPrice(std::vector<std::string>& vect);
		void processContractAction(std::vector<std::string>& vect);
		void processMarketAction(std::vector<std::string>& vect);

		PRICE calculatePrice(PRICE closePrice, int perc);
		void printSize();

	};
}
#endif