#ifndef FIN_ORDER_GTWY_OMS_UDP_SOCKET_HANDLER_H
#define FIN_ORDER_GTWY_OMS_UDP_SOCKET_HANDLER_H
#pragma once

#include <string>             // For std::string
#include <atomic>             // For std::atomic<bool>
#include <thread>             // For std::thread
#include <cstddef>            // For size_t

#undef _WINSOCKAPI_   // Force winsock2.h to be included instead of winsock.h
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")


#include "threadSafeQueue.h"  // For ThreadSafeQueue
#include "defines.h"          // Custom definitions
#include "NanoLog.hpp"        // For LOG_INFO
#include "multicastSender.h"  // For multicast sender
#include "unicastReceiver.h"  // For UnicastRcvr
#include "orderBook.h"        // For OrderBook (if used in cpp)
#include "sender.h"           // For UnicastSenderNative
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
			void connect(const std::string& ip, const int port)
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
