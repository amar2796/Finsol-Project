#include "Application.h"
#include"Nanolog.hpp"
//172.20.21.11
void initializeLogging(int& exchNum)
{
	std::string _path = "";

	FIN::ConfigReader confReader;
	FIN::CurrentPath _currentPath;
	std::string filePath = _currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
	confReader.loadData(filePath);
	std::string getLogAttribute = FIN::FIN_CONST::LOGGING + FIN::FIN_CONST::DOT + FIN::FIN_CONST::LogPath;
	_path = confReader.getValue < std::string >(getLogAttribute, FIN::FIN_CONST::EMPTY_STRING);
	std::cout << "Log path : " << _path << std::endl;

	exchNum = confReader.getValue < int >(FIN::FIN_CONST::IIBXEXCHANGEDETAILS + FIN::FIN_CONST::DOT + FIN::FIN_CONST::Exchange_Number, FIN::FIN_CONST::EMPTY_INTEGER);

	std::time_t t = time(NULL) + 19800;
	auto gmtime = std::gmtime(&t);
	char buffer[32];
	strftime(buffer, 32, "%Y%m%d%T", gmtime);
	std::string t1(buffer);
	t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
	std::string fileName = "IIBX-" + t1 + "__";    
	nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size

	LOG_INFO << "*********************************************************";
	LOG_INFO << "IIBX Build Date: " << __DATE__ << "-" << __TIME__;
	LOG_INFO << "*********************************************************";
}
int main()
{
	try
	{
		int exchNumber = 0;
		SetConsoleTitle(TEXT("IIBX FUT MARKET"));
		initializeLogging(exchNumber);

		std::cout << "***********IIBX FUT Version 1.0.4*********************************" << std::endl;
		std::cout << "IIBX_FUT_MARKET Build Date : " << __DATE__ << "-" << __TIME__ << std::endl;		
		
		FIN::IIBX::Application app(exchNumber);
		app.init();
	}
	catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
	catch (...) {
		std::cerr << "Caught unknown exception." << std::endl;
	}
	return 0;
}
