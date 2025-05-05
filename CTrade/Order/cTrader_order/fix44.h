#ifndef FIN_MD_MT5_ORDER_FIX_RCV_H
#define FIN_MD_MT5_ORDER_FIX_RCV_H

#define _CRT_SECURE_NO_WARNINGS

#include "time.h"
#include "path.h"

#include "rabbitmqProducer.h"
#include "rabbitmqConsumer.h"

#include <unordered_set>
#include <fstream>
#include <chrono>
#include <quickfix/Application.h>
#include <quickfix/MessageCracker.h>
#include <quickfix/FileStore.h>
#include <quickfix/SocketInitiator.h>
#include <quickfix/SessionSettings.h>
#include <quickfix/Log.h>
#include <quickfix/FileLog.h>
#include <quickfix/fix44/Logon.h>
#include <quickfix/fix44/Logout.h>
#include <quickfix/fix44/NewOrderSingle.h>
#include <quickfix/fix44/MarketDataRequest.h>
#include <quickfix/fix44/MarketDataSnapshotFullRefresh.h>
#include <quickfix/fix44/MarketDataIncrementalRefresh.h>
#include <quickfix/fix44/SecurityStatusRequest.h>
#include <quickfix/fix44/MarketDataRequestReject.h>
#include <quickfix/fix44/SecurityDefinitionRequest.h>
#include <quickfix/fix44/SecurityDefinition.h>
#include <quickfix/fix44/ExecutionReport.h>
#include <quickfix/fix44/OrderCancelReject.h>
#include <quickfix/fix44/OrderCancelRequest.h>
#include <quickfix/fix44/OrderCancelReplaceRequest.h>
#include <quickfix/fix44/OrderMassCancelRequest.h>
#include <quickfix/fix44/BusinessMessageReject.h>

#include <quickfix/fix44/OrderStatusRequest.h>
#include "logger.h"
#include "configReader.h"
#include "omsSocketHandler.h"
#include "stringParser.h"
#include "UdpRecevier.h"
#include "UdpSender.h"
#include "Nanolog.hpp"
#include <unordered_map>
namespace FIN {

	struct OrderParam
	{
		char side;
		std::string symbol;
		std::string clOrdId;
		std::string orderId;
	};

