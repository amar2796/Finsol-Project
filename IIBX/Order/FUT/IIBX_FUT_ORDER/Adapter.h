#pragma once
#ifndef FIN_ADAPTER_H
#define FIN_ADAPTER_H

#include "IOServiceHanlder.h"
#include "ExchangeConnection.h"
#include "omsSocketHandler.h"
#include "udpHandler/UdpSender.h"
#include "udpHandler/UdpRecevier.h"
#include "throttling.h"
#include "ExchangeRequestProcess.h"
#include "ExchangeResponseProcess.h"
#include "rabbitHandler/rabbitmqConsumer.h"
#include "rabbitHandler/rabbitmqProducer.h"
#include <iostream>
#include <string>
#include "color.h"
#include "TransCodes.h"
#include "OrderBook.h"
#include <ctime>
#include <random>
#include "ConnectionForContract.h"

#define DPR_MSG "DPR Close Update"
#define priceDevider 10000

namespace FIN
{
	namespace IIBX
	{
		class Adapter: public ExchangeConnection,
			public UDP_RECEVIER,
			public RabbitMqConsumer,
			public ExchangeRequestProcess
		{
		public:					
			HeaderResponse parseHeaderMsg(std::string msg);
			IECCONNECTION parseIECMsg(std::string resp);

			Adapter(boost::asio::io_service& io_service);
			void handleMessage(std::string&data, int len);
			void processLogonMsg(HeaderResponse hres);
			void processNewOrderMsg(HeaderResponse hres, std::string msg);
			void processModifyOrderMsg(HeaderResponse hres, std::string msg);
			void processDeleteOrderMsg(HeaderResponse hres, std::string msg);
			void processTradeNotificationMsg(HeaderResponse hres, std::string msg);
			void processLimitViolationNotificationMsg(std::string msg);
			void processOrderKillNotificationMsg(std::string msg);
			void processMemberActionNotificationMsg(std::string msg);
			void processClientActionNotificationMsg(std::string msg);
			void processReturnSelfMatchTradeMsg(HeaderResponse hres, std::string msg);
			void processStopLossConvertedNotificationMsg(std::string msg);
			void processDownloadPendingOrderMsg(HeaderResponse hres, std::string msg);
			void processDownloadReturnOrderMsg(HeaderResponse hres, std::string msg);
			void processDownloadReturnStopLossMsg(HeaderResponse hres, std::string msg);
			void processDownloadTradeMsg(HeaderResponse hres, std::string msg);
			void processDownloadStopLossMsg(HeaderResponse hres, std::string msg);
			void processDownloadMarketPictureBBOMsg(HeaderResponse hres, std::string msg);
			void processChangePasswordMsg(HeaderResponse hres);
			void processTechnicalFailure(HeaderResponse hres);
			void processContractMsg(HeaderResponse hres, std::string msg);
			void insertValueInFile(std::string value);
			bool getDPRValue(std::vector<std::string>& value, std::string msg);
			void logonSuccess();
			void processLogoffMsg(HeaderResponse hres);
			void init();			
			void startExchangeConnection();
			void disableConnection();
			void startOMSChannel();
			void doOMSConnect();
			void restartOmsConnections();
			void sendLogonRequest();
			void sendNewOrder();
			void sendModifyOrder();
			void sendCancelOrder();
			void sendDownloadPendingOrder();
			void sendDownloadReturnOrderMsg();
			void sendDownloadTradeMsg();
			void sendDownloadStopLossMsg();
			void sendDownloadReturnedStopLossMsg();
			void sendChangePassword(std::string newPass);
			void sendLogOffRequest();
			void sendContractDownload();
			void sendDownloadMarketPictureBBOMsg();
			void receiveFromOMS(const std::string&);
			std::string getCurrentDay();
			std::string generateCustomPassword();
			void sendToOms(char* resp);

		private:
			std::string cMemCode;
			std::string tMemCode;
			std::string dealerCode;
			std::string ctclCode;
			std::string password;
			std::string udp_sender_ip;
			int sender_port;
			int rcv_port;
			int confirmationPort;
			int rabbit_udp;
			CurrentPath _path;
			UDP_Sender oms_udp_sender;			
			RabbitMqProducer _omsRabbitSender;
			OmsSenderServer _omsSender;					
			FIN::SlidingWindowThrottling* slidingThrottel;
			bool isLoggedOn;
			bool isFirst;
			ExchangeResponseProcess exchResp;
			int seqNumber = 0;
			bool isIECConnectionRes = false;
			//int priceDevider = 10000;
			bool isPasswordChange = false;
			std::string newPassword="Reset.123";
			std::string configFileLocation="";
			bool isContractDownload = false;
			std::string contractIP;
			int contractPort;
			ConnectionForContract* contract_send;
			int throttlemsg = 0;
			int throttlesec = 0;
			std::string algoID="";
			std::unordered_map<std::string, int> contractClosePrice;	// <symbol, <dpr percentage, close price>>
			bool isMarketPictureDownload = false;
		};
	}
}



#endif