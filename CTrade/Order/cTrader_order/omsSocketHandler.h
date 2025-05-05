#ifndef FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H
#define FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H

#include <boost/asio.hpp>
#include "threadSafeQueue.h"
#include "defines.h"
#include <iostream>

namespace FIN {

    class OmsAcceptor
    {
        public :
            OmsAcceptor ( boost::asio::io_service &ioService)
                :_ioService (ioService )
            {
                _acc = nullptr ;
                _socket = nullptr ;
            }

            void init( int port )
            {
                if ( _acc != nullptr ) 
                    delete _acc ;
                if ( _socket != nullptr ) 
                    delete _socket ;

                _port = port ;
                //accept socket
                _acc = new boost::asio::ip::tcp::acceptor ( _ioService, boost::asio::ip::tcp::endpoint( 
                            boost::asio::ip::tcp::v4(), port ) );

                _socket = new boost::asio::ip::tcp::socket ( _ioService);
            }

            void accept()
            {
                init( _port );
                FIN_WARN(  "Waiting for OMS Connection [Port]: " << _port ); 
                _acc->accept( *_socket );
				FIN_INFO("OMS Connected [Port]: "<< _port);
            }

        protected :
            int _port ;
            boost::asio::ip::tcp::acceptor * _acc ;  
            boost::asio::ip::tcp::socket * _socket ;
            boost::asio::io_service & _ioService ;
    };

    class OmsRcvServer : public OmsAcceptor
    {
        std::string lastKeyVal ;
        public :
            OmsRcvServer( boost::asio::io_service &ioService)
                : OmsAcceptor ( ioService )
            {
                _connectionAccepted = false ;
                lastKeyVal = "" ;
                lastKeyVal.append( 1 + "10=000" ) ;
            }

            std::string leftOverString ;
            void rcvFromOms()
            {
                if ( !_connectionAccepted ) 
                {
                    accept()  ;
                    _connectionAccepted = true ;
                }

                char buf[ 1024 * 8 ] ;
                int dataLength ;
                try
                {
                    dataLength = _socket->read_some(boost::asio::buffer ( buf, 1024*8 )) ;

                    std::string dataRcvd1  ( buf , dataLength) ;
                    std::string dataRcvd ;
                    dataRcvd.append( leftOverString + dataRcvd1 );
                    //std::cout << "DataRcvd: " << dataRcvd << std::endl ;
                    leftOverString = "" ;

                    size_t i ;
                    for ( i = 0 ; i < dataRcvd.size() ; i = i + 6)
                    { 
                        int temp = i ;
                        i = dataRcvd.find ( lastKeyVal, i ) ;
                        if ( i == std::string::npos ) 
                        {
                            i = temp ;
                            break ;
                        }
                        std::string message ( dataRcvd, temp , i + 6 - temp);
                        std::cout << "[INBOUND]:  " << message << std::endl ;
                        handleOmsWrite(message) ;
                    }
                    if ( i != dataRcvd.size() ) 
                    {
                        leftOverString = std::string ( dataRcvd, i , dataRcvd.size() - i ) ;
                    }
                }
                catch(...)
                {
                    _connectionAccepted = false ;
                }
            }

            bool _connectionAccepted ;

            virtual void handleOmsWrite( const std::string & str )  = 0;
    };

    class OmsSenderServer : public OmsAcceptor
    {
        public :
            OmsSenderServer( boost::asio::io_service &ioService)
                : OmsAcceptor ( ioService )
            {
                _process = false ;
            }

            void sendStringToOms( const std::string &msg )
            {
                _exchRespQ.push ( std::move ( msg ) ) ;
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
                static std::thread t( &OmsSenderServer::writeToOms, this ) ;
            }

			void startOmsHealthThread()
			{
                //create seprate thread
				static std::thread t(&OmsSenderServer::checkHealth, this);
			}

            void stopProcessing()
            {
                _process = false ;
            }

            void accept()
            {
                //accept oms connection
                OmsAcceptor::accept() ;
                _process = true ;
            }
        private : 

            bool _process ;

			void checkHealth()
			{
				while (true)
				{
					try 
					{ 
						char buf[1];
                        //read_some -> This function is used to read data from the stream socket. The function call will block until one or more bytes of data has been read successfully, or until an error occurs.
						_socket->read_some(boost::asio::buffer(buf,1));
					}
					catch (...)
					{
						FIN_ERROR("OMS Disconnected [Port]: " << _port);
						_process = false;
						OmsAcceptor::accept();
						_process = true;
					}
				}
			}

            void writeToOms()
            {
                std::cout << "Start Processing" << std::endl ;
                std::string msg ;
                while(true)
                {
                    accept() ;
                    while ( true ) 
                    {
                        if ( _process ) 
                        {
                            try 
                            { 
                                msg = _exchRespQ.top() ;
                                boost::asio::write( *_socket, boost::asio::buffer(msg ) );
                                std::cout << "[OUTBOUND]: " << msg << std::endl ;
                            }
                            catch (std::exception &e ) 
                            {
                                
                                _process = false ;
                                break ;

                            }
                            if ( _process ) 
                                _exchRespQ.pop() ;
                        }
                        else
                        {
                            Sleep ( 10 ) ;
                        }
                    }
                    _process = true ;
                }
            }
            
            ThreadSafeQueue < std::string >  _exchRespQ ;
            
    };

}
#endif