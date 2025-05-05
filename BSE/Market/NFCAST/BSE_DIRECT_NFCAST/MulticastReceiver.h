#ifndef MULTICASTRECEIVER_H
#define MULTICASTRECEIVER_H
//#include <chrono>
#include "ThreadSafeQueue.h"
#include <iostream>
#include "constants.h"
#include "Nanolog.hpp"
#include <Winsock2.h> // before Windows.h, else Winsock 1 conflict
#include <Ws2tcpip.h> // needed for ip_mreq definition for multicast
#include <Windows.h>  // Then include Windows.h



namespace FIN
{
	struct MessagePacket
	{
	public:
		int32_t msgType;
		char *msg;
		int32_t length;
	};


	struct MessageType
	{
		int32_t msgType;
	};
	

	class MulticastReceiver
	{
	public:
		MulticastReceiver(Config_Details* conf_det);
		ThreadSafeQueue<MessagePacket> multicast_q;
		void start_Reciver();
		void init();
	private :
		
		enum { max_length = 2000 };
		char data_[max_length];
		char dest[3000];
		char* ptr;
		Config_Details* local_conf;


	private:
		SOCKET socket_fd;
	};
}


#endif
