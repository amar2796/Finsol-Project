#pragma once
#include <iostream>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <winsock.h>
#include <windows.h>
#pragma comment(lib, "Ws2_32.lib")

#include "Common.h"
//using namespace std;
#ifndef UDP_SENDER_H
#define UDP_SENDER_H

class UDP_Sender 
{
public:
	UDP_Sender();
	void init(std::string, int);
	void initializeUdpSocket();
	void connectToBC_server();
	bool sendToOMS(const char* msg, size_t len);
	//bool sendToOMS(std::string);

	bool _enableLog = false;
	int port;
	std::string ip;
private:
	bool _isConnected = false;
	SOCKET    SendingSocket;
	SOCKADDR_IN  ReceiverAddr, SrcInfo;
	WSADATA   wsaData;
	int sentBytes = 0;

};

#endif