#ifndef FIN_MULTICAST_RCVER_DATA
#define FIN_MULTICAST_RCVER_DATA
#include <boost/shared_ptr.hpp>
#include<boost/asio/io_service.hpp>
#include<boost/asio/io_service.hpp>
#include <boost\asio\ip\address.hpp>
#include<boost/asio/ip/udp.hpp>
#include <boost\asio\ip\multicast.hpp>
#include <boost\asio\placeholders.hpp>
#include <chrono>
#include "ThreadSafeQueue.h"
#include <iostream>
#include "constants.h"
//#include <WinSock2.h>
//#include <WS2tcpip.h>
namespace FIN
{
	class MessagePacket
	{
	public:
		char* msg;
		int len;

	};
	class MulticastReceiver
	{
	public:
		MulticastReceiver(Config_Details* conf_det);
		ThreadSafeQueue<MessagePacket> multicast_q;
		void start_Reciver();
		void init();
	private:

		//	boost::asio::ip::udp::socket socket;
		//boost::asio::ip::udp::endpoint sender_endpoint;
		SOCKET recvSocket;
		enum { max_length = 7000 };
		char data_[max_length];
		//char dest[3000];
		//char* ptr;
		Config_Details* local_conf;
		u_int yes = 1;




	};
}


#endif
