#ifndef FIN_MD_CME_FIX_RCV_H
#define FIN_MD_CME_FIX_RCV_H

#include "subscriptionHandler.h"
#include "time.h"
#include "path.h"
//#include "../marketDataStore.h"
#include "marketDataMessage.h"
#include "publisher.h"
#include <unordered_set>
#include <fstream>
#include <chrono>
#include "quickfix/Application.h"
#include "quickfix/FileStore.h"
#include "quickfix/MessageCracker.h"
#include "quickfix/SocketInitiator.h"
#include "quickfix/SessionSettings.h"
#include "quickfix/Log.h"
#include "quickfix/fix44/Logon.h"
#include "quickfix/fix44/Logout.h"
#include "quickfix/fix44/MarketDataRequest.h"
#include "quickfix/fix44/MarketDataSnapshotFullRefresh.h"
#include "quickfix/fix44/MarketDataIncrementalRefresh.h"
#include "quickfix/fix44/SecurityStatusRequest.h"
#include "quickfix/fix44/MarketDataRequestReject.h"
#include "quickfix/fix44/SecurityListRequest.h"
#include "quickfix/fix44/SecurityList.h"
#include "logger.h"
#include "commons.h"
#include "marketDataStore.h"

namespace FIN {
	namespace MDGtwy {
		namespace CTrade {

			class FIX44Adapter :
				public FIX::Application,
				public FIX::MessageCracker//,
				//public securitySubscriptionHandler
			{

			public:
				FIX44Adapter()
				{
				}
				~FIX44Adapter()
				{
					_initiator->stop();
					delete(_initiator);
					delete(_logFactory);
					delete(_storeFactory);
					delete(_settings);
				}
				void start();
				void stop();
				void init();
				void startDataProcessing() { while (1)Sleep(1000); }

				void sendLogout(const FIX::SessionID & sessionId);

			public:
				int _curr_counter = 0;
				int _max = 2000;

			protected:
				//void handlingSubscription(std::string);
				//void handlingUnSubscription(std::string);
				//void sendMarketDataRqst(const FIX::SessionID&, const bool readingFlag);
				//bool readingSymbolFromFile(FIX44::MarketDataRequest&, FIX44::MarketDataRequest::NoRelatedSym&);
				//void setSymbolFromString(FIX44::MarketDataRequest&, FIX44::MarketDataRequest::NoRelatedSym&, std::string);
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
				//void onMessage(const FIX44::MarketDataRequest&, const FIX::SessionID&);
				void onMessage(const FIX44::SecurityList&, const FIX::SessionID&);
				void onMessage(const FIX44::MarketDataSnapshotFullRefresh&, const FIX::SessionID&);
				void onMessage(const FIX44::MarketDataIncrementalRefresh&, const FIX::SessionID&);
				void sendLogonReq(FIX44::Logon &, const FIX::SessionID &);
				void sendMarketDataReq(const FIX::SessionID&, std::string symbol, const char requestType = '1');
				//void sendSecurityList(const FIX::SessionID&);
				void readSecurityFile(std::string);
				void initializeLogging(std::string);

				template < typename MSG >
				bool sendToTarget(MSG &msg, const FIX::SessionID&);

			protected:
				FIX::SessionSettings  * _settings;
				FIX::FileStoreFactory * _storeFactory;
				FIX::ScreenLogFactory * _logFactory;
				FIX::SocketInitiator  * _initiator;

				FIX::SessionID _sessionID;
				std::vector< uint64_t > _records;
				int count = 0;

				void handleSecDefMessage(const FIX44::SecurityList&);

				FIX::SecurityID secId;
				FIX::MDEntryType mdEntryType;
				FIX::MDUpdateAction mdUpdateAction;
				FIX::MDEntryID mdEntryID;
				FIX::MDEntryPx price;
				FIX::MDEntrySize qty;
				FIX::Symbol sym;
				FIX::NoMDEntries noMdEntries;
				FIX::MDEntryPositionNo entryPosition;
				FIX44::MarketDataSnapshotFullRefresh::NoMDEntries entries;
				FIX::FieldMap map;

				std::unordered_map<std::string, MarketData > _marketDataMap;
				std::unordered_map<std::string, MarketData >::iterator _marketDataMapItr;

				UpdateRecord uRec;
				MarketDataStore _mdStore;
				bool isContractDownload;
				bool isLogEnable;
				bool _nanoLogEnabled;
				int noOfHearbeats = 0;
				std::string contractList;
				void sendSecurityListReq(const FIX::SessionID&);
				std::string contract_exchange;


				std::string x;
				std::string _senderSubID;
				std::string _password;
				std::string _SenderCompID;
				std::string _TargetCompID;
				int _HeartBtInt;
				std::string _key;
				std::string _encryptedPass;
				std::string _username;
				long long _milliseconds;
				bool _sendSecurityListReq;
				bool _firstSecList;
				bool _enableScreenLog;
				int _counterMReq;
				//MarketDataStore _mdStore;
				std::unordered_set< std::string > _symbolSet;

				void fetchSubscribedSymbols();


				int c = 0;
				
				bool _isLogEnabledIncoming = false;
				bool _isLogEnabledOutgoing = false;

				void printMenu();
				void sendUnsubscribe();

			private:
				CurrentPath _currentPath;

			};
			
		}
	}
}
#endif
