#ifndef FIN_MD_PUBLISHER_H
#define FIN_MD_PUBLISHER_H

#include "marketDataMessage.h"
#include "configReader.h"
#include "multicastSender.h"
#include "tcpSender.h"
#include "time.h"
#include "common.h"
#include "path.h"
#include "logger.h"
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <chrono>
//#include "MarketDepth_5.h"

namespace FIN {
	namespace MDGtwy {

		class Publisher
		{
		public:
			bool _enableLog;
			void send(const FIN::MarketData_5Depth& msg);
			//void send(const FIN::MarketData_10Depth& msg);
			
			static Publisher* getInstance()
			{
				if (!_instance)
					_instance = new Publisher();

				return _instance;
			}
			static Publisher* getContractInstance()
			{
				if (!_contract_instance)
					_contract_instance = new Publisher();

				return _contract_instance;
			}

			void send(const ModuleConnectivity& mdConn)
			{
				send((const BroadCastMsgHeader&)mdConn);
				if (_enableLog)
					mdConn.print();
			}

			void send(const OHLC& ohlc)
			{

				send((const BroadCastMsgHeader&)ohlc);
				if (_enableLog)
					ohlc.print();
			}

			void send(const SymbolNotice& notice)
			{
				send((const BroadCastMsgHeader&)notice);
				if (_enableLog)
					notice.print();
			}

			void send(const MarketData&);
			void send(const BROADCAST::MarketData& md)
			{
				//std::cout << "Size correct : " << sizeof(md) << std::endl;
				send((const BroadCastMsgHeader&)md);
				if (_enableLog)
					md.print();
			}

			//void send(const BROADCAST::MarketData_default& md)
			//{
			//	//std::cout << "Size correct : " << sizeof(md) << std::endl;
			//	send((const BroadCastMsgHeader&)md);
			//	if (_enableLog)
			//		md.print();
			//}
			void send(const std::string& data);
			void init(boost::asio::io_service& ioService,
				const std::string& configFIle,
				const UNSIGNED_INT_16 exchange,
				const SIGNED_INT_8 moduleType);


			void setModuleState(const SIGNED_INT_8 state)
			{
				auto now = std::chrono::system_clock::now();
				auto currentTime = std::chrono::system_clock::to_time_t(now);

				_moduleConnectivity->setUpdateTime(currentTime);
				_moduleConnectivity->setModuleState(state);
				send(*_moduleConnectivity);
			}

			void handleTimer(const boost::system::error_code&);


		private:

			static Publisher* _instance;
			static Publisher* _contract_instance;
			Publisher()
			{
				_sender = nullptr;
			}

			void send(const BroadCastMsgHeader&);
			void initMulticast(ConfigReader&, boost::asio::io_service&);
			void initUnicast(ConfigReader&, boost::asio::io_service&);
			void initContractUnicast(ConfigReader& confR, boost::asio::io_service& ioService);
			void initTcp(ConfigReader&, boost::asio::io_service&);
			bool _useMulticast;
			bool _useUnicast;
			bool _useTcp;


			std::string _logPath;
			AbstractDataSender* _sender;
			CurrentPath _currentPath;
			bool _isSecurityList=false;
			int securityListPort = 0;

			typedef std::unordered_map<std::string, BROADCAST::MarketData > SendMap;
			SendMap _sendMap;
			std::condition_variable _conditionVar;
			std::mutex _sendMapLock;

			boost::asio::deadline_timer* _connectivityTimer;

			ModuleConnectivity* _moduleConnectivity;



		};
	}
}


#endif

