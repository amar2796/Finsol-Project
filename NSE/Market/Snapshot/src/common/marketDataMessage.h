#ifndef FIN_COMMON_MARKET_DATA
#define FIN_COMMON_MARKET_DATA

#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <string.h>
#include "defines.h"
#include "constants.h"
#pragma pack ( push, 1) 
namespace FIN
{
	typedef int16_t INT_16;

	static const SIGNED_INT_8 BroadCastMsgType_BAB = 9;
	/*!
	\Base structure
	\market data structure inherits BroadcastMsgHeader structure
	\Have basic functions like msgtype,msglength and exchange name
	*/
	struct BroadCastMsgHeader
	{
	public:

		BroadCastMsgHeader(const UNSIGNED_INT_16 msgLength, const SIGNED_INT_8 msgType, const UNSIGNED_INT_16 exchange)
			:_msgLength(msgLength),
			_msgType(msgType),
			_exchange(exchange),
			_updateTime(0)
		{

		}

		// All Getters
		UNSIGNED_INT_16 getMsgLength() const;
		SIGNED_INT_8 getMsgType() const;
		UNSIGNED_INT_16 getExchange() const;
		SIGNED_INT_32 getUpdateTime() const;
		BroadCastMsgHeader& getBroadCastMsgHeader();

		void setMsgLength(const UNSIGNED_INT_16 msgLength);
		void setMsgType(const SIGNED_INT_8 msgType);
		void setExchange(const UNSIGNED_INT_16 exchange);
		void setUpdateTime(const SIGNED_INT_32 updateTime);

		void print() const;
	private:
		UNSIGNED_INT_16 _msgLength;
		SIGNED_INT_8    _msgType;
		SIGNED_INT_32  _updateTime;
		UNSIGNED_INT_16 _exchange;

	};


	struct Depth
	{
	public:
		Depth() = default;


		bool isEmpty() const;
		PRICE getPrice() const;
		void reset();
		UNSIGNED_INT_32 getQty() const;
		void setPrice_qty(const PRICE price, UNSIGNED_INT_32 qty);

	private:
		PRICE _price;
		UNSIGNED_INT_32 _qty;
	};

	/*!
	 \MarketData structure have all the functionality that market data of NSETBT needs
	 \ Have open,close ,High,low,last trade prices and last trade quantity
	 \Have function like insert,update,delete buy depth and sell depth and apply trade
	 \Have depth structure
	*/
	struct MarketData : public BroadCastMsgHeader
	{


		char          _secId[25];  // 25 Byte
		Depth         _buyDepth[FIN::FIN_CONST::DEPTH_SIZE]; // 5
		Depth         _sellDepth[FIN::FIN_CONST::DEPTH_SIZE]; // 5
		PRICE         _ltp;
		SIGNED_INT_64 _volume;
		SIGNED_INT_32  _lastTradeTime;
		SIGNED_INT_64  _openInterest;


		MarketData(const uint16_t exchange, const std::string& secId) :BroadCastMsgHeader(sizeof(MarketData), 1, exchange), _ltp(0),
			_volume(0),
			_lastTradeTime(0),
			_openInterest(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
			deleteCompleteDepth();
		}


		std::string getSecId() const { std::string id(_secId); return id; }

		void applyTrade(PRICE price);
		void setVolume(SIGNED_INT_64 total_volume);
		void deleteCompleteDepth();
		std::string getString() const;
		void fromString(const std::string& s);

		void print() const;


	};
	struct OHLC : public BroadCastMsgHeader
	{
	public:

		OHLC(const uint16_t exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(OHLC), 5, exchange),
			_openPrice(0),
			_highPrice(0),
			_closePrice(0),
			_lowPrice(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}
		OHLC(const uint16_t exchange, const char* secId)
			:BroadCastMsgHeader(sizeof(OHLC), 5, exchange),
			_openPrice(0),
			_highPrice(0),
			_closePrice(0),
			_lowPrice(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId, 25);
		}

		void print() const;
		void setOpenPrice(const PRICE);
		void setHighPrice(const PRICE);
		void setClosePrice(const PRICE);
		void setLowPrice(const PRICE);
		void setSecId(const char*);
		void setSecId(const std::string&);

		PRICE getOpenPrice() const;
		PRICE getHighPrice() const;
		PRICE getClosePrice() const;
		PRICE getLowPrice() const;
		std::string getSecId() const;

		void reset()
		{
			memset(_secId, 0, 25);
			setUpdateTime(0);
			_openPrice = 0;
			_highPrice = 0;
			_closePrice = 0;
			_lowPrice = 0;
		}





