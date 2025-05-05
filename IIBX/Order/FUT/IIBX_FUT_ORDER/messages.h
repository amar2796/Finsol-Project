#pragma once
#ifndef FIN_MESSAGES_H
#define FIN_MESSAGES_H
#include "defines.h"
#include <sstream>
#include <iomanip> // for std::setw
#include <cstring> // for std::strlen
#include <string>
namespace FIN 
{
	struct IECCONNECTION {
		char apiName[20];         // API Name (20 characters)
		char apiVersion[10];      // API Version No (10 characters)
		char messageType[30];     // Message Type (30 characters)
		int responseCode;         // Response Code (integer)
		char responseDetails[100];// Response Details (100 characters)

		// Constructor for default initialization
		IECCONNECTION()
			: responseCode(0) // Default value for integer
		{
			memset(apiName, 0, sizeof(apiName));
			memset(apiVersion, 0, sizeof(apiVersion));
			memset(messageType, 0, sizeof(messageType));
			memset(responseDetails, 0, sizeof(responseDetails));

			// Set default values for compulsory fields
			strcpy(apiName, "IIBX_EXCH_CTCL");
			strcpy(apiVersion, "1.0.1");
		}
	};
	struct HEADER
	{
		char apiName[20];                // API Name (20 characters)
		char apiVersion[10];             // API Version No (10 characters)
		char messageType[30];            // Message Type (30 characters)
		int messageEcho;                 // Message Echo (integer)
		char clearingMemberCode[10];     // Clearing Member Code (10 characters)
		char tradingMemberCode[10];      // Trading Member Code (10 characters)
		char dealerCode[10];             // Dealer Code (10 characters)
		char ctclTerminalId[20];         // CTCL Terminal Id (20 characters, optional)

		// Constructor for easy initialization
		HEADER()
			: messageEcho(0) // Default value for integer
		{
			memset(apiName, 0, sizeof(apiName));
			memset(apiVersion, 0, sizeof(apiVersion));
			memset(messageType, 0, sizeof(messageType));
			memset(clearingMemberCode, 0, sizeof(clearingMemberCode));
			memset(tradingMemberCode, 0, sizeof(tradingMemberCode));
			memset(dealerCode, 0, sizeof(dealerCode));
			memset(ctclTerminalId, 0, sizeof(ctclTerminalId));

			// Set default values for apiName and apiVersion
			strncpy(apiName, "IIBX_EXCH_CTCL", sizeof(apiName) - 1);
			strncpy(apiVersion, "1.0.1", sizeof(apiVersion) - 1);
		}
	};

	struct HeaderResponse {
		char apiName[20];           // API Name (20 characters)
		char apiVersion[10];        // API Version No (10 characters)
		char messageType[30];       // Message Type (30 characters)
		int messageEcho;            // Message Echo (integer)
		char clearingMemberCode[10];// Clearing Member Code (10 characters, optional)
		char tradingMemberCode[10]; // Trading Member Code (10 characters)
		char dealerCode[10];        // Dealer Code (10 characters)
		char ctclTerminalId[20];    // CTCL Terminal Id (20 characters)
		int responseCode;           // Response Code (integer)
		char responseDetails[100];  // Response Details (100 characters)
		int checkMsgType;

		// Default constructor for initialization
		HeaderResponse()
			: messageEcho(0), responseCode(0), checkMsgType(0) // Default values for integers
		{
			memset(apiName, 0, sizeof(apiName));
			memset(apiVersion, 0, sizeof(apiVersion));
			memset(messageType, 0, sizeof(messageType));
			memset(clearingMemberCode, 0, sizeof(clearingMemberCode));
			memset(tradingMemberCode, 0, sizeof(tradingMemberCode));
			memset(dealerCode, 0, sizeof(dealerCode));
			memset(ctclTerminalId, 0, sizeof(ctclTerminalId));
			memset(responseDetails, 0, sizeof(responseDetails));

			// Set default values for compulsory fields
			strcpy(apiName, "IIBX_EXCH_CTCL");
			strcpy(apiVersion, "1.0.1");
		}
	};

