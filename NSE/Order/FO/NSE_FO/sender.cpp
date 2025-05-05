#include "sender.h"

namespace FIN {

	UnicastSenderNative::UnicastSenderNative(const std::string& address, int32_t port)
	{
		if (_sock == -1)
		{
			try
			{
				_sock = socket(AF_INET, SOCK_DGRAM, 0);
			}
			catch (std::exception& e)
			{
				std::cout << e.what() << "\n";
			}
		}

		std::cout << "******************************" << std::endl;
		std::cout << "Configuring UDP Unicast Sender" << std::endl;
		std::cout << "Address : " << address << std::endl;
		std::cout << "Port    : " << port << std::endl;
		std::cout << "******************************" << std::endl;
		memset(&_serverAddr, '0', sizeof(_serverAddr));
		_serverAddr.sin_family = AF_INET;
		_serverAddr.sin_port = htons(port);
		_serverAddr.sin_addr.s_addr = inet_addr(address.c_str());

		

		try
		{
			if (connect(_sock, (struct sockaddr*)&_serverAddr, sizeof(_serverAddr)) < 0)
			{
				perror("bind failed");
				exit(EXIT_FAILURE);
			}
			_isConnected = true;

		}
		catch (std::exception& e)
		{
			std::cout << e.what() << "\n";
		}
	}

	void UnicastSenderNative::send(const char* ptr, int32_t size)
	{
		try {

			if (_isConnected)
			{
				sendto(_sock, ptr, size, 0, (struct sockaddr*)NULL, sizeof(_serverAddr));
			}
			else
			{
				DEVLOG("sender is not connected \n");
			}
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << "\n";
		}

	}

}
