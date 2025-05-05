//#include "stdafx.h"
#include <Windows.h>
#include "../HeaderFile/AllHeaders.h"
#include "ReadConfigurationFile.h"
#include <string>
#define MAX_PATH 512 
using namespace std;

ReadConfigurationFile::ReadConfigurationFile()
{
}

ReadConfigurationFile::ReadConfigurationFile(SessionSettings* s)
{
	s1 = s;
	SeqNum = 0;
}

std::string ReadConfigurationFile::SeqNumFile(std::string)
{
	std::string seqs;
	while (1)
	{
		Sleep(0);
		WriteSeqNumfile.open("C:\\text1.txt");
		WriteSeqNumfile << "SeqNum :" << ++SeqNum << std::endl;
		std::ifstream ReadSeqNumfile("C:\\text1.txt");
		ReadSeqNumfile >> SeqNum;
		seqs = std::to_string(SeqNum);
		WriteSeqNumfile.clear();
		WriteSeqNumfile.close();
		ReadSeqNumfile.close();
		return seqs;
	}
}


void ReadConfigurationFile::ReadConfigurationParametersFromFile()
{
	try
	{
		ConfigReader confReader;

		char buffer[512];
		GetModuleFileNameA(NULL, buffer, 512);     // windows operation so will also made generice for both os.
		std::string path(buffer);
		size_t slashPosition1 = path.rfind("\\");
		path = path.substr(0, slashPosition1 + 1);

		confReader.loadData(path + FIN::FIN_CONFIG_FILE);

		s1->certificatePath = path;

		s1->ExchangeIp = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::ExchangeIP,
			FIN::EMPTY_STRING);

		s1->ExchangePort = confReader.getValue < INT >(
			FIN::SESSION + FIN::SEP_DOT + FIN::ExchangePort,
			FIN::EMPTY_INTEGER);


		s1->udp_senderIp = confReader.getValue < std::string >(
			FIN::UDP_CONNECTION + FIN::SEP_DOT + FIN::SendIP,
			FIN::EMPTY_STRING);

		s1->udp_senderPort = confReader.getValue < INT >(
			FIN::UDP_CONNECTION + FIN::SEP_DOT + FIN::SendPort,
			FIN::EMPTY_INTEGER);

		s1->udp_rcverPort = confReader.getValue < INT >(
			FIN::UDP_CONNECTION + FIN::SEP_DOT + FIN::RcvPort,
			FIN::EMPTY_INTEGER);

		s1->SessionId = confReader.getValue < INT >(
			FIN::SESSION + FIN::SEP_DOT + FIN::SessionId,
			FIN::EMPTY_INTEGER);

		s1->HeartBt = confReader.getValue < INT >(
			FIN::SESSION + FIN::SEP_DOT + FIN::HeartBeat,
			FIN::EMPTY_INTEGER);

		s1->Password = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::Password,
			FIN::EMPTY_STRING);

		s1->newPassword = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::NewPassword,
			FIN::EMPTY_STRING);

		s1->changePassword = confReader.getValue < int >(
			FIN::SESSION + FIN::SEP_DOT + FIN::ChangePassword,
			FIN::EMPTY_INTEGER);

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

		s1->TradeFile_path = s1->log_path = confReader.getValue < std::string >(
			FIN::LOGGING + FIN::SEP_DOT + FIN::LogPath,
			FIN::EMPTY_STRING);

		s1->rabbitMqIP = confReader.getValue < std::string >(
			FIN::RMQ_CONNECTION + FIN::SEP_DOT + FIN::rabbitMqIP,
			FIN::EMPTY_STRING);

		s1->rabbitMqPort = confReader.getValue < INT >(
			FIN::RMQ_CONNECTION + FIN::SEP_DOT + FIN::rabbitMqPort,
			FIN::EMPTY_INTEGER);

		s1->ExchangeNumber = confReader.getValue < std::string >(
			FIN::RMQ_CONNECTION + FIN::SEP_DOT + FIN::ExchangeNumber,
			FIN::EMPTY_STRING);

		s1->listenPort = confReader.getValue < INT >(
			FIN::OMS_CONNECTION + FIN::SEP_DOT + FIN::ListenPort,
			FIN::EMPTY_INTEGER);

		s1->listnerIP = "127.0.0.1";
		s1->algoId = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::AlgoID,
			FIN::EMPTY_STRING);

		//s1->TradeFile_path += "\\seq_number.txt";

		s1->oms_channel = confReader.getValue < INT >(
			FIN::OMS_GW_COM + FIN::SEP_DOT + FIN::OMS_Channel,
			FIN::EMPTY_INTEGER);

		s1->throttleLimit_msg = confReader.getValue<INT>(
			FIN::OMS_THROTTLE + FIN::SEP_DOT + FIN::THROTTLE_MSG_LIMIT, FIN::EMPTY_INTEGER
		);

		s1->throttleLimit_time = confReader.getValue<INT>(
			FIN::OMS_THROTTLE + FIN::SEP_DOT + FIN::THROTTLE_TIME_WINDOW, FIN::EMPTY_INTEGER
		);

	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		throw;
	}
}
