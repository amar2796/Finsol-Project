#include "fix44.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>
#include <iostream>
#include "logger.h"
#include "Nanolog.hpp"



int main(int argc, char* argv[]) 
{
	while (1)
	{
		std::cout << __DATE__ << " " << __TIME__;
		std::cout << " Updated" << std::endl;
		boost::asio::io_service omsSenderIO;
		FIN::FIX44Adapter cTraderOrderAdapter(omsSenderIO);

		cTraderOrderAdapter.init();
		cTraderOrderAdapter.start();
		std::thread logoutThread(&FIN::FIX44Adapter::listenLogOutFromConsole, &cTraderOrderAdapter);
		cTraderOrderAdapter.startOmsRcv();
		logoutThread.join();
	}
	return 0;
}