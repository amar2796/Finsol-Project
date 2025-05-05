#ifndef FIN_MARKET_DATA_H
#define FIN_MARKET_DATA_H

#include <iostream>
#include <stdio.h>
#include <unordered_map>

#include "common.h"
#include <string.h>


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
	static const SIGNED_INT_8 BroadCastMsgType_Invalid = 0;
	static const SIGNED_INT_8 BroadCastMsgType_MarketData = 1;
	static const SIGNED_INT_8 BroadCastMsgType_MarketNotice = 2;
	static const SIGNED_INT_8 BroadCastMsgType_SymbolNotice = 3;
	static const SIGNED_INT_8 BroadCastMsgType_DPR = 4;
	static const SIGNED_INT_8 BroadCastMsgType_OHLC = 5;
	static const SIGNED_INT_8 BroadCastMsgType_Connectivity = 6;

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
		BroadCastMsgHeader* getBroadCastMsgHeader() const;
		//All Setters

		void setMsgLength(const UNSIGNED_INT_16 msgLength);
		void setMsgType(const SIGNED_INT_8 msgType);
		void setExchange(const UNSIGNED_INT_16 exchange);
		void setUpdateTime(const SIGNED_INT_32 time);
		void print() const;

	private:
		UNSIGNED_INT_16 _msgLength;
		SIGNED_INT_8    _msgType;
		SIGNED_INT_32   _updateTime;
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
		UNSIGNED_INT_16 getQty() const;
		void setPrice(const PRICE price);
		void setQty(UNSIGNED_INT_16 qty);
	private:
		PRICE _price;
		UNSIGNED_INT_16 _qty;
	};

	//Size of MarketData           : 246 Bytes.
	struct MarketData : public BroadCastMsgHeader
	{
	public:
		//constructer with string of secId.
		MarketData(const UNSIGNED_INT_16 exchange, const std::string &secId)
			:BroadCastMsgHeader(sizeof(MarketData), BroadCastMsgType_MarketData, exchange),
			_ltp(0),
			_volume(0)
		{
			memcpy(_secId, secId.c_str(), 25);
			deleteCompleteDepth();
		}

		//constructer with array of secId.
		MarketData(const UNSIGNED_INT_16 exchange, const char *secId)
			:BroadCastMsgHeader(sizeof(MarketData), BroadCastMsgType_MarketData, exchange),
			_ltp(0),
			_volume(0)
		{
			memcpy(_secId, secId, 25);
			deleteCompleteDepth();
		}

		//public Functions

		void applyTrade(const PRICE);
		void deleteBuyDepth(const PRICE);
		void deleteSellDepth(const PRICE);
		void insertBuyDepth(const PRICE, const UNSIGNED_INT_16);
		void insertSellDepth(const PRICE, const UNSIGNED_INT_16);
		void updateBuyDepth(const PRICE, const UNSIGNED_INT_16);
		void updateSellDepth(const PRICE, const UNSIGNED_INT_16);
		bool getPositionForBuyPrice(const PRICE, UNSIGNED_INT_16 &);
		bool getPositionForSellPrice(const PRICE, UNSIGNED_INT_16 &);
		void deleteCompleteDepth();
		void setSecId(const char *);
		void setSecId(const std::string&);
		void setVolume(const SIGNED_INT_64);
		void setLtp(const PRICE);
		void print() const;
		PRICE getLtp() const;
		std::string getSecId() const;
		char * getSecIdPtr() {
			return _secId;
		}
		bool checkBestBidAsk() const;
		SIGNED_INT_64 getVolume() const;
		bool checkCorrectnessBuy() const;
		bool checkCorrectnessSell() const;
		PRICE getBuyDepthPrice(const UNSIGNED_INT_16) const;
		PRICE getSellDepthPrice(const UNSIGNED_INT_16) const;
		UNSIGNED_INT_16 getBuyDepthQty(const UNSIGNED_INT_16) const;
		UNSIGNED_INT_16 getSellDepthQty(const UNSIGNED_INT_16) const;

		Depth & getBuyDepthAt(const UNSIGNED_INT_16 pos)
		{
			return _buyDepth[pos];
		}
		
		Depth & getSellDepthAt(const UNSIGNED_INT_16 pos)
		{
			return _sellDepth[pos];
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
		Depth         _buyDepth[20];
		Depth         _sellDepth[20];
		PRICE         _ltp;
		SIGNED_INT_64 _volume;
		SIGNED_INT_32 _lastTradeTime;
		SIZE _openInterest;
	};

	//Size of OHLC                 : 62 Bytes.
	struct OHLC : public BroadCastMsgHeader
	{
	public:

		OHLC(const UNSIGNED_INT_16 exchange, const std::string &secId)
			:BroadCastMsgHeader(sizeof(OHLC), BroadCastMsgType_OHLC, exchange),
			_openPrice(0),
			_highPrice(0),
			_closePrice(0),
			_lowPrice(0)
		{
			memcpy(_secId, secId.c_str(), 25);
		}
		OHLC(const UNSIGNED_INT_16 exchange, const char *secId)
			:BroadCastMsgHeader(sizeof(OHLC), BroadCastMsgType_OHLC, exchange),
			_openPrice(0),
			_highPrice(0),
			_closePrice(0),
			_lowPrice(0)
		{
			memcpy(_secId, secId, 25);
		}

		void print() const;
		void setOpenPrice(const PRICE);
		void setHighPrice(const PRICE);
		void setClosePrice(const PRICE);
		void setLowPrice(const PRICE);
		void setSecId(const char *);
		void setSecId(const std::string&);

		PRICE getOpenPrice() const;
		PRICE getHighPrice() const;
		PRICE getClosePrice() const;
		PRICE getLowPrice() const;
		std::string getSecId() const;
	private:
		char  _secId[25];
		PRICE _openPrice;
		PRICE _highPrice;
		PRICE _closePrice;
		PRICE _lowPrice;
	};

	//Size of DPR                  : 46 Bytes.
	struct DPR : public BroadCastMsgHeader
	{
	public:
		DPR(const UNSIGNED_INT_16 exchange, const std::string &secId)
			:BroadCastMsgHeader(sizeof(DPR), BroadCastMsgType_DPR, exchange),
			_upperLimit(0),
			_lowerLimit(0)
		{
			memcpy(_secId, secId.c_str(), 25);
		}

		DPR(const UNSIGNED_INT_16 exchange, const char *secId)
			:BroadCastMsgHeader(sizeof(DPR), BroadCastMsgType_DPR, exchange),
			_upperLimit(0),
			_lowerLimit(0)
		{
			memcpy(_secId, secId, 25);
		}

		void print() const;
		void setUpperLimit(const PRICE);
		void setLowerLimit(const PRICE);
		void setSecId(const char *);
		void setSecId(const std::string&);

		PRICE getUpperLimit() const;
		PRICE getLowerLimit() const;
		std::string getSecId() const;

	private:
		char  _secId[25];
		PRICE _upperLimit;
		PRICE _lowerLimit;
	};

	//Size of MarketNotice         : 206 Bytes.
	struct MarketNotice : public BroadCastMsgHeader
	{
	public:
		MarketNotice(const UNSIGNED_INT_16 exchange)
			:BroadCastMsgHeader(sizeof(MarketNotice), BroadCastMsgType_MarketNotice, exchange),
			_noticeProducer(Producer_Unknown)
		{
			memset(_message, 0, 250);
		}

		void print() const;
		void setNoticeProducer(const SIGNED_INT_8);
		void setMessage(const std::string &);
		void setMessage(const char*);

		SIGNED_INT_8 getNoticeProducer() const;
		std::string getMessage() const;

	private:
		SIGNED_INT_8 _noticeProducer;
		char  _message[250];
	};

	//Size of SymbolNotice         : 231 Bytes.
	struct SymbolNotice : public BroadCastMsgHeader
	{

	public:

		SymbolNotice(const UNSIGNED_INT_16 exchange, const std::string &secId)
			:BroadCastMsgHeader(sizeof(SymbolNotice), BroadCastMsgType_SymbolNotice, exchange),
			_noticeProducer(Producer_Gateway)
		{
			memcpy(_secId, secId.c_str(), 25);
			memset(_message, 0, 250);
		}

		SymbolNotice(const UNSIGNED_INT_16 exchange, const char *secId)
			:BroadCastMsgHeader(sizeof(SymbolNotice), BroadCastMsgType_SymbolNotice, exchange),
			_noticeProducer(Producer_Unknown)
		{
			memcpy(_secId, secId, 25);
			memset(_message, 0, 250);
		}

		void print() const;
		void setNoticeProducer(const SIGNED_INT_8);
		void setMessage(const std::string &);
		void setMessage(const char*);
		void setSecId(const char *);
		void setSecId(const std::string&);

		SIGNED_INT_8 getNoticeProducer() const;
		std::string getMessage() const;
		std::string getSecId() const;

	private:
		SIGNED_INT_8 _noticeProducer;
		char         _secId[25];
		char         _message[250];
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

		std::string getModuleTypeStr(SIGNED_INT_8 moduleType) const
		{
			// Possible values of Module Type in Connectivity Status
			switch (moduleType)
			{
				case ModuleType_Invalid: return "ModuleType_Invalid";
				case ModuleType_Order: return "ModuleType_Order";
				case ModuleType_Market: return "ModuleType_Market";
				case ModuleType_Drop_COPY: return "ModuleType_Drop_COPY";
				default :return "ModuleType_Invalid";
			}

		}

		std::string getModuleStateStr(SIGNED_INT_8 moduleConn) const
		{
			switch (moduleConn)
			{
				case ModuleState_Connected   :  return "ModuleState_Connected";
				case ModuleState_LoggedIn    :  return "ModuleState_LoggedIn";
				case ModuleState_LoggedOut   :  return "ModuleState_LoggedOut";
				case ModuleState_Disconnected:  return "ModuleState_Disconnected";
				case ModuleState_Unknown     :  return "ModuleState_Unknown";
				default : return "ModuleState_Unknown";
			}
		}

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

	namespace BROADCAST
	{
		const static int DEPTH_SIZE = 10;
		struct MarketData : public BroadCastMsgHeader
		{
			void copyMD(FIN::MarketData &md)
			{
				this->setUpdateTime(md.getUpdateTime());
				memcpy(_secId, md.getSecIdPtr(), 25);
				this->setMsgType(md.getMsgType());
				this->setMsgLength(sizeof(MarketData));
				this->setExchange(md.getExchange());
				
				this->ltp = md.getLtp();
				this->volume = md.getVolume();
				this->lastTradeTime = md.getLastTradeTime();
				
				for (int i = 0; i < DEPTH_SIZE; i++)
				{
					this->buyDepth[i].price = md.getBuyDepthAt(i).getPrice();
					this->sellDepth[i].price = md.getSellDepthAt(i).getPrice();
					this->sellDepth[i].qty = md.getSellDepthAt(i).getQty();
					this->buyDepth[i].qty = md.getBuyDepthAt(i).getQty();
				}
			}

			MarketData(FIN::MarketData &md):BroadCastMsgHeader(
					sizeof(MarketData), md.getMsgType(), md.getExchange())
			{
				this->setUpdateTime( md.getUpdateTime() );
				memcpy(_secId, md.getSecIdPtr(), 25);
				
				this->ltp = md.getLtp();
				this->volume = md.getVolume();
				this->lastTradeTime = md.getLastTradeTime();
				for (int i = 0; i < DEPTH_SIZE; i++)
				{
					this->buyDepth[i].price = md.getBuyDepthAt(i).getPrice();
					this->sellDepth[i].price = md.getSellDepthAt(i).getPrice();
					this->sellDepth[i].qty = md.getSellDepthAt(i).getQty();
					this->buyDepth[i].qty = md.getBuyDepthAt(i).getQty();
				}

			}

			struct Depth
			{
				Depth();
				void reset();
				void print() const;
				uint16_t  getQty() const;
				bool isEmpty() const;
				PRICE getPrice() const;

				PRICE price;
				uint16_t qty;
				//SIZE noOfOrders;
			};

			
			char _secId[25];
			Depth buyDepth[DEPTH_SIZE];
			Depth sellDepth[DEPTH_SIZE];

			bool getPositionForBuyPrice(PRICE price, short &pos);
			bool getPositionForSellPrice(PRICE price, short &pos);

		public:

			
			PRICE ltp;
			SIZE  volume;
			SIGNED_INT_32 lastTradeTime;
			SIZE openInterest;

			
			MarketData(const uint16_t exchange, const std::string &secId);
			void print() const;

			std::string getSecId() const { std::string id(_secId); return id; }
			void insertBuyDepth(PRICE price, uint16_t);
			void insertSellDepth(PRICE price, uint16_t);

			void updateBuyDepth(PRICE price, uint16_t);
			void updateSellDepth(PRICE price, uint16_t);

			void deleteBuyDepth(PRICE price);
			void deleteSellDepth(PRICE price);

			void applyTrade(PRICE price);

			std::string getString() const;
			void fromString(const std::string &s);
		};

	};


}

#pragma pack ( pop ) 
#endif
