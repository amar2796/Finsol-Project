#include "tcpSender.h"

#include <iostream>

namespace FIN {

	void TcpServer::send( const char *data, const size_t size ) 
	{
		return ;
		std::cout << "IN SEND" << std::endl ;
		if ( !_connected ) 
		{
			std::cout << "No Connection Yet" << std::endl ;
			return ;
		}
		auto buff = std::make_shared<std::string>( data, size );	
		// JUST AN HACK Right now || WIll Only Send Data to one client 
		//  If required will need to update for multiple client.
		ConnectionListItr conHandle = _connections.begin() ;
		auto handler = boost::bind( &TcpServer::handleWrite, this, conHandle, 
				buff, boost::asio::placeholders::error );

		boost::asio::async_write( conHandle->_socket, boost::asio::buffer( *buff ), handler );
	}

	void TcpServer::handleRead( ConnectionListItr conHandle, boost::system::error_code const & err, 
			size_t bytes_transfered ) 
	{
		if( bytes_transfered > 0 ) 
		{
			std::istream is( &conHandle->_readBuffer );
			std::string line;
			std::getline( is, line );
			std::cout << "Message Received: " << line << std::endl;
		}

		if( !err ) 
		{
			doAsyncRead( conHandle );
		} 
		else 
		{
			std::cerr << "We had an error: " << err.message( ) << std::endl;
			_connections.erase( conHandle );
		}
	}

        void TcpServer::doAsyncRead( ConnectionListItr conHandle ) 
	{
		auto handler = boost::bind( &TcpServer::handleRead, this, conHandle,
				boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred );
		boost::asio::async_read_until( conHandle->_socket, conHandle->_readBuffer, "\n", handler );
	}

        void TcpServer::handleWrite( ConnectionListItr conHandle, std::shared_ptr<std::string> msg_buffer, 
			boost::system::error_code const & err ) 
	{
		if( !err ) 
		{
			if( conHandle->_socket.is_open( ) ) 
			{
				std::cout << "Write completed successfully and connection is open" << std::endl ;
			}
			else
			{
				std::cout << "Socket is closed" << std::endl ;
			}
		} 
		else 
		{
			std::cerr << "We had an error: " << err.message( ) << std::endl;
			_connections.erase( conHandle );
		}
	}

	void TcpServer::handleAccept( ConnectionListItr conHandle, boost::system::error_code const & err ) 
	{
		if( !err ) 
		{
			_connected = true ;
			std::cout << "Connection Accepted" << std::endl ;
			std::string s( "Hello" ) ;
			auto buff = std::make_shared<std::string>( s, s.size() );
			// TODO :: Check this functionality	
			// JUST AN HACK Right now || WIll Only Send Data to one client 
			//  If required will need to update for multiple client.
			ConnectionListItr conHandle = _connections.begin() ;
			auto handler = boost::bind( &TcpServer::handleWrite, this, conHandle, 
					buff, boost::asio::placeholders::error );

			boost::asio::async_write( conHandle->_socket, boost::asio::buffer( *buff ), handler );
			doAsyncRead ( conHandle ) ;
		} 
		else 
		{
			std::cerr << "We had an error: " << err.message( ) << std::endl;
			_connections.erase( conHandle );
		}

		//startAccept( );
	}


	void TcpServer::startAccept( ) 
	{
		auto conHandle = _connections.emplace( _connections.begin( ), _ioService );
		auto handler = boost::bind( &TcpServer::handleAccept, this, conHandle, boost::asio::placeholders::error );
		std::cout << "Async Accept" << std::endl ;
		std::cout << "COnnections List Size : " << _connections.size() <<std::endl ;
		_acceptor.async_accept( conHandle->_socket, handler );
	}	

	void TcpServer::listen ( const int port ) 
	{
		std::cout << "********************************" << std::endl ;
		std::cout << "Configuring TCP Sender Publisher" << std::endl ;
		std::cout << "Listening Port : " << port << std::endl ;
		std::cout << "********************************" << std::endl ;
                auto endpoint = boost::asio::ip::tcp::endpoint( boost::asio::ip::tcp::v4( ), port );
                _acceptor.open( endpoint.protocol( ) );
                _acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ) );
                _acceptor.bind( endpoint );
                _acceptor.listen( );
                startAccept( );
	}
}
