#ifndef FIN_TEMPLATE_CONNECTION_H
#define FIN_TEMPLATE_CONNECTION_H

#include "connectionStatus.h"
#include "common.h"

#include <boost/asio.hpp>
namespace FIN 
{


	class TemplateConnection
	{
		public:
			void init () ;
			void stop () ;
			void start() ;

			TemplateConnection( boost::asio::io_service &ioService ) ;

		protected :

            bool sendToTarget( const char * data, unsigned int length ) ;
            virtual void handleReceive( const char * data, uint32_t size ) = 0 ;
            void recvFromTarget() ;
			void connectSocket() ;				

        private :

			ConnectionParam _connectionParam ;	

			boost::shared_ptr < boost::asio::ip::tcp::socket > _socket ;
			boost::asio::io_service &_ioService ;
	};
}

#endif
