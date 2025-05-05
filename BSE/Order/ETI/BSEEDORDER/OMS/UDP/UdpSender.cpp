#include "UdpSender.h"


UDP_Sender::UDP_Sender()
{

}

void UDP_Sender::init(string ip, int port)
{
	this->ip = ip;
	this->port = port;
	FIN_DEVLOG("UDP PUBLISHER : IP " + ip + " PORT " + std::to_string(port));
	initializeUdpSocket();
}

void UDP_Sender::initializeUdpSocket()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		FIN_DEVLOG("CLIENT: WSASTARTUP FAILED WITH ERROR " + std::to_string(WSAGetLastError()));
		WSACleanup();
	}

	else
	{
		SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (SendingSocket == INVALID_SOCKET)
		{
			FIN_DEVLOG("CLIENT : ERROR AT SOCKET " + std::to_string(WSAGetLastError()));
			WSACleanup();
		}
		else
		{
			FIN_DEVLOG("UDP PUBLISHER IS READY TO SEND ");
			connectToBC_server();
		}
	}
}

void UDP_Sender::connectToBC_server()
{
	memset((char*)&ReceiverAddr, 0, sizeof(ReceiverAddr));
	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(port);
	ReceiverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	if (connect(SendingSocket, (struct sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr)) < 0)
	{
		FIN_DEVLOG(" Error : Connect Failed " + std::to_string(WSAGetLastError()));
		exit(0);
	}
	_isConnected = true;

}

bool UDP_Sender::sendToOMS(const char* msg, size_t len)
{

	if (_isConnected)
	{
		if (sendto(SendingSocket, msg, len, 0, (struct sockaddr*)NULL, sizeof(ReceiverAddr)) <= 0)
		{
			FIN_DEVLOG("Data sent error " + std::to_string(WSAGetLastError()));
		}
		LOG_INFO <<"[INFO] " << "[TO OMS] " + std::string(msg);
		return true;
	}
	else
	{
		FIN_DEVLOG("CLIENT IS NOT CONNECTED " + std::to_string(WSAGetLastError()));
		initializeUdpSocket();
	}

	return false;
}

