#pragma once
#ifndef FIN_SYMBOL_PROCESS_H
#define FIN_SYMBOL_PROCESS_H
#include "common.h"
#include "publisher.h"
#include "message.h"
class SymbolProcess
{
public:
	SymbolProcess(const uint16_t exchangeId, std::string secId);
	~SymbolProcess();

	FIN::MarketData market_data;
	FIN::OHLC ohlc;
	FIN::BEST_ASK_BID _bab;

	FIN::MarketData_D d_market_data;
	FIN::OHLC_D d_ohlc;
	FIN::BEST_ASK_BID_D d_bab;

	void sendMarketData(bool isDouble);
	void sendOHLC(bool isDouble);
	void sendBAB(bool isDouble);
	void apply_marketDetph(FIN::NSE::COM::InteractiverBuffer* mbp, bool isBAB);
	void apply_marketDetph_D(FIN::NSE::COM::InteractiverBuffer* mbp, bool isBAB);
	void apply_ohlc_D(FIN::NSE::COM::InteractiverBuffer* mbp);
	void apply_ohlc(FIN::NSE::COM::InteractiverBuffer* mbp);
	void apply_indexValue(FIN::NSE::COM::INDEX indexes, bool isDouble);
	void applyOpenInterest(LONG open_interest, bool isBAB, bool isDouble);
	int isProcessedFirstTime;
private:


};




#endif