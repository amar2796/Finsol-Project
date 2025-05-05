#ifndef FIN_COMMON_MARKET_DATA
#define FIN_COMMON_MARKET_DATA

#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <string.h>

#include <vector>
#include <sstream> 

#include "constants.h"
#include "defines.h"
#pragma pack ( push, 1) 
namespace FIN
{


	/*!
	  \Base structure
	  \market data structure inherits BroadcastMsgHeader structure
	  \Have basic functions like msgtype,msglength and exchange name
	  */
	  //Size of BroadCastMsgHeader   : 5 Bytes.
	  // Possible values of MsgType in MsgHeader
	static const std::string CSV_SEP = ",";
	static const SIGNED_INT_8 BroadCastMsgType_Invalid = 0;
	static const SIGNED_INT_8 BroadCastMsgType_MarketData = 1;
	static const SIGNED_INT_8 BroadCastMsgType_MarketNotice = 2;
	static const SIGNED_INT_8 BroadCastMsgType_SymbolNotice = 3;
	static const SIGNED_INT_8 BroadCastMsgType_DPR = 4;
	static const SIGNED_INT_8 BroadCastMsgType_OHLC = 5;
	static const SIGNED_INT_8 BroadCastMsgType_Connectivity = 6;
	static const SIGNED_INT_8 BroadCastMsgType_BAB = 7;
	static const SIGNED_INT_8 BroadCastMsgType_Contract_Action = 10;
	static const SIGNED_INT_8 BroadCastMsgType_Market_Resume_Action = 11;

	// Possible values of Notice Producer
	static const SIGNED_INT_8 Producer_Unknown = 0;
	static const SIGNED_INT_8 Producer_Exchange = 1;
	static const SIGNED_INT_8 Producer_Gateway = 2;

	// Possible values of Module Type in Connectivity Status
	static const SIGNED_INT_8 ModuleType_Invalid = 0;
	static const SIGNED_INT_8 ModuleType_Order = 1;
	static const SIGNED_INT_8 ModuleType_Market = 2;
	static const SIGNED_INT_8 ModuleType_Drop_COPY = 3;

	// Possible values of Module State in Connectivity Status
	static const SIGNED_INT_8 ModuleState_Connected = 0;
	static const SIGNED_INT_8 ModuleState_LoggedIn = 1;
	static const SIGNED_INT_8 ModuleState_LoggedOut = 2;
	static const SIGNED_INT_8 ModuleState_Disconnected = 3;
	static const SIGNED_INT_8 ModuleState_Unknown = -1;
	// Possible values of Market Type in Session Info
	static const SIGNED_INT_8 MarketType_Normal = 0;
	static const SIGNED_INT_8 MarketType_Oddlot = 1;
	static const SIGNED_INT_8 MarketType_Spot = 2;
	static const SIGNED_INT_8 MarketType_Auction = 3;
	// Possible values of Market Session in Session Info
	static const SIGNED_INT_8 MarketSession_Preopen = 0;
	static const SIGNED_INT_8 MarketSession_Open = 1;
	static const SIGNED_INT_8 MarketSession_Closed = 2;
	static const SIGNED_INT_8 MarketSession_PreopenClosed = 3;
	static const SIGNED_INT_8 MarketSession_PostClose = 4;
	static const SIGNED_INT_8 MarketSession_Unknown = -1;

	//Size of BroadCastMsgHeader           : 9 Bytes.
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



	/*!
	  \MarketData structure have all the functionality that market data of NSETBT needs
	  \ Have open,close ,High,low,last trade prices and last trade quantity
	  \Have function like insert,update,delete buy depth and sell depth and apply trade
	  \Have depth structure
	  */

	  // Helper Structure for MarketData (10 Bytes)
	struct Depth
	{
	public:
		Depth() = default;

		void print() const;
		bool isEmpty() const;
		PRICE getPrice() const;
		void reset();
		QUANTITY getQty() const;
		void setPrice(const PRICE price);
		void setQty(QUANTITY qty);
	private:
		PRICE _price;
		QUANTITY _qty;
	};

	//Size of BEST_ASK_BID                  : 50 Bytes.
	struct BEST_ASK_BID : public BroadCastMsgHeader
	{
	private:
		char  _secId[25];
		PRICE  bid;
		PRICE  ask;

	public:

		BEST_ASK_BID(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(BEST_ASK_BID), BroadCastMsgType_BAB, exchange),
			bid(0),
			ask(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}

		void reset()
		{
			memset(_secId, 0, 25);
			setUpdateTime(0);
			bid = 0;
			ask = 0;
		}

