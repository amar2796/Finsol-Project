#include "marketDataMessage.h"
#include <iomanip>

namespace FIN
{
	/**
	* Get msg length.
	* @Return UNSIGNED_INT_16.
	*/
	UNSIGNED_INT_16 BroadCastMsgHeader::getMsgLength() const
	{
		return _msgLength;
	}
	/**
	* Get msg type.
	* @Return SIGNED_INT_8.
	*/
	SIGNED_INT_8 BroadCastMsgHeader::getMsgType() const
	{
		return _msgType;
	}
	/**
	* Get exchange name .
	* @Return UNSIGNED_INT_16.
	*/
	UNSIGNED_INT_16 BroadCastMsgHeader::getExchange() const
	{
		return _exchange;
	}

	SIGNED_INT_32 BroadCastMsgHeader::getUpdateTime() const
	{
		return _updateTime;
	}

	void BroadCastMsgHeader::setUpdateTime(const SIGNED_INT_32 t)
	{
		_updateTime = t;
	}
	/**
	* Get broadCast msg header by referrance .
	* @Return BroadCastMsgHeader referrance.
	*/
	BroadCastMsgHeader& BroadCastMsgHeader::getBroadCastMsgHeader()
	{
		return *this;
	}

	/**
	* Set msgLength of the message.
	*@Param msgLength - constant UNSIGNED_INT_16 number one.
	*@Return nothing.
	*/
	void BroadCastMsgHeader::setMsgLength(const UNSIGNED_INT_16 msgLength)
	{
		_msgLength = msgLength;
	}
	/**
	* Set message type.
	*@Param msgType - constant SIGNED_INT_8 number one.
	*@Return nothing.
	*/
	void BroadCastMsgHeader::setMsgType(const SIGNED_INT_8 msgType)
	{
		_msgType = msgType;
	}
	/**
	* Set exchange name.
	*@Param exchange - constant UNSIGNED_INT_16 number one.
	*@Return nothing.
	*/
	void BroadCastMsgHeader::setExchange(const UNSIGNED_INT_16 exchange)
	{
		_exchange = exchange;
	}
	/**
	*Print BroadCastMsgHeader.
	* @Return nothing.
	*/

	void BroadCastMsgHeader::print() const
	{
		std::cout << "------ Header ------" << std::endl;
		std::cout << "MsgLength : " << _msgLength << std::endl;
		std::cout << "MsgType : " << char(_msgType + 48) << std::endl;
		std::cout << "Exch : " << _exchange << std::endl;
		std::cout << "UpdateTime : " << _updateTime << std::endl;
		std::cout << "------ Header ------" << std::endl;
	}

	/**
	*Print Depth.
	* @Return nothing.
	*/
	void Depth::print() const
	{
		std::cout.precision(2);
		std::cout << "Price : " << _price << std::endl;
		std::cout << "Qty : " << _qty << std::endl;
	}
	/**
	* Reset price and quantity of depth.
	*@Return nothing.
	*/
	void Depth::reset()
	{
		_price = 0;
		_qty = 0;
	}
	/**
	* Check the depth is empty or not.
	*@Return boolean.
	*/
	bool Depth::isEmpty() const
	{
		return (_price == 0 && _qty == 0);
	}
	/**
	* Get price .
	* @Return PRCICE.
	*/
	PRICE Depth::getPrice() const
	{
		return _price;
	}
	/**
	* Get Quantity of respective price.
	* @Return UNSIGNED_INT_16.
	*/
	QUANTITY Depth::getQty() const
	{
		return _qty;
	}
	/**
	* Set price of respective symbol.
	* @Param price - constant PRICE number one.
	*@Return nothing.
	*/
	void Depth::setPrice(const PRICE price)
	{
		_price = price;
	}
	/**
	* Set quantity of depth.
	*@Param qty - constant UNSIGNED_INT_16 number one.
	*@Return nothing.
	*/
	void Depth::setQty(const QUANTITY qty)
	{
		_qty = qty;
	}
	/**
	* Get Buy Depth price of particular position.
	*@Param pos - constant UNSIGNED_INT_16 number one.
	* @Return PRICE.
	*/
	PRICE MarketData::getBuyDepthPrice(const UNSIGNED_INT_16 pos) const
	{
		return _buyDepth[pos].getPrice();
	}
	/**
	* Get sell Depth price of particular position.
	*@Param pos - constant UNSIGNED_INT_16 number one.
	* @Return PRICE.
	*/
	PRICE MarketData::getSellDepthPrice(const UNSIGNED_INT_16 pos) const
	{
		return _sellDepth[pos].getPrice();
	}
	/**
	* Get Buy Depth quantity of particular position.
	*@Param pos - constant UNSIGNED_INT_16 number one.
	* @Return UNSIGNED_INT_16.
	*/
	UNSIGNED_INT_16 MarketData::getBuyDepthQty(const UNSIGNED_INT_16 pos) const
	{
		return _buyDepth[pos].getQty();
	}
	/**
	* Get sell Depth quantity of particular position.
	*@Param pos - constant UNSIGNED_INT_16 number one.
	* @Return UNSIGNED_INT_16.
	*/