	struct LOOKUPSERVICERESPONSE
	{
		int messageType;              
		int messageTag;               
		int responseCode;             
		char responseDetails[100];    
		char iecServiceIp[15];        
		int iecServicePort;           
		char multicastGroupIp[15];    
		int multicastPort;            
		char encryptionKeySegment[16]; 

		std::string generateString() const
		{
			std::ostringstream result;

			result << messageType
				<< messageTag
				<< responseCode
				<< responseDetails
				<< iecServiceIp
				<< iecServicePort
				<< multicastGroupIp
				<< multicastPort
				<< encryptionKeySegment;

			return result.str();
		}
	};

	struct IECCONNECTIONRESPONSE
	{
		char apiName[20];
		char apiVersionNumber[10];
		char msgType[30];
		int responseCode;
		char responseDetails[100];
	};

	struct ResponseMessageHeader
	{
		char APIName[20];
		char APIVersionNumber[10];
		char MsgType[30];
		int seqNumber;			//seqNumber=MessageEcho
		char clearingMemberCode[10];
		char tradngMemberCode[10];
		char dealerCode[10];
		char ctclTerminalID[20];
		int responseCode;
		char responseDetails[100];				//headerResponse size 218
	};
	/*
	struct Header
	{		
		char messageName[40];
		char transCode[10];
		Header()
		{
			reset();
		}
		void reset()
		{			
			memset(messageName, ' ', 40);
			memset(transCode, ' ', 10);
		}
		std::string getString() {
			std::string str;			
			str += messageName;
			str += transCode;
		}		
	};
	struct LoginRequest:public Header //7310
	{
		//Default value of messageName="Memberlogin"
		char memberCode[10];
		char loginId[10];
		char password[10];
		LoginRequest() {
			reset();
		}
		void reset() {
			memset(memberCode, ' ', 10);
			memset(loginId, ' ', 10);
			memset(password, ' ', 10);
		}
		std::string getString() {
			std::string str;
			str += memberCode;
			str += loginId;
			str += password;
		}
	};
	struct LoginResponse :public Header //7321: Success, 7320: Rejection
	{
		char memberCode[10];
		char loginId[10];
		char errorCode[3];
		char errorMessage[100];
	};
	struct LogOffRequest :public Header//7312
	{
		char memberCode[10];
		char loginId[10];

	};
	struct LogOffResponse :public Header //7326: Success, 7335: Rejection
	{
		char memberCode[10];
		char loginId[10];
		char errorCode[3];
		char errorMessage[100];
	};
	struct OrderDeltaDownloadRequest : public Header //7315
	{
		char orderDeltaDate[8];
		SIGNED_LONG orderEntryDate;
	};
	struct OrderDeltaDownloadRejection : public Header //7330
	{
		char orderDeltaDate[8];
		SIGNED_LONG orderEntryDate;
		char errorCode[3];
		char errorMessage[100];
	};	
	struct OrderDeltaDownloadStartComplete : public Header //7331:start, 7332:End
	{
		char orderDeltaDate[8];
		SIGNED_LONG orderEntryDate;
		SIGNED_LONG orderConfirmCount;
	};
	struct TradeDeltaDownloadRequest : public Header //7317
	{
		char tradeDeltaDate[8];
		SIGNED_LONG tradeEntryDate;
	};
	struct TradeDeltaDownloadRejection : public Header //7335
	{
		char tradeDeltaDate[8];
		SIGNED_LONG tradeEntryDate;
		char errorCode[3];
		char errorMessage[100];
	};	
	struct TradeDeltaDownloadStartComplete : public Header //7336:start, 7337:End
	{
		char tradeDeltaDate[8];
		SIGNED_LONG tradeEntryDate;
		SIGNED_LONG tradeConfirmCount;
	};
	struct TBT_DeltaDownloadRequest : public Header //7314
	{
		char orderDeltaDate[8];
		SIGNED_LONG orderEntryDate;
	};
	struct TBT_DeltaDownloadRejection : public Header //7338
	{
		char orderDeltaDate[8];
		SIGNED_LONG orderEntryDate;
		char errorCode[3];
		char errorMessage[100];
	};
	struct TBT_DeltaDownloadStartComplete : public Header //7339:start, 7341:End
	{
		char orderDeltaDate[8];
		SIGNED_LONG orderEntryDate;
		SIGNED_LONG TBTConfirmCount;
	};
	//MessageName-> TCP DELLTA: TBTDelta
	//				Bradcast: TBT
	struct TBT_DeltaDownloadMsg : public Header //7340
	{
		SIGNED_SHORT scripCode;
		SIGNED_SHORT buySellIncicator;
		SIGNED_LONG price;
		SIGNED_LONG quantity;
		SIGNED_LONG lastTradePrice;
		SIGNED_LONG lastTradeQtyVolume;

	};
	struct HeartbeatImlRequest :public Header//7318
	{
		char memberCode[10];
		SIGNED_LONG currentDateTime;
	};
	//First 10Bytes = blank, 30bytes = messageName
	struct HeartbeatImlResponse:public Header //7100
	{
		char fillers[151];
		//body
		SIGNED_LONG marketInterId;
		char messageName[30];
	};

	//Order Related	
	struct OrderHeader
	{
		//Header
		char filler0[10];
		char messageName[30];
		char transCode[10];
		char filler1;
		char tokenNumber[10];
		char isinNumber[30];
		char filler2[10];
		char symbol[20];
		char filler3[2];
		char expiryDate[10];
		char filler4[20];
		char filler5[10];
		char filler6;
		char market;
		char segment;
		char filler7[2];
		char exchange;
		char filler8;
		char filler9[10];
		char filler10[10];
		char filler11[10];
		char filler12;
		//Total size is 201 bytes
	};
	//Making one strcuture for all order related request as all having similar values
	struct OrderBody: public OrderHeader
	{
		SIGNED_LONG orderInterId;//1
		SIGNED_LONG filler13;
		SIGNED_LONG filler14;//3
		char		alphaChar[2];//4
		SIGNED_LONG transCode;
		SIGNED_LONG errorCode;//6
		SIGNED_LONG timeStamp1;//7
		SIGNED_LONG lastTradeDate;//8
		SIGNED_LONG filler15;
		char		filler16[10];
		SIGNED_LONG filler17;
		SIGNED_LONG orderNumber;//12
		SIGNED_LONG orderType;//13
		SIGNED_LONG buySell;//14
		SIGNED_LONG quantity;//15
		SIGNED_LONG volumeRemaining;//16
		SIGNED_LONG discloseQuantity;//17
		SIGNED_LONG discloseVolRemaining;//18
		SIGNED_LONG filler18;
		SIGNED_LONG volumeFilledToday;
		SIGNED_LONG price;//21
		SIGNED_LONG triggerPrice;
		SIGNED_LONG orderFlag;//23
		char		brokerId[10];//24
		SIGNED_LONG imlLoginId;
		SIGNED_LONG filler19;
		SIGNED_LONG remarks;
		SIGNED_LONG orderEntryDateTime;//28
		SIGNED_LONG lastModifiedDateTime;//29
		char		clientId[25];
		SIGNED_LONG filler20;
		SIGNED_LONG filler21;
		char		filler22;
		SIGNED_LONG reasonCode;//34
		SIGNED_LONG filler23;
		char		filler24[10];
		char		filler25;
		SIGNED_LONG filler26;
		char		brokerId2[10];//39
		SIGNED_LONG filler27;
		SIGNED_LONG filler28;
		SIGNED_LONG clientType;
		char		filler29;
		char		filler30;
		char		filler31;
		SIGNED_LONG	filler32;
		SIGNED_LONG	filler33;
		char		orderMessage[100];//48
		SIGNED_LONG	filler34;
		SIGNED_LONG	filler35;
		SIGNED_LONG	filler36;
		char		filler37[10];
		SIGNED_LONG	filler38;
		SIGNED_LONG	filler39;
		SIGNED_LONG	filler40;
		char		orderEntryDateTime2[19];
		SIGNED_LONG	filler41;
		char		filler42[8];
		SIGNED_LONG	filler43;
		char		filler44[8];
		char		filler45[10];
		SIGNED_LONG locationId;
		SIGNED_LONG	filler46;
		char		marketId[2];
	};
	struct TradeConfirmation //5445
	{
		//Header
		char filler0[10];
		char messageName[30];
		char transCode[10];
		char filler1;
		char tokenNumber[10];
		char isinNumber[30];
		char filler2[10];
		char symbol[20];
		char filler3[2];
		char expiryDate[10];
		char filler4[20];
		char filler5[10];
		char filler6;
		char market;
		char segment;
		char filler7[2];
		char exchange;
		char filler8;
		char filler9[10];
		char filler10[10];
		char tradeTime[10];
		char filler11;
		//201 bytes header
		//Body
		SIGNED_LONG tradeInterId;//0
		SIGNED_LONG filler12;//1
		SIGNED_LONG tradeTime2;//2
		char		alphaChar[2];
		SIGNED_LONG transCode2;
		SIGNED_LONG errorCode;//5
		SIGNED_LONG filler13;
		SIGNED_LONG lastTradeDate;
		SIGNED_LONG clientType;//8
		SIGNED_LONG tradeNumber;//9
		SIGNED_LONG buySell;//10
		SIGNED_LONG quantity;//11
		SIGNED_LONG price;//12
		SIGNED_LONG orderNumber;//13
		char		brokerId[10];
		SIGNED_LONG imlLoginId;
		SIGNED_LONG originalQuantity;//16
		SIGNED_LONG originalDisclosedQuantity;//17
		SIGNED_LONG remainingQuantity;//18
		SIGNED_LONG discloseQuantityRemaining;//19
		SIGNED_LONG volumeFilledToday;//20
		SIGNED_LONG tradeTime3;//21
		char		counterOrderNumber[10];//22
		SIGNED_LONG filler14;//23
		SIGNED_LONG filler15;//24
		char		filler16[10];//25
		SIGNED_LONG filler17;//26
		char		filler18[10];//27
		SIGNED_LONG filler19;//
		char		filler20[10];
		char		filler21[10];
		SIGNED_LONG locationId;
		SIGNED_LONG filler22;
		SIGNED_LONG filler23;
		SIGNED_LONG filler24;
		char		oldClientId[15];//35
		char		clientId[15];
		char		brokerCode[10];//37
		char		filler25[10];
		SIGNED_LONG filler26;
		char		marketId[2];//40
	};
	struct TradeModification
	{
		//Header
		char filler0[10];
		char messageName[30];
		char transCode[10];
		char filler1;
		char scripCode[10];
		char isinNumber[30];
		char filler2[10];
		char symbol[20];
		char filler3[2];
		char expiryDate[10];
		char filler4[20];
		char filler5[10];
		char filler6;
		char market;
		char segment;
		char filler7[2];
		char exchange;
		char filler8;
		char filler9[10];
		char filler10[10];
		char filler11[10];
		char filler12;
		//Body
		SIGNED_LONG tradeInterId;		
		SIGNED_LONG tradeTime;
		char		alphaChar[2];
		SIGNED_LONG transCode2;
		SIGNED_LONG errorCode;
		SIGNED_LONG filler13;
		SIGNED_LONG lastTradeDate;
		SIGNED_LONG clientType;
		SIGNED_LONG tradeNumber;
		SIGNED_LONG quantity;
		SIGNED_LONG price;		
		SIGNED_LONG filler14;
		char		filler15[10];
		char		filler16[10];
		SIGNED_LONG filler17;
		SIGNED_LONG filler18;
		SIGNED_LONG imlClientLoginId;
		SIGNED_LONG buySell;
		char		filler19[10];
		char		buyClientId[15];
		char		sellClientId[15];
		char		buyBrokerCode[10];
		char		sellBrokerCode[10];
		char		filler20[10];
		char		filler21[10];
		char		filler22[10];
		char		filler23[10];
		char		oldClientId[15];
		char		newClientId[15];
		char		filler24[10];
		char		brokerCode[10];
		char		filler25[10];			
		SIGNED_LONG orderNumber;	
		char		filler26[10];
		SIGNED_LONG filler27;
		char		marketId[2];
	};
	struct TradeModificationRejection
	{
		//Header
		char filler0[10];
		char messageName[30];
		char transCode[10];
		char filler1;
		char scripCode[10];
		char isinNumber[30];
		char filler2[10];
		char symbol[20];
		char filler3[2];
		char expiryDate[10];
		char filler4[20];
		char filler5[10];
		char filler6;
		char market;
		char segment;
		char filler7[2];
		char exchange;
		char filler8;
		char filler9[10];
		char filler10[10];
		char tradeTime[10];
		char filler11;
		//Body
		SIGNED_LONG tradeInterId;
		SIGNED_LONG filler12;
		SIGNED_LONG tradeTime2;
		char		alphaChar[2];
		SIGNED_LONG transCode2;
		SIGNED_LONG errorCode;
		SIGNED_LONG filler13;
		SIGNED_LONG lastTradeDate;
		SIGNED_LONG clientType;
		SIGNED_LONG tradeNumber;
		SIGNED_LONG buySell;
		SIGNED_LONG quantity;
		SIGNED_LONG price;
		SIGNED_LONG orderNumber;
		char		brokerId[10];
		SIGNED_LONG imlLoginId;
		SIGNED_LONG originalQuantity;
		SIGNED_LONG originalDisclosedQuantity;
		SIGNED_LONG remainingQuantity;
		SIGNED_LONG discloseQuantityRemaining;
		SIGNED_LONG volumeFilledToday;
		SIGNED_LONG tradeTime3;
		char		counterOrderNumber[10];
		SIGNED_LONG filler14;
		SIGNED_LONG filler15;
		char		filler16[10];
		SIGNED_LONG filler17;
		char		filler18[10];
		SIGNED_LONG filler19;
		char		filler20[10];
		char		filler21[10];
		SIGNED_LONG locationId;
		SIGNED_LONG filler22;
		SIGNED_LONG filler23;
		SIGNED_LONG filler24;
		char		oldClientId[15];
		char		clientId[15];
		char		brokerCode[10];
		char		filler25[10];
		SIGNED_LONG filler26;
		char		marketId[2];
	};
	struct TradeModificationConfirmation
	{
		//Header
		char filler0[10];
		char messageName[30];
		char transCode[10];
		char filler1;
		char scripCode[10];
		char isinNumber[30];
		char filler2[10];
		char symbol[20];
		char filler3[2];
		char expiryDate[10];
		char filler4[20];
		char filler5[10];
		char filler6;
		char market;
		char segment;
		char filler7[2];
		char exchange;
		char filler8;
		char filler9[10];
		char filler10[10];
		char tradeTime[10];
		char filler11;
		//Body
		SIGNED_LONG tradeInterId;
		SIGNED_LONG filler12;
		SIGNED_LONG tradeTime2;
		char		alphaChar[2];
		SIGNED_LONG transCode2;
		SIGNED_LONG errorCode;
		SIGNED_LONG filler13;
		SIGNED_LONG lastTradeDate;
		SIGNED_LONG clientType;
		SIGNED_LONG tradeNumber;
		SIGNED_LONG buySell;
		SIGNED_LONG quantity;
		SIGNED_LONG price;
		SIGNED_LONG orderNumber;
		char		brokerId[10];
		SIGNED_LONG imlLoginId;
		SIGNED_LONG originalQuantity;
		SIGNED_LONG originalDisclosedQuantity;
		SIGNED_LONG remainingQuantity;
		SIGNED_LONG discloseQuantityRemaining;
		SIGNED_LONG volumeFilledToday;
		SIGNED_LONG tradeTime3;
		SIGNED_LONG	counterOrderNumber;
		char		counterBroker[10];
		SIGNED_LONG filler14;
		SIGNED_LONG filler15;
		char		filler16[10];
		SIGNED_LONG filler17;
		char		filler18[10];
		SIGNED_LONG filler19;
		char		filler20[10];
		char		filler21[10];
		SIGNED_LONG locationId;
		SIGNED_LONG filler22;
		SIGNED_LONG filler23;
		SIGNED_LONG filler24;
		char		oldClientId[15];
		char		clientId[15];
		char		filler25[10];
		char		brokerCode[10];
		SIGNED_LONG filler26;
		char		marketId[2];
	};
	*/
	struct OmsResponseOrder
	{		
		OmsResponseOrder()
		{
			orderStatus = 0;
			ClOrdID = 0;
			memset(orderNumber, ' ', sizeof(orderNumber));
			orderType = 0;
			buySell = 0;
			quantity = 0;
			volumeRemaining = 0;
			volumeFilledToday = 0;
			lastFilledQty = 0;
			price = 0;
			triggerPrice = 0;
			memset(tradeNumber, ' ', sizeof(tradeNumber));
			memset(errorMessage, ' ', sizeof(errorMessage));
		}
		short orderStatus;               // 0 = pending, 1 = partial fill, 2 = filled, 4 = Cancelled, 5 = Replaced, 8 = rejected 
										 // for Member [11 = Revoked/Active, 12 = Suspended, 13 = SquareOffMode]
										 // for Client [21 = Revoked/Active, 22 = Suspended, 23 = SquareOffMode] 
										 // dpr 100
										 // for Limit Violation [31 = CM entered RRM, 32 = CM entered LVM, 33 = TM entered RRM, 34 = TM entered LVM]
		int64_t ClOrdID;                 // Unique clorderid sent by OMS
		char orderNumber[25];            // Exchange order id (fixed-size array)
		short orderType;                 // Order type: like limit / market / spotlimit
		short buySell;                   // Order side
		int32_t quantity;                // Total quantity
		int32_t volumeRemaining;         // Volume remaining pending to exchange
		int32_t volumeFilledToday;       // Total filled quantity
		int32_t lastFilledQty;           // Last filled quantity
		int32_t price;                   // Pending price or filled price on trade
		int32_t triggerPrice;            // Stop price
		char tradeNumber[25];            // Exchange order id (fixed-size array)
		char errorMessage[100];          // Error message on reject or any message received from exchange

