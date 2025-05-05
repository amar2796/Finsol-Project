#define _CRT_SECURE_NO_WARNINGS
#include "InitializeProcess.h"
#include "constants.h"
#include "Nanolog.hpp"
#include <stdio.h>
#include <time.h>


void initializeLog()
{
	string _path = "";

	FIN::ConfigReader confReader;
	FIN::CurrentPath _currentPath;
	confReader.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);


	string getLogAttribute = "LOGGING.Log_Path";
	_path = confReader.getValue < std::string >(getLogAttribute,	FIN::FIN_CONST::EMPTY_STRING);

	std::time_t t = time(NULL) + 19800;
	auto gmtime = std::gmtime(&t);
	char buffer[32];
	strftime(buffer, 32, "%Y%m%d%T", gmtime);
	std::string t1(buffer);
	t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
	std::string fileName = "bseed_nfcast-" + t1 + "__"; 
	nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size
}

int main()
{
	SetConsoleTitle(TEXT("BSECM NFCAST"));
	string build_string = "BSEED BUILD ON NFCAST \nBUILD DATE : ";

	initializeLog();

	std::cout << build_string << __DATE__ << std::endl;
	LOG_INFO << "*********************************************************";
	LOG_INFO << "Binary Build Date: " << __DATE__ << "-" << __TIME__;
	LOG_INFO << "*********************************************************";
	
	FIN::Initialize_process process(FIN::FIN_CONST::EXCHANGE_NUMBER);
	process.init();

}
