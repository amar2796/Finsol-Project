#include <iostream>
#include <unordered_set>
#include <string>
#include <boost/asio.hpp>
#include <fstream>
#include <WinSock2.h>


namespace FIN
{

	typedef std::unordered_set <std::string> symbolsSet;



	class processMonitorUpdate
	{
	public:
		bool isSymbolSuscribed(const std::string &symbol)
		{
			if (Reader->empty() || Reader->find(symbol) == Reader->end())
			{
				std::cout << "symbol " << symbol << " not found\n";
				return false;
			}
			else
			{
				std::cout << "symbol " << symbol << " found\n";
				return true;
			}
		}

		void isupdate(const std::string &symbol)
		{
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

		}

		void fileUpdate(const std::string &symbol)
		{
			std::fstream file;
			file.open("suscribedSymbol.txt", std::fstream::in | std::fstream::out | std::fstream::app);
			if (!(file.is_open()))
			{
				std::cout << "Error in opening file /conf/SymbolVolumeFile.txt\n";
				exit(0);
			}
			//std::cout << "inserting into file\n";
			file << symbol << std::endl;
			file.close();

		}

	private:
		symbolsSet set1 = { "abc" };
		symbolsSet set2 = { "abc" };
		symbolsSet set3 = { "abc" };
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
			std::cout << "tcpsocket constructor called\n";
		}

		void connect()
		{
			try
			{
				std::cout << "came for connection\n";
				_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 23347));
				std::cout << "Connected to 127.0.0.1:" << "port:23347" << std::endl;
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
		TcpRcvr(boost::asio::io_service &ioService)
			: tcpSocket(ioService)
		{
			std::cout << "TcpRcvr constructor called\n";
		}

		void readline()
		{
			while (1)
			{
				std::cout << "Waiting to read line" << std::endl;
				boost::asio::streambuf buf;
				std::string lastKey = 1 + "10=";
				//std::string lastKey = "fnf";
				boost::asio::read_until(_socket, buf, lastKey);
				//std::cout << "shubham\n";
				std::string data = boost::asio::buffer_cast<const char*>(buf.data());
				unsigned length = data.length();
				std::cout << "length:" << length << "\n";
				char str[1000];
				for (unsigned i = 0; i < length; i++)
				{
					str[i] = data[i];
				}
				unsigned first = data.find("48=");
				std::cout << "first:" << first << "\n";
				unsigned second;
				char sep = (char)1;
				std::cout << sep << "\n";
				for (unsigned i = 0; i < length; i++)
				{
					if (i == first)
					{
						while (str[i] != sep)
						{
							i++;
						}
						std::cout << "\n";
						second = i;
						std::cout << "second" << second << "\n";
						break;
					}

				}
				//std::cout << "data:" << data << "\n";
				//unsigned first = data.find("48=");
				//std::cout << "first:" << first << "\n";
				//std::cout << "second:" << second << "\n";
				//std::istringstream f(data.c_str());
				//std::string s;
				//int pos = 0;
				//char keyvalueseparation = (char)1;
				//while (getline(f, s,keyvalueseparation)) {
					//pos = s.find("48=");

				//}
				//char sep = (char)1;
				//unsigned second = data.find(sep); 
				std::string symbol = data.substr((first)+3, second - first - 3);
				std::cout << "RESP : " << symbol << std::endl;
				//std::cout << "\n\n\n\n";
				//Sleep(3000);

				if (!isSymbolSuscribed("PBFCEPSR3M"))
				{
					fileUpdate(symbol);
					isupdate(symbol);
				}
				Sleep(3000);
				isSymbolSuscribed("PBFCEPSR3M");
			}
		}
	};
}