	UNSIGNED_INT_16  MarketData::getSellDepthQty(const UNSIGNED_INT_16 pos) const
	{
		return _sellDepth[pos].getQty();
	}
	/**
	*Deleting buy ladder or depth of MarketData at given price
	*@Param price - constant price number one.
	*@Return nothing.
	*/
	void MarketData::deleteBuyDepth(const PRICE price)
	{

		UNSIGNED_INT_16 pos = 0;
		if (getPositionForBuyPrice(price, pos))
		{
			for (int i = pos; i < FIN::FIN_CONST::DEPTH_SIZE - 1; ++i)
				_buyDepth[i] = _buyDepth[i + 1];
			_buyDepth[FIN::FIN_CONST::DEPTH_SIZE - 1].setPrice(0);
			_buyDepth[FIN::FIN_CONST::DEPTH_SIZE - 1].setQty(0);

		}
	}
	void MarketData::deleteBuyDepthThru(const PRICE, uint16_t pos)
	{
		for (size_t i = 0; i < pos; i++)
		{
			deleteBuyDepth(_buyDepth[0].getPrice());
		}
	}

	void MarketData::deleteBuyDepthFrom(const PRICE, uint16_t pos)
	{
		for (size_t i = (pos - 1); i < FIN::FIN_CONST::DEPTH_SIZE; i++)
		{
			if (_buyDepth[0].getPrice() != 0)
				deleteBuyDepth(_buyDepth[0].getPrice());
			else
				break;
		}
	}

	/**
	*Deleting sell ladder or depth of MarketData at given price
	*@Param price - constant price number one.
	*@Return nothing.
	*/
	void MarketData::deleteSellDepth(const PRICE price)
	{
		UNSIGNED_INT_16 pos = 0;
		if (getPositionForSellPrice(price, pos))
		{
			for (int i = pos; i < FIN::FIN_CONST::DEPTH_SIZE - 1; ++i)
				_sellDepth[i] = _sellDepth[i + 1];
			_sellDepth[FIN::FIN_CONST::DEPTH_SIZE - 1].setPrice(0);
			_sellDepth[FIN::FIN_CONST::DEPTH_SIZE - 1].setQty(0);
		}
	}
	void MarketData::deleteSellDepthThru(const PRICE, uint16_t pos)
	{
		for (size_t i = 0; i < pos; i++)
		{
			deleteSellDepth(_sellDepth[0].getPrice());
		}
	}
	void MarketData::deleteSellDepthFrom(const PRICE, uint16_t pos)
	{
		for (size_t i = (pos - 1); i < FIN::FIN_CONST::DEPTH_SIZE; i++)
		{
			if (_sellDepth[0].getPrice() != 0)
				deleteBuyDepth(_sellDepth[0].getPrice());
			else
				break;
		}
	}


