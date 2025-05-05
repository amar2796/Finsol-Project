#pragma once
#include <iostream>
#include <string>
#include <winsock.h>
#include "defines.h"
using namespace std;
using namespace FIN;
#ifndef UDP_SENDER_H
#define UDP_SENDER_H

class UDP_Sender
{
public:
	UDP_Sender();
	void init(string, int);
	void initializeUdpSocket();
	void connectToBC_server();
	bool sendToOMS(char* msg, size_t len);

	bool _enableLog = false;
	int port;
	string ip;
private:
	bool _isConnected = false;
	SOCKET    SendingSocket;
	SOCKADDR_IN  ReceiverAddr, SrcInfo;
	WSADATA   wsaData;
	int sentBytes = 0;

};

#endif