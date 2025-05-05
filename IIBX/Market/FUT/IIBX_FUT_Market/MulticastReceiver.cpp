#include "MulticastReceiver.h"
namespace FIN
{
	namespace IIBX
	{
		
		MulticastRcvr::MulticastRcvr(UINT_16 exchgId) 
		{
			
		}
		void MulticastRcvr::init(std::string ip, int port, std::string interface_ip)
		{
		
			std::thread t(&MulticastRcvr::start_Receiver, this, ip, port, interface_ip);

			t.detach();
		}

		void MulticastRcvr::start_Receiver(std::string multicast_ip, int port, std::string interface_ip)
		{
			std::cout << "Joining multicast group, IP : " << multicast_ip << " PORT : " << port << " WITH INTERFACE " << interface_ip << std::endl;
			// Initialize Winsock
			WSADATA wsaData;
			if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
				std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
				return;
			}

			// Create a UDP socket
			SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
			if (sock == INVALID_SOCKET) {
				FIN::setConsoleColor(4);
				std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
				FIN::setConsoleColor(7);
				WSACleanup();
				return;
			}

			// Allow multiple sockets to use the same PORT number
			BOOL reuse = TRUE;
			if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, sizeof(reuse)) < 0) {
				std::cerr << "Setting SO_REUSEADDR failed with error: " << WSAGetLastError() << std::endl;
				closesocket(sock);
				WSACleanup();
				return;
			}

			// Bind the socket to the local address and port
			sockaddr_in localAddr;
			memset(&localAddr, 0, sizeof(localAddr));
			localAddr.sin_family = AF_INET;
			localAddr.sin_port = htons(port);
			localAddr.sin_addr.s_addr = INADDR_ANY;

			if (bind(sock, (struct sockaddr*)&localAddr, sizeof(localAddr)) == SOCKET_ERROR) {
				FIN::setConsoleColor(4);
				std::cerr << "Binding socket failed with error: " << WSAGetLastError() << std::endl;
				FIN::setConsoleColor(7);
				closesocket(sock);
				WSACleanup();
				return;
			}

			// Set up the multicast group address structure
			ip_mreq multicastRequest;
			multicastRequest.imr_multiaddr.s_addr = inet_addr(multicast_ip.c_str());
			multicastRequest.imr_interface.s_addr = inet_addr(interface_ip.c_str());

			// Join the multicast group
			if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&multicastRequest, sizeof(multicastRequest)) == SOCKET_ERROR) {
				FIN::setConsoleColor(4);
				std::cerr << "Joining multicast group failed with error: " << WSAGetLastError() << std::endl;
				FIN::setConsoleColor(7);
				closesocket(sock);
				WSACleanup();
				return;
			}

			FIN::setConsoleColor(2);
			std::cout << "Successfully joined multicast group " << multicast_ip << " on port " << port << std::endl;
			FIN::setConsoleColor(7);

			// Receive data loop
			char msgbuf[1024];
			sockaddr_in senderAddr;
			int senderAddrSize = sizeof(senderAddr);

			while (true) 
			{
				int nbytes = recvfrom(sock, msgbuf, sizeof(msgbuf), 0, (sockaddr*)&senderAddr, &senderAddrSize);
				if (nbytes < 0) {
					FIN::setConsoleColor(4);
					std::cerr << "recvfrom failed with error: " << WSAGetLastError() << std::endl;
					FIN::setConsoleColor(7);
					break;
				}

				std::string str(msgbuf, nbytes);
				/*LOG_INFO << "Received message: " << str;
				std::cout << "Received message: " << str << std::endl;*/

				multicast_q.push(str);
			}

			// Leave the multicast group
			if (setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char*)&multicastRequest, sizeof(multicastRequest)) == SOCKET_ERROR) {
				std::cerr << "Leaving multicast group failed with error: " << WSAGetLastError() << std::endl;
			}

			// Close the socket and clean up Winsock
			closesocket(sock);
			WSACleanup();
		}
	}
}