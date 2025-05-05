#ifndef FIN_COMMON_TCP_SERVER_H
#define FIN_COMMON_TCP_SERVER_H
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>     
#include <stdlib.h>
#include <iostream>  
#include <string.h>
#include <string>
#include<map>
#include <thread>
#ifdef _WIN32
#include<WinSock2.h>
#endif
#include <ctime>
#include <sstream>
#include "logger.h"
#include "defines.h"

namespace FIN {

	class tcpServer
	{

	public:


		virtual void readData(SOCKET&, char[], int) = 0; // please give the defination in derived class.

		void socketCreation(int port)
		{
			try
			{
				_sock = socket(AF_INET, SOCK_STREAM, 0);
				if (_sock == -1)
				{
					(setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)));
				}
			}

			catch (std::exception& e)
			{
				FIN_ERROR(e.what());
			}

			if (_sock > 0)
			{
				std::cout << "socket Created\n";
				memset(&_serverAddr, '0', sizeof(_serverAddr));
				_serverAddr.sin_family = AF_INET;
				_serverAddr.sin_port = htons(port);
				_serverAddr.sin_addr.s_addr = INADDR_ANY;
				socketBinding();
			}
		}


	private:

		void socketBinding()
		{

			try
			{

				if (bind(_sock, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) >= 0)
				{
					std::cout << "socket Binded successfully\n";
					acceptConnection();
				}

			}
			catch (std::exception& e)
			{
				FIN_ERROR(e.what());
			}
		}
	public:
		void acceptConnection()
		{

			if (listen(_sock, 3) < 0)
			{
				perror("listen");
				exit(EXIT_FAILURE);
			}

			try
			{
				new_socket = accept(_sock, (struct sockaddr *)&_serverAddr, (int*)&addrlen);
				if (new_socket >= 0)
				{
					std::cout << "connection accepted from Process Monitor\n";
					BOOST_LOG_TRIVIAL(info) << "connection accepted from Process Monitor";
					readData(new_socket, buffer, sizeof(buffer));
				}
			}
			catch (std::exception& e)
			{
				FIN_ERROR(e.what());
			}
		}



	private:

		char buffer[65535];
		SOCKET _sock = -1, new_socket;
		char  opt = 1;
		struct sockaddr_in _serverAddr;
		int addrlen = sizeof(_serverAddr);

	};
}

#endif