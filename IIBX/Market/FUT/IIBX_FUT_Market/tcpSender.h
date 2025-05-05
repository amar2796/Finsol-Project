#ifndef FIN_COMMON_TCP_SENDER_H
#define FIN_COMMON_TCP_SENDER_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <list>

#include "abstractDataSender.h"

namespace FIN {

	class TcpConnection
	{
		public :
			TcpConnection( boost::asio::io_service & ioService ) 
				: _socket( ioService ), _readBuffer( ) 
			{ 
			}

			TcpConnection( boost::asio::io_service & ioService, size_t maxBufferSize ) 
				:_socket( ioService ), _readBuffer( maxBufferSize ) 
			{
			}

			boost::asio::ip::tcp::socket _socket;
			boost::asio::streambuf _readBuffer;
	};

	typedef std::list < TcpConnection > ConnectionList ;
	typedef ConnectionList::iterator ConnectionListItr ;

	class TcpServer : public AbstractDataSender
	{
		public :	
			void sendData( const char *data, const size_t size ) ;	
			TcpServer ( boost::asio::io_service &ioService, const int port ) 
				: 
					_ioService( ioService ),
					_acceptor ( _ioService ) 
		{
			listen(port) ;
		}
	
		private :
			void listen ( const int port ) ;	

			void doAsyncRead ( ConnectionListItr conHandle ) ;
			void handleRead( ConnectionListItr con_handle, boost::system::error_code const & err, 
					size_t bytes_transfered ) ;
			void handleWrite ( ConnectionListItr conHandle, std::shared_ptr<std::string> msg_buffer, 
					boost::system::error_code const & err ) ;
			void handleAccept( ConnectionListItr conHandle, boost::system::error_code const & err ) ;
			void startAccept() ;

		private :	
			bool _connected = false; 
			std::list < TcpConnection > _connections ;

			boost::asio::io_service &_ioService;
			boost::asio::ip::tcp::acceptor _acceptor;
	};
} 


#endif
