#pragma warning(suppress : 4996)
#include "fix44.h"
//#include "marketDataStore.cpp"

namespace FIN {
	namespace MDGtwy {
		namespace CTrade {
			int count = 0;
			void FIX44Adapter::start()
			{
				if (_nanoLogEnabled)
				{
					LOG_INFO << "[CTRADE] START";
				}
				_initiator->start();
			}

			void FIX44Adapter::stop()
			{
				_initiator->stop();
			}

			void FIX44Adapter::init()
			{
				ConfigReader confR;
				confR.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
				std::string config = _currentPath.getCurrentPath() + FIN::FIN_CONST::QUICKFIX_CONFIG_FILE;
				isContractDownload = confR.getValue<bool>("CONTRACT_DOWNLOAD" + FIN::FIN_CONST::SEP_DOT + "Enable", false);
				isLogEnable = confR.getValue<bool>("PUBLISHER" + FIN::FIN_CONST::SEP_DOT + "Log_Enabled", false);
				
				contractList = confR.getValue<std::string>("CONTRACT_DOWNLOAD" + FIN::FIN_CONST::SEP_DOT + "SECURITY_TYPE", FIN::FIN_CONST::EMPTY_STRING); //comma separated list like FUT,MLEG,OPT
				contract_exchange = confR.getValue<std::string>("CONTRACT_DOWNLOAD" + FIN::FIN_CONST::SEP_DOT + "Exchange", FIN::FIN_CONST::EMPTY_STRING);

				std::string configToken = _currentPath.getCurrentPath() + FIN::FIN_CONST::TOKEN_NAME;
				readSecurityFile(configToken);

				confR.loadData(config);

				//_gatewayLogEnabled = confR.getValue<bool>("DEFAULT.Enable_Gateway_Log", false);
				_nanoLogEnabled = confR.getValue<bool>("DEFAULT.Enable_Nano_Log", false);
				
				if (_nanoLogEnabled)
				{
					std::string logPath = confR.getValue<std::string>("DEFAULT.FileLogPath", "");
					initializeLogging(logPath);
				}

				_settings = new FIX::SessionSettings(config);
				_storeFactory = new FIX::FileStoreFactory(*_settings);
				_logFactory = new FIX::ScreenLogFactory(*_settings);
				_initiator = new FIX::SocketInitiator(*this, *_storeFactory, *_settings, *_logFactory);

				std::cout << "[CTRADE] INIT" << std::endl;
				if (_nanoLogEnabled)
				{
					LOG_INFO << "[CTRADE] INIT";
				}
			}
			
			//this vector store in subscribedSymbols
			std::vector<std::string> subscribedSymbols;
			void FIX44Adapter::readSecurityFile(const std::string configToken)
			{
				std::ifstream inputFile(configToken);
				if (!inputFile.is_open()) {
					std::cout << "Error: Could not open the file." << std::endl;
				}
				std::string line;
				while (std::getline(inputFile, line)) {
					subscribedSymbols.push_back(line);
				}
				inputFile.close();
			}

			void FIX44Adapter::initializeLogging(const std::string _path)
			{
				std::time_t t = time(NULL) + 19800;
				auto gmtime = std::gmtime(&t);

				char buffer[32];
				strftime(buffer, 32, "%Y%m%d%T", gmtime);
				std::string t1(buffer);
				t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
				std::string fileName = "cTrader-" + t1 + "__";
				nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size

				if (_nanoLogEnabled)
				{
					LOG_INFO << "*********************************************************";
					LOG_INFO << "cTrader Build Date: " << __DATE__ << "-" << __TIME__;
					LOG_INFO << "*********************************************************";
				}

			}

			void FIX44Adapter::sendLogout(const FIX::SessionID& sessionId)
			{
				std::cout << "send logout forcefully" << std::endl;
				_initiator->getSession(sessionId)->logout();
			}

