#ifndef process_Monitor
#define process_Monitor

#include <iostream>
#include <unordered_set>
#include <string>
#include <boost/asio.hpp>
#include <fstream>
#include <WinSock2.h>
#include <string>

namespace FIN
{
	
	class LockFreeSet
	{
	public:

		std::unordered_set <std::string> set1;
		std::unordered_set <std::string> set2;
		std::unordered_set <std::string> set3;
	};

	typedef std::unordered_set <std::string> symbolsSet;
	enum { max_length = 1024 };
	
	class processMonitorUpdate
	{
	public:
		LockFreeSet _sets;
		processMonitorUpdate(FIN::LockFreeSet &SET)
		{
			_sets = SET;
			std::cout << "processMonitor constructor called\n";
		}

		void init()
		{
			ConfigReader _confReader;
			_confReader.loadData(FIN_CONST::FIN_CONFIG_FILE);
			_confReader.loadData(FIN_CONST::FIN_CONFIG_FILE);
			fileName = _confReader.getValue < std::string >(
				FIN_CONST::MD_SUBSCRIPTION + FIN_CONST::SEP_DOT + FIN_CONST::FILE_NAME,
				FIN_CONST::EMPTY_STRING
				);
			registerFile();
		}

		void registerFile()
		{
			std::string line;
			std::ifstream myfile(fileName);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					_sets.set1.insert(line);
					_sets.set2.insert(line);
					_sets.set3.insert(line);
				}
				for (auto itr = _sets.set3.begin(); itr != _sets.set3.end(); itr++)
				{
					std::cout << "symbols\t" << *itr << "\n";
				}
				myfile.close();
			}
		}

		bool isSymbolSuscribed(const std::string &symbol)
		{
			if (Reader->empty() || Reader->find(symbol) == Reader->end())
			{
				return false;
			}
			else
			{
				return true;
			}
		}

		void isupdate(const std::string &symbol)
		{
			if (Writer == &_sets.set2)
			{
				Writer->insert(symbol);
				_sets.set3.insert(symbol);
				Reader = &_sets.set2;
				Writer = &_sets.set3;
				_sets.set1 = _sets.set3;
			}
			else if (Writer == &_sets.set3)
			{
				Writer->insert(symbol);
				_sets.set1.insert(symbol);
				Reader = &_sets.set3;
				Writer = &_sets.set1;
				_sets.set2 = _sets.set1;
			}
			else
			{
				Writer->insert(symbol);
				_sets.set2.insert(symbol);
				Reader = &_sets.set1;
				Writer = &_sets.set2;
				_sets.set3 = _sets.set2;
			}

		}
	
		void fileUpdate(const std::string &symbol)
		{
			std::fstream file;
			file.open(fileName, std::fstream::in | std::fstream::out | std::fstream::app);
			if (!(file.is_open()))
			{
				std::cout << "Error in opening file /conf/SymbolVolumeFile.txt\n";
				exit(0);
			}
			file << symbol << std::endl;
			file.close();
			sendMarketDataRequest();

		}

		virtual void sendMarketDataRequest() = 0;

	private:
		std::string fileName;
		//symbolsSet set1 ;
		//symbolsSet set2 ;
		//symbolsSet set3 = { "abc" };
		symbolsSet* Reader = &_sets.set1;
		symbolsSet* Writer = &_sets.set2;
	};

	class tcpSocket
	{
	public:

		tcpSocket(boost::asio::io_service &ioService)
			: _ioService(ioService),
			_socket(_ioService)
		{
			std::cout << "tcpsocket constructor called\n";
		}

		void connect()
		{
			try
			{
				_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 23347));
			}

			catch (std::exception &e)
			{
				std::cout << "Got Exception : " << e.what() << std::endl;
			}

		}

	protected:

		boost::asio::io_service &_ioService;
		boost::asio::ip::tcp::socket _socket;
	};

	class TcpRcvr : public tcpSocket, public processMonitorUpdate
	{
	public:
		TcpRcvr(boost::asio::io_service &ioService, FIN::LockFreeSet &SET)
			: tcpSocket(ioService),processMonitorUpdate(SET)
		{
			init();
			std::cout << "TcpRcvr constructor called\n";
		}

		void handler(const boost::system::error_code& error, std::size_t bytes_transferred)
		{
			if (!error)
			{
			
				unsigned length = strlen(data);
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
						second = i;
						break;
					}

				}
				std::string str(data);
				std::string symbol = str.substr((first)+3, second - first - 3);
				fileUpdate(symbol);
				isupdate(symbol);
				_socket.async_receive(boost::asio::buffer(data,512),0, boost::bind(&TcpRcvr::handler, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
			}
			else
			{
				std::cout << "error in Receiving Data\n";
			}
		}

		void readline()
		{
			std::cout << "Waiting to read line" << std::endl;
			_socket.async_receive(boost::asio::buffer(data,512),0, boost::bind(&TcpRcvr::handler, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
			std::cout << "outside handler\n";
			unsigned length = strlen(data);
			char str[1000];
			for (unsigned i = 0; i < length; i++)
			{
				str[i] = data[i];
			}
			
		}
	private:
		char data[max_length];
		unsigned first;
		unsigned second;
	};
}

#endif