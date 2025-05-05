#include "publisher.h"
#include "commons.h"

namespace FIN {
	namespace MDGtwy {

		Publisher* Publisher::_instance = nullptr;
		Publisher* Publisher::_contract_instance = nullptr;

		void Publisher::init(boost::asio::io_service& ioService, const std::string& configFile,
			const UNSIGNED_INT_16 exchange, const SIGNED_INT_8 moduleType)
		{
			try
			{
				ConfigReader confR;
				confR.loadData(_currentPath.getCurrentPath() + configFile);

				std::string isTCPKey = "PUBLISHER.UseTCP";
				std::string isUdpUnicastKey = "PUBLISHER.UseUDP_Unicast";
				std::string isUdpMulticastKey = "PUBLISHER.UseUDP_Multicast";
				std::string isLogEnabled = "PUBLISHER.Log_Enabled";
				std::string isLogPath = "PUBLISHER.FileStorePath";
				std::string isContractDownload = "CONTRACT_DOWNLOAD.Enable";

				_useTcp = confR.getValue < bool >(isTCPKey, false);
				_useUnicast = confR.getValue < bool >(isUdpUnicastKey, false);
				_useMulticast = confR.getValue < bool >(isUdpMulticastKey, false);
				_enableLog = confR.getValue < bool >(isLogEnabled, false);
				_logPath = confR.getValue <std::string>(isLogPath, "");
				_isSecurityList = confR.getValue<bool>(isContractDownload, false);

				/*SYSTEMTIME st;
				GetLocalTime(&st);*/
				/*logging::add_file_log(_logPath+"\\logs_"+ std::to_string(st.wDay)+"-"+std::to_string(st.wMonth)+"-"+
										std::to_string(st.wYear)+"_"+std::to_string(st.wHour)+":"+std::to_string(st.wMinute)+":"+
										std::to_string(st.wSecond)+":"+std::to_string(st.wMilliseconds)+".txt");*/

										//logging::add_file_log(_logPath + "\\logs_" + std::to_string(st.wDay) + "-" + std::to_string(st.wMonth) + "-" +
										//	std::to_string(st.wYear) + "_" + std::to_string(st.wHour) + "-" + std::to_string(st.wMinute) +
										//	"-" + std::to_string(st.wSecond) + "-" + std::to_string(st.wMilliseconds) + ".txt");

										//logging::add_common_attributes();

										/*if (_enableLog)
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
										}*/

				if (_useMulticast)
				{
					initMulticast(confR, ioService);
				}
				else if (_useUnicast)
				{
					if (_contract_instance != nullptr)
						initContractUnicast(confR, ioService);
					else
						initUnicast(confR, ioService);
				}
				else if (_useTcp)
				{
					initTcp(confR, ioService);
				}


				_moduleConnectivity = new  ModuleConnectivity(exchange);
				/*	_moduleConnectivity->setModuleType(moduleType);

					_connectivityTimer = new boost::asio::deadline_timer(ioService);
					_connectivityTimer->expires_from_now(boost::posix_time::seconds(5));
					_connectivityTimer->async_wait(boost::bind(&Publisher::handleTimer, this, boost::asio::placeholders::error));*/
			}
			catch (std::exception& e)
			{
				std::cout << "Excpetion IN Publisher init : " << e.what() << std::endl;
				BOOST_LOG_TRIVIAL(warning) << "Excpetion IN Publisher init : " << e.what();
			}
		}

		void Publisher::send(const MarketData& md)
		{
			std::cout << "Size wrong: " << sizeof(md) << std::endl;
			send((const BroadCastMsgHeader&)md);
			if (_enableLog)
				md.print();
		}

		void Publisher::send(const std::string& msg)
		{
			try
			{
				if (_sender != nullptr)
				{
					_sender->send((const char*)msg.c_str(), msg.length());
				}
				else
				{
					std::cout << "Sender isnt initialised" << std::endl;
					BOOST_LOG_TRIVIAL(error) << "Sender isnt initialised";
				}
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
				BOOST_LOG_TRIVIAL(warning) << "Exception IN Publisher Send : " << e.what();
			}

		}
		void Publisher::send(const BroadCastMsgHeader& msg)
		{
			try
			{
				if (_sender != nullptr)
				{
					_sender->send((const char*)&msg, msg.getMsgLength());// +sizeof(BroadCastMsgHeader));
				}
				else
				{
					std::cout << "Sender isnt initialised" << std::endl;
					BOOST_LOG_TRIVIAL(error) << "Sender isnt initialised";
				}
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
				BOOST_LOG_TRIVIAL(warning) << "Exception IN Publisher Send : " << e.what();
			}
			//_connectivityTimer->expires_from_now(boost::posix_time::seconds(5));
			//_connectivityTimer->async_wait(boost::bind(&Publisher::handleTimer, this, boost::asio::placeholders::error));
		}
		
