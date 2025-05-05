#pragma once
#ifndef FIN_EXCHANGE_PROCESSOR_H
#define FIN_EXCHANGE_PROCESSOR_H
#include "ExchangeConnection.h"
#include "Logger.h"
#include "omsSocketHandler.h"
#include "UdpSender.h"
#include "UdpRecevier.h"
#include "Struct_To_String.h"
#include "throttling.h"
#include "ApplSeq.h"
#include "Password_Encryption.h"
#include "rabbitmqConsumer.h"
#include "rabbitmqProducer.h"
#include <cstdint>
#include "TradeHandler.h"
using namespace Derivatives;
namespace FIN {

	namespace ETI
	{

		class Exchange_Processor : public ExchangeConnection,
			public UDP_RECEVIER,
			public RabbitMqConsumer, TradeHandler
		{
		public:

			Exchange_Processor(boost::asio::io_service& ioService);
			virtual void handleMessage(unsigned char* data, UINT16 templateID , int len);
			void startCommunication();
			void startExchangeConnection();
			void startOMSChannel();
			void doOMSConnect();
			void restartOmsConnections();
			void sendGatewayRequest();
			void sendLogonRequest();
			void sendHeartbeat();
			void sendUserLogon();
			void sendNewOrder();
			void sendModifyOrder();
			void sendCancelOrder();
			void sendRetransmissionTrade(uint64_t start_seq, uint32_t pID);
			void sendRetransmittOrderEvent();
			void sendSubscriptionRequest(uint8_t);
			void sendPasswordChange();
			void GenrateIvKey();
			virtual void receiveFromOMS(const std::string&);
			Password_Encryption _passwordEncryption;
			int64_t floatToInt64Multiplied(double dVal, int64_t multiplier);

			//Below three functions used to generate random password                
			int selectArray();
			int getKey();
			std::string generate_password(int length);
			//void updateConfigFile(std::string password, std::string newPassword, std::string date);
			void updateConfigFile(std::string password);
			void readCustodian(std::string fileName);
			void sendDeleteAllBroadcastMsg();

		protected:
			uint32_t userId;
			std::string DefaultCstmApplVerID;
			std::string password;
			std::string newPassword;
			std::string unencryptedPassword;
			std::string unencryptedNewPassword;


			std::string IV;
			std::string key;



			//std::string custodian;

			std::unordered_map<std::string, std::string>custodian;
			UINT64 strategyID;
			bool isLoggedOn;
			bool isFirst;
			std::string udp_sender_ip;
			int sender_port;
			int rcv_port;
			int confirmationPort;
			int rabbit_udp;
			uint64_t terminalInfo;
			uint8_t SMPF;
			uint8_t persistent;
			Common_Req_Structure _common_struct;
			ApplSeq trade_applSeq;
			CurrentPath _path;
			UDP_Sender oms_udp_sender;
			RabbitMqProducer _omsRabbitSender;
			OmsSenderServer _omsSender;
			FixStringParser _fixStringParser;
			Struct_To_String _structConvertion;
			FIN::SlidingWindowThrottling* slidingThrottel;
			std::string publicKeyFilePath;
			bool resetHeartbeat = false;
			bool isSendDeleteBroadCastMsg = false;
			bool isFirstPartition = true;
			int tradeCount = 0;
			int partitionID = 1;
		private:

			Logger logger;
			void onMessage(ConnectionGatewayResponseT* gwRes);
			void onMessage(LogonResponseT* gwLogonRes);
			void onMessage(HeartbeatNotificationT* heartbeat);
			void onMessage(UserLoginResponseT* userLogon);
			void onMessage(NewOrderNRResponseT* statndardNewRes);
			void onMessage(NewOrderResponseT* shortNewRes);
			void onMessage(RejectT* rejectRes);
			void onMessage(DeleteOrderNRResponseT* cancelRes);
			void onMessage(DeleteOrderResponseT* cancelRes);
			void onMessage(ModifyOrderNRResponseT* modRes);
			void onMessage(ModifyOrderResponseT* modRes);
			void onMessage(OrderExecNotificationT* bookExecution);
			void onMessage(OrderExecResponseT* bookExecution);
			void onMessage(TradingSessionStatusBroadcastT* trdaingStateEvent);
			void onMessage(ThrottleUpdateNotificationT* throttleRes);
			void onMessage(SessionPasswordChangeResponseT* passChange);
			void onMessage(LogoutResponseT* logout);
			void onMessage(ForcedUserLogoutNotificationT* forcedLogout);
			void onMessage(ForcedLogoutNotificationT* forcedLogout);
			void onMessage(OrderExecReportBroadcastT* orderExec);
			void onMessage(TradeBroadcastT* tradeRecovery);
			void onMessage(RetransmitResponseT* retransmitResponse);
			void onMessage(DeleteOrderBroadcastT* cancelBroadcast);
			void onMessage(MarketStatusNotificationT* marketStatus);
			void onMessage(DeleteAllOrderQuoteEventBroadcastT* response);
			void onMessage(SubscribeResponseT*);
			void onMessage(RetransmitMEMessageResponseT*);
			void onMessage(NewsBroadcastT*);
			void onMessage(DeleteAllOrderBroadcastT*);
			void onMessage(TradeEnhancementNotificationT*);
			void sendResponseToOMS(char* response, int bodyLen, int messageType, uint64_t orderId);
			
			void initiateHeartbeat(int);
			uint64_t endSeqNumber;
			uint64_t lastSeqNumber;
			bool isConnected = false;
			uint32_t heartbeat = 0;

		};
	}
}






#endif