		void setBestBid(const PRICE);
		void setBestAsk(const PRICE);
		void setSecId(const char*);
		void setSecId(const std::string&);
		void print() const;
		PRICE getAskPrice() const;
		PRICE getBidPrice() const;
		std::string getSecId() const;

	};
	//Size of MarketData           : 182 Bytes.
	struct MarketData : public BroadCastMsgHeader
	{
	public:
		//constructer with string of secId.
		MarketData(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(MarketData), BroadCastMsgType_MarketData, exchange),
			_ltp(0),
			_volume(0),
			_lastTradeTime(0),
			_openInterest(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
			deleteCompleteDepth();
		}

		//constructer with array of secId.
		MarketData(const UNSIGNED_INT_16 exchange, const char* secId)
			:BroadCastMsgHeader(sizeof(MarketData), BroadCastMsgType_MarketData, exchange),
			_ltp(0),
			_volume(0),
			_lastTradeTime(0),
			_openInterest(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId, 25);
			deleteCompleteDepth();
		}

		//public Functions

		void            applyTrade(const PRICE);
		void            deleteBuyDepth(const PRICE);
		void            deleteBuyDepthThru(const PRICE, uint16_t pos);
		void            deleteBuyDepthFrom(const PRICE, uint16_t pos);

		void            deleteSellDepthThru(const PRICE, uint16_t pos);
		void            deleteSellDepthFrom(const PRICE, uint16_t pos);

		void            deleteSellDepth(const PRICE);
		void            insertBuyDepth(const PRICE, const QUANTITY);

		void            insertBuyPriceInPosition(const PRICE, const int32_t);
		void            insertBuyQtyInPosition(const QUANTITY, const int32_t);
		void            insertSellPriceInPosition(const PRICE, const int32_t);
		void            insertSellQtyInPosition(const QUANTITY, const int32_t);

		void            insertSellDepth(const PRICE, const QUANTITY);
		void            updateBuyDepth(const PRICE, const QUANTITY);
		void            updateBuyDepth_Overlay(const PRICE, const QUANTITY, UNSIGNED_INT_16);
		void            updateSellDepth(const PRICE, const QUANTITY);
		void            updateSellDepth_Overlay(const PRICE, const QUANTITY, UNSIGNED_INT_16);
		bool            getPositionForBuyPrice(const PRICE, UNSIGNED_INT_16&);
		bool            getPositionForSellPrice(const PRICE, UNSIGNED_INT_16&);
		void            setSecId(const char*);
		void            setSecId(const std::string&);
		void            setVolume(const VOLUME);
		void            setLtp(const PRICE);
		void            setOpenInterest(const SIGNED_INT_64);
		PRICE           getBuyDepthPrice(const UNSIGNED_INT_16) const;
		PRICE           getSellDepthPrice(const UNSIGNED_INT_16) const;
		UNSIGNED_INT_16 getBuyDepthQty(const UNSIGNED_INT_16) const;
		UNSIGNED_INT_16 getSellDepthQty(const UNSIGNED_INT_16) const;

		void            print() const;
		bool            checkBestBidAsk() const;
		bool            checkCorrectnessBuy() const;
		bool            checkCorrectnessSell() const;
		PRICE           getLtp() const;
		std::string     getSecId() const;
		VOLUME   getVolume() const;
		SIGNED_INT_64   getOpenInterest() const;

		void deleteCompleteDepth();

		void reset()
		{
			memset(_secId, 0, 25);
			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
			{
				_buyDepth[i].reset();
				_sellDepth[i].reset();
			}
			_ltp = 0;
			_volume = 0;
			_openInterest = 0;
			_lastTradeTime = 0;

			setUpdateTime(0);
		}

		void fromCsvString(const std::string& str)
		{
			std::stringstream ss(str);
			std::vector<std::string> result;

			while (ss.good())
			{
				std::string substr;
				getline(ss, substr, ',');
				result.push_back(substr);
			}

			int index = 0;

			setMsgLength(std::atoi(result.at(index++).c_str()));
			setMsgType(std::atoi(result.at(index++).c_str()));
			setUpdateTime(std::atoi(result.at(index++).c_str()));
			setExchange(std::atoi(result.at(index++).c_str()));
			std::string sec = result.at(index++);
			memcpy(_secId, sec.c_str(), sec.size());

			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				_buyDepth[i].setPrice(std::stod(result.at(index++)));
			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				_buyDepth[i].setQty(std::stoi(result.at(index++)));

			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				_sellDepth[i].setPrice(std::stod(result.at(index++)));
			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				_sellDepth[i].setQty(std::stoi(result.at(index++)));

			_ltp = std::stod(result.at(index++));;
			_volume = std::stol(result.at(index++));
			_lastTradeTime = std::stol(result.at(index++));
			_openInterest = std::stol(result.at(index++));

		}

