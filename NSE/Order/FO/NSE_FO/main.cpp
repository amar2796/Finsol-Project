#include<iostream>
#include "adapter.h"
#include "ioServiceHandler.h"
#include "CurrentPath.h"

void initializeLogging()
{
	std::string _path = "";

	FIN::ConfigReader confReader;
	FIN::CurrentPath _currentPath;
	std::string filePath = _currentPath.getCurrentPath() + FIN::CONSTANT::NSE_FO_ORDER_CONFIG;
	confReader.loadData(filePath);


	std::string getLogAttribute = "LOGGING.LogPath";
	_path = confReader.getValue < std::string >(getLogAttribute, FIN::CONSTANT::EMPTY_STRING);
	std::cout << "Log path : " << _path << std::endl;
	std::cout << "** Build On: " << __DATE__ << "-" << __TIME__ " CO : 5282  **" << std::endl;
	std::cout << "**************************************\n\n";


	std::time_t t = time(NULL) + 19800;
	auto gmtime = std::gmtime(&t); 

	char buffer[32];
	strftime(buffer, 32, "%Y%m%d%T", gmtime);
	std::string t1(buffer);
	t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
	std::string fileName = "nseLogFo-" + t1 + "__";
	std::cout << "FName: " << fileName << std::endl;    
	nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 20);//20 is 20mb max file size

	LOG_INFO << "*********************************************************";
	LOG_INFO << "NSEFO Build Date: " << __DATE__ << "-" << __TIME__;
	LOG_INFO << "*********************************************************";

}
int main()
{
	try
	{
		initializeLogging();
		FIN::IOServiceHandler ioServiceHandler;

		FIN::OrdGtwy::NNF::FO::Adapter _adapter(ioServiceHandler.getIOServiceRef());

		std::thread t(&FIN::OrdGtwy::NNF::FO::Adapter::start, &_adapter);

		ioServiceHandler.start();
		_adapter.startOmsRcv();

		t.join();

		/*std::thread t1(&FIN::ETI::ETI_Adapter::start_exch_con, &_adapter);
		ioServiceHandler.start();
		_adapter.startOMSChannel();

		t1.join();*/

		ioServiceHandler.stop();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception : " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown exception caught" << std::endl;
	}
	return 0;
}