#include "templaticConnection.h"
namespace FIN 
{

	TemplateConnection::TemplateConnection( boost::asio::io_service &ioService ) 
		:_ioService ( ioService ) 
	{
	}

	void TemplateConnection::connectSocket()
	{
		_socket = boost::shared_ptr< boost::asio::ip::tcp::socket> ( 
				new boost::asio::ip::tcp::socket( _ioService ) );

		boost::asio::ip::tcp::resolver resolver (_ioService ) ;
		boost::asio::ip::tcp::resolver::query query ( boost::asio::ip::tcp::v4(),
				_connectionParam.getConnectHost().c_str() ,
				std::to_string(_connectionParam.getConnectPort()).c_str() ) ;
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve( query ) ;
		
		_socket->connect ( *iterator ) ;
		_socket->set_option( boost::asio::ip::tcp::no_delay ( true )) ;
	}

	void TemplateConnection::start()
	{
		FIN_DEVLOG ( "Connecting to : " << _connectionParam.getParamsString() ) ;
		connectSocket() ;
		FIN_DEVLOG ( "Successfully connected to : " << _connectionParam.getParamsString() ) ;

		FIN_DEVLOG ( "Starting Login Prcess: " ) ;
	}	

}
