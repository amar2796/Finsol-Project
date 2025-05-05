#include "UdpSender.h"


UDP_Sender::UDP_Sender()
{

}

void UDP_Sender::init(std::string ip, int port)
{
	this->ip = ip;
	this->port = port;
	std::cout << "UDP PUBLISHER : IP " << ip << " PORT " + std::to_string(port) << std::endl;
	initializeUdpSocket();
}

void UDP_Sender::initializeUdpSocket()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		std::cout << "CLIENT: WSASTARTUP FAILED WITH ERROR " << std::to_string(WSAGetLastError()) << std::endl;
		WSACleanup();
	}

	else
	{
		SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (SendingSocket == INVALID_SOCKET)
		{
			std::cout << "CLIENT : ERROR AT SOCKET " << std::to_string(WSAGetLastError()) << std::endl;
			WSACleanup();
		}
		else
		{
			std::cout << "UDP PUBLISHER IS READY TO SEND " << std::endl;
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
		std::cout << " Error : Connect Failed " << std::to_string(WSAGetLastError());
		exit(0);
	}
	_isConnected = true;

}

bool UDP_Sender::sendToOMS(char* msg, size_t len)
{
	if (_isConnected)
	{

		if (sendto(SendingSocket, msg, len, 0, (struct sockaddr*)NULL, sizeof(ReceiverAddr)) <= 0)
		{
			std::cout << "Data sent error " + std::to_string(WSAGetLastError());
		}

		return true;
	}
	else
	{
		std::cout << "CLIENT IS NOT CONNECTED " + std::to_string(WSAGetLastError());
		initializeUdpSocket();
	}

	return false;
}
bool UDP_Sender::sendToOMS(const std::string& msg)
{

	if (_isConnected)
	{

		if (sendto(SendingSocket, msg.c_str(), msg.length(), 0, (struct sockaddr*)NULL, sizeof(ReceiverAddr)) <= 0)
		{
			std::cout << "Data sent error " + std::to_string(WSAGetLastError());
		}		

		return true;
	}
	else
	{
		std::cout << "CLIENT IS NOT CONNECTED " + std::to_string(WSAGetLastError());
		initializeUdpSocket();
	}

	return false;
}
