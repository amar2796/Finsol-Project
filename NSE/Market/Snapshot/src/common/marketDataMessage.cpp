#include "marketDataMessage.h"
#include <iomanip>
#include<string>


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




	/*!
	\public member function of MarketData
	\shows values of market data
	*/
	void MarketData::print() const
	{
		std::cout << "***** MARKET_DATA_START ******" << std::endl;
		BroadCastMsgHeader::print();

		std::cout << "SecId : " << _secId
			<< std::endl << "LTP		    : " << std::to_string(_ltp)
			//	<< std::endl << "LTQ   : " << ltq    
			<< std::endl << "Volume			: " << _volume


			//<< std::endl << "HIGHPRICERANGE : " << std::to_string(HighPriceRange)
			<< std::endl;
		for (int i = 0; i < FIN::FIN_CONST::DEPTH; ++i)
		{
			std::cout
				<< std::left << std::setw(15)
				<< _buyDepth[i].getQty()
				<< std::left << std::setw(15)
				<< std::to_string(_buyDepth[i].getPrice())
				<< std::left << std::setw(15)
				<< " | "
				<< std::left << std::setw(15)
				<< std::to_string(_sellDepth[i].getPrice())
				<< std::left << std::setw(15)
				<< _sellDepth[i].getQty()
				<< std::left << std::setw(15)
				<< std::endl;
		}
		std::cout << "****** MARKET_DATA_END *******" << std::endl;
	}




	void MarketData::deleteCompleteDepth()
	{
		for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; i++)
		{
			_buyDepth[i].reset();
			_sellDepth[i].reset();
		}
	}

	void MarketData::setVolume(SIGNED_INT_64 total_volume)
	{
		_volume = total_volume;
	}

	std::string MarketData::getString() const
	{
		std::string mdAsString((char*)this, sizeof(MarketData));
		return mdAsString;
	}

	void MarketData::fromString(const std::string& s)
	{
		size_t size = sizeof(*this);
		if (s.size() <= size)
			size = s.size();
		memcpy(this, s.c_str(), size);
	}

	/*!
	\public member function of marketData
	\insert new price and quantity into buy depth.
	*/

	/*!
	\public member function of market data
	\update last trade price and quantity if any trade happenend.
	*/
	void MarketData::applyTrade(PRICE price)
	{
		_ltp = price / FIN_CONST::NSE_COM_MULTIPLIER;
		//ltq = qty ;
	}




	/*!
	\public member function of public structure Depth of MarketData
	\reset Price,quantity and noOfOrfers equals to zero.
	*/
	void Depth::reset()
	{
		_price = 0;
		_qty = 0;
		//noOfOrders = 0 ;
	}

	/*!
	\public member function of public structure Depth of MarketData
	\returns true if price and quantity and no of orders equals to 0 otherwise returns false.
	*/
	bool Depth::isEmpty() const
	{
		return (_price == 0 && _qty == 0); // && noOfOrders == 0 ) ;
	}

	PRICE Depth::getPrice() const
	{
		return _price;
	}

	UNSIGNED_INT_32 Depth::getQty() const
	{
		return _qty;
	}
	void Depth::setPrice_qty(const PRICE price, UNSIGNED_INT_32 qty)
	{
		_price = price / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_qty = qty;
	}

	/**
	*Print OHLC .
	* @Return nothing.
	*/
	void  OHLC::print() const
	{
		std::cout << "*********** OHLC_START ************";
		BroadCastMsgHeader::print();
		//std::cout.precision(2);
		std::cout << "SecId: " << _secId << std::endl;
		std::cout << "OpenPrice: " << _openPrice;
		std::cout << "\nHighPrice: " << _highPrice;
		std::cout << "\nClosePrice: " << _closePrice;
		std::cout << "\nLowPrice: " << _lowPrice << std::endl;
		std::cout << "************ OHLC_END *************";
	}
	/**
	* Set open price of respective security.
	*@Param open - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setOpenPrice(const PRICE open)
	{
		_openPrice = open / FIN_CONST::NSE_COM_MULTIPLIER;;
	}
	/**
	* Set high price of respective security.
	*@Param high - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setHighPrice(const PRICE high)
	{
		_highPrice = high / FIN_CONST::NSE_COM_MULTIPLIER;;
	}
	/**
	* Set close price of respective security.
	*@Param close - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setClosePrice(const PRICE close)
	{
		_closePrice = close / FIN_CONST::NSE_COM_MULTIPLIER;;
	}
	/**
	* Set low price of respective security.
	*@Param low - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC::setLowPrice(const PRICE low)
	{
		_lowPrice = low / FIN_CONST::NSE_COM_MULTIPLIER;;
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

	void BEST_ASK_BID::setBestBid(const PRICE _bidPrice, const INT_32 _bidQty)
	{
		_bid = _bidPrice;
		_bid_qty = _bidQty;
	}
	void BEST_ASK_BID::setBestAsk(const PRICE _askPrice, const INT_32 _askQty)
	{
		_ask = _askPrice;
		_ask_qty = _askQty;
	}
	void BEST_ASK_BID::setOpeninterest(const long openInterest)
	{
		_openInterest = openInterest;
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
		DEVLOG("*********** B_ASK_BID_START ************");
		BroadCastMsgHeader::print();
		//std::cout.precision(2);
		std::cout << "SecId: " << _secId << std::endl;
		std::cout << "B_ASK: " << _ask << " :" << _ask_qty << " ";
		std::cout << "\nB_BID: " << _bid << " :" << _bid_qty << " ";
		std::cout << "\nB_BID: " << _ltp;
		std::cout << "\nB_BID: " << _volume;
		std::cout << "\nB_BID: " << _lastTradeTime << std::endl;

		DEVLOG("************ B_ASK_BID_END *************");
	}
	PRICE BEST_ASK_BID::getAskPrice() const
	{
		return _ask;
	}
	PRICE BEST_ASK_BID::getBidPrice() const
	{
		return _bid;
	}
	std::string BEST_ASK_BID::getSecId() const
	{
		return std::string(_secId);;
	}
	void BEST_ASK_BID::applyTrade(PRICE price, SIGNED_INT_32 ltt)
	{
		_ltp = price / FIN_CONST::NSE_COM_MULTIPLIER;
		_lastTradeTime = ltt;
		//ltq = qty ;
	}
	void BEST_ASK_BID::setVolume(SIGNED_INT_64 total_volume)
	{
		_volume = total_volume;
	}
	void BEST_ASK_BID::setBIDPrice_qty(const PRICE price, INT_32 qty)
	{
		_bid = price / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_bid_qty = qty;
	}
	void BEST_ASK_BID::setAskPrice_qty(const PRICE price, INT_32 qty)
	{
		_ask = price / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_ask_qty = qty;
	}





	// Structure for the double price 



		/*!
		\public member function of MarketData
		\shows values of market data
		*/
	void MarketData_D::print() const
	{
		std::cout << "***** MARKET_DATA_START ******" << std::endl;
		BroadCastMsgHeader::print();

		std::cout << "SecId : " << _secId
			<< std::endl << "LTP		    : " << std::to_string(_ltp)
			<< std::endl << "OpenINterest   : " << _openInterest
			<< std::endl << "Volume			: " << _volume


			//<< std::endl << "HIGHPRICERANGE : " << std::to_string(HighPriceRange)
			<< std::endl;
		for (int i = 0; i < FIN::FIN_CONST::DEPTH; ++i)
		{
			std::cout
				<< std::left << std::setw(15)
				<< _buyDepth[i].getQty()
				<< std::left << std::setw(15)
				<< std::to_string(_buyDepth[i].getPrice())
				<< std::left << std::setw(15)
				<< " | "
				<< std::left << std::setw(15)
				<< std::to_string(_sellDepth[i].getPrice())
				<< std::left << std::setw(15)
				<< _sellDepth[i].getQty()
				<< std::left << std::setw(15)
				<< std::endl;
		}
		std::cout << "****** MARKET_DATA_END *******" << std::endl;
	}




	void MarketData_D::deleteCompleteDepth()
	{
		for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; i++)
		{
			_buyDepth[i].reset();
			_sellDepth[i].reset();
		}
	}

	void MarketData_D::setVolume(SIGNED_INT_64 total_volume)
	{
		_volume = total_volume;
	}
	void MarketData_D::setLtp(const PRICE_D price)
	{
		_ltp = price / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
	}

	std::string MarketData_D::getString() const
	{
		std::string mdAsString((char*)this, sizeof(MarketData));
		return mdAsString;
	}

	void MarketData_D::fromString(const std::string& s)
	{
		size_t size = sizeof(*this);
		if (s.size() <= size)
			size = s.size();
		memcpy(this, s.c_str(), size);
	}

	/*!
	\public member function of marketData
	\insert new price and quantity into buy depth.
	*/

	/*!
	\public member function of market data
	\update last trade price and quantity if any trade happenend.
	*/
	void MarketData_D::applyTrade(PRICE_D price)
	{
		_ltp = price / FIN_CONST::NSE_COM_MULTIPLIER;
		//ltq = qty ;
	}
	/*!
	\public member function of public structure Depth of MarketData
	\reset Price,quantity and noOfOrfers equals to zero.
	*/
	void Depth_D::reset()
	{
		_price = 0;
		_qty = 0;
		//noOfOrders = 0 ;
	}

	/*!
	\public member function of public structure Depth of MarketData
	\returns true if price and quantity and no of orders equals to 0 otherwise returns false.
	*/
	bool Depth_D::isEmpty() const
	{
		return (_price == 0 && _qty == 0); // && noOfOrders == 0 ) ;
	}

	PRICE_D Depth_D::getPrice() const
	{
		return _price;
	}

	UNSIGNED_INT_32 Depth_D::getQty() const
	{
		return _qty;
	}
	void Depth_D::setPrice_qty(const PRICE_D price, UNSIGNED_INT_32 qty)
	{
		_price = price / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_qty = qty;
	}

	/**
	*Print OHLC .
	* @Return nothing.
	*/
	void  OHLC_D::print() const
	{
		std::cout << "*********** OHLC_START ************";
		BroadCastMsgHeader::print();
		//std::cout.precision(2);
		std::cout << "SecId: " << _secId << std::endl;
		std::cout << "OpenPrice: " << _openPrice;
		std::cout << "\nHighPrice: " << _highPrice;
		std::cout << "\nClosePrice: " << _closePrice;
		std::cout << "\nLowPrice: " << _lowPrice << std::endl;
		std::cout << "************ OHLC_END *************";
	}
	/**
	* Set open price of respective security.
	*@Param open - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC_D::setOpenPrice(const PRICE_D open)
	{
		_openPrice = open / FIN_CONST::NSE_COM_MULTIPLIER;;
	}
	/**
	* Set high price of respective security.
	*@Param high - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC_D::setHighPrice(const PRICE_D high)
	{
		_highPrice = high / FIN_CONST::NSE_COM_MULTIPLIER;;
	}
	/**
	* Set close price of respective security.
	*@Param close - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC_D::setClosePrice(const PRICE_D close)
	{
		_closePrice = close / FIN_CONST::NSE_COM_MULTIPLIER;
	}
	/**
	* Set low price of respective security.
	*@Param low - constant PRICE number one
	* @Return nothing.
	*/
	void  OHLC_D::setLowPrice(const PRICE_D low)
	{
		_lowPrice = low / FIN_CONST::NSE_COM_MULTIPLIER;;
	}
	/**
* Set security Id.
* @Param  secId - constant character pointer number one .
* @Return nothing.
*/
	void  OHLC_D::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	/**
	* Set security Id.
	* @Param  secId - constant string number one .
	* @Return nothing.
	*/

	void  OHLC_D::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}

	/**
	* Get open price of respective security.
	* @Return PRICE.
	*/
	PRICE_D OHLC_D::getOpenPrice() const
	{
		return _openPrice;
	}
	/**
	* Get high price of respective security.
	* @Return PRICE.
	*/
	PRICE_D OHLC_D::getHighPrice() const
	{
		return _highPrice;
	}
	/**
	* Get close price of respective security.
	* @Return PRICE.
	*/
	PRICE_D OHLC_D::getClosePrice() const
	{
		return _closePrice;
	}
	/**
	* Get low price of respective security.
	* @Return PRICE.
	*/
	PRICE_D OHLC_D::getLowPrice() const
	{
		return _lowPrice;
	}
	/**
	* Get security Id.
	* @Return string.
	*/
	std::string OHLC_D::getSecId() const
	{
		return std::string(_secId);
	}

	void BEST_ASK_BID_D::setBestBid(const PRICE_D _bidPrice, const INT_32 _bidQty)
	{
		_bid = _bidPrice / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_bid_qty = _bidQty;
	}

	void BEST_ASK_BID_D::setBestAsk(const PRICE_D _askPrice, const INT_32 _askQty)
	{
		_ask = _askPrice / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_ask_qty = _askQty;
	}

	void BEST_ASK_BID_D::setOpeninterest(const long openInterest)
	{
		_openInterest = openInterest;
	}

	void BEST_ASK_BID_D::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);

	}

	void BEST_ASK_BID_D::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);

	}

	void BEST_ASK_BID_D::print() const
	{
		DEVLOG("*********** B_ASK_BID_START ************");
		BroadCastMsgHeader::print();
		//std::cout.precision(2);
		std::cout << "SecId: " << _secId << std::endl;
		std::cout << "B_ASK: " << _ask << " :" << _ask_qty << " ";
		std::cout << "\nB_BID: " << _bid << " :" << _bid_qty << " ";
		std::cout << "\nLTP: " << _ltp;
		std::cout << "\nVOL: " << _volume;
		std::cout << "\nOI: " << _openInterest;
		std::cout << "\nLTT: " << _lastTradeTime << std::endl;

		DEVLOG("************ B_ASK_BID_END *************");
	}

	PRICE_D BEST_ASK_BID_D::getAskPrice() const
	{
		return _ask;

	}

	PRICE_D BEST_ASK_BID_D::getBidPrice() const
	{
		return _bid;

	}

	std::string BEST_ASK_BID_D::getSecId() const
	{
		return std::string(_secId);

	}

	void BEST_ASK_BID_D::applyTrade(PRICE_D price, SIGNED_INT_32 ltt)
	{
		_ltp = price / FIN_CONST::NSE_COM_MULTIPLIER;
		_lastTradeTime = ltt;

	}

	void BEST_ASK_BID_D::setVolume(SIGNED_INT_64 total_volume)
	{
		_volume = total_volume;
	}

	void BEST_ASK_BID_D::setBIDPrice_qty(const PRICE_D price, INT_32 qty)
	{
		_bid = price / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_bid_qty = qty;
	}
	void BEST_ASK_BID_D::setAskPrice_qty(const PRICE_D price, INT_32 qty)
	{
		_ask = price / FIN::FIN_CONST::NSE_COM_MULTIPLIER;
		_ask_qty = qty;
	}
}
