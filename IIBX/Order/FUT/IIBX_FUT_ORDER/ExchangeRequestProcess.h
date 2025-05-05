#pragma once
#ifndef FIN_EXCHANGE_REQUEST_PROCESS_H
#define FIN_EXCHANGE_REQUEST_PROCESS_H
#include <iostream>
#include "constants.h"
#include <unordered_map>
#include "Nanolog.hpp"
#include "messages.h"
#include "TransCodes.h"
#include "fixStringParser.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include "OrderBook.h"

namespace FIN
{
	class ExchangeRequestProcess
	{
		std::string transCode;
		std::string messageName;
		std::string traderId;
		std::string memberId;
		std::string password;
	public:	
		ExchangeRequestProcess();
		std::string getMessageHeaderForLogon(std::string msgType, int seqNum, std::string cMemCode, std::string tMemCode, std::string dCode, std::string ctclTCode);
		std::string getMessageHeaderOnly(std::string msgType, int seqNum, std::string cMemCode, std::string tMemCode, std::string dCode, std::string ctclTCode);
		std::string getMessageHeader(std::string msgType, int seqNum, std::string tMemCode, std::string dCode, std::string ctclTCode);
		
		std::string getLoginRequest(std::string&, std::string&);
		std::string getNewOrderReq(std::string&, int seq);		
		std::string getModifyOrderReq(std::string&, int seq);		
		std::string getCancelOrderReq(std::string&, int seq);
		bool processPendingOrderHistory(std::string msg, OmsResponseOrder& omsResp);
		bool processReturnOrderHistory(std::string msg, OmsResponseOrder& omsResp);
		bool processReturnStopLossOrder(std::string msg, OmsResponseOrder& omsResp);
		bool processTradeOrderHistory(std::string msg, OmsResponseOrder& omsResp);
		bool processStopLossOrderHistory(std::string msg, OmsResponseOrder& omsResp);
		bool processMarketPictureBBOHistory(std::string msg, OmsResponseOrder& omsResp, std::unordered_map<std::string, int>& contractClosePrice);

		std::string getCurrentDate();
		std::string getCurrentDateAndTime();
		FixStringParser _fixStringParser;
		std::string expiry = "19700101";
	protected:
		int priceDevider = 10000;
	
		std::unordered_map<int, std::pair<int, int>> storeSeqNumber;	//<seqNum, <ClOrdId, Price>>
	};

}




#endif