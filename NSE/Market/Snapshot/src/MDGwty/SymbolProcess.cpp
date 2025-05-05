#include "SymbolProcess.h"

SymbolProcess::SymbolProcess(const uint16_t exchangeId, std::string secId) :market_data(exchangeId, secId), ohlc(exchangeId, secId), d_market_data(exchangeId, secId), d_ohlc(exchangeId, secId), _bab(exchangeId, secId), d_bab(exchangeId, secId)
{
	isProcessedFirstTime = false;
}


SymbolProcess::~SymbolProcess()
{
}

void SymbolProcess::sendMarketData(bool isDouble)
{
	if (!isDouble)
		FIN::MDGtwy::Publisher::getInstance()->send(market_data, sizeof(FIN::MarketData));
	else
		FIN::MDGtwy::Publisher::getInstance()->send(d_market_data, sizeof(FIN::MarketData_D));




}

void SymbolProcess::sendOHLC(bool isDouble)
{
	if (!isDouble)
		FIN::MDGtwy::Publisher::getInstance()->send(ohlc, sizeof(FIN::OHLC));
	else
		FIN::MDGtwy::Publisher::getInstance()->send(d_ohlc, sizeof(FIN::OHLC_D));
}
void SymbolProcess::sendBAB(bool isDouble)
{
	if (isDouble)
		FIN::MDGtwy::Publisher::getInstance()->send(d_bab, sizeof(FIN::BEST_ASK_BID_D));
	else
		FIN::MDGtwy::Publisher::getInstance()->send(_bab, sizeof(FIN::BEST_ASK_BID));

}

void SymbolProcess::apply_marketDetph(FIN::NSE::COM::InteractiverBuffer* mbp, bool isBAB)
{
	if (isBAB) {
		_bab.applyTrade(mbp->getLastTradingPrice(), mbp->getLastTradeTime());
		_bab.setVolume(mbp->getVolumeTradedToday());
		_bab.setBIDPrice_qty(mbp->getBuyRecordBuffer(0).getPrice(), (INT32)mbp->getBuyRecordBuffer(0).getQuantity());
		_bab.setAskPrice_qty(mbp->getSellRecordBuffer(0).getPrice(), (INT32)mbp->getSellRecordBuffer(0).getQuantity());
		sendBAB(false);
		if (!isProcessedFirstTime) {
			LOG_INFO << "Initial [" << mbp->getToken() << "] -> " << mbp->getBuyRecordBuffer(0).getPrice() << " & " << (INT32)mbp->getBuyRecordBuffer(0).getQuantity()
				<< mbp->getSellRecordBuffer(0).getPrice() << " & " << (INT32)mbp->getSellRecordBuffer(0).getQuantity();
		}
	}
	else
	{
		market_data.applyTrade(mbp->getLastTradingPrice());
		market_data.setVolume(mbp->getVolumeTradedToday());
		if (!isProcessedFirstTime)
			LOG_INFO << "Initial [" << mbp->getToken() << "]";
		for (int i = 0; i < FIN::FIN_CONST::DEPTH; i++)
		{

			market_data._buyDepth[i].setPrice_qty(mbp->getBuyRecordBuffer(i).getPrice(), mbp->getBuyRecordBuffer(i).getQuantity());
			market_data._sellDepth[i].setPrice_qty(mbp->getSellRecordBuffer(i).getPrice(), mbp->getSellRecordBuffer(i).getQuantity());
			if (!isProcessedFirstTime)
			{
				LOG_INFO << market_data._buyDepth[i].getPrice() << " : " << market_data._buyDepth[i].getQty() <<
					" || " << market_data._sellDepth[i].getPrice() << " : " << market_data._sellDepth[i].getQty();
			}
		}
		sendMarketData(false);
	}
}
void SymbolProcess::apply_marketDetph_D(FIN::NSE::COM::InteractiverBuffer* mbp, bool isBAB)
{
	if (isBAB) {
		d_bab.applyTrade(mbp->getLastTradingPrice(), mbp->getLastTradeTime());
		d_bab.setVolume(mbp->getVolumeTradedToday());
		d_bab.setBIDPrice_qty(mbp->getBuyRecordBuffer(0).getPrice(), (INT32)mbp->getBuyRecordBuffer(0).getQuantity());
		d_bab.setAskPrice_qty(mbp->getSellRecordBuffer(0).getPrice(), (INT32)mbp->getSellRecordBuffer(0).getQuantity());
		sendBAB(true);
		if (!isProcessedFirstTime) {
			LOG_INFO << "Initial [" << mbp->getToken() << "] -> " << mbp->getBuyRecordBuffer(0).getPrice() << " & " << (INT32)mbp->getBuyRecordBuffer(0).getQuantity()
				<< mbp->getSellRecordBuffer(0).getPrice() << " & " << (INT32)mbp->getSellRecordBuffer(0).getQuantity();
		}
	}
	else
	{
		d_market_data.applyTrade(mbp->getLastTradingPrice());
		d_market_data.setVolume(mbp->getVolumeTradedToday());

		for (int i = 0; i < FIN::FIN_CONST::DEPTH; i++)
		{
			d_market_data._buyDepth[i].setPrice_qty(mbp->getBuyRecordBuffer(i).getPrice(), mbp->getBuyRecordBuffer(i).getQuantity());
			d_market_data._sellDepth[i].setPrice_qty(mbp->getSellRecordBuffer(i).getPrice(), mbp->getSellRecordBuffer(i).getQuantity());
		}
		sendMarketData(true);
	}
}
void SymbolProcess::apply_ohlc_D(FIN::NSE::COM::InteractiverBuffer* mbp)
{
	d_ohlc.setOpenPrice(mbp->getOpeningPrice());
	d_ohlc.setHighPrice(mbp->getHighingPrice());
	d_ohlc.setClosePrice(mbp->getClosingPrice());
	d_ohlc.setLowPrice(mbp->getLowingPrice());
	sendOHLC(true);
}
void SymbolProcess::applyOpenInterest(LONG openInterest, bool isBAB, bool isDouble)
{
	if (!isDouble)
	{
		if (isBAB)
		{
			_bab.setOpeninterest(openInterest);
			sendBAB(isDouble);
		}
		else
		{
			market_data._openInterest = openInterest;
			sendMarketData(isDouble);

		}
	}
	else
	{
		if (isBAB)
		{
			d_bab.setOpeninterest(openInterest);
			sendBAB(isDouble);

		}
		else
		{
			d_market_data._openInterest = openInterest /*/ FIN::FIN_CONST::NSE_COM_MULTIPLIER*/;
			sendMarketData(isDouble);

		}
	}

}