		void reset()
		{
			orderStatus = 0;
			ClOrdID = 0;
			memset(orderNumber, ' ', sizeof(orderNumber));
			orderType = 0;
			buySell = 0;
			quantity = 0;
			volumeRemaining = 0;
			volumeFilledToday = 0;
			lastFilledQty = 0;
			price = 0;
			triggerPrice = 0;
			memset(tradeNumber, ' ', sizeof(tradeNumber));
			memset(errorMessage, ' ', sizeof(errorMessage));
		}

		std::string toString() const 
		{
			std::ostringstream oss;

			// Build the common part of the string
			oss << orderStatus << '|'
				<< ClOrdID << '|'
				<< std::string(orderNumber, sizeof(orderNumber)) << '|'
				<< orderType << '|'
				<< buySell << '|'
				<< quantity << '|'
				<< volumeRemaining << '|'
				<< volumeFilledToday << '|'
				<< lastFilledQty << '|'
				<< price << '|'
				<< triggerPrice << '|';

			// Append trade number if it's not empty
			std::string tradeNum(tradeNumber, sizeof(tradeNumber));
			if (tradeNum.find_first_not_of(' ') != std::string::npos) {
				oss << '|' << tradeNum;
			}

			// Trim and append order message only if it's non-empty
			std::string messageTrimmed(errorMessage, sizeof(errorMessage));
			messageTrimmed.erase(messageTrimmed.find_last_not_of('\0') + 1); // Trim trailing spaces

			if (!messageTrimmed.empty()) {
				oss << '|' << messageTrimmed;
			}

			return oss.str();
		}

		std::string toResponceDetails()
		{
			// Trim and append order message only if it's non-empty
			std::string messageTrimmed(errorMessage, sizeof(errorMessage));
			messageTrimmed.erase(messageTrimmed.find_last_not_of('\0') + 1); // Trim trailing spaces
			return messageTrimmed;
		}

	};

}
#endif