#ifndef process_Monitor
#define process_Monitor

#include <iostream>
#include <unordered_set>
#include <string>
#include <boost/asio.hpp>
#include <fstream>
#include <WinSock2.h>
#include "configReader.h"
//#include "NseFoMarketData.h
#include "../marketDataGw/cme/fix44.h"

namespace FIN
{

	typedef std::unordered_set <std::string> symbolsSet;
	enum { max_length = 229 };



	class processMonitorUpdate
	{
	public:

		void init()
		{
			/*
			ConfigReader _confReader;
			_confReader.loadData(FIN_CONST::FIN_CONFIG_FILE);
			*/
			/*std::string fileName = _confReader.getValue < std::string >(
				"NSE-MULTICAST-RECEIVER.suscribedSymbolStoreFile",
				FIN_CONST::EMPTY_STRING
				);*/
			/*
			_confReader.loadData(FIN_CONST::FIN_CONFIG_FILE);
			fileName = _confReader.getValue < std::string >(
				FIN_CONST::MD_SUBSCRIPTION + FIN_CONST::SEP_DOT + FIN_CONST::FILE_NAME,
				FIN_CONST::EMPTY_STRING
				);
			registerFile();
			*/
		}

		void registerFile()
		{
			/*
			//m_fileName = fileName;
			std::string line;
			std::ifstream myfile(fileName);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					//m_symbolsInFile.insert(line);
					set1.insert(line);
					set2.insert(line);
					set3.insert(line);
				}
				myfile.
				();
			}
			*/
		}

		bool isSymbolSuscribed(const std::string &symbol)
		{
			/*
			if (Reader->empty() || Reader->find(symbol) == Reader->end())
			{
				std::cout << "symbol " << symbol << " not found\n";
				return false;
			}
			else
			{
				//std::cout << "symbol " << symbol << " found\n";
				return true;
			}
			*/
			return true;
		}

		void isupdate(const std::string &symbol)
		{
			/*
			if (Writer == &set2)
			{
				Writer->insert(symbol);
				set3.insert(symbol);
				Reader = &set2;
				Writer = &set3;
				set1 = set3;
			}
			else if (Writer == &set3)
			{
				Writer->insert(symbol);
				set1.insert(symbol);
				Reader = &set3;
				Writer = &set1;
				set2 = set1;
			}
			else
			{
				Writer->insert(symbol);
				set2.insert(symbol);
				Reader = &set1;
				Writer = &set2;
				set3 = set2;
			}
			sendMarketDataRequest(symbol);
			*/
		}

		void fileUpdate(const std::string &symbol)
		{
			/*
			//std::cout << "filename:" << fileName << "\n";
			//registerFile(fileName);
			std::fstream file;
			file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::app);
			if (!(file.is_open()))
			{
				std::cout << "Error in opening file /conf/SymbolVolumeFile.txt\n";
				Sleep(10000);
				exit(0); 
			}
			//std::cout << "inserting into file\n";
			file << std::endl;
			file << symbol;
			file.close();
			*/
		
			
			
		}

		virtual void sendMarketDataRequest(const std::string &symbol) = 0;

	private:
		std::string fileName;
		symbolsSet set1 ;
		symbolsSet set2 ;
		symbolsSet set3 ;
		symbolsSet* Reader = &set1;
		symbolsSet* Writer = &set2;
		
	};

	class tcpSocket
	{
	public:

		tcpSocket(boost::asio::io_service &ioService)
			: _ioService(ioService),
			_socket(_ioService)
		{
			//std::cout << "tcpsocket constructor called\n";
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

	class TcpRcvr : public tcpSocket, public processMonitorUpdate
	{
	public:
		//boost::asio::streambuf buf;
		TcpRcvr(boost::asio::io_service &ioService)
			: tcpSocket(ioService)
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