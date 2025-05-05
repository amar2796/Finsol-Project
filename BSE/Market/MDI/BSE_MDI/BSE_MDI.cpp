#include "InitializeProcess.h"
#include "constants.h"

void initializeLogging()
{
	std::string _path = "";

	FIN::ConfigReader confReader;
	FIN::CurrentPath _currentPath;
	std::string filePath = _currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
	confReader.loadData(filePath);

	std::string getLogAttribute = "LOGGING.Log_Path";
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
	std::string fileName = "MDI_Logger" + t1;
	nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 20);//20 is 20mb max file size

	std::cout << "[INFO ][BINARY UPDATED DATE] " << __DATE__ << "  CO : 5534, CV : 5534" << std::endl;
	LOG_INFO << "[BINARY UPDATED DATE] " << __DATE__ << " CO : 5534, CV : 5534";
	std::cout << "[INFO ]Logger initialization completed " << std::endl;
}
int main()
{

	std::string build_string = "BSE MARKET BUILD ON MDI BUILD DATE :";

	initializeLogging();

	std::cout << "[INFO ]" << build_string << __DATE__ << std::endl;
	LOG_INFO << "*********************************************************";
	LOG_INFO << "Binary Build Date: " << __DATE__ << "-" << __TIME__;
	LOG_INFO << "*********************************************************";

	FIN::Initialize_process process;
	process.init();

	getchar();
	return 0;

}
