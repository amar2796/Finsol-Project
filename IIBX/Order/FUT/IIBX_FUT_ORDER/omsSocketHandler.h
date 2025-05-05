#ifndef FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H
#define FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H

#include <boost/asio.hpp>
#include "threadSafeQueue.h"
#include <iostream>
#include "color.h"

namespace FIN {

	class OmsAcceptor
	{
	public:
		OmsAcceptor(boost::asio::io_service& ioService)
			:_ioService(ioService)
		{
			_acc = nullptr;
			_socket = nullptr;
		}

		void init(int port)
		{
			if (_acc != nullptr)
				delete _acc;
			if (_socket != nullptr)
				delete _socket;

			_port = port;
			_acc = new boost::asio::ip::tcp::acceptor(_ioService, boost::asio::ip::tcp::endpoint(
				boost::asio::ip::tcp::v4(), port));

			_socket = new boost::asio::ip::tcp::socket(_ioService);
		}

		void accept()
		{
			init(_port);
			std::cout << "Waiting for OMS Connection [Port]: "
				<< _port << std::endl;
			_acc->accept(*_socket);
			FIN::setConsoleColor(2);
			std::cout << "OMS Connected [Port]: " << _port << std::endl;
			FIN::setConsoleColor(7);
		}
		void close()
		{
			_socket->close();
	
		}
		void closeSocketConnection()
		{
			_socket->close();
			_acc->close();
		}

	protected:
		int _port;
		boost::asio::ip::tcp::acceptor* _acc;
		boost::asio::ip::tcp::socket* _socket;
		boost::asio::io_service& _ioService;
	};


	class OmsSenderServer : public OmsAcceptor
	{
	public:
		OmsSenderServer(boost::asio::io_service& ioService)
			: OmsAcceptor(ioService)
		{
			_process = false;
		}

		void sendStringToOms(const std::string& msg)
		{
			_exchRespQ.push(std::move(msg));
		}
		/*
		void sendResponseToOms( const OrderExchResponse &response )
		{
			response.print() ;
		   // _exchRespQueue.push ( std::move ( response ) ) ;
		}
		*/
		void startOmsSendThread()
		{
			static std::thread t(&OmsSenderServer::writeToOms, this);
		}

		void startOmsHealthThread()
		{
			static std::thread t(&OmsSenderServer::checkHealth, this);
		}

		void stopProcessing()
		{
			_process = false;
		}

		void accept()
		{
			OmsAcceptor::accept();
			_process = true;
		}

		std::string logoutMessage()
		{
			return logoutMsg;
		}

		void sendLogoutMsg(const std::string msg)
		{

			boost::asio::write(*_socket, boost::asio::buffer(msg));

		}

		void stopConnection()
		{
			_checkExchange = false;
		}

		void startConnection()
		{
			_checkExchange = true;
		}
	public:
		bool _process;
	private:

		bool _checkExchange;
		bool isFirst;
		std::string logoutMsg = "Logout session";

		void checkHealth()
		{
			isFirst = true;
			while (true)
			{
				try
				{
					char buf[1];
					_socket->read_some(boost::asio::buffer(buf, 1));
				}
				catch (...)
				{
					if (isFirst)
					{
						FIN::setConsoleColor(4);
						std::cout << "OMS Disconnected [Port]: " << _port << std::endl;
						FIN::setConsoleColor(7);
						std::cout << "Waiting for exchnage..." << std::endl;
						isFirst = false;
					}

					if (_checkExchange)
					{
						_process = false;
						OmsAcceptor::accept();
						_process = true;
					}
				}
			}
		}

		void writeToOms()
		{
			std::cout << "Start Processing" << std::endl;
			std::string msg;
			while (true)
			{
				accept();
				while (true)
				{
					if (_process)
					{
						try
						{
							msg = _exchRespQ.top();
							boost::asio::write(*_socket, boost::asio::buffer(msg));
							std::cout << "[OUTBOUND]: " << msg << std::endl;
						}
						catch (std::exception& e)
						{

							_process = false;
							break;

						}
						if (_process)
							_exchRespQ.pop();
					}
					else
					{
						Sleep(10);
					}
				}
				_process = true;
			}
		}

		ThreadSafeQueue < std::string >  _exchRespQ;

	};

}
#endif