			void FIX44Adapter::onCreate(const FIX::SessionID& sessionId)
			{
				if (_settings->get(sessionId).has("Password"))
				{
					_password = _settings->get(sessionId).getString("Password");
				}
				if (_settings->get(sessionId).has("SenderCompID"))
				{
					_SenderCompID = _settings->get(sessionId).getString("SenderCompID");
				}
				if (_settings->get(sessionId).has("TargetCompID"))
				{
					_TargetCompID = _settings->get(sessionId).getString("TargetCompID");
				}
				if (_settings->get(sessionId).has("HeartBtInt"))
				{
					_HeartBtInt = _settings->get(sessionId).getInt("HeartBtInt");
				}
				if (_settings->get(sessionId).has("Key"))
				{
					_key = _settings->get(sessionId).getString("Key");
				}
				if (_settings->get(sessionId).has("Username"))
				{
					_username = _settings->get(sessionId).getString("Username");
				}
				if (_settings->get(sessionId).has("SenderSubID"))
				{
					_senderSubID = _settings->get(sessionId).getString("SenderSubID");
				}
				if (_settings->get(sessionId).has("SendSecList"))
				{
					_sendSecurityListReq = _settings->get(sessionId).getBool("SendSecList");
				}

				if (_settings->get(sessionId).has("ScreenLogShowIncoming"))
				{
					_isLogEnabledIncoming = _settings->get(sessionId).getBool("ScreenLogShowIncoming");
				}

				if (_settings->get(sessionId).has("ScreenLogShowOutgoing"))
				{
					_isLogEnabledOutgoing = _settings->get(sessionId).getBool("ScreenLogShowOutgoing");
				}

				//_firstSecList = true;
				//FIN::MDGtwy::Publisher::getInstance()->setModuleState(FIN::ModuleState_Connected);

			}

			void FIX44Adapter::onLogon(const FIX::SessionID& sessionID)
			{
				FIN_INFO("[CTRADE] Logged ON  [" << sessionID << "]");
				if (_nanoLogEnabled)
				{
					LOG_INFO << "[CTRADE] Logged ON  [" << sessionID.toString() << "]";
				}

				FIN::MDGtwy::Publisher::getInstance()->setModuleState(ModuleState_LoggedIn);

				_mdStore.reset();
				_sessionID = sessionID;
				_counterMReq = 0;

				if (isContractDownload)
				{
						sendSecurityListReq(sessionID);
				}
				else
				{	
					for (std::string symbol : subscribedSymbols)
					{
						sendMarketDataReq(sessionID, symbol);
					}
					//int symbol = 3;
					//sendMarketDataReq(sessionID, std::to_string(symbol));
				}
				
			}

			void FIX44Adapter::sendSecurityListReq(const FIX::SessionID& sessionId)
			{
				FIX44::SecurityListRequest secListReq;
				
				//create unique name with time
				std::string file="securityfile";
				std::string mdReqId = file + std::to_string(time(NULL));

				secListReq.setField(FIX::SecurityReqID(mdReqId));
				secListReq.setField(FIX::SecurityListRequestType(0));
				secListReq.setField(FIX::SenderSubID(_senderSubID));
				//secListReq.setField(FIX::TargetSubID(_senderSubID));

				//secListReq.set(FIX::Symbol("2"));

				if (FIX::Session::sendToTarget(secListReq, sessionId))
				{
					FIN_INFO( "SEC LIST REQ SENT : for Exchange:");
					if (_nanoLogEnabled)
					{
						LOG_INFO << "SEC LIST REQ SENT : for Exchange";
					}
				}
			}


			void FIX44Adapter::sendMarketDataReq(const FIX::SessionID& sessionId, std::string symbol, const char requestType)
			{
				FIX44::MarketDataRequest mdReq;

				mdReq.setField(FIX::SubscriptionRequestType(requestType));
				
				mdReq.setField(FIX::MarketDepth(0));

				mdReq.setField(FIX::MDUpdateType(1));
				mdReq.setField(FIX::SenderSubID(_senderSubID));


				FIX44::MarketDataRequest::NoMDEntryTypes mdEntryTypes;
				mdEntryTypes.set(FIX::MDEntryType('0'));
				mdReq.addGroup(mdEntryTypes);
				mdEntryTypes.set(FIX::MDEntryType('1'));
				mdReq.addGroup(mdEntryTypes);

				
				FIX44::MarketDataRequest::NoRelatedSym relatedSym;
				relatedSym.setField(FIX::MDReqID(std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count()) +
					std::to_string(++_counterMReq)));


				relatedSym.set(FIX::Symbol(symbol));
				mdReq.addGroup(relatedSym);

				_mdStore.updateMDForTT(symbol);

