#ifndef FIN_COMMON_MULTICAST_RCVR
#define FIN_COMMON_MULTICAST_RCVR

#include "marketDataMessage.h"
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include "common.h"
//#include "threadSafeQueue.h"
#include <boost/lockfree/spsc_queue.hpp>
#pragma warning( disable : 4996);
#pragma comment(lib, "Ws2_32.lib")
namespace FIN
{
	class MulticastRcvr
	{
	public:
		MulticastRcvr();
		
		void init(std::string ip, std::string nic_add, int port);
		void start_Receiver();
		
		boost::lockfree::spsc_queue<char*, boost::lockfree::capacity<1024>> queue;
		std::mutex cv_mutex;
		std::condition_variable cv;
		std::atomic<bool> consumer_waiting = false;  // Flag to track if the consumer is waiting

		//virtual void process(char* data, size_t size) = 0;
	private:
		std::string multicast_add;
		std::string nic_card;
		int m_port;
		enum { max_length = 1500 };
		char dest[3000];
		char* ptr;
	};


}
#endif
