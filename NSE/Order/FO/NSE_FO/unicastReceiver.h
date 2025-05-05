#pragma once

#ifndef FIN_COMMON_UNICAST_RCVR
#define FIN_COMMON_UNICAST_RCVR

#include <iostream>
#include <boost/array.hpp>
//#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

//#define WINSOCK_H
//#include <WinSock2.h>

namespace FIN
{
	class UnicastRcvr
	{
	public:
		UnicastRcvr();
		void configure(int port)
		{
			_port = port;
		}
		void startRead();
		virtual void process(char* data, size_t size) = 0;
	private:

		void handle_receive_from(const boost::system::error_code& error,
			size_t bytes_recvd);

		int _port;
		enum { max_length = 1024 };
		char data_[max_length];
	};
}
#endif