				if (FIX::Session::sendToTarget(mdReq, sessionId))
				{
					FIN_DEVLOG("[CTRADE] MD REQ SENT : Symbol :: "+symbol);
					if (_nanoLogEnabled)
					{
						LOG_INFO << "[CTRADE] MD REQ SENT: Symbol :: " + symbol;
					}
				}
				
			}


			void FIX44Adapter::onLogout(const FIX::SessionID& sessionID)
			{
				FIN::MDGtwy::Publisher::getInstance()->setModuleState(ModuleState_LoggedOut);
				FIN_INFO("[CTRADE] Logged OUT  [" << sessionID << "]");
				if (_nanoLogEnabled)
				{
					LOG_INFO << "[CTRADE] Logged OUT  [" << sessionID.toString() << "]";
				}
			}

			void FIX44Adapter::toAdmin(FIX::Message& msg, const FIX::SessionID& sessionId)
			{
				if (_isLogEnabledOutgoing)
				{
					FIN_INFO ( msg.toString());
				}
				if (_nanoLogEnabled)
				{
					LOG_INFO<<msg.toString();
				}
				const std::string& msgTypeValue = msg.getHeader().getField(FIX::FIELD::MsgType);
				if (msgTypeValue == FIX::MsgType_Logon)
				{
					sendLogonReq((FIX44::Logon&)msg, sessionId);
				}
				
			}

			void FIX44Adapter::sendLogonReq(FIX44::Logon& msgLogon, const FIX::SessionID&)
			{
				//targetSubId      is same of      SenderSubId
				msgLogon.setField(FIX::TargetSubID(_senderSubID));
				msgLogon.setField(FIX::SenderSubID(_senderSubID));
				msgLogon.setField(FIX::Username(_username));
				msgLogon.setField(FIX::Password(_password));
			}

			void FIX44Adapter::toApp(FIX::Message& msg, const FIX::SessionID&)
				throw(FIX::DoNotSend)
			{
				if (_isLogEnabledOutgoing)
				{
					FIN_INFO( msg.toString());
				}
				if (_nanoLogEnabled)
				{
					LOG_INFO<<msg.toString();
				}
			}

			void FIX44Adapter::fromAdmin(const FIX::Message& msg, const FIX::SessionID& sessionId)
				throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
			{
				if (_isLogEnabledIncoming) 
				{ 
					FIN_INFO( msg.toString());
				}
				if (_nanoLogEnabled)
				{
					LOG_INFO<<msg.toString();
				}
				crack(msg, sessionId);
			}

			void FIX44Adapter::fromApp(const FIX::Message& msg, const FIX::SessionID& sessionId)
				throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
			{
				
				if (msg.getHeader().getField(FIX::FIELD::MsgType) == "y")
				{
					
					std::string value = msg.toString();
					_mdStore.sendContractString(value);
					LOG_INFO << msg.toString();
					//handleSecDefMessage((FIX44::SecurityList)msg);
					FIN_INFO("Security List send");
					//exit(0);
				}

				if (_isLogEnabledIncoming) 
				{ 
					FIN_INFO( msg.toString());
				}
				if (_nanoLogEnabled)
				{
					LOG_INFO<<msg.toString();
				}
				crack(msg, sessionId);
			}

