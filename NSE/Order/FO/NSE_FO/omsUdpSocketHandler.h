#ifndef FIN_ORDER_GTWY_OMS_UDP_SOCKET_HANDLER_H
#define FIN_ORDER_GTWY_OMS_UDP_SOCKET_HANDLER_H

#include <boost/asio.hpp>
#include "threadSafeQueue.h"
#include "defines.h"
#include "NanoLog.hpp"
#include "multicastSender.h"
#include "unicastReceiver.h"
#include "orderBook.h"
#include "NanoLog.hpp"
#include "sender.h"
namespace FIN
{
	namespace OrdGtwy
	{

		class OmsUDPRcv : public UnicastRcvr
		{
		private:
			ThreadSafeQueue<std::string> _exchRequestQ;
			std::atomic<bool> _isConnected;
			void process(char* data, size_t size)
			{

				if (data[0] == 'H')
				{
				}
				else
				{
					LOG_INFO << "[UDP from OMS] " << std::string(data, size);
					_exchRequestQ.push(std::string(data, size));
				}
			}

			void readData()
			{
				UnicastRcvr::startRead();
			}

		public:
			void sendHeartBeat()
			{
				_exchRequestQ.push("H");
			}
			virtual void receiveFromOMS(const std::string& msg) = 0;
			void connect(int port)
			{
				UnicastRcvr::configure(port);
				static std::thread tOmsRead(&OmsUDPRcv::readData, this);
			}

			void isConnected(bool value)
			{
				_isConnected = value;
			}

			void getOMSReq()
			{
				while (1)
				{
					if (_isConnected)
					{
						receiveFromOMS(_exchRequestQ.top());
						_exchRequestQ.pop();
					}
					else
					{
						Sleep(1000);
					}
				}
			}
		};

		class OmsUDPSend
		{
		private:
			// UnicastSender *sender ;
			std::string heartbeat = "H";
			UnicastSenderNative* sender;
			ThreadSafeQueue<std::string> _exchResponseQ;
			std::atomic<bool> _isConnected;
			std::string msg;
			void omsWrite()
			{
				while (true)
				{
					if (_isConnected)
					{

						msg = (_exchResponseQ.top());
						sender->send(msg.c_str(), msg.size());
						LOG_INFO << "[UDP to OMS] " << msg;
						_exchResponseQ.pop();
					}
					else
					{
						Sleep(1000);
					}
				}
			}
			void send_heartbeat()
			{
				while (true)
				{
					if (_isConnected)
					{
						sender->send(heartbeat.c_str(), heartbeat.size());
						//LOG_INFO << "[UDP to OMS] " << heartbeat;
						Sleep(10000);
					}
					else
					{
						Sleep(1000);
					}
				}
			}

		public:
			void connect(boost::asio::io_service& ioService, const std::string& ip, const int port)
			{
				// sender = new UnicastSender (ioService, ip, port) ;
				sender = new UnicastSenderNative(ip, port);
				static std::thread tOmsWrite(&OmsUDPSend::omsWrite, this);
				static std::thread tOmsHeartbeat(&OmsUDPSend::send_heartbeat, this);
			}

			void send(std::string& msg)
			{
				_exchResponseQ.push(msg);
			}
			void isConnected(bool val)
			{
				_isConnected = val;
			}
		};

	}
}
#endif
