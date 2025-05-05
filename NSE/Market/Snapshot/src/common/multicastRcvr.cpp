#include "multicastRcvr.h"
#include <iostream>
namespace FIN
{
	MulticastRcvr::MulticastRcvr()/*: data(1)*/
	{
	}

	void MulticastRcvr::init(std::string multicast, std::string nic_add, int port)
	{

		nic_card = nic_add;
		this->multicast_add = multicast;
		this->m_port = port;

		WSADATA wsaData;
		if (WSAStartup(0x0101, &wsaData)) {
			perror("WSAStartup");
			LOG_INFO << "WSAStartup error " << WSAGetLastError();
			return;
		}
		/*std::thread t(&MulticastRcvr::start_Receiver, this);
		t.detach();*/
		LOG_INFO << "MULTICAST CRED : IP " << multicast << " PORT " << port << " INTERFACE " << nic_add;
		std::cout << "MULTICAST CRED : IP " << multicast << " PORT " << port << " INTERFACE " << nic_add << std::endl;;
		start_Receiver();
	}

	void MulticastRcvr::start_Receiver()

	{

		int fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (fd < 0) {
			perror("socket");
			LOG_INFO << "ERROR WHILE CREATING SOCKET " << WSAGetLastError();

			return;
		}


		u_int yes = 1;

		if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(yes)) < 0) {
			perror("Reusing ADDR failed");
			LOG_INFO << "Reusing ADDR failed " << WSAGetLastError();
			return;
		}


		struct sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = htonl(INADDR_ANY); // differs from sender
		addr.sin_port = htons(m_port);


		if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
			perror("bind");
			LOG_INFO << "BIND ERROR " << WSAGetLastError();
			//fprintf(stderr, "bind: %d\n", errno);
			return;
		}

		struct ip_mreq mreq {};
		mreq.imr_multiaddr.s_addr = inet_addr(multicast_add.c_str());
		mreq.imr_interface.s_addr = inet_addr(nic_card.c_str());
		if (
			setsockopt(
				fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&mreq, sizeof(mreq)
			) < 0
			) {
			std::cout << WSAGetLastError() << std::endl;
			LOG_INFO << "SET SOCKET OPTION " << WSAGetLastError();
			return;
		}


		char msgbuf[max_length];
		int addrlen = sizeof(addr);
		LOG_INFO << "START RECVING DATA ";
		std::cout << "START RECVING DATA FROM MULTICAST " << std::endl;
		while (1) {
			

			try {
				int nbytes = recvfrom(fd, msgbuf, max_length, 0, (struct sockaddr*)&addr, &addrlen);
				char* message = new char[nbytes];
				std::memcpy(message, msgbuf, nbytes);
				if (!queue.push(message)) 
				{
					delete[] message;  
				}
				if (consumer_waiting)
				{
					{
						std::lock_guard<std::mutex> lock(cv_mutex);
						consumer_waiting = false;  // Reset the flag
					}
					cv.notify_one();  // Notify the waiting consumer
				}
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				std::cout << "Exception in multicast receiving\n";
				LOG_INFO << "Exception in multicast receiving\n";
				break;
			}
		}
	}
}