		//void Publisher::send(const string msg)
		//{
		//	try
		//	{
		//		if (_sender != nullptr)
		//		{
		//			_sender->send((const char*)&msg, msg.getMsgLength());// +sizeof(BroadCastMsgHeader));
		//		}
		//		else
		//		{
		//			std::cout << "Sender isnt initialised" << std::endl;
		//			BOOST_LOG_TRIVIAL(error) << "Sender isnt initialised";
		//		}
		//	}
		//	catch (std::exception& e)
		//	{
		//		std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
		//		BOOST_LOG_TRIVIAL(warning) << "Exception IN Publisher Send : " << e.what();
		//	}
		//	//_connectivityTimer->expires_from_now(boost::posix_time::seconds(5));
		//	//_connectivityTimer->async_wait(boost::bind(&Publisher::handleTimer, this, boost::asio::placeholders::error));
		//}

		void Publisher::send(const FIN::MarketData_5Depth& msg)
		{
			try
			{
				if (_sender != nullptr)
				{
					_sender->send((const char*)&msg, msg.getMsgLength());// +sizeof(BroadCastMsgHeader));
					if (_enableLog)
						msg.print();
				}
				else
				{
					std::cout << "Sender isnt initialised" << std::endl;
					BOOST_LOG_TRIVIAL(error) << "Sender isnt initialised";
				}
			}
			catch (std::exception& e)
			{
				std::cout << "Exception IN Publisher Send : " << e.what() << std::endl;
				BOOST_LOG_TRIVIAL(warning) << "Exception IN Publisher Send : " << e.what();
			}
			//_connectivityTimer->expires_from_now(boost::posix_time::seconds(5));
			//_connectivityTimer->async_wait(boost::bind(&Publisher::handleTimer, this, boost::asio::placeholders::error));
		}
		void Publisher::handleTimer(const boost::system::error_code& ec)
		{
			if (!ec)
			{
				//std::cout << "Timer Elapsed at: " << time(NULL) << std::endl;
				_moduleConnectivity->setUpdateTime(time(NULL));
				send(*_moduleConnectivity);
				_connectivityTimer->expires_from_now(boost::posix_time::seconds(5));
				_connectivityTimer->async_wait(boost::bind(&Publisher::handleTimer, this, boost::asio::placeholders::error));
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
			int port = 0;

			port = confR.getValue < int >(unicastPortKey, 0);
			//std::cout << "[TT] Market feed started , will publish data on " << port << std::endl;
			//LOG_INFO << "[TT] Market feed , will publish data on " << std::to_string(port);


			_sender = new UnicastSender(ioService, address, port);
		}
		void Publisher::initContractUnicast(ConfigReader& confR, boost::asio::io_service& ioService)
		{
			std::string unicastAddrKey = "UDP-UNICAST-PUBLISHER.unicast_addr";
			std::string contractDownloadPort = "CONTRACT_DOWNLOAD.Publish_Port";


			std::string address = confR.getValue < std::string >(unicastAddrKey, strEmptyString);
			int port = confR.getValue < int >(contractDownloadPort, 0);
			if (_isSecurityList)
			{
				port = confR.getValue < int >(contractDownloadPort, 0);
				std::cout << "[CTRADE] Contract downloader started , will publish data on " << port << std::endl;
				LOG_INFO << "[CTRADE] Contract loader started , will publish data on " << std::to_string(port);
			}
			_sender = new UnicastSender(ioService, address, port);
		}
		void Publisher::initTcp(ConfigReader& confR, boost::asio::io_service& ioService)
		{
			std::cout << "Using TCP Channel to publish data" << std::endl;
			BOOST_LOG_TRIVIAL(debug) << "Using TCP Channel to publish data";
			std::string tcpPortKey = "TCP-PUBLISHER.listen_port";
			int port = confR.getValue < int >(tcpPortKey, 0);
			_sender = new TcpServer(ioService, port);
		}
	}
}