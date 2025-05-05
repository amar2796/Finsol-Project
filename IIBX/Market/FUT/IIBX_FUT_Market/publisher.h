#ifndef FIN_MD_NSETBT_PUBLISHER
#define FIN_MD_NSETBT_PUBLISHER

#include "marketDataMessage.h"
//#include "sender.h"
#include "multicastSender.h"
#include "tcpSender.h"
#include "configReader.h"
#include "constants.h"
#include <string>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <mutex>
#include "CurrentPath.h"
#include "Nanolog.hpp"
namespace FIN::IIBX
{
	/*!
	  /publisher class use to intialize sender as tcp sender or udp unicast or multicast sender
	  /send data to clients
	  */
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
		void send(const BEST_ASK_BID&, INT_32);
		void send(const DPR& dpr, INT_32 size);
		void send(const CONTRACTACTION& conAction, INT_32 size);
		void send(const MARKETRESUMEACTION& mktAction, INT_32 size);

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



#endif