		std::string getCsvString() const
		{
			std::string str("");
			str += std::to_string(getMsgLength()) + CSV_SEP;
			str += std::to_string(getMsgType()) + CSV_SEP;
			str += std::to_string(getUpdateTime()) + CSV_SEP;
			str += std::to_string(getExchange()) + CSV_SEP;
			str += std::string(_secId) + CSV_SEP;

			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				str += std::to_string(_buyDepth[i].getPrice()) + CSV_SEP;

			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				str += std::to_string(_buyDepth[i].getQty()) + CSV_SEP;

			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				str += std::to_string(_sellDepth[i].getPrice()) + CSV_SEP;

			for (int i = 0; i < FIN::FIN_CONST::DEPTH_SIZE; ++i)
				str += std::to_string(_sellDepth[i].getQty()) + CSV_SEP;

			str += std::to_string(_ltp) + CSV_SEP;
			str += std::to_string(_volume) + CSV_SEP;
			str += std::to_string(_lastTradeTime) + CSV_SEP;
			str += std::to_string(_openInterest);
			return str;
		}

		SIGNED_INT_32 getLastTradeTime() const
		{
			return _lastTradeTime;
		}

		void setLastTradeTime(const SIGNED_INT_32 t)
		{
			_lastTradeTime = t;
		}

