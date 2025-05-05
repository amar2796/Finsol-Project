#ifndef FIN_APPLICATION_H
#define FIN_APPLICATION_H
#include<iostream>
#include<string>
#include"CurrentPath.h"
#include "configReader.h"
#include "constants.h"
#include <boost/asio.hpp>
#include"MulticastReceiver.h"
#include "publisher.h"
#include "SymbolProcess.h"
#include "boost/algorithm/string/trim.hpp"
#include <boost/asio.hpp>
#include "Nanolog.hpp"
#include "color.h"
#include <chrono>
#include <thread>
#include <atomic>

using boost::asio::ip::tcp;

namespace FIN::IIBX
{
	class Application
	{
	public:
		Application(int exchId);
		~Application();
		void init();
		bool createTCPConnection();
		void handleMessage(std::string& msg);
		void start();

		void checkConnection();

	private:				
		SecurityFileHandler _secHandler;
		MulticastRcvr recvr;
		int exchId;
		bool isLoggedOn;		
		std::string multicast_ip;
		std::string tcpRcvIp;
		int tcpRcvPort;
		int multicast_port;
		boost::asio::io_service io_service;
		ConfigReader config_param;
		CurrentPath currPath;
		std::unordered_map<std::string, std::unique_ptr<SymbolProcess>> m_mdatamap;
		std::unordered_set <std::string> symblSet;
		bool isLogEnable;
		std::string host_ip_res;
		int host_port_res;
		std::string host_interface_ip;

		int firstTime = 0;
		int secondTime = 0;
		bool isConnection = false;

	};
}
#endif // !FIN_APPLICATION_H
