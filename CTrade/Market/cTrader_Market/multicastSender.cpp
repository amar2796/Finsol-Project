#include "multicastSender.h"
#include "marketDataMessage.h"
#include <chrono>
#include <iostream>

namespace FIN {

	void UdpSenderInterface::send ( const char * data, const size_t size ) 
	{
		// std::cout << "Write on socket" << std::endl ;
		//auto start = std::chrono::high_resolution_clock::now();
		_socket->send_to ( boost::asio::buffer ( data, size ), *_remoteEndpoint ) ;
		//auto end = std::chrono:: high_resolution_clock::now();
		//double x = std::chrono::duration<double, std::micro>(end - start).count();
		//std::cout << "Time1 " << std::to_string(x) << std::endl;

	}

	MulticastSender::MulticastSender()
	{
	}

	MulticastSender::MulticastSender( 
			boost::asio::io_service &ioService,
			const std::string multicastIp,
			const std::string multicastAdd,
			const int port  
			) 
	{
		configure(  ioService, multicastIp, multicastAdd, port  ) ;
	}

	void MulticastSender::configure(  
			boost::asio::io_service &ioService,
			const std::string multicastIp,
			const std::string multicastAdd,
			const int port  
			)
	{
		std::cout << "******************************" << std::endl ;	
		std::cout << "Configuring UDP Multicast Sender" << std::endl ;
		std::cout << "Address : " << multicastIp << std::endl ;
		std::cout << "IP      : " << multicastAdd << std::endl ;
		std::cout << "Port    : " << port << std::endl ;
		std::cout << "******************************" << std::endl ;

		/*LOG_INFO << "******************************" ;
		LOG_INFO << "Configuring UDP Multicast Sender";
		LOG_INFO << "Address : " << multicastIp ;
		LOG_INFO << "IP      : " << multicastAdd ;
		LOG_INFO << "Port    : " << port;
		LOG_INFO << "******************************";*/

		_remoteEndpoint = boost::shared_ptr< boost::asio::ip::udp::endpoint > (
				new boost::asio::ip::udp::endpoint(
					boost::asio::ip::address::from_string ( multicastAdd ) , port ) ) ;

		boost::asio::ip::address_v4 localInterface ( 
				boost::asio::ip::address_v4::from_string ( multicastIp ) );

		boost::asio::ip::multicast::outbound_interface option ( localInterface ) ;

		_socket = boost::shared_ptr< boost::asio::ip::udp::socket > (
				new boost::asio::ip::udp::socket ( ioService, _remoteEndpoint->protocol() ));

		_socket->set_option( option ) ;
		_socket->set_option ( boost::asio::ip::udp::socket::reuse_address(true) ) ;
	}

	UnicastSender::UnicastSender()
	{
	}

	UnicastSender::UnicastSender(
			boost::asio::io_service &ioService,
			const std::string& address,
			const int port  
			)
	{
		configure ( ioService , address , port ) ;
	}

	void UnicastSender::configure (
			boost::asio::io_service &ioService,
			const std::string& address,
			const int port  
			)
	{
		std::cout << "******************************" << std::endl ;	
		std::cout << "Configuring UDP Unicast Sender" << std::endl ;
		std::cout << "Address : " << address << std::endl ;
		std::cout << "Port    : " << port << std::endl ;
		std::cout << "******************************" << std::endl ;

		/*LOG_INFO << "******************************" ;
		LOG_INFO << "Configuring UDP Unicast Sender" ;
		LOG_INFO << "Address : " << address          ;		
		LOG_INFO << "Port    : " << port             ;		
		LOG_INFO << "******************************" ;*/

		_socket = boost::shared_ptr< boost::asio::ip::udp::socket > (
				new boost::asio::ip::udp::socket ( 
					ioService, boost::asio::ip::udp::endpoint( 
						boost::asio::ip::udp::v4(), 0 )));

		boost::asio::ip::udp::resolver resolver ( ioService ) ;
		
		boost::asio::ip::udp::resolver::query query (
				boost::asio::ip::udp::v4(),
				address.c_str(),
				std::to_string( port).c_str() ) ;

		_remoteEndpoint = boost::shared_ptr < boost::asio::ip::udp::endpoint > ( new 
			boost::asio::ip::udp::endpoint ( *resolver.resolve( query ) ) ) ; 
	}



}
