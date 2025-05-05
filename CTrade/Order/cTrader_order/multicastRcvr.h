#ifndef FIN_COMMON_MULTICAST_RCVR
#define FIN_COMMON_MULTICAST_RCVR

#include "marketDataMessage.h"
#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace FIN
{
	class MulticastRcvr
	{
		public:
			MulticastRcvr(
					boost::asio::io_service& io_service,
					const boost::asio::ip::address& listen_address,
					const boost::asio::ip::address& multicast_address,
					const int multicast_port ) ;

			virtual void process ( char * data, size_t size ) = 0 ;
		private:

			void handle_receive_from(const boost::system::error_code& error,
					size_t bytes_recvd) ;
	
			boost::asio::ip::udp::socket socket_;
			boost::asio::ip::udp::endpoint sender_endpoint_;
			enum { max_length = 1024 };
			char data_[max_length];
	};

	class MulticastDataHandler : public MulticastRcvr
	{
		public : 
			MulticastDataHandler(
					boost::asio::io_service& io_service,
					const boost::asio::ip::address& listen_address,
					const boost::asio::ip::address& multicast_address,
					const int multicast_port ) : 
				MulticastRcvr(io_service, listen_address, multicast_address, multicast_port)
			{
			}
	
		protected : 

			virtual void handleMessage( const MarketData & ) = 0 ;
		private : 
			void process(char *data , size_t size) ;
	};
}
#endif
