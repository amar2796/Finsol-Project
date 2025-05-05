#pragma once
#include "marketDataMessage.h"
#include "Publisher.h"
namespace FIN
{
	class HandleData
	{
	public:
		HandleData(const uint16_t exchangeId, std::string secId, Publisher& publisher) : _md(exchangeId, secId), _ohlc(exchangeId, secId),
			/*_dpr(exchangeId, secId),*/ _publisher(publisher)
		{

		}

		void insertBuyDepth(const PRICE price, const UNSIGNED_INT_32 qty, int32_t pos);
		void insertSellDepth(const PRICE price, const UNSIGNED_INT_32 qty, int32_t pos);
		void sendToBroadcast(bool flag);
		void setLTP(const PRICE price);
		void setOpenPrice(const PRICE open);
		void setHighPrice(const PRICE high);
		void setClosePrice(const PRICE close);
		void setLowPrice(const PRICE low);
		void setVolume(const SIGNED_INT_64 volume);
		void setOpenInterest(const SIGNED_INT_64 volume);
		/*void setUpperLimit(const PRICE uLimit);
		void setLowerLimit(const PRICE lLimit);*/

	
	private:
		MarketData _md;
		OHLC _ohlc;
		//DPR _dpr;
		Publisher _publisher;
	};
}