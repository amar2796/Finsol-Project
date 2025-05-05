#include "MulticastReceiver.h"

FIN::MulticastReceiver::MulticastReceiver(Config_Details* conf_det)
{
	local_conf = conf_det;
}

void FIN::MulticastReceiver::start_Reciver()
{

	try {
		int count = 0;
		std::cout << "[INFO ] Multicast started for " << local_conf->incremental_multicast_addr << " " << local_conf->incremental_multicast_port << " " << local_conf->interfacea_Addr << std::endl;
		LOG_INFO << "[INFO ] Multicast started for " << local_conf->incremental_multicast_addr << " " << std::to_string(local_conf->incremental_multicast_port) << " " << local_conf->interfacea_Addr;

		sockaddr_in fromAddr;
		int fromAddrLen = sizeof(fromAddr);
		for (;;)
		{
			/*size_t rcved_data = socket.receive_from(boost::asio::buffer(data_, max_length), sender_endpoint);*/
			//std::cout << "RCV " << rcved_data << std::endl;
			// Receive and process incoming data

			int rcved_data = recvfrom(recvSocket, data_, max_length, 0, (sockaddr*)&fromAddr, &fromAddrLen);
			if (rcved_data == SOCKET_ERROR)
			{
				std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
				LOG_INFO << "Receive failed : " << WSAGetLastError();
				break;
			}
			
			MessagePacket packet;
			packet.msg = new char[rcved_data];
			memcpy(packet.msg, data_, rcved_data);
			packet.len = rcved_data;

			multicast_q.push(packet);
			std::memset(data_, 0, max_length);
		}

	}
	catch (std::exception& e)
	{
		LOG_INFO << e.what();
		std::cout << e.what() << std::endl;
	}


}

void FIN::MulticastReceiver::init()
{
	try
	{
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (result != 0) {
			std::cerr << "[INFO] WSAStartup failed: " << result << std::endl;
			LOG_INFO << "WSAStartup failed: " << result;
			return;
		}

		recvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (recvSocket == INVALID_SOCKET) {
			std::cerr << "[INFO] Failed to create socket: " << WSAGetLastError() << std::endl;
			LOG_INFO << "Failed to create socket: " << WSAGetLastError();
			WSACleanup();
			return;
		}

		// Set the receive buffer size
		int nTCPRecvBufSize = 134217728 * 2;
		if (setsockopt(recvSocket, SOL_SOCKET, SO_RCVBUF, (const char*)&nTCPRecvBufSize, sizeof(nTCPRecvBufSize)) == SOCKET_ERROR) {
			std::cerr << "unable to set UDP RCV buffer\n";
			LOG_INFO << "unable to set UDP RCV buffer: " << WSAGetLastError();
			return;
		}

		// Set the SO_REUSEADDR option
		BOOL yes = TRUE;
		if (setsockopt(recvSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)) == SOCKET_ERROR) {
			std::cerr << "Reusing ADDR failed\n";
			LOG_INFO << "Reusing ADDR failed " << WSAGetLastError();

			closesocket(recvSocket);
			WSACleanup();
			return;
		}

		sockaddr_in localAddr;
		localAddr.sin_family = AF_INET;
		localAddr.sin_addr.s_addr = inet_addr(local_conf->interfacea_Addr.c_str());
		localAddr.sin_port = htons(local_conf->incremental_multicast_port);


		// Bind the socket to the local endpoint
		result = bind(recvSocket, (sockaddr*)&localAddr, sizeof(localAddr));
		if (result == SOCKET_ERROR) {
			std::cerr << "[INFO] Bind failed: " << WSAGetLastError() << std::endl;
			LOG_INFO << "[INFO] Bind failed: " << WSAGetLastError();

			closesocket(recvSocket);
			WSACleanup();
			return;
		}

		// Join the multicast group
		ip_mreq multicastReq;
		multicastReq.imr_multiaddr.s_addr = inet_addr(local_conf->incremental_multicast_addr.c_str()); // Multicast group address
		multicastReq.imr_interface.s_addr = inet_addr(local_conf->interfacea_Addr.c_str()); // Use any available interface
		result = setsockopt(recvSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&multicastReq, sizeof(multicastReq));
		if (result == SOCKET_ERROR) {
			std::cerr << "[INFO] Joining multicast group failed: " << WSAGetLastError() << std::endl;
			LOG_INFO << "[INFO] Joining multicast group failed: " << WSAGetLastError();

			closesocket(recvSocket);
			WSACleanup();
			return;
		}

		std::cout << "[INFO ]INCREMENTAL MULTICAST DETAILS " << local_conf->incremental_multicast_addr << " " << local_conf->incremental_multicast_port  << " "<< local_conf->interfacea_Addr << std::endl;
		LOG_INFO << "INCREMENTAL MULTICAST DETAILS " << local_conf->incremental_multicast_addr << " " << local_conf->incremental_multicast_port << " "<< local_conf->interfacea_Addr;

	}
	catch (std::exception& ex)
	{
		LOG_INFO << ex.what();
		return;
	}

}
