#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#ifndef FIN_COMMON_UNICAST_SENDER
#define FIN_COMMON_UNICAST_SENDER
#include "marketDataMessage.h"
#include "ConfigDetails.h"
#include <winsock2.h>
#include <ws2tcpip.h>  // Required for InetPtonA()
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

namespace FIN {

	class Publisher
	{
	public:
		Publisher(Config_Details*);
		void init();
		void initializeUdpSocket(std::string ip, int port);
		void connectToBC_server(std::string ip, int port);
		bool sendToBroadCast(char* msg);
		void send(MarketData& _md, INT_32 size);
		void send(OHLC& ohlc, INT_32 size);
		void send(DPR& _dpr, INT_32 size);
		bool _enableLog = false;
		std::string ip;
		int port;

		void initializeUdpMulticastSocket(std::string ip, int port, std::string _interface);
		std::string multi_ip;
		int multi_port;
		std::string multi_interface;
	private:
		Config_Details *local_config;
		bool _isConnected = false;
		SOCKET    SendingSocket;
		SOCKADDR_IN  ReceiverAddr, SrcInfo;
		WSADATA   wsaData;
		int sentBytes = 0;

	};


}





#endif