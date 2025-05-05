#include "publisher.h"

namespace FIN {
	namespace MDGtwy {

		Publisher* Publisher::_instance = nullptr;

		void Publisher::init(boost::asio::io_service& ioService, const std::string& configFile)
		{
			try
			{
				ConfigReader confR;
				confR.loadData(configFile);
				std::string isTCPKey = "PUBLISHER.UseTCP";
				std::string isUdpUnicastKey = "PUBLISHER.UseUDP_Unicast";
				std::string isUdpMulticastKey = "PUBLISHER.UseUDP_Multicast";
				std::string isLogEnabled = "PUBLISHER.Log_Enabled";

				_useTcp = confR.getValue < bool >(isTCPKey, false);
				_useUnicast = confR.getValue < bool >(isUdpUnicastKey, false);
				_useMulticast = confR.getValue < bool >(isUdpMulticastKey, false);
				_enableLog = confR.getValue < bool >(isLogEnabled, false);

				if (_useMulticast)
				{

					initMulticast(confR, ioService);
				}
				else if (_useUnicast)
				{
					initUnicast(confR, ioService);
				}
				else if (_useTcp)
				{
					initTcp(confR, ioService);
				}
			}
			catch (std::exception& e)
			{
				std::cout << "Excpetion IN Publisher init : " << e.what() << std::endl;
			}
		}

		void Publisher::send(const MarketData& md, INT_32 size)
		{
			try
			{

				if (_sender != nullptr)
				{
					_sender->send((const char*)&md, size);
					if (_enableLog)
						md.print();
				}
				else
					std::cout << "Sender isnt initialised" << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
			}
		}
		void Publisher::send(const OHLC& ohlc, INT_32 size)
		{
			try
			{

				if (_sender != nullptr)
				{
					if (_enableLog)
						ohlc.print();
					_sender->send((const char*)&ohlc, size);
				}
				else
					std::cout << "Sender isnt initialised" << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
			}
		}
		void Publisher::send(BEST_ASK_BID& best_ask_bid, INT_32 size)
		{
			try
			{

				if (_sender != nullptr)
				{

					auto sendBytes = _sender->send((char*)(&best_ask_bid), 78);
					
					if (_enableLog)
					{
						best_ask_bid.print();
					}
				}
				else
					std::cout << "Sender isnt initialised" << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
			}
		}
		void Publisher::send(BEST_ASK_BID_D& best_ask_bid, INT_32 size)
		{
			try
			{

				if (_sender != nullptr)
				{

					auto sendBytes = _sender->send((char*)(&best_ask_bid), size);

					if (_enableLog)
					{
						best_ask_bid.print();
					}
				}
				else
					std::cout << "Sender isnt initialised" << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
			}
		}
		void Publisher::send(const BroadCastMsgHeader& msg)
		{
			try
			{
				//msg.print();
				if (_sender != nullptr)

					_sender->send((const char*)&msg, msg.getMsgLength() + sizeof(BroadCastMsgHeader));
				else
					std::cout << "Sender isnt initialised" << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
			}
		}
		void Publisher::send(const MarketData_D& md, INT_32 size)
		{
			try
			{

				if (_sender != nullptr)
				{
					_sender->send((const char*)&md, size);
					if (_enableLog)
						md.print();
				}
				else
					std::cout << "Sender isnt initialised" << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
			}
		}
		void Publisher::send(const OHLC_D& ohlc, INT_32 size)
		{
			try
			{

				if (_sender != nullptr)
				{
					if (_enableLog)
						ohlc.print();
					_sender->send((const char*)&ohlc, size);
				}
				else
					std::cout << "Sender isnt initialised" << std::endl;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
			}
		}

		void Publisher::initMulticast(ConfigReader& confR, boost::asio::io_service& ioService)
		{
			std::string multicastAddrKey = "UDP-MULTICAST-PUBLISHER.multicast_addr";
			std::string multicastPortKey = "UDP-MULTICAST-PUBLISHER.multicast_port";
			std::string multicastIpKey = "UDP-MULTICAST-PUBLISHER.listen_interface";

			std::string multicastAdd = confR.getValue < std::string >(multicastAddrKey, strEmptyString);
			std::string multicastIp = confR.getValue < std::string >(multicastIpKey, strEmptyString);
			int port = confR.getValue < int >(multicastPortKey, 0);

			_sender = new MulticastSender(ioService, multicastIp, multicastAdd, port);
		}

		void Publisher::initUnicast(ConfigReader& confR, boost::asio::io_service& ioService)
		{
			std::string unicastAddrKey = "UDP-UNICAST-PUBLISHER.unicast_addr";
			std::string unicastPortKey = "UDP-UNICAST-PUBLISHER.unicast_port";

			std::string address = confR.getValue < std::string >(unicastAddrKey, strEmptyString);
			int port = confR.getValue < int >(unicastPortKey, 0);
			std::cout << "PUBLISHER CRED : IP " << address << " PORT " << port << std::endl;
			LOG_INFO << "PUBLISHER CRED : IP " << address << " PORT " << port;
			_sender = new UnicastSender(ioService, address, port);
		}

		void Publisher::initTcp(ConfigReader& confR, boost::asio::io_service& ioService)
		{
			std::cout << "Using TCP Channel to publish data" << std::endl;
			std::string tcpPortKey = "TCP-PUBLISHER.listen_port";
			int port = confR.getValue < int >(tcpPortKey, 0);
			_sender = new TcpServer(ioService, port);
		}
	}
}