			void FIX44Adapter::handleSecDefMessage(const FIX44::SecurityList& msg)
			{
				std::string message = msg.toString();
				_mdStore.sendContractString(message);
			}
			void FIX44Adapter::onMessage(const FIX44::MarketDataSnapshotFullRefresh& snpMsg, const FIX::SessionID&)
			{


				std::string fix_message = snpMsg.toString();
				// Split the FIX message into individual tags
				std::vector<std::string> tags;
				size_t pos = 0;
				while ((pos = fix_message.find('')) != std::string::npos) {
					tags.push_back(fix_message.substr(0, pos));
					fix_message.erase(0, pos + 1);
				}
				tags.push_back(fix_message);  // Add the last tag

				std::vector<std::string> tag_269;
				std::vector<std::string> tag_270;
				std::vector<std::string> tag_271;
				std::vector<std::string> tag_278;

				for (const std::string& tag : tags) {
					size_t equals_pos = tag.find('=');
					if (equals_pos != std::string::npos) {
						std::string tag_number = tag.substr(0, equals_pos);
						std::string tag_value = tag.substr(equals_pos + 1);
						if (tag_number == "269") {
							tag_269.push_back ( tag_value);
						}
						else if (tag_number == "270") {
							tag_270.push_back(tag_value);
						}
						else if (tag_number == "271") {
							tag_271.push_back(tag_value);
						}
						else if (tag_number == "278") {
							tag_278.push_back(tag_value);
						}
					}
					
				}

				FIX::SendingTime time;
				auto t = time.getValue().getTimeT();

				if (snpMsg.isSet(noMdEntries))
				{

					FIX::Symbol symbol;
					snpMsg.getField(symbol);
					_mdStore.updateMDForTT(symbol.getString());


					uRec.secId = symbol.getString();

					snpMsg.get(noMdEntries);
					for (int i = 0; i < noMdEntries; ++i)
					{
						/*snpMsg.getGroup(i, entries);
						
						entries.getFieldIfSet(mdEntryType);
						entries.getFieldIfSet(price);
						entries.getFieldIfSet(qty);*/



						uRec.updateTime = t;
						uRec.updateAction = MDUpdateAction_NEW;
						uRec.MDEntryId= std::stod(tag_278[i].c_str());
						try
						{
							uRec.qty = atoi(tag_271[i].c_str()) ;
							uRec.price = std::stod(tag_270[i].c_str());
						}
						catch (std::exception& e)
						{
							if (_nanoLogEnabled)
							{
								LOG_WARN << "Excetion in qty or price:" << e.what();
							}
						}

						switch (atoi(tag_269[i].c_str()))
						{

						case 0: uRec.updateType = MDUpdateType_BID; break;
						case 1: uRec.updateType = MDUpdateType_OFFER; break;
						default: 
							if (_nanoLogEnabled)
							{
								LOG_INFO << "INVALID Value Update Type : " << mdEntryType;
							}
							uRec.updateType = MDUpdateType_INVALID;
						}
						_mdStore.applyUpdate(uRec);
					}
				}
				_mdStore.messageUpdatesProcessed();
			}

			void FIX44Adapter::onMessage(const FIX44::MarketDataIncrementalRefresh& incMSg, const FIX::SessionID&)
			{
				if (incMSg.isSet(noMdEntries))
				{

					incMSg.get(noMdEntries);
					for (int i = 1; i <= noMdEntries; ++i)
					{
						incMSg.getGroup(i, entries);

						entries.getFieldIfSet(sym);
						//entries.getFieldIfSet(secId);
						uRec.secId = sym.getString();

						entries.getFieldIfSet(price);
						entries.getFieldIfSet(qty);
						entries.getFieldIfSet(entryPosition);
						entries.getFieldIfSet(mdEntryType);
						entries.getFieldIfSet(mdUpdateAction);
						entries.getFieldIfSet(mdEntryID);
						
						uRec.MDEntryId = std::stod(mdEntryID.getString());

						try
						{
							uRec.qty = std::stod(qty.getString());
							uRec.price = std::stod(price.getString());
						}
						catch (const std::exception& e)
						{
							if (_nanoLogEnabled)
							{
								LOG_WARN << "Excetion in price or qty:" << e.what();
							}
						}

						switch (mdUpdateAction)
						{
						case '0': uRec.updateAction = MDUpdateAction_NEW; break;
						case '2': uRec.updateAction = MDUpdateAction_DELETE; break;
						default:
							if (_nanoLogEnabled)
							{
								LOG_INFO << "INVALID Value Update Action : " << mdUpdateAction;
							}
							uRec.updateAction = MDUpdateAction_INVALID;
						}

						switch (mdEntryType)
						{
						case '0': uRec.updateType = MDUpdateType_BID; break;
						case '1': uRec.updateType = MDUpdateType_OFFER; break;
						default:
							if (_nanoLogEnabled)
							{
								LOG_INFO << "INVALID Value Update Type : " << mdEntryType;
							}
							uRec.updateType = MDUpdateType_INVALID;
						}
						_mdStore.applyUpdate(uRec);
					}
				}
				_mdStore.messageUpdatesProcessed();
			}

			void FIX44Adapter::onMessage(const FIX44::SecurityList& incMSg, const FIX::SessionID&)
			{
				/*std::string value = incMSg.toString();
				_mdStore.sendContractString(value);*/
			}
			
		}
	}
}
