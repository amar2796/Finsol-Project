#ifndef FIN_COMMON_TCP_RCVR_H
#ifndef FIN_COMMON_TCP_RCVR_H

#include <boost/asio.hpp>
#include <boost/array.hpp>
namespace FIN {


	class TcpRcvr
	{
		public:
			TcpRcvr ( boost::asio::io_service &ioService ) 
			void init( const std::string &configFile ) ;
			bool connect() ;

			virtual void process( char * data, size_t size ) = 0 ;

		protected :
			const char * HELLO_PORT_STR = "50013" ;

			boost::asio::io_service & _ioService;
			boost::asio::ip::tcp::socket _socket(io_service);
			boost::system::error_code _error = boost::asio::error::host_not_found ;

			////boost::asio::io_service io_service;
			////boost::asio::ip::tcp::resolver resolver(io_service);
			////boost::asio::ip::tcp::resolver::query query(argv[1], HELLO_PORT_STR);
			////boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
			////boost::asio::ip::tcp::resolver::iterator end;
			////boost::asio::ip::tcp::socket socket(io_service);
			////boost::system::error_code error = boost::asio::error::host_not_found;
	};

	{
		boost::asio::ip::tcp::resolver resolver ;
		boost::asio::ip::tcp::resolver::query query ;
		boost::asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		boost::asio::ip::tcp::resolver::iterator end;
		while(error && endpoint_iterator != end)
		{
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}
		if(error)
			throw boost::system::system_error(error);
	}

	{
		for(;;)
		{
			boost::array<char, 1024> buf;
			std::cout << std::endl << std::endl << "Read Some" << std::endl ;
			size_t len = socket.read_some(boost::asio::buffer(buf), error);
			if(error == boost::asio::error::eof)
				break;
			else if(error)
				throw boost::system::system_error(error);
			std::cout.write(buf.data(), len);
			std::cout << "----- READ DONE----" << std::endl ;
		}
		catch (std::exception& e)
		{
			std::cout << e.what() << std::endl ;
		}
	}
}

#endif
