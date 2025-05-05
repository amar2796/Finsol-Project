#pragma once
#ifndef FIN_MD_NSETBT_SENDER
#define FIN_MD_NSETBT_SENDER
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#undef _WINSOCKAPI_   // Force winsock2.h to be included instead of winsock.h
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

#include <cstdint>
#include <string>
#include <stdexcept>
#include <iostream>
#include "defines.h"

#pragma comment(lib, "Ws2_32.lib")  // Link with Winsock library

namespace FIN {

	class UnicastSenderNative
	{

	public:
		/*!
		  default public constructor of UnicastSender structure.
		  */
		UnicastSenderNative(const std::string& address, int32_t port);
		void send(const char*, int32_t);

	private:

		int32_t  _sock = -1;
		int32_t  _count = 1;
		struct sockaddr_in  _serverAddr;
		struct sockaddr_in  _clientAddr;
		bool _isConnected = false;

	};

}

#endif