	private:
		char  _secId[25];
		PRICE _openPrice;
		PRICE _highPrice;
		PRICE _closePrice;
		PRICE _lowPrice;
	};
	struct BEST_ASK_BID : public BroadCastMsgHeader
	{
	private:
		char _secId[25];
		PRICE _bid;
		INT_32 _bid_qty;
		PRICE _ask;
		INT_32 _ask_qty;
		PRICE _ltp;
		SIGNED_INT_64 _volume;
		SIGNED_INT_32 _lastTradeTime;
		SIGNED_INT_64  _openInterest;
		int8_t status;

	public:

		BEST_ASK_BID(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(BEST_ASK_BID), 1, exchange),
			_bid(0), _ask(0),
			_bid_qty(0), _ask_qty(0), _ltp(0), _volume(0), _lastTradeTime(0), status(1)

		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}

		void reset()
		{
			memset(_secId, 0, 25);
			setUpdateTime(0);
			_bid = 0;
			_ask = 0;
			_bid_qty = 0;
			_ask_qty = 0;
			_ltp = 0;
			_volume = 0; _lastTradeTime = 0;
		}

		void setBestBid(const PRICE, const INT_32);
		void setBestAsk(const PRICE, const INT_32);
		void setOpeninterest(const long);
		void setSecId(const char*);
		void setSecId(const std::string&);
		void print() const;
		PRICE getAskPrice() const;
		PRICE getBidPrice() const;
		std::string getSecId() const;
		void applyTrade(PRICE price, SIGNED_INT_32 ltt);
		void setVolume(SIGNED_INT_64 total_volume);
		void setBIDPrice_qty(const PRICE price, INT_32 qty);
		void setAskPrice_qty(const PRICE price, INT_32 qty);
	};


	struct Depth_D
	{
	public:
		Depth_D() = default;


		bool isEmpty() const;
		PRICE_D getPrice() const;
		void reset();
		UNSIGNED_INT_32 getQty() const;
		void setPrice_qty(const PRICE_D price, UNSIGNED_INT_32 qty);

	private:
		PRICE_D _price;
		UNSIGNED_INT_32 _qty;
	};

	struct MarketData_D : public BroadCastMsgHeader
	{


		char          _secId[25];  // 25 Byte
		Depth_D         _buyDepth[FIN::FIN_CONST::DEPTH_SIZE]; // 5
		Depth_D         _sellDepth[FIN::FIN_CONST::DEPTH_SIZE]; // 5
		PRICE_D         _ltp;
		SIGNED_INT_64 _volume;
		SIGNED_INT_32  _lastTradeTime;
		SIGNED_INT_64  _openInterest;


		MarketData_D(const uint16_t exchange, const std::string& secId) :BroadCastMsgHeader(sizeof(MarketData_D), 1, exchange), _ltp(0),
			_volume(0),
			_lastTradeTime(0),
			_openInterest(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
			deleteCompleteDepth();
		}


		std::string getSecId() const { std::string id(_secId); return id; }

		void applyTrade(PRICE_D price);
		void setLtp(const PRICE_D price);
		void setVolume(SIGNED_INT_64 total_volume);
		void deleteCompleteDepth();
		std::string getString() const;
		void fromString(const std::string& s);

		void print() const;


	};
	struct OHLC_D : public BroadCastMsgHeader
	{
	public:

		OHLC_D(const uint16_t exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(OHLC_D), 5, exchange),
			_openPrice(0),
			_highPrice(0),
			_closePrice(0),
			_lowPrice(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}


		void print() const;
		void setOpenPrice(const PRICE_D);
		void setHighPrice(const PRICE_D);
		void setClosePrice(const PRICE_D);
		void setLowPrice(const PRICE_D);
		void setSecId(const char*);
		void setSecId(const std::string&);

		PRICE_D getOpenPrice() const;
		PRICE_D getHighPrice() const;
		PRICE_D getClosePrice() const;
		PRICE_D getLowPrice() const;
		std::string getSecId() const;

		void reset()
		{
			memset(_secId, 0, 25);
			setUpdateTime(0);
			_openPrice = 0;
			_highPrice = 0;
			_closePrice = 0;
			_lowPrice = 0;
		}





	private:
		char  _secId[25];
		PRICE_D _openPrice;
		PRICE_D _highPrice;
		PRICE_D _closePrice;
		PRICE_D _lowPrice;
	};
	struct BEST_ASK_BID_D : public BroadCastMsgHeader
	{
	private:
		char _secId[25];
		PRICE_D _bid;
		INT_32 _bid_qty;
		PRICE_D _ask;
		INT_32 _ask_qty;
		PRICE_D _ltp;
		SIGNED_INT_64 _volume;
		SIGNED_INT_32 _lastTradeTime;
		SIGNED_INT_64  _openInterest;
		int8_t status;

	public:

		BEST_ASK_BID_D(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(BEST_ASK_BID_D), BroadCastMsgType_BAB, exchange),
			_bid(0), _ask(0),
			_bid_qty(0), _ask_qty(0), _ltp(0), _volume(0), _lastTradeTime(0), status(1), _openInterest(0)

		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}

		void reset()
		{
			memset(_secId, 0, 25);
			setUpdateTime(0);
			_bid = 0;
			_ask = 0;
			_bid_qty = 0;
			_ask_qty = 0;
			_ltp = 0;
			_volume = 0; _lastTradeTime = 0;
		}

		void setBestBid(const PRICE_D, const INT_32);
		void setBestAsk(const PRICE_D, const INT_32);
		void setOpeninterest(const long);

		void setSecId(const char*);
		void setSecId(const std::string&);
		void print() const;
		PRICE_D getAskPrice() const;
		PRICE_D getBidPrice() const;
		std::string getSecId() const;
		void applyTrade(PRICE_D price, SIGNED_INT_32 ltt);
		void setVolume(SIGNED_INT_64 total_volume);
		void setBIDPrice_qty(const PRICE_D price, INT_32 qty);
		void setAskPrice_qty(const PRICE_D price, INT_32 qty);
	};

}

#pragma pack ( pop ) 
#endif
