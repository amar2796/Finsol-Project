#include "InitApp.h"
using namespace FIN;
InitApp::InitApp()
{
	try
	{
		std::cout << __DATE__ << " ETI Verison 1.3\n";
		initializeLogging();

		FIN::IOServiceHandler* ioServiceHandler = new FIN::IOServiceHandler;
		FIN::ETI::ETI_Adapter* _adapter = new FIN::ETI::ETI_Adapter(ioServiceHandler->getIOServiceRef());

		std::thread t1(&FIN::ETI::ETI_Adapter::start_exch_con, _adapter);
		ioServiceHandler->start();
		_adapter->startOMSChannel();
		t1.join();
		ioServiceHandler->stop();

		delete _adapter;
		delete ioServiceHandler;

		LOG_INFO << "Application closed ";
		std::cout << "Application closed " << std::endl;

	}
	catch (std::exception& ex)
	{
		std::cout << "[EXCEPTION ]" << ex.what() << std::endl;
	}
}

InitApp::~InitApp()
{
}

void FIN::InitApp::initializeLogging()
{
	try
	{
		std::string _path = "";

		FIN::ConfigReader confReader;
		FIN::CurrentPath _currentPath;
		std::string filePath = _currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
		confReader.loadData(filePath);

		std::string getLogAttribute = "LOGGING.LogPath";
		_path = confReader.getValue < std::string >(
			getLogAttribute,
			FIN::FIN_CONST::EMPTY_STRING);


		std::time_t t = time(NULL) + 19800;
		auto gmtime = std::gmtime(&t);
		char buffer[32];
		//strftime(buffer, 32, "%Y%m%d-%T", gmtime);
		strftime(buffer, 32, "%Y%m%d%T", gmtime);
		std::string t1(buffer);
		t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
		std::string fileName = "ETI_logger-" + t1;
		nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size

		std::cout << "[BINARY UPDATED DATE] " << __DATE__ << "  CO : 4650" << std::endl;
		LOG_INFO << "[BINARY UPDATED DATE] " << __DATE__ << " CO : 4650";
		std::cout << "[INFO ]Logger initialization completed " << std::endl;
	}
	catch (std::exception& ex)
	{
		std::cout << "[EXCEPTION ]" << ex.what() << std::endl;
	}
}
