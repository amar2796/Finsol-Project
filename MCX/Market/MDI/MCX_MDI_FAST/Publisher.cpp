#include "Publisher.h"
namespace FIN
{

	Publisher::Publisher(Config_Details* conf_Det)
	{
		local_config = conf_Det;



	}

	void Publisher::init()
	{
		_enableLog = local_config->enable_log;

		if (local_config->enable_Unicast)
		{
			ip = local_config->publisher_uni_ip;
			port = local_config->publisher_uni_port;

			initializeUdpSocket(ip, port);
			std::cout << "Broadcast IP " << ip << " Broadcast PORT " << port << " logging " << _enableLog << std::endl;
			LOG_INFO << "Broadcast IP " << ip << " Broadcast PORT " << port << " logging " << _enableLog;
		}

		if (local_config->enable_Multicast)
		{
			multi_ip = local_config->publisher_multi_ip;
			multi_port = local_config->publisher_multi_port;
			multi_interface = local_config->publisher_interface;
			initializeUdpMulticastSocket(multi_ip, multi_port, multi_interface);
			std::cout << "Broadcast IP " << multi_ip << " Broadcast PORT " << multi_port << " Broadcast NIC IP " << multi_interface << " logging " << _enableLog << std::endl;
			LOG_INFO << "Broadcast IP " << multi_ip << " Broadcast PORT " << multi_port << " Broadcast NIC IP " << multi_interface << " logging " << _enableLog;
		}
	}

	void Publisher::initializeUdpSocket(std::string ip, int port)
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
				connectToBC_server(ip, port);
			}
		}
	}

	void Publisher::connectToBC_server(std::string ip, int port)
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

	void Publisher::initializeUdpMulticastSocket(std::string ip, int port, std::string _interfaceAddr)
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
				// 🔹 Bind to specific NIC IP
				sockaddr_in localAddr = {};
				localAddr.sin_family = AF_INET;
				localAddr.sin_port = htons(0);  // OS picks any available port

				if (InetPtonA(AF_INET, _interfaceAddr.c_str(), &localAddr.sin_addr) != 1) {
					LOG_INFO << "Client: Invalid NIC IP address format";
					closesocket(SendingSocket);
					WSACleanup();
					return;
				}

				if (bind(SendingSocket, (sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
				{
					LOG_INFO << "Client: Bind failed " << WSAGetLastError();
					closesocket(SendingSocket);
					WSACleanup();
					return;
				}

				// 🔹 Set multicast interface (NIC IP)
				struct in_addr localInterface = {};
				if (InetPtonA(AF_INET, _interfaceAddr.c_str(), &localInterface) != 1) {
					LOG_INFO << "Client: Invalid NIC IP address format";
					closesocket(SendingSocket);
					WSACleanup();
					return;
				}

				if (setsockopt(SendingSocket, IPPROTO_IP, IP_MULTICAST_IF, (char*)&localInterface, sizeof(localInterface)) == SOCKET_ERROR)
				{
					LOG_INFO << "Client: Failed to set multicast interface " << WSAGetLastError();
					closesocket(SendingSocket);
					WSACleanup();
					return;
				}


				connectToBC_server(ip, port);
			}
		}
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
				return;
			}

			if (_enableLog)
				_md.print();

		}
		else
		{
			LOG_INFO << "Cleint is not connected " << WSAGetLastError();
			initializeUdpSocket(ip, port);
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
				return;
			}
			if (_enableLog)
				ohlc.print();
		}
		else
		{
			LOG_INFO << "Cleint is not connected " << WSAGetLastError();
			initializeUdpSocket(ip, port);
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
				return;
			}
			if (_enableLog)
				_dpr.print();
		}
		else
		{
			LOG_INFO << "Cleint is not connected " << WSAGetLastError();
			initializeUdpSocket(ip, port);
		}

	}
}


