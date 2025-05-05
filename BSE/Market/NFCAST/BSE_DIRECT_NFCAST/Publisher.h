#pragma once
#ifndef FIN_COMMON_UNICAST_SENDER
#define FIN_COMMON_UNICAST_SENDER
#include "marketDataMessage.h"
#include "ConfigDetails.h"
#include <Windows.h>
#include "Nanolog.hpp"

namespace FIN {

	class Publisher
	{
	public:
		Publisher(Config_Details*);
		void init();
		void initializeUdpSocket();
		void connectToBC_server();
		bool sendToBroadCast(char* msg);
		void send(MarketData& _md, INT_32 size);
		void send(OHLC& ohlc, INT_32 size);
		void send(DPR& dpr, INT_32 size);
		bool _enableLog = false;
		int port;
		string ip;
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