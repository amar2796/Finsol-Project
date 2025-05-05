#ifndef FIN_PROCESS_MONITOR_H
#define FIN_PROCESS_MONITOR_H

#include <iostream>
#include <unordered_set>
#include <string>
#include <boost/asio.hpp>
#include <fstream>
#include <WinSock2.h>
#include "configReader.h"
namespace FIN
{
	class ProcessMonitor
	{
		public:

			void init(const std::string &config );
			void start();
			virtual void handlePmON(const std::string &message) = 0;
			virtual void handlePmOFF(const std::string &message) = 0;

		private:
			void connect();
			void read();
	};

	class TcpSocket
	{
	public:

		TcpSocket(boost::asio::io_service &ioService)
			: _ioService(ioService),
			_socket(_ioService)
		{
		}

		void connect()
		{
			/*
			try
			{
				//std::cout << "came for connection\n";
				_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 23347));
				std::cout << "Connected to 127.0.0.1:" << "port:23347" << std::endl;
				//Sleep(5000);
			}
			catch (std::exception &e)
			{
				std::cout << "Got Exception : " << e.what() << std::endl;
			}
			*/
		}

	protected:

		boost::asio::io_service &_ioService;
		boost::asio::ip::tcp::socket _socket;
	};

	class TcpRcvr : public TcpSocket, public ProcessMonitor
	{
	public:
		//boost::asio::streambuf buf;
		TcpRcvr(boost::asio::io_service &ioService)
			: TcpSocket(ioService)
		{
			/*
			init();
			std::cout << "TcpRcvr constructor called\n";
			*/
		}

		void handler()
		{
			/*
			//std::cout << "shubham *******##########################\n";
			//processUpdateSymbol(data, bytes_transferred);
			unsigned length = data.length();
			//std::cout << "length:" << length << "\n";
			//std::cout << "In handle function\n";
			//Sleep(2000);
			//char str[1000];
			//for (unsigned i = 0; i < length; i++)
			//{
				//str[i] = data[i];
			//}
			char sep = (char)1;
			for (unsigned i = 0; i < length; i++)
			{
				if (data[i] == '4'&&data[i+1]=='8'&&data[i+2] == '=')
				{
					first = i;
					while (data[i] != sep)
					{
						i++;
					}
					//std::cout << "\n";
					second = i;
					//std::cout << "second" << second << "\n";
					break;
				}

			}
			//std::cout << "first:" << first << "\n";
			//std::cout << "second:" << second << "\n";

			std::string str(data);

			std::string symbol = str.substr((first)+3, second - first - 3);
			//std::cout << "RESP : " << symbol << std::endl;
			//std::cout << "\n\n\n\n";
			//Sleep(3000);

			if (!isSymbolSuscribed(symbol))
			{
				fileUpdate(symbol);
				isupdate(symbol);
			////fileUpdate(data);
			////isupdate(data);
			}
			Sleep(2000);


			/*_socket.async_receive(boost::asio::buffer(data,512),0, boost::bind(&TcpRcvr::read_handler, this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
			*/
			
			/*else
			{
				std::cout << "error in Receiving Data\n";
			}*/
			
		}

		void readline()
		{
			/*
			std::cout << "Waiting to read line" << std::endl;
			boost::asio::streambuf buf;
			std::string lastKey = 1 + "10=";
			//_socket.async_receive(boost::asio::buffer(buf, 512),0,read_handler);
			//std::string lastKey = "fnf";
			boost::asio::read_until(_socket, buf, lastKey);
			//std::future<std::size_t> len = _socket.async_read_some(buf, boost::asio::use_future);
			//_socket.non_blocking(true);
			//_socket.async_read_some(null_buffers(), read_handler);
			//std::string data = (const char*)buf;
			//std::cout << "shubham\n";
			//data = { NULL };
			//memset(&data[0], 0, sizeof(data));

			//std::cout << "before receive data from process monitor\n";
			/*_socket.async_receive(boost::asio::buffer(data,max_length),0, boost::bind(&TcpRcvr::read_handler, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
				*/
			//Sleep(10000);
			/*std::string*/// TEST -> AYUSH data = boost::asio::buffer_cast<const char*>(buf.data());
			// TEST-> AYUSH //handler();
			/*std::cout << "blocking Call\n";
			Sleep(2000);
			//processUpdateSymbol(data, bytes_transferred);
			unsigned length = data.length();
			std::cout << data << "\n";
			std::cout << "length:" << length << "\n";
			Sleep(3000);*/
		}
	private:
		//char data[max_length];
		std::string data;
		unsigned first;
		unsigned second;
	};
}

/*int main()
{
	//processMonitorUpdate p1;
	//p1.isSymbolSuscribed("abc");
	//p1.isSymbolSuscribed("cdf");
	//p1.isupdate("cdf");
	//p1.isSymbolSuscribed("cdf");

	boost::asio::io_service ioService;

	while (1)
	{
		TcpRcvr symbolrcv(ioService);
		symbolrcv.connect();
		symbolrcv.readline();
		while (1)
		{
			std::cout << "shubham\n";
			Sleep(10000);
			symbolrcv.isSymbolSuscribed("PBFCEPSR3M");
		}
	}


	return 0;
}*/
#endif