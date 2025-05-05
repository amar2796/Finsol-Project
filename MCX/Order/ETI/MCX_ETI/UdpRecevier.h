#pragma once
#ifndef UDP_RECEVIER_H
#define UDP_RECEVIER_H
#include <WinSock.h>
#include <iostream>
#include <string>

using namespace std;

class UDP_RECEVIER
{
public:
	UDP_RECEVIER();
	void init_rcv(int);
	void initializeUdp();
	void ConnectUdp();
	void StartReceiving();
	void disposeSocket();
	SOCKET    SendingSocket;
	SOCKADDR_IN  ReceiverAddr, cliaddr;
	int   ByteReceived;
	int   BufLength = 65535;
	char  ReceiveBuf[65535];
	int port;

	virtual void receiveFromOMS(const std::string&)=0;
private:
	WSADATA   wsaData;


};




#endif