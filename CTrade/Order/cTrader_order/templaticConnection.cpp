#include "templaticConnection.h"
namespace FIN
{
	void TemplateConnection::start()
	{
		//?
		FIN_DEVLOG("Connecting to : " << _connectionParam.getParamsString());
		//call function
		connectSocket();
		FIN_DEVLOG("Successfully connected to : " << _connectionParam.getParamsString());
		FIN_DEVLOG("Starting Login Process");
	}

	void TemplateConnection::connectSocket()
	{
		//initilize _socket 
		//Synchronous -> The data is sent from one end to another in the form of frames or blocks.
		//Asynchronous -> The data is sent from one end to another in the form of a byte of characters.
		//shared_ptr -> can be used to hold a pointer to a dynamically allocated array
		_socket = boost::shared_ptr < boost::asio::ip::tcp::socket >(
			new boost::asio::ip::tcp::socket(_ioService) );

		//?
		boost::asio::ip::tcp::resolver resolver(_ioService);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
			_connectionParam.getConnectHost().c_str(),
			std::to_string(_connectionParam.getConnectPort()).c_str());

		//iterates through the address list of the host that you specified
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

		//connect to socket
		_socket->connect(*iterator);
		_socket->set_option(boost::asio::ip::tcp::no_delay(true));
	}
}