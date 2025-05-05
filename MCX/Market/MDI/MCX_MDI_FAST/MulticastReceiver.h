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
#include <boost/lockfree/spsc_queue.hpp>
namespace FIN
{
	class MessagePacket
	{
	public:
		char *msg;
		int len;

	};
	class MulticastReceiver
	{
	public:
		MulticastReceiver(boost::asio::io_service& io_service, Config_Details* conf_det);
		FIN::ThreadSafeQueue<MessagePacket> multicast_q;
		//boost::lockfree::spsc_queue<MessagePacket, boost::lockfree::capacity<50000>> multicast_q;
		std::mutex cv_mutex;
		std::condition_variable cv;
		std::atomic<bool> consumer_waiting = false;  // Flag to track if the consumer is waiting

		void start_Reciver();
		void init();
	private :
		
		boost::asio::ip::udp::socket socket;
		boost::asio::ip::udp::endpoint sender_endpoint;
		enum { max_length = 1500 };
		char data_[max_length];
		char dest[3000];
		char* ptr;
		Config_Details* local_conf;


	private:

	};
}


#endif
