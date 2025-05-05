#ifndef FIN_COMMON_MULTICAST_SENSER_H
#define FIN_COMMON_MULTICAST_SENSER_H

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "abstractDataSender.h"

namespace FIN {

	class UdpSenderInterface : public AbstractDataSender
	{
		// Does Nothing || Just an interface
	public:
		void send(const char* data, const size_t size);

	protected:
		UdpSenderInterface() {} // So that no one can create its object

		boost::shared_ptr < boost::asio::ip::udp::socket  > _socket;
		boost::shared_ptr < boost::asio::ip::udp::endpoint> _remoteEndpoint;
		int c = 0;
	};

	class MulticastSender : public UdpSenderInterface
	{
	public:
		// Call configure later may be????
		MulticastSender();
		MulticastSender(
			boost::asio::io_service& ioService,
			const std::string multicastIp,
			const std::string multicastAdd,
			const int port
		);

		void configure(
			boost::asio::io_service& ioService,
			const std::string multicastIp,
			const std::string multicastAdd,
			const int port
		);

	};

	class UnicastSender : public UdpSenderInterface
	{
	public:
		UnicastSender();
		UnicastSender(
			boost::asio::io_service& ioService,
			const std::string& address,
			const int port
		);

		void configure(
			boost::asio::io_service& ioService,
			const std::string& address,
			const int port
		);
	};
}

#endif
