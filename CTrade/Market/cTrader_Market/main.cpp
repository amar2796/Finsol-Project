#include "fix44.h"
#include "publisher.h"
#include "common.h"
#include <boost/thread.hpp>

int main( int argc, char *argv[] )
{
	std::cout << "\n**************************************\n";
	std::cout << "** CTRADE Gateway start ** CO : " << std::endl;
	std::cout << "** Build On: " << __DATE__ << "-" << __TIME__ " **" << std::endl;
	std::cout << "**************************************\n\n";

	boost::asio::io_service ioService ;
	
	FIN::MDGtwy::Publisher::getInstance()->init( ioService, FIN::FIN_CONST::FIN_CONFIG_FILE, 0, FIN::ModuleType_Market);
	
	FIN::MDGtwy::Publisher::getInstance()->setModuleState(FIN::ModuleState_Disconnected);

	boost::thread * m_thread;
	m_thread = new boost::thread(boost::bind(&boost::asio::io_service::run, & ioService));
	
	while (1)
	{
		FIN::MDGtwy::CTrade::FIX44Adapter ctradeAdapter;
		ctradeAdapter.init();
		ctradeAdapter.start();
		ctradeAdapter.startDataProcessing();
	}
	return 0;
}
   