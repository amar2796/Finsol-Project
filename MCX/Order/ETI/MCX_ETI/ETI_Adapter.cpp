#include "ETI_Adapter.h"

using namespace FIN::ETI;

ETI_Adapter::ETI_Adapter(boost::asio::io_service& ioService)
	: Exchange_Processor(ioService),
	_ioService(ioService)
{

	init();
}

bool ETI_Adapter::init()
{

	return false;
}

void ETI_Adapter::stop() {


}

void ETI_Adapter::start_exch_con()
{
	startExchangeConnection();
}


