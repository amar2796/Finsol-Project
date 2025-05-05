#ifndef FIN_MULTICAST_RECEIVER
#define FIN_MULTICAST_RECEIVER
#ifdef _WIN32
#include <Winsock2.h> // before Windows.h, else Winsock 1 conflict
#include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
#include <Windows.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#pragma warning( disable : 4996);
#pragma comment(lib, "Ws2_32.lib")
#include "defines.h"
#include "threadSafeQueue.h"
#include "Nanolog.hpp"
#include "color.h"

namespace FIN
{
	namespace IIBX
	{
		class MulticastRcvr
		{
		public:
			MulticastRcvr() {

			};
			//MulticastRcvr(boost::asio::io_service& io_service,UINT16 exchgId);			
			MulticastRcvr(UINT_16 exchgId);			
			ThreadSafeQueue<std::string> multicast_q;		
			void init(std::string ip, int port, std::string interface_ip);
			void start_Receiver(std::string ip, int port, std::string interface_ip);
			enum { max_length = 70000 };
			
			char dest[3000];
			char* ptr=NULL;
			int m_port;
		};
	}
}

#endif