/*#ifndef FIN_NSE_MARKETDATA
#define FIN_NSE_MARKETDATA

#include <iostream>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

namespace FIN {
	namespace NSE {
		namespace TRIMMED {
			namespace FO {

				class suscribedSymbols
				{
				public:
					void init();
					void start();
					void stop();

				private:
					std::string multicastAdd;
					std::string multicastIp;
					int port;
					boost::asio::io_service io_service;
				};

			}
		}
	}
}

#endif

