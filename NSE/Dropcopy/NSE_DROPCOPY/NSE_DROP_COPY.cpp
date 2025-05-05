#include <iostream>
#include <boost/asio.hpp>
#include "ioServiceHandler.h"
#include "Adapter.h"
#include  <WinSock2.h>
#include <algorithm>
#include "CurrentPath.h"
#include "configReader.h"
#include "DropFile.h"
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>
void initializeLogging(std::string logPath)
{
	std::time_t t = time(NULL) + 19800;
	auto gmtime = std::gmtime(&t);
	char buffer[32];
	//strftime(buffer, 32, "%Y%m%d-%T", gmtime);
	strftime(buffer, 32, "%Y%m%d%T", gmtime);
	std::string t1(buffer);
	t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
	std::string fileName = "DEVLOGS_DROPCOPY-" + t1 + "__";
	//std::cout << "FName: " << fileName << std::endl;    
	nanolog::initialize(nanolog::GuaranteedLogger(), logPath + "\\", fileName, 200);//200 is 200mb max file size
}
int main()
{
	SetConsoleTitle(TEXT("NSE FO DropCopy"));
	FIN::ConfigReader _config;
	FIN::CurrentPath _path;
	std::string fileName = _path.getCurrentPath() + FIN::CONSTANT::NSE_DROPCOPY_CONFIG;
	_config.loadData(fileName);
	std::string logPath = _config.getValue<std::string>("LOGGING.LOG_PATH", FIN::CONSTANT::EMPTY_STRING);
	initializeLogging(logPath);
	LOG_INFO << "*********************************************************";
	LOG_INFO << "NSEDROPCOPY Build Date: " << __DATE__ << "-" << __TIME__ << "CO : 5199";
	std::cout << "NSEDROPCOPY Build Date: " << __DATE__ << "-" << __TIME__ << "CO : 5199" << std::endl;

	LOG_INFO << "*********************************************************";
	try {


		FIN::IOServiceHandler ioServiceHandler;
		FIN::Adapter _adapter(ioServiceHandler.getIOServiceRef());
		ioServiceHandler.start();
		//std::thread t1(&FIN::Adapter::start, &_adapter);
		
		//_adapter.startRabbitMqConnection();
		_adapter.start();

		//while(1){}
		//t1.join();

		ioServiceHandler.stop();
	}
	catch (std::exception& e)
	{
		CONSOLE_ERROR_LOG("Exception : " << e.what());
	}
	catch (...)
	{
		CONSOLE_ERROR_LOG("Unknown exception caught");
	}

	do {
		getchar();
	} while (true);




	return 0;

}
