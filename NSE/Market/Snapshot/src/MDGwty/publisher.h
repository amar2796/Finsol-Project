#ifndef FIN_MD_PUBLISHER_H
#define FIN_MD_PUBLISHER_H

#include "../common/marketDataMessage.h"
#include "../common/configReader.h"
#include "../common/multicastSender.h"
#include "../common/tcpSender.h"
#include "common.h"

namespace FIN {
	namespace MDGtwy {


		class CurrentPath
		{
		public:
			CurrentPath() {}

			std::string getCurrentPath()
			{
				char buffer[512];
				GetModuleFileNameA(NULL, buffer, 512);     // windows operation so will also made generice for both os.
				std::string path(buffer);
				size_t slashPosition1 = path.rfind("\\");
				path = path.substr(0, slashPosition1 + 1);
				return path;
			}

		};
		class Publisher
		{
		public:

			static Publisher* getInstance()
			{
				if (!_instance)
					_instance = new Publisher();
				//std::cout << "getinstance function1\n";
				return _instance;
			}

			void send(const MarketData&, INT_32 size);
			void send(const OHLC&, INT_32 size);
			void send(BEST_ASK_BID&, INT_32);
			void send(BEST_ASK_BID_D&, INT_32);
			void send(const MarketData_D&, INT_32 size);
			void send(const OHLC_D&, INT_32 size);
			void init(boost::asio::io_service& ioService,
				const std::string& configFIle);
			bool _enableLog;
		private:

			static Publisher* _instance;
			Publisher() {}

			void send(const BroadCastMsgHeader&);

			void initMulticast(ConfigReader&, boost::asio::io_service&);
			void initUnicast(ConfigReader&, boost::asio::io_service&);
			void initTcp(ConfigReader&, boost::asio::io_service&);

			bool _useMulticast;
			bool _useUnicast;
			bool _useTcp;


			AbstractDataSender* _sender;
		public:

			std::string multicastAdd;
			std::string multicastIp;
			int port;

		};
	}
}


#endif
