// IIBX_IML.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "configReader.h"
#include "CurrentPath.h"
#include "IOServiceHanlder.h"
#include "Adapter.h"
#include "color.h"

void initializeLogging()
{
	std::string _path = "";

	FIN::ConfigReader confReader;
	FIN::CurrentPath _currentPath;
	std::string filePath = _currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
	confReader.loadData(filePath);


	std::string getLogAttribute = "LOGGING.LogPath";
	_path = confReader.getValue < std::string >(getLogAttribute, FIN::FIN_CONST::EMPTY_STRING);
	std::cout << "Log path : " << _path << std::endl;

	std::time_t t = time(NULL) + 19800;
	auto gmtime = std::gmtime(&t);
	char buffer[32];
	//strftime(buffer, 32, "%Y%m%d-%T", gmtime);
	strftime(buffer, 32, "%Y%m%d%T", gmtime);
	std::string t1(buffer);
	t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
	std::string fileName = "IIBX_FUT-" + t1 + "__";
	//std::cout << "FName: " << fileName << std::endl;    
	nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size

	std::cout << "[BINARY UPDATED DATE] " << __DATE__ << std::endl;
	LOG_INFO << "*********************************************************";
	LOG_INFO << "IIBX FUT ORDER Build Date: " << __DATE__ << "-" << __TIME__;
	LOG_INFO << "*********************************************************";

}
int main()
{
	try
	{
		//At revision: 
		SetConsoleTitle(TEXT("IIBX FUT ORDER"));
		initializeLogging();
		FIN::IOServiceHandler ioServiceHandler;

		FIN::IIBX::Adapter _adapter(ioServiceHandler.getIOServiceRef());

		std::thread t1(&FIN::IIBX::Adapter::startExchangeConnection, &_adapter);
		ioServiceHandler.start();
		_adapter.startOMSChannel();

		t1.join();

		ioServiceHandler.stop();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception : " << e.what() << std::endl;
		LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ +	" Line: " + std::to_string(__LINE__);
	}
	catch (...)
	{
		std::cout << "Unknown exception caught" << std::endl;
	}
	return 0;
}
