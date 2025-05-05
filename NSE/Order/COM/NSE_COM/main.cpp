#include <iostream>
#include "adapter.h"
#include "CurrentPath.h"
#include "configReader.h"
#include "configParameter.h"
#include "messages.h"

void initializeLog(FIN::ConfigReader& confReader)
{
    std::string _path = "";

    FIN::CurrentPath _currentPath;
    std::string filePath = _currentPath.getCurrentPath() + FIN::CONSTANT::NSECOM_ORDER_CONFIG;
    confReader.loadData(filePath);

    std::string getLogAttribute = "LOGGING.LogPath";
    _path = confReader.getValue < std::string >(getLogAttribute, FIN::CONSTANT::EMPTY_STRING);
    std::cout << "** Build On: " << __DATE__ << "-" << __TIME__ " CO : 5396, CV : 5396 **" << std::endl;
    std::cout << "**********************************************\n\n";

    std::time_t t = time(NULL) + 19800;
    auto gmtime = std::gmtime(&t);
    char buffer[32];
    strftime(buffer, 32, "%Y%m%d-%T", gmtime);
    std::string t1(buffer);
    t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
    std::string fileName = "nseLogCOM-" + t1 + "_";
    nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size

    LOG_INFO << "*********************************************************";
    LOG_INFO << "NSECOM Build Date: " << __DATE__ << "-" << __TIME__;
    LOG_INFO << "*********************************************************";
}

int main()
{
    SetConsoleTitle(TEXT("NSE COM order"));

    FIN::ConfigReader confReader;
    ConfigParameter* confDetails = new ConfigParameter;

    initializeLog(confReader);        //initilize nanoLog

    confDetails->ReadConfigurationParametersFromFile(confReader);

    FIN::OrdGtwy::NNF::COM::Adapter _adapter(confDetails);

    try {

        std::thread t(&FIN::OrdGtwy::NNF::COM::Adapter::start, &_adapter);

        _adapter.startOmsRcv();

        t.join();
    }
    catch (std::exception& e)
    {
        DEVLOG("Exception : " << e.what());
        delete confDetails;
    }
    return 0;
}