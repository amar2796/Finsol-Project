#pragma once
#ifndef FIN_COMMON_UNICAST_RCVR
#define FIN_COMMON_UNICAST_RCVR

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#undef _WINSOCKAPI_   // Force winsock2.h to be included instead of winsock.h
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

#include <iostream>

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

		int _port;
		enum { max_length = 1024 };
		char data_[max_length];
	};
}
#endif
