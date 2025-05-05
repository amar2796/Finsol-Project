//#include "stdafx.h"
#include <Windows.h>
#include "AllHeaders.h"
#include "ReadConfigurationFile.h"
#include <string>
#include "Nanolog.hpp"
#define MAX_PATH 512 
using namespace std;

ReadConfigurationFile::ReadConfigurationFile()
{
}

ReadConfigurationFile::ReadConfigurationFile(SessionSettings* s)
{
	s1 = s;
	SeqNum = 0;
	ReadConfigurationParametersFromFile();
	initializeLogging(s->log_path);
}

void ReadConfigurationFile::initializeLogging(const std::string _path)
{
	std::time_t t = time(NULL) + 19800;
	auto gmtime = std::gmtime(&t);

	char buffer[32];
	strftime(buffer, 32, "%Y%m%d%T", gmtime);
	std::string t1(buffer);
	t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
	std::string fileName = "BSE_FO_DropCopy-" + t1 + "__";
	nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200);//200 is 200mb max file size


	LOG_INFO << "*********************************************************";
	LOG_INFO << "[BINARY UPDATED DATE] " << __DATE__ << "-" << __TIME__;
	LOG_INFO << "*********************************************************";
	
	std::cout << "*********************************************************" << std::endl;
	std::cout << "** Build On: " << __DATE__ << "-" << __TIME__ " CO : 5298  **" << std::endl;
	std::cout << "*********************************************************" << std::endl;

}


void ReadConfigurationFile::ReadConfigurationParametersFromFile()
{
	ConfigReader confReader;

	char buffer[512];
	GetModuleFileNameA(NULL, buffer, 512);     // windows operation so will also made generice for both os.
	std::string path(buffer);
	size_t slashPosition1 = path.rfind("\\");
	path = path.substr(0, slashPosition1 + 1);

	confReader.loadData(path + FIN::FIN_CONFIG_FILE);

	s1->SessionId = confReader.getValue < INT >(
		FIN::SESSION + FIN::SEP_DOT + FIN::SessionId,
		FIN::EMPTY_INTEGER);

	s1->heartbeat = confReader.getValue < INT >(
		FIN::SESSION + FIN::SEP_DOT + FIN::HeartBeat,
		FIN::EMPTY_INTEGER);

	s1->Password = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::Password,
		FIN::EMPTY_STRING);

	s1->DefaultCstmApplVerID = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::DefaultCstmApplVerID,
		FIN::EMPTY_STRING);

	s1->ApplUsageOrders = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::ApplUsageOrders,
		FIN::EMPTY_STRING);

	s1->ApplUsageQuotes = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::ApplUsageQuotes,
		FIN::EMPTY_STRING);

	s1->OrderRoutingIndicator = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::OrderRoutingIndicator,
		FIN::EMPTY_STRING);

	s1->ApplicationSystemName = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::ApplicationSystemName,
		FIN::EMPTY_STRING);

	s1->ApplicationSystemVersion = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::ApplicationSystemVersion,
		FIN::EMPTY_STRING);

	s1->ApplicationSystemVendor = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::ApplicationSystemVendor,
		FIN::EMPTY_STRING);

	s1->Username = confReader.getValue < INT >(
		FIN::SESSION + FIN::SEP_DOT + FIN::Username,
		FIN::EMPTY_INTEGER);

	s1->SenderLocationId = confReader.getValue < long long >(
		FIN::SESSION + FIN::SEP_DOT + FIN::SenderLocationId,
		FIN::EMPTY_INTEGER);

	s1->ExchangeIp = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::ExchangeIP,
		FIN::EMPTY_STRING);

	s1->ExchangePort = confReader.getValue < INT >(
		FIN::SESSION + FIN::SEP_DOT + FIN::ExchangePort,
		FIN::EMPTY_INTEGER);

	s1->algoId = confReader.getValue < std::string >(
		FIN::SESSION + FIN::SEP_DOT + FIN::AlgoID,
		FIN::EMPTY_STRING);

	s1->TradingSessionIdPath = confReader.getValue < std::string >(
		FIN::TRADINGSESSIONPATH + FIN::SEP_DOT + FIN::TradingSessionIdPath,
		FIN::EMPTY_STRING);

	s1->contractFilePath = confReader.getValue < std::string >(
		FIN::CONTRACTFILE + FIN::SEP_DOT + FIN::ContractFilePath,
		FIN::EMPTY_STRING);

	s1->log_path = confReader.getValue < std::string >(
		FIN::LOGGING + FIN::SEP_DOT + FIN::LogPath,
		FIN::EMPTY_STRING);

	s1->dropCopyPath = confReader.getValue < std::string >(
		FIN::DROPCOPY + FIN::SEP_DOT + FIN::DropCopyFilePath,
		FIN::EMPTY_STRING);

	s1->certificatePath = path;

	s1->partitionID = confReader.getValue < INT >(
		FIN::PARTITIONID + FIN::SEP_DOT + FIN::PartitionID,
		FIN::EMPTY_INTEGER);

}