	/**
	*updating buy ladder or depth of MarketData by updating quantity at given price
	*@Param price - constant price number one.
	*@Param qty - constant UNSIGNED_INT_16 number two.
	*@Return nothing.
	*/
	void MarketData::updateBuyDepth(const PRICE price, const QUANTITY qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (getPositionForBuyPrice(price, pos))
		{
			_buyDepth[pos].setPrice(price);
			_buyDepth[pos].setQty(qty);
		}
	}
	void MarketData::updateBuyDepth_Overlay(const PRICE changed_price, const QUANTITY qty, UNSIGNED_INT_16 pos)
	{
		if (pos > 4)
			return;
		_buyDepth[pos].setPrice(changed_price);
		if (qty != 0)_buyDepth[pos].setQty(qty);
	}

	/**
	*updating sell ladder or depth of MarketData by updating quantity at given price
	*@Param price - constant price number one.
	*@Param qty - constant UNSIGNED_INT_16 number two.
	*@Return nothing.
	*/
	void MarketData::updateSellDepth(const PRICE price, const QUANTITY qty)
	{

		UNSIGNED_INT_16 pos = 0;
		if (getPositionForSellPrice(price, pos))
		{
			_sellDepth[pos].setPrice(price);
			_sellDepth[pos].setQty(qty);

		}
	}
	void MarketData::updateSellDepth_Overlay(const PRICE changed_price, const QUANTITY qty, UNSIGNED_INT_16 pos)
	{

		_sellDepth[pos].setPrice(changed_price);
		if (qty != 0)
			_sellDepth[pos].setQty(qty);
	}

	/**
	*Print  Market Data.
	* @Return nothing.
	*/
	void MarketData::print() const
	{
		FIN_DEVLOG("*********** MARKET_DATA_START ************");
		//BroadCastMsgHeader::print() ;

		/*
		if (!checkBestBidAsk())
		{
			std::cout<<"incorrect bid ask"<<std::endl;
		}

		if (!checkCorrectnessBuy())
		{
			std::cout<<"incorrect buy depth"<<std::endl;
		}

		if (!checkCorrectnessSell())
		{
			std::cout<<"incorrect sell depth"<<std::endl;
		}
		*/


		FIN_DEVLOG(
			"Time  : "
			<< std::endl << "Sec   : " << _secId
			<< std::endl << "LTP   : " << _ltp
			<< std::endl << "LTT   : " << _lastTradeTime
			<< std::endl << "Open_Interest   : " << _openInterest
			<< std::endl << "Volume: " << _volume);

		for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
		{
			FIN_DEVLOG(

				std::left << std::setw(10)
				<< _buyDepth[i].getQty()
				<< std::left << std::setw(10)
				//<<std::fixed
				//<< std::setprecision(3)
				<< std::to_string(_buyDepth[i].getPrice())
				//<<(_buyDepth[i].getPrice())
				<< std::left << std::setw(10)
				<< " | "
				<< std::left << std::setw(10)
				//		<<std::fixed
				//		<<std::setprecision(5)
				<< std::to_string(_sellDepth[i].getPrice())
				//<<(_sellDepth[i].getPrice())
				<< std::left << std::setw(10)
				<< _sellDepth[i].getQty()
				<< std::left << std::setw(10)
			);
		}
		FIN_DEVLOG("************ MARKET_DATA_END *************");
	}

