#ifndef FIN_COMMON_MULTICAST_SENSER_H
#define FIN_COMMON_MULTICAST_SENSER_H

#include "abstractDataSender.h"

namespace FIN {

	class UdpSenderInterface : public AbstractDataSender
	{
		// Does Nothing || Just an interface
	public:
		void send(const char* data, const size_t size);

	protected:
		UdpSenderInterface() {} // So that no one can create its object

		
		int c = 0;
	};

	class MulticastSender : public UdpSenderInterface
	{
	public:
		// Call configure later may be????
		MulticastSender();
		MulticastSender(
			const std::string multicastIp,
			const std::string multicastAdd,
			const int port
		);

		void configure(
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
			const std::string& address,
			const int port
		);

		void configure(
			const std::string& address,
			const int port
		);
	};
}

#endif