	private:
		char          _secId[25];  // 25 Byte
		Depth         _buyDepth[FIN::FIN_CONST::DEPTH_SIZE]; // 5
		Depth         _sellDepth[FIN::FIN_CONST::DEPTH_SIZE]; // 5
		PRICE         _ltp;
		VOLUME _volume;
		SIGNED_INT_32  _lastTradeTime;
		SIGNED_INT_64  _openInterest;
	};

	//Size of OHLC                 : 66 Bytes.
	struct OHLC : public BroadCastMsgHeader
	{
	public:

		OHLC(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(OHLC), BroadCastMsgType_OHLC, exchange),
			_openPrice(0),
			_highPrice(0),
			_closePrice(0),
			_lowPrice(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}
		OHLC(const UNSIGNED_INT_16 exchange, const char* secId)
			:BroadCastMsgHeader(sizeof(OHLC), BroadCastMsgType_OHLC, exchange),
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

		void fromCsvString(const std::string& str)
		{
			std::stringstream ss(str);
			std::vector<std::string> result;

			while (ss.good())
			{
				std::string substr;
				getline(ss, substr, ',');
				result.push_back(substr);
			}

			int index = 0;

			setMsgLength(std::atoi(result.at(index++).c_str()));
			setMsgType(std::atoi(result.at(index++).c_str()));
			setUpdateTime(std::atoi(result.at(index++).c_str()));
			setExchange(std::atoi(result.at(index++).c_str()));
			std::string sec = result.at(index++);
			memcpy(_secId, sec.c_str(), sec.size());
			_openPrice = std::stod(result.at(index++));;
			_highPrice = std::stod(result.at(index++));;
			_closePrice = std::stod(result.at(index++));;
			_lowPrice = std::stod(result.at(index++));;
		}

		std::string getCsvString() const
		{
			std::string str("");
			str += std::to_string(getMsgLength()) + CSV_SEP;
			str += std::to_string(getMsgType()) + CSV_SEP;
			str += std::to_string(getUpdateTime()) + CSV_SEP;
			str += std::to_string(getExchange()) + CSV_SEP;
			str += std::string(_secId) + CSV_SEP;

			str += std::to_string(_openPrice) + CSV_SEP;
			str += std::to_string(_highPrice) + CSV_SEP;
			str += std::to_string(_closePrice) + CSV_SEP;
			str += std::to_string(_lowPrice);
			return str;
		}

	private:
		char  _secId[25];
		PRICE _openPrice;
		PRICE _highPrice;
		PRICE _closePrice;
		PRICE _lowPrice;
	};
	
	//Size of DPR                  : 58 Bytes.
	struct DPR : public BroadCastMsgHeader
	{
	public:
		DPR(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(DPR), BroadCastMsgType_DPR, exchange),
			_upperLimit(0),
			_lowerLimit(0),
			_closePrice(0)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}

		void print() const;
		void setUpperLimit(const PRICE);
		void setLowerLimit(const PRICE);
		void setClosePrice(const PRICE);
		void setSecId(const char*);
		void setSecId(const std::string&);

		PRICE getUpperLimit() const;
		PRICE getLowerLimit() const;
		PRICE getClosePrice() const;
		std::string getSecId() const;

	private:
		char  _secId[25];
		PRICE _upperLimit;
		PRICE _lowerLimit;
		PRICE _closePrice;

	};

	//Size of MarketNotice         : 206 Bytes.
	struct MarketNotice : public BroadCastMsgHeader
	{
	public:
		MarketNotice(const UNSIGNED_INT_16 exchange)
			:BroadCastMsgHeader(sizeof(MarketNotice), BroadCastMsgType_MarketNotice, exchange),
			_noticeProducer(Producer_Unknown)
		{
			memset(_message, 0, 200);
		}

		void print() const;
		void setNoticeProducer(const SIGNED_INT_8);
		void setMessage(const std::string&);
		void setMessage(const char*);

		SIGNED_INT_8 getNoticeProducer() const;
		std::string getMessage() const;

	private:
		SIGNED_INT_8 _noticeProducer;
		char  _message[200];
	};

	//Size of SymbolNotice         : 231 Bytes.
	struct SymbolNotice : public BroadCastMsgHeader
	{

	public:

		SymbolNotice(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(SymbolNotice), BroadCastMsgType_SymbolNotice, exchange),
			_noticeProducer(Producer_Gateway)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), 25);
			memset(_message, 0, 200);
		}

		SymbolNotice(const UNSIGNED_INT_16 exchange, const char* secId)
			:BroadCastMsgHeader(sizeof(SymbolNotice), BroadCastMsgType_SymbolNotice, exchange),
			_noticeProducer(Producer_Unknown)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId, 25);
			memset(_message, 0, 200);
		}

		void print() const;
		void setNoticeProducer(const SIGNED_INT_8);
		void setMessage(const std::string&);
		void setMessage(const char*);
		void setSecId(const char*);
		void setSecId(const std::string&);

		SIGNED_INT_8 getNoticeProducer() const;
		std::string getMessage() const;
		std::string getSecId() const;

	private:
		SIGNED_INT_8 _noticeProducer;
		char         _secId[25];
		char         _message[200];
	};

	//Size of ModuleConnectivity   : 7 Bytes.
	struct ModuleConnectivity : public BroadCastMsgHeader
	{
	public:

		ModuleConnectivity(const UNSIGNED_INT_16 exchange)
			:BroadCastMsgHeader(sizeof(ModuleConnectivity), BroadCastMsgType_Connectivity, exchange),
			_moduleType(ModuleType_Invalid),
			_moduleState(ModuleState_Unknown)
		{
		}

		void print() const;
		void setModuleType(const SIGNED_INT_8);
		void setModuleState(const SIGNED_INT_8);

		SIGNED_INT_8 getModuleType() const;
		SIGNED_INT_8 getModuleState() const;

	private:
		SIGNED_INT_8 _moduleType;
		SIGNED_INT_8 _moduleState;
	};

	//Size of SessionInfo          : 7 Bytes.
	struct SessionInfo : public BroadCastMsgHeader
	{
	public:
		SessionInfo(const UNSIGNED_INT_16 exchange)
			:BroadCastMsgHeader(sizeof(SessionInfo), BroadCastMsgType_Connectivity, exchange),
			_marketType(MarketType_Normal),
			_marketSession(MarketSession_Unknown)
		{

		}
		void print() const;
		void setMarketType(const SIGNED_INT_8);
		void setMarketSession(const SIGNED_INT_8);

		SIGNED_INT_8 getMarketType() const;
		SIGNED_INT_8 getMarketSession() const;

	private:
		SIGNED_INT_8 _marketType;
		SIGNED_INT_8 _marketSession;
	};

	//Size of CONTRACTACTION                  : 35 Bytes.
	struct CONTRACTACTION : public BroadCastMsgHeader
	{
	public:
		CONTRACTACTION(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(CONTRACTACTION), BroadCastMsgType_Contract_Action, exchange),
			_action(1)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}

		void print() const;
		void setAction(const SIGNED_INT_8);
		SIGNED_INT_8 getAction() const;

	private:
		char  _secId[25];
		SIGNED_INT_8 _action;					// 1 = Active, 0 = Suspend

	};

	//Size of MARKETRESUMEACTION                  : 10 Bytes.
	struct MARKETRESUMEACTION : public BroadCastMsgHeader
	{
	public:
		MARKETRESUMEACTION(const UNSIGNED_INT_16 exchange, const std::string& secId)
			:BroadCastMsgHeader(sizeof(MARKETRESUMEACTION), BroadCastMsgType_Market_Resume_Action, exchange),
			_action(1)
		{
			memset(_secId, 0, 25);
			memcpy(_secId, secId.c_str(), secId.size());
		}

		void print() const;
		void setAction(const SIGNED_INT_8);

		SIGNED_INT_8 getAction() const;

	private:
		char  _secId[25];
		SIGNED_INT_8 _action;					// 1 = Active, 0 = Suspend

	};

}

#pragma pack ( pop ) 
#endif
