#include "MulticastReceiver.h"

FIN::MulticastReceiver::MulticastReceiver(Config_Details* conf_det) 
{
	local_conf = conf_det;
}

void FIN::MulticastReceiver::start_Reciver()
{

	try {
		int count = 0;
		for (;;)
		{
			//char buffer[2000]; // Adjust buffer size as needed
			struct sockaddr_in sender_addr;
			int sender_addr_len = sizeof(sender_addr);
			int bytes_received = recvfrom(socket_fd, data_, sizeof(data_), 0, (struct sockaddr*)&sender_addr, &sender_addr_len);
			if (bytes_received == SOCKET_ERROR) {
				std::cerr << "recvfrom failed with error: " << WSAGetLastError() << std::endl;
				return;
			}
			std::cout << "yes";
            ptr = new char[bytes_received];
            memcpy(ptr, data_, bytes_received);

            MessagePacket msgPacket;
            std::memcpy(&msgPacket.msgType, ptr, sizeof(int32_t));
            msgPacket.msgType = ntohl(msgPacket.msgType);
            msgPacket.length = bytes_received;
            msgPacket.msg = ptr;

            multicast_q.push(msgPacket);
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
		// Initialize Winsock
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0) {
			std::cout << "WSAStartup failed with error: " << result << std::endl;
			return;
		}

		// Create socket
		SOCKET fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (fd == INVALID_SOCKET) {
			std::cout << "socket creation failed with error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return;
		}

		// Bind to the specified interface and port
		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(local_conf->interfacea_Addr.c_str());
		addr.sin_port = htons(local_conf->snapshot_multicast_port);

		if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
			std::cout << "bind failed with error: " << WSAGetLastError() << std::endl;
			closesocket(fd);
			WSACleanup();
			return;
		}

		// Join the multicast group
		ip_mreq mreq{};
		mreq.imr_multiaddr.s_addr = inet_addr(local_conf->snapshot_multicast_addr.c_str());
		mreq.imr_interface.s_addr = inet_addr(local_conf->interfacea_Addr.c_str());
		if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (const char*)&mreq, sizeof(mreq)) == SOCKET_ERROR) {
			std::cout << "setsockopt IP_ADD_MEMBERSHIP failed with error: " << WSAGetLastError() << std::endl;
			closesocket(fd);
			WSACleanup();
			return;
		}

		std::cout << "Successfully created socket IP: " << local_conf->snapshot_multicast_addr << ", PORT: " << local_conf->snapshot_multicast_port << " & INTERFACE: " << local_conf->interfacea_Addr << std::endl;
		LOG_INFO << "Successfully created socket IP: " << local_conf->snapshot_multicast_addr << ", PORT: " << local_conf->snapshot_multicast_port << " & INTERFACE: " << local_conf->interfacea_Addr;

		// Main receiving loop
		while (true) {
			//char msgbuf[max_length];
			int addrlen = sizeof(addr);
			int nbytes = recvfrom(fd, data_, max_length, 0, (struct sockaddr*)&addr, &addrlen);

			if (nbytes == SOCKET_ERROR) {
				std::cerr << "recvfrom failed with error: " << WSAGetLastError() << std::endl;
				closesocket(fd);
				WSACleanup();
				return;
			}

			// Process received packet
			ptr = new char[nbytes];
			memcpy(ptr, data_, nbytes);

			MessagePacket msgPacket;
			std::memcpy(&msgPacket.msgType, ptr, sizeof(int32_t));
			msgPacket.msgType = ntohl(msgPacket.msgType);
			msgPacket.length = nbytes;
			msgPacket.msg = ptr;

			multicast_q.push(msgPacket);
			//std::cout << multicast_q.size() <<" : "<<msgPacket.msgType << " ";
		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
}

//void FIN::MulticastReceiver::init()
//{
//    try {
//        boost::asio::ip::udp::endpoint listen_endpoint(boost::asio::ip::address::from_string(local_conf->interfacea_Addr), local_conf->snapshot_multicast_port);
//        socket.open(listen_endpoint.protocol());
//        socket.set_option(boost::asio::ip::udp::socket::reuse_address(true));
//        socket.bind(listen_endpoint);
//
//        // Join the multicast group.
//        socket.set_option(
//            boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(local_conf->snapshot_multicast_addr)));
//
//        std::cout << "SNAP SHOT MULTICAST DETAILS " << local_conf->snapshot_multicast_addr << ", " << local_conf->snapshot_multicast_port<<", "<<local_conf->interfacea_Addr << std::endl;
//        LOG_INFO << "SNAP SHOT MULTICAST DETAILS " << local_conf->snapshot_multicast_addr << ", " << local_conf->snapshot_multicast_port << ", " << local_conf->interfacea_Addr;
//
//        // If reached here without exception, socket initialization successful
//        std::cout << "Socket connection successful." << std::endl;
//    }
//    catch (const std::exception& e) {
//        // Print error message if any exception occurred during socket initialization
//        std::cerr << "Error initializing socket: " << e.what() << std::endl;
//    }
//}