	/**
	*Check correctness of the top bid and top ask.
	*@Return boolean.
	*/
	bool MarketData::checkBestBidAsk() const
	{
		if ((_sellDepth[0].getPrice() == 0) && (_buyDepth[0].getPrice() == 0))
		{
			return false;
		}
		else if (_buyDepth[0].getPrice() > _sellDepth[0].getPrice())
		{
			if (_sellDepth[0].getPrice() != 0)
				return false;
		}

		return true;

	}
	/**
	*Check correctness of the sell depth.
	*@Return boolean.
	*/
	bool MarketData::checkCorrectnessBuy() const
	{
		for (int i = 1; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
		{
			if (_buyDepth[i].getPrice() != 0 && (_buyDepth[i].getPrice() >= _buyDepth[i - 1].getPrice()))
			{
				return false;
			}
			if (_buyDepth[i - 1].getQty() == 0 && _buyDepth[i].getQty() != 0)
			{
				return false;
			}
		}

		return true;
	}
	/**
	*Check correctness of the sell depth.
	*@Return boolean.
	*/
	bool MarketData::checkCorrectnessSell() const
	{
		for (int i = 1; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
		{
			if (_sellDepth[i].getQty() != 0 && (_sellDepth[i].getPrice() <= _sellDepth[i - 1].getPrice()))
			{
				return false;
			}
			if (_sellDepth[i - 1].getQty() == 0 && _sellDepth[i].getQty() != 0)
			{
				return false;
			}
		}

		return true;
	}
	/**
*insert new price and quantity into buy depth.
*@Param price - constant price number one.
*@Param qty - quantity (uint16) number two.
*@Return nothing.
*/
	void MarketData::insertBuyDepth(const PRICE price, const QUANTITY qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (!getPositionForBuyPrice(price, pos))
		{

			if (pos >= FIN::FIN_CONST::DEPTH_SIZE)
			{

				return;
			}
			for (int i = FIN::FIN_CONST::DEPTH_SIZE - 1; i > pos; --i)
			{
				_buyDepth[i] = _buyDepth[i - 1];
			}

			_buyDepth[pos].setPrice(price);
			_buyDepth[pos].setQty(qty);

		}

	}
	/**
	*return position for given price into buy depth.
	*@Param price - constant price number one.
	*@Param pos - UNSIGNED_INT_16 reference number two.
	*@Return boolean.
	*/
	bool MarketData::getPositionForBuyPrice(const PRICE price, UNSIGNED_INT_16& pos)
	{

		bool found = false;

		for (; (!_buyDepth[pos].isEmpty() && pos < FIN::FIN_CONST::DEPTH_SIZE); ++pos)
		{
			if (price == _buyDepth[pos].getPrice())
			{
				found = true;
				break;
			}
			if (price > _buyDepth[pos].getPrice())
			{
				break;
			}
		}

		return found;
	}

	/**
	*insert new price and quantity into sell depth.
	*@Param price - constant price number one.
	*@Param qty - UNSIGNED_INT_16 number two.
	*@Return nothing.
	*/
	void MarketData::insertSellDepth(const PRICE price, const QUANTITY qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (!getPositionForSellPrice(price, pos))
		{
			if (pos >= FIN::FIN_CONST::DEPTH_SIZE) return;
			for (int i = FIN::FIN_CONST::DEPTH_SIZE - 1; i > pos; --i)
				_sellDepth[i] = _sellDepth[i - 1];
			_sellDepth[pos].setPrice(price);
			_sellDepth[pos].setQty(qty);
		}
	}



	/**
	*return position for given price into sell depth.
	*@Param price - constant price number one.
	*@Param pos - UNSIGNED_INT_16 reference number two.
	*@Return boolean.
	 */
	bool MarketData::getPositionForSellPrice(const PRICE price, UNSIGNED_INT_16& pos)
	{
		bool found = false;

		for (; (!_sellDepth[pos].isEmpty() && pos < FIN::FIN_CONST::DEPTH_SIZE); ++pos)
		{

			if (price == _sellDepth[pos].getPrice())
			{
				found = true;
				break;
			}
			if (price < _sellDepth[pos].getPrice())
			{
				break;
			}
		}

		return found;

	}
	/**
	* Apply trade request of the market.
	* @Param  price - constant PRICE number one .
	* @Return nothing.
	*/
	void MarketData::applyTrade(const PRICE price)
	{
		_ltp = price;
	}
	/**
	* Reset buyDepth and sellDepth of the market.
	* @Return nothing.
	*/
	void MarketData::deleteCompleteDepth()
	{
		for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; i++)
		{
			_buyDepth[i].reset();
			_sellDepth[i].reset();
		}
	}
	/**
	* Set security Id.
	* @Param  secId - constant string number one .
	* @Return nothing.
	*/
	void MarketData::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	/**
	* Set security Id.
	* @Param  secId - constant character pointer number one .
	* @Return nothing.
	*/
	void MarketData::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	/**
	* Set volume of the market.
	* @Param  volume - constant SIGNED_INT_64 number one .
	* @Return nothing.
	*/
	void MarketData::setVolume(const VOLUME volume)
	{
		_volume = volume;
	}
	/**
	* Set last trade price of the market.
	* @Param  price - constant PRICE number one .
	* @Return nothing.
	*/
	void MarketData::setLtp(const PRICE price)
	{
		_ltp = price;
	}
	/**
	* Get last trade price .
	* @Return PRICE.
	*/
	PRICE MarketData::getLtp() const
	{
		return _ltp;
	}
	void MarketData::setOpenInterest(const SIGNED_INT_64 openInt)
	{
		_openInterest = openInt;
	}
	SIGNED_INT_64 MarketData::getOpenInterest() const
	{
		return _openInterest;
	}
	/**
	* Get sec Id.
	* @Return string.
	*/
	std::string MarketData::getSecId() const
	{
		return std::string(_secId);
	}
	/**
	* Get volume of market data.
	* @Return SIGNED_INT_64.
	*/
	SIGNED_INT_64 MarketData::getVolume() const
	{
		return _volume;
	}
	/**
	*Print OHLC .
	* @Return nothing.
	*/
	void  OHLC::print() const
	{
		FIN_DEVLOG("*********** OHLC_START ************");
		BroadCastMsgHeader::print();
		//std::cout.precision(2);
		std::cout << "SecId: " << _secId << std::endl;
		std::cout << "OpenPrice: " << _openPrice;
		std::cout << "\nHighPrice: " << _highPrice;
		std::cout << "\nClosePrice: " << _closePrice;
		std::cout << "\nLowPrice: " << _lowPrice << std::endl;
		FIN_DEVLOG("************ OHLC_END *************");
	}
	/**
	* Set open price of respective security.
	*@Param open - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setOpenPrice(const PRICE open)
	{
		_openPrice = open;
	}
	/**
	* Set high price of respective security.
	*@Param high - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setHighPrice(const PRICE high)
	{
		_highPrice = high;
	}
	/**
	* Set close price of respective security.
	*@Param close - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setClosePrice(const PRICE close)
	{
		_closePrice = close;
	}
	/**
	* Set low price of respective security.
	*@Param low - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setLowPrice(const PRICE low)
	{
		_lowPrice = low;
	}
	/**
* Set security Id.
* @Param  secId - constant character pointer number one .
* @Return nothing.
*/
	void  OHLC::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	/**
	* Set security Id.
	* @Param  secId - constant string number one .
	* @Return nothing.
	*/

	void  OHLC::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	/**
	* Get open price of respective security.
	* @Return PRICE.
	*/
	PRICE OHLC::getOpenPrice() const
	{
		return _openPrice;
	}
	/**
	* Get high price of respective security.
	* @Return PRICE.
	*/
	PRICE OHLC::getHighPrice() const
	{
		return _highPrice;
	}
	/**
	* Get close price of respective security.
	* @Return PRICE.
	*/
	PRICE OHLC::getClosePrice() const
	{
		return _closePrice;
	}
	/**
	* Get low price of respective security.
	* @Return PRICE.
	*/
	PRICE OHLC::getLowPrice() const
	{
		return _lowPrice;
	}
	/**
	* Get security Id.
	* @Return string.
	*/
	std::string OHLC::getSecId() const
	{
		return std::string(_secId);
	}
	/**
	*Print DPR .
	* @Return nothing.
	*/
	void DPR::print() const
	{
		//std::cout.precision(2);
		FIN_DEVLOG("*********** DPR_START ************");
		BroadCastMsgHeader::print();
		std::cout << "upperLimit: " << _upperLimit << "\n";
		std::cout << " secid " << _secId << "\n";
		std::cout << " closeprice  " << _closePrice << "\n";

		std::cout << "\nlowerLimit: " << _lowerLimit << "\n";
		FIN_DEVLOG("*********** DPR_START ************");

	}
	/**
	* Set upper limit.
	* @Param upper - constant PRICE number one .
	* @Return nothing.
	*/
	void DPR::setUpperLimit(const PRICE upper)
	{
		_upperLimit = upper;
	}

	/**
	* Set lower limit.
	* @Param lower - constant PRICE number one .
	* @Return nothing.
	*/
	void DPR::setClosePrice(const PRICE close)
	{
		_closePrice = close;
	}
	/**
	* Set lower limit.
	* @Param lower - constant PRICE number one .
	* @Return nothing.
	*/
	void DPR::setLowerLimit(const PRICE lower)
	{
		_lowerLimit = lower;
	}
	/**
	* Set security Id.
	* @Param  secId - constant character pointer number one .
	* @Return nothing.
	*/
	void DPR::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	/**
	* Set security Id.
	* @Param  secId - constant string number one .
	* @Return nothing.
	*/
	void DPR::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	/**
	* Get upper limit of the market.
	* @Return PRICE.
	*/
	PRICE DPR::getUpperLimit() const
	{
		return _upperLimit;
	}
	/**
	* Get lower limit of the market.
	* @Return PRICE.
	*/
	PRICE DPR::getLowerLimit() const
	{
		return _lowerLimit;
	}
	/**
	* Get security Id .
	* @Return string.
	*/
	std::string DPR::getSecId() const
	{
		return std::string(_secId);
	}
	/**
	*Print Market Notice .
	* @Return nothing.
	*/
	void MarketNotice::print() const
	{

	}
	/**
	* Set notice producer.
	* @Param notice - constant SIGNED_INT_8 number one .
	* @Return nothing.
	*/
	void MarketNotice::setNoticeProducer(const SIGNED_INT_8 notice)
	{
		_noticeProducer = notice;
	}
	/**
	* Set message.
	* @Param  message - constant string number one .
	* @Return nothing.
	*/
	void MarketNotice::setMessage(const std::string& message)
	{
		memcpy(_message, message.c_str(), 200);
	}
	/**
	* Set message.
	* @Param  message - constant character pointer number one .
	* @Return nothing.
	*/
	void MarketNotice::setMessage(const char* message)
	{
		memcpy(_message, message, 200);
	}
	/**
	* Get notice producer .
	* @Return SIGNED_INT_8.
	*/
	SIGNED_INT_8 MarketNotice::getNoticeProducer() const
	{
		return _noticeProducer;
	}
	/**
	* Get message of market .
	* @Return string.
	*/
	std::string MarketNotice::getMessage() const
	{
		return std::string(_message);
	}
	/**
	*Print symbol Notice.
	* @Return nothing.
	*/
	void SymbolNotice::print() const
	{

	}
	/**
	* Set notice producer of market.
	* @Param notice - constant SIGNED_INT_8 number one .
	* @Return nothing.
	*/
	void SymbolNotice::setNoticeProducer(const SIGNED_INT_8 notice)
	{
		_noticeProducer = notice;
	}
	/**
* Set security Id of market.
* @Param session - constant character pointer number one.
* @Return nothing.
*/
	void SymbolNotice::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	/**
	* Set security Id of market .
	* @Param secId - constant string number one .
	* @Return nothing.
	*/
	void SymbolNotice::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	/**
	* Set message of market .
	* @Param message - constant string number one.
	* @Return nothing.
	*/
	void SymbolNotice::setMessage(const std::string& message)
	{
		memcpy(_message, message.c_str(), 200);
	}
	/**
	* Set message of market .
	* @Param message - constant character pointer number one .
	* @Return nothing.
	*/
	void SymbolNotice::setMessage(const char* message)
	{
		memcpy(_message, message, 200);
	}
	/**
	* Get notice producer.
	* @Return SIGNED_INT_8.
	*/
	SIGNED_INT_8 SymbolNotice::getNoticeProducer() const
	{
		return _noticeProducer;
	}
	/**
	* Get security Id.
	* @Return string.
	*/
	std::string SymbolNotice::getSecId() const
	{
		return std::string(_secId);
	}
	/**
	* Get message of market .
	* @Return string.
	*/
	std::string SymbolNotice::getMessage() const
	{
		return std::string(_message);
	}
	/**
* Set market session status.
* @Param session - constant SIGNED_INT_8 number one.
* @Return nothing.
*/
	void ModuleConnectivity::print() const
	{
		FIN_DEVLOG("*********** MODULE_CONNECTIVITY_START ************");
		BroadCastMsgHeader::print();
		FIN_DEVLOG("************ MODULE_CONNECTIVITY_END *************");
	}
	/**
	* Set module type of market.
	* @Param module type - constant SIGNED_INT_8 number one.
	* @Return nothing.
	*/
	void ModuleConnectivity::setModuleType(const SIGNED_INT_8 moduletype)
	{
		_moduleType = moduletype;
	}
	/**
	* Set module state of market.
	* @Param module state - constant SIGNED_INT_8 number one.
	* @Return nothing.
	*/
	void ModuleConnectivity::setModuleState(const SIGNED_INT_8 moduleState)
	{
		_moduleState = moduleState;
	}
	/**
	* Get Module type.
	* @Return SIGNED_INT_8.
	*/
	SIGNED_INT_8 ModuleConnectivity::getModuleType() const
	{
		return _moduleType;
	}
	/**
	* Get Module status.
	* @Return SIGNED_INT_8.
	*/
	SIGNED_INT_8 ModuleConnectivity::getModuleState() const
	{
		return _moduleState;
	}
	/**
	*Print session Info.
	* @Return nothing.
	*/
	void SessionInfo::print() const
	{
		FIN_DEVLOG("*********** SESSION_INFO_START ************");
		BroadCastMsgHeader::print();
		FIN_DEVLOG("************ SESSION_INFO_END *************");
	}
	/**
	* Set market market type.
	* @Param marketType - constant SIGNED_INT_8 number one.
	* @Return nothing.
	*/
	void SessionInfo::setMarketType(const SIGNED_INT_8 marketType)
	{
		_marketType = marketType;
	}
	/**
	* Set market session status.
	* @Param session - constant SIGNED_INT_8 number one.
	* @Return nothing.
	*/
	void SessionInfo::setMarketSession(const SIGNED_INT_8 session)
	{
		_marketSession = session;
	}
	/**
	* Get market type.
	* @Return SIGNED_INT_8.
	*/
	SIGNED_INT_8 SessionInfo::getMarketType() const
	{
		return _marketType;
	}
	/**
	* Get market session status.
	* @Return SIGNED_INT_8.
	*/
	SIGNED_INT_8 SessionInfo::getMarketSession() const
	{
		return _marketSession;
	}
	void BEST_ASK_BID::setBestBid(const PRICE _bid)
	{
		bid = _bid;
	}
	void BEST_ASK_BID::setBestAsk(const PRICE _ask)
	{
		ask = _ask;
	}
	void BEST_ASK_BID::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	void BEST_ASK_BID::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	void BEST_ASK_BID::print() const
	{
		FIN_DEVLOG("*********** B_ASK_BID_START ************");
		BroadCastMsgHeader::print();
		//std::cout.precision(2);
		std::cout << "SecId: " << _secId << std::endl;
		std::cout << "B_ASK: " << ask;
		std::cout << "\nB_BID: " << bid;

		FIN_DEVLOG("************ B_ASK_BID_END *************");
	}
	PRICE BEST_ASK_BID::getAskPrice() const
	{
		return ask;
	}
	PRICE BEST_ASK_BID::getBidPrice() const
	{
		return bid;
	}
	std::string BEST_ASK_BID::getSecId() const
	{
		return std::string(_secId);;
	}
}
