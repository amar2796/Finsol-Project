#pragma once
#ifndef FIN_MD_NSETBT_SENDER
#define FIN_MD_NSETBT_SENDER
#include <Windows.h>
//#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#include "defines.h"
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "abstractDataSender.h"

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
