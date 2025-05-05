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
	nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size

	std::cout << "[BINARY UPDATED DATE] " << __DATE__ << "  CO : 5450" << std::endl;
	LOG_INFO << "[BINARY UPDATED DATE] " << __DATE__ << " CO : 5450";
	std::cout << "Logger initialization completed " << std::endl;
}

LONG WINAPI CustomUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo) {

	std::cerr << "Unhandled exception occurred!" << std::endl;
	LOG_INFO << "UNHANDLED EXCEPTION CAUGHT";
	if (ExceptionInfo != nullptr)
	{
		EXCEPTION_RECORD* exceptionRecord = ExceptionInfo->ExceptionRecord;
		CONTEXT* contextRecord = ExceptionInfo->ContextRecord;

		if (exceptionRecord != nullptr)
		{
			std::cerr << "Exception Code: 0x" << std::hex << exceptionRecord->ExceptionCode << std::dec << std::endl;
			std::cerr << "Exception Address: 0x" << std::hex << exceptionRecord->ExceptionAddress << std::dec << std::endl;

			std::cerr << "Exception Flags: " << exceptionRecord->ExceptionFlags << std::endl;
		}


		LOG_INFO << "UNHANDLED EXCEPTION CAUGHT";
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

int main()
{
	SetConsoleTitle(TEXT("MCX MDI Market"));
	std::string build_string = "MCX BUILD ON MDI \nBUILD DATE : ";
	initializeLogging();

	std::cout << build_string << __DATE__ << std::endl;
	LOG_INFO << "*********************************************************";
	LOG_INFO << "Binary Build Date: " << __DATE__ << "-" << __TIME__;
	LOG_INFO << "*********************************************************";

	SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
	FIN::Initialize_process process;
	process.init();

	getchar();	
	return 0;

}