void SymbolProcess::apply_ohlc(FIN::NSE::COM::InteractiverBuffer* mbp)
{
	if ((ohlc.getOpenPrice() * FIN::FIN_CONST::NSE_COM_MULTIPLIER) != (mbp->getOpeningPrice()) ||
		(ohlc.getClosePrice() * FIN::FIN_CONST::NSE_COM_MULTIPLIER) != (mbp->getClosingPrice()) ||
		(ohlc.getLowPrice() * FIN::FIN_CONST::NSE_COM_MULTIPLIER) != (mbp->getLowingPrice()) ||
		(ohlc.getHighPrice() * FIN::FIN_CONST::NSE_COM_MULTIPLIER) != (mbp->getHighingPrice()))

	{
		ohlc.setOpenPrice(mbp->getOpeningPrice());
		ohlc.setHighPrice(mbp->getHighingPrice());
		ohlc.setClosePrice(mbp->getClosingPrice());
		ohlc.setLowPrice(mbp->getLowingPrice());
		sendOHLC(false);
		if (!isProcessedFirstTime)
		{
			LOG_INFO << "INITIAL_OHLC:[" << ohlc.getSecId() << "]: Open->" << ohlc.getOpenPrice() << " High->" << ohlc.getHighPrice() << " Close->" << ohlc.getClosePrice() << " Low->" << ohlc.getLowPrice();
		}
	}

}

void SymbolProcess::apply_indexValue(FIN::NSE::COM::INDEX indexes, bool isDouble)
{
	if (!isDouble)
	{
		market_data._buyDepth[0].setPrice_qty(indexes.getindexValue(), 1);
		market_data._sellDepth[0].setPrice_qty(indexes.getindexValue(), 1);
	}
	else
	{
		d_market_data._buyDepth[0].setPrice_qty(indexes.getindexValue(), 1);
		d_market_data._sellDepth[0].setPrice_qty(indexes.getindexValue(), 1);
	}
	sendMarketData(isDouble);

}
