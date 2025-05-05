#include "SymbolProcess.h"
namespace FIN {
	SymbolProcess::SymbolProcess(const uint16_t exchangeId, std::string secId)
		:market_data(exchangeId, secId), ohlc(exchangeId, secId), _bab(exchangeId, secId), dpr(exchangeId, secId), contract_action(exchangeId, secId), market_action(exchangeId, secId)
	{
		
	}
	SymbolProcess::~SymbolProcess()
	{
	}

	void SymbolProcess::sendMarketData()
	{
		FIN::IIBX::Publisher::getInstance()->send(market_data, sizeof(FIN::MarketData));
	}

	void SymbolProcess::sendOHLC()
	{
		FIN::IIBX::Publisher::getInstance()->send(ohlc, sizeof(FIN::OHLC));
	}

	void SymbolProcess::sendBAB()
	{
		FIN::IIBX::Publisher::getInstance()->send(_bab, sizeof(FIN::BEST_ASK_BID));
	}

	void SymbolProcess::sendDPR()
	{
		FIN::IIBX::Publisher::getInstance()->send(dpr, sizeof(FIN::DPR));
	}

	void SymbolProcess::sendCONTRACTACTION()
	{
		FIN::IIBX::Publisher::getInstance()->send(contract_action, sizeof(FIN::CONTRACTACTION));
	}

	void SymbolProcess::sendMARKETACTION()
	{
		FIN::IIBX::Publisher::getInstance()->send(market_action, sizeof(FIN::MARKETRESUMEACTION));
	}

	std::vector<std::string> FIN::SymbolProcess::seprateString(std::string msg, char checkVal)
	{
		std::vector<std::string> values;
		try
		{
			std::stringstream ss(msg);
			std::string val;

			while (std::getline(ss, val, checkVal))
			{
				values.push_back(val);
			}
		}
		catch (const std::exception& ex)
		{
			std::cout << std::to_string(__LINE__) << " " << ex.what() << std::endl;
			LOG_INFO << std::string("Exception occur: ") + ex.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
		}
		return values;
	}

	void FIN::SymbolProcess::processMarketPicture(std::vector<std::string>& vect)
	{
		try
		{
			market_data.deleteCompleteDepth();

			//generate depth for buy
			int buyDepth = stoi(vect[17]);
			if (buyDepth == 1)
			{

				market_data.insertBuyPriceInPosition(stod(vect[20]), 0);
				market_data.insertBuyQtyInPosition(stoi(vect[21]), 0);
			}
			else if (buyDepth > 1)
			{
				std::vector<std::string> valuesPrice = seprateString(vect[20], ',');
				for (int i = 0; i < valuesPrice.size(); i++)
				{
					market_data.insertBuyPriceInPosition(stod(valuesPrice[i]), i);
				}

				std::vector<std::string> valuesQty = seprateString(vect[21], ',');
				for (int i = 0; i < valuesQty.size(); i++)
				{
					market_data.insertBuyQtyInPosition(stoi(valuesQty[i]), i);
				}
			}
			
			//generate depth for sell
			int sellDepth = stoi(vect[24]);
			if (sellDepth == 1)
			{
				market_data.insertSellPriceInPosition(stod(vect[27]), 0);
				market_data.insertSellQtyInPosition(stoi(vect[28]), 0);
			}
			else if (sellDepth > 1)
			{
				std::vector<std::string> valuesPrice = seprateString(vect[27], ',');
				for (int i = 0; i < valuesPrice.size(); i++)
				{
					market_data.insertSellPriceInPosition(stod(valuesPrice[i]), i);
				}

				std::vector<std::string> valuesQty = seprateString(vect[28], ',');
				for (int i = 0; i < valuesQty.size(); i++)
				{
					market_data.insertSellQtyInPosition(stoi(valuesQty[i]), i);
				}
			}

			//set ohlc
			ohlc.setOpenPrice(stod(vect[8]));
			ohlc.setHighPrice(stod(vect[6]));
			ohlc.setLowPrice(stod(vect[7]));
			ohlc.setClosePrice(stod(vect[9]));

			//set dpr close price
			dpr.setClosePrice(std::stod(vect[9]));

			//set marketdata
			market_data.setLtp(stod(vect[4]));
			market_data.setVolume(stoll(vect[12]));
			market_data.setOpenInterest(stoll(vect[14]));

			sendMarketData();
			sendOHLC();
		}
		catch (const std::exception& ex)
		{
			std::cout << std::to_string(__LINE__) << " " << ex.what() << std::endl;
			LOG_INFO << std::string("Exception occur: ") + ex.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
		}
	}
	void SymbolProcess::processDPRPrice(std::vector<std::string>& vect)
	{
		PRICE cPrice = dpr.getClosePrice();

		dpr.setUpperLimit(cPrice + calculatePrice(cPrice, std::stoi(vect[4])));
		dpr.setLowerLimit(cPrice - calculatePrice(cPrice, std::stoi(vect[4])));

		sendDPR();
	}

	PRICE SymbolProcess::calculatePrice(PRICE closePrice, int perc)
	{
		return ((closePrice ) * perc) / 100;
	}

	void SymbolProcess::processContractAction(std::vector<std::string>& vect)
	{
		if (vect[4] == "S")
			contract_action.setAction(0);						// zero for suspend
		else
			contract_action.setAction(1);						// one for active

		sendCONTRACTACTION();
	}

	void SymbolProcess::processMarketAction(std::vector<std::string>& vect)
	{
		market_action.setAction(0);								// zero for suspend

		sendMARKETACTION();
	}

	
	void SymbolProcess::printSize()
	{
		std::cout << "size market : " << sizeof(market_data) << std::endl;
		std::cout << "size ohlc : " << sizeof(ohlc) << std::endl;
		std::cout << "size bab : " << sizeof(_bab) << std::endl;
		std::cout << "size dpr : " << sizeof(dpr) << std::endl;
		std::cout << "size contract : " << sizeof(contract_action) << std::endl;
		std::cout << "size marketAction : " << sizeof(market_action) << std::endl;
	}
}