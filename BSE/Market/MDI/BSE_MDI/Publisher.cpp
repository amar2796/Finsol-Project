#include "Publisher.h"
namespace FIN
{
	Publisher::Publisher(Config_Details* conf_Det)
	{
		local_config = conf_Det;



	}

	void Publisher::init()
	{

		ip = local_config->publisher_ip;
		port = local_config->publisher_port;



		_enableLog = local_config->enable_log;
		std::cout << "[INFO ]Broadcast IP " << ip << " Broadcast PORT " << port << " logging " << _enableLog << std::endl;
		LOG_INFO << "Broadcast IP " << ip << " Broadcast PORT " << port << " logging " << _enableLog;
		initializeUdpSocket();

	}

	void Publisher::initializeUdpSocket()
	{
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)

		{
			LOG_INFO << "Client: WSAStartup failed with error " << WSAGetLastError();
			WSACleanup();
		}

		else
		{
			LOG_INFO << "Client: The Winsock DLL status is " << wsaData.szSystemStatus;
			SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (SendingSocket == INVALID_SOCKET)
			{
				LOG_INFO << "Client: Error at socket " << WSAGetLastError();
				WSACleanup();
			}
			else
			{
				LOG_INFO << "Client: socket() is OK! ";
				connectToBC_server();
			}
		}
	}

	void Publisher::connectToBC_server()
	{
		memset((char*)&ReceiverAddr, 0, sizeof(ReceiverAddr));
		ReceiverAddr.sin_family = AF_INET;
		ReceiverAddr.sin_port = htons(port);
		ReceiverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

		if (connect(SendingSocket, (struct sockaddr*)&ReceiverAddr, sizeof(ReceiverAddr)) < 0)
		{
			LOG_INFO << " Error : Connect Failed " << WSAGetLastError();
			exit(0);
		}
		_isConnected = true;
	}

	bool Publisher::sendToBroadCast(char* msg)
	{
		return false;
	}

	void Publisher::send(MarketData& _md, INT_32 size)
	{
		if (_isConnected)
		{
			auto ptr = (char*)&_md;
			if (sendto(SendingSocket, ptr, size, 0, (struct sockaddr*)NULL, sizeof(ReceiverAddr)) <= 0)
			{
				LOG_INFO << "Data sent error " << WSAGetLastError();
			}

			if (_enableLog)
				_md.print();

		}
		else
		{
			LOG_INFO << "Cleint is not connected " << WSAGetLastError();
			initializeUdpSocket();
		}
	}
	void Publisher::send(OHLC& ohlc, INT_32 size)
	{
		if (_isConnected)
		{
			auto ptr = (char*)&ohlc;
			if (sendto(SendingSocket, ptr, size, 0, (struct sockaddr*)NULL, sizeof(ReceiverAddr)) <= 0)
			{
				LOG_INFO << "Data sent error " << WSAGetLastError();
			}
			if (_enableLog)
				ohlc.print();
		}
		else
		{
			LOG_INFO << "Cleint is not connected " << WSAGetLastError();
			initializeUdpSocket();
		}

	}
	void Publisher::send(DPR& _dpr, INT_32 size)
	{
		if (_isConnected)
		{
			auto ptr = (char*)&_dpr;
			if (sendto(SendingSocket, ptr, size, 0, (struct sockaddr*)NULL, sizeof(ReceiverAddr)) <= 0)
			{
				LOG_INFO << "Data sent error " << WSAGetLastError();
			}
			if (_enableLog)
				_dpr.print();
		}
		else
		{
			LOG_INFO << "Cleint is not connected " << WSAGetLastError();
			initializeUdpSocket();
		}

	}
}