	class FIX44Adapter :
		public FIX::Application,
		public FIX::MessageCracker,
		public UDP_RECEVIER,
		public RabbitMqConsumer
	{

	public:
		FIX44Adapter(boost::asio::io_service& ioService)
			:_omsSender(ioService)
		{
			_loggedIn = false;
		}

		~FIX44Adapter()
		{
			_initiator->stop();
			delete(_initiator);
			delete(_logFactory);
			delete(_storeFactory);
			delete(_settings);
		}
		//bool _enableLog;
		UDP_Sender oms_udp_sender;
		/*UDP_RECEVIER udp_recevier;*/
		RabbitMqProducer _omsRabbitSender;
		OmsSenderServer _omsSender;
		FixStringParser _fixStringParser;
		//void receiveFromOms(const std::string &omsInput);
		void receiveFromOMS(const std::string&);
		std::atomic<bool> _loggedIn;
		int _tcpPort;
		std::string _senderSubID;
		bool rabbit_udp;

		void startOmsRcv();
		void doOmsConnect();
		void restartOmsConnections();
		void sendNewOrderOMS();
	
		void sendCancelOrderOMS();
		void sendCancel_ReplaceOrderOMS();
		std::string getCurrency(const std::string& str)
		{
			return str.substr(0, str.find('/'));
		}

		void start();
		void stop();
		void init();
		void startDataProcessing();

		void sendLogout(const FIX::SessionID& sessionId);
		void listenLogOutFromConsole();
	public:
		int _curr_counter = 0;
		int _max = 2000;

		int recever_port = 0;
		std::string sender_ip;
		int sender_port;

	protected:
		void ReadSymbolMapping(std::string config);
		void handlingSubscription(std::string);
		void handlingUnSubscription(std::string);
		//void sendMarketDataRqst(const FIX::SessionID&, const bool readingFlag);
		bool readingSymbolFromFile(FIX44::MarketDataRequest&, FIX44::MarketDataRequest::NoRelatedSym&);
		void setSymbolFromString(FIX44::MarketDataRequest&, FIX44::MarketDataRequest::NoRelatedSym&, std::string);
		void onCreate(const FIX::SessionID&);
		void onLogon(const FIX::SessionID& sessionID);
		void onLogout(const FIX::SessionID& sessionID);
		void toAdmin(FIX::Message&, const FIX::SessionID&);
		void toApp(FIX::Message&, const FIX::SessionID&)
			throw(FIX::DoNotSend);
		void fromAdmin(const FIX::Message&, const FIX::SessionID&)
			throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon);
		void fromApp(const FIX::Message& message, const FIX::SessionID& sessionID)
			throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType);
		void onMessage(const FIX44::BusinessMessageReject&, const FIX::SessionID&);
		void onMessage(const FIX44::ExecutionReport&, const FIX::SessionID&);
		void onMessage(const FIX44::OrderCancelReject&, const FIX::SessionID&);

		void sendLogonReq(FIX44::Logon&, const FIX::SessionID&);

		template < typename MSG >
		bool sendToTarget(MSG& msg, const FIX::SessionID&);


		//void handleSecDefMessage(const FIX44::SecurityDefinition &);
	protected:
		FIX::SessionSettings* _settings;
		FIX::FileStoreFactory* _storeFactory;
		FIX::FileLogFactory* _logFactory;
		FIX::SocketInitiator* _initiator;
		FIX::ScreenLogFactory* _screenLogFactory;

		FIX::SessionID _sessionID;
		std::vector< uint64_t > _records;
		int count = 0;



		FIX::SecurityID secId;
		FIX::MDEntryType mdEntryType;
		FIX::MDUpdateAction mdUpdateAction;
		FIX::MDEntryPx price;
		FIX::MDEntrySize qty;
		FIX::NoMDEntries noMdEntries;
		FIX::MDEntryPositionNo entryPosition;
		FIX44::MarketDataSnapshotFullRefresh::NoMDEntries entries;
		FIX::FieldMap map;

		//UpdateRecordPtr  uRec = std::make_shared < UpdateRecord >();
		//UpdateRecordsPtr uRecVctr = std::make_shared < UpdateRecords>();

		std::string _exchangeNumber;
		std::string _senderCompId;
		std::string _password;
		std::string _key;
		std::string _encryptedPass;
		std::string _username;
		long long _milliseconds;
		bool _sendSecurityListReq;
		bool _firstSecList;
		bool _enableScreenLog;
		int _counterMReq;

		std::unordered_set< std::string > _replaceClIdSet;


		int c = 0;

		bool _isLogEnabledIncoming = false;
		bool _isLogEnabledOutgoing = false;
		bool _gatewayLogEnabled;
		bool _boostLogEnabled;
	public:

		void sendNewOrder();
		void sendReplaceOrder();
		void sendCancelOrder();
		void sendMassCancelReq();
		void sendOrderStatusRequest();
		void sendSelfLogin();
		void commonOrderFields();

		void printMenu();
		std::unordered_map <std::string, OrderParam> orderParamMap;
		std::unordered_map <std::string, OrderParam>::iterator orderParamItr;
	private:
		CurrentPath _currentPath;
		std::unordered_map < std::string, std::string >  _symbol_mapping;
		std::string getCurrentTime()
		{
			SYSTEMTIME st;
			GetLocalTime(&st);

			std::string timeStr(
				std::to_string(st.wDay) + "-"
				+ std::to_string(st.wMonth) + "-"
				+ std::to_string(st.wYear) + "_"
				+ std::to_string(st.wHour) + ":"
				+ std::to_string(st.wMinute) + ":"
				+ std::to_string(st.wSecond) + "-" + std::to_string(st.wMilliseconds)
				+ " | "
			);


			return timeStr;
		}

		void initializeLogging(const std::string _path)
		{
			std::cout << "Log path : " << _path << std::endl;
			std::cout << "\n\n**************************************\n";
			std::cout << "** Build On: " << __DATE__ << "-" << __TIME__ " **" << std::endl;
			std::cout << "**************************************\n\n";


			std::time_t t = time(NULL) + 19800;
			auto gmtime = std::gmtime(&t);

			char buffer[32];
			strftime(buffer, 32, "%Y%m%d%T", gmtime);
			std::string t1(buffer);
			t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
			std::string fileName = "cTrader-" + t1 + "__";
			nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size

			LOG_INFO << "*********************************************************";
			LOG_INFO << "cTrader Build Date: " << __DATE__ << "-" << __TIME__;
			LOG_INFO << "*********************************************************";

		}

		void initBoostLogger(const std::string& logPath, const bool _enableLog)
		{
			SYSTEMTIME st;
			GetLocalTime(&st);

			std::string fName(logPath +
				"\\logs_" + std::to_string(st.wDay) + "-" + std::to_string(st.wMonth) + "-" +
				std::to_string(st.wYear) + "_" + std::to_string(st.wHour) + "-" + std::to_string(st.wMinute) +
				"-" + std::to_string(st.wSecond) + "-" + std::to_string(st.wMilliseconds) + ".txt");
			std::cout << "Log File Name: " << fName << std::endl;
			//add_file_log functions are used to configure where the log messages are sent
			logging::add_file_log
			(
				keywords::file_name = fName,
				keywords::target_file_name = fName,
				keywords::auto_flush = true
			);
			logging::add_common_attributes();

			if (_enableLog)
			{
				logging::core::get()->set_filter
				(
					logging::trivial::severity >= logging::trivial::debug
				);
			}
			else
			{
				logging::core::get()->set_filter
				(
					logging::trivial::severity >= logging::trivial::info
				);
			}
		}
	};
}
#endif
