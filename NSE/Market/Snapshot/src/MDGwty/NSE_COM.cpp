#include "publisher.h"
#include <iostream>
#include "message.h"
#include "NseComMarketData.h"
#include "common.h"
#include <map>
#include <ctime>
#include <algorithm>
//#include "../rabbit/rabbitmqConsumer.h"
enum instruction { wrong, ohlc, queue_size, depth, log_e, log_d };
std::map<std::string, instruction> instructions;
class console_inst
{
public:
	std::string input;
	console_inst()
	{
		register_levels();

	}
	void register_levels()
	{
		instructions["ohlc"] = ohlc;
		instructions["queue_size"] = queue_size;
		instructions["log_e"] = log_e;
		instructions["depth"] = depth;
		instructions["log_d"] = log_d;

	}
	void process_console_instructions()
	{
		while (true)
		{
			std::cin >> input;
			std::cout << input << std::endl;
			switch (instructions[input])
			{
			case wrong:
				std::cout << "Wrong instruction" << std::endl;
				break;

			case log_d:
				FIN::MDGtwy::Publisher::getInstance()->_enableLog = false;
				std::cout << "[ CONSOLE LOG DIABLE ]" << std::endl;
				break;
			case log_e:
				FIN::MDGtwy::Publisher::getInstance()->_enableLog = true;
				std::cout << "[ CONSOLE LOG ENABLE ]" << std::endl;
				break;


			}
		}
	}
};
int main()
{
	try
	{
		SetConsoleTitle(TEXT("NSE COM market"));

		FIN::ConfigReader confReader;
		FIN::MDGtwy::CurrentPath _currentPath;
		confReader.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);

		std::time_t t = time(NULL) + 19800;
		auto gmtime = std::gmtime(&t);
		char buffer[32];
		std::string logPath = confReader.getValue < std::string >(FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + "Log_Path", FIN::FIN_CONST::EMPTY_STRING
			);
		//strftime(buffer, 32, "%Y%m%d-%T", gmtime);
		strftime(buffer, 32, "%Y%m%d%T", gmtime);
		std::string t1(buffer);
		std::string fileName;
		t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
		fileName = "NSE_COM-" + t1 + "__";
		nanolog::initialize(nanolog::GuaranteedLogger(), logPath + "/", fileName, 200);

		std::cout << "Binary Build Date: " << __DATE__ << "-" << __TIME__ << " - CO 5398, CV 5398 " << std::endl;
		LOG_INFO << "*********************************************************";
		LOG_INFO << "Binary Build Date: " << __DATE__ << "-" << __TIME__;
		LOG_INFO << "*********************************************************";

		boost::asio::io_service ioService;
		FIN::MDGtwy::Publisher::getInstance()->init(ioService, _currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);

		FIN::ConfigReader _ConfigReader;
		_ConfigReader.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);

		std::string nse_listen_interface = _ConfigReader.getValue < std::string >(
			FIN::FIN_CONST::NSE_COM_MULTICAST_RECEIVER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::NSE_LISTEN_INTERFACE,
			FIN::FIN_CONST::EMPTY_STRING
			);
		std::string nse_multicast_addr = _ConfigReader.getValue < std::string >(
			FIN::FIN_CONST::NSE_COM_MULTICAST_RECEIVER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::NSE_MULTICAST_ADDR,
			FIN::FIN_CONST::EMPTY_STRING
			);

		int nse_multicast_port = _ConfigReader.getValue <int>(
			FIN::FIN_CONST::NSE_COM_MULTICAST_RECEIVER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::NSE_MULTICAST_PORT,
			FIN::FIN_CONST::EMPTY_INTEGER
			);
		std::string _fileName = _currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
		int exchangeNumber = _ConfigReader.getValue <int>(
			FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + "Exchange_number", FIN::FIN_CONST::EMPTY_INTEGER);

		std::string price_format = _ConfigReader.getValue < std::string >(
			"PUBLISHER.PriceFormat",
			FIN::FIN_CONST::EMPTY_STRING
			);

		FIN::NSE::COM::NseComMd* nseMd=new FIN::NSE::COM::NseComMd(_fileName, exchangeNumber);
		nseMd->IsDoublePriceFormat(price_format);

		console_inst* inst = new console_inst();
		std::thread start_process(&console_inst::process_console_instructions, inst);
		start_process.detach();

		//for listen token from tbt
		bool listenTokenFlag = confReader.getValue < bool >(
			FIN::FIN_CONST::RABBIT_SUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::GET_TOKEN, false);

		bool listenLiveRequestFlag = _ConfigReader.getValue <bool>(
			FIN::FIN_CONST::RABBIT_SUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::LISTENTOLIVEREQUEST, false);

		if (listenLiveRequestFlag && listenTokenFlag)
		{
			std::string rabbit_ip = confReader.getValue < std::string >(
				FIN::FIN_CONST::RABBIT_SUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RABBIT_IP,
				FIN::FIN_CONST::EMPTY_STRING);

			int rabbit_port = confReader.getValue < FIN::INT_32 >(
				FIN::FIN_CONST::RABBIT_SUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RABBIT_PORT,
				FIN::FIN_CONST::EMPTY_INTEGER);

			std::thread rabbitSubProcess(&FIN::NSE::COM::NseComMd::startRabbitListenToken, nseMd, rabbit_ip, rabbit_port);
			rabbitSubProcess.join();
		}

		//read Security file 
		nseMd->read_securityId_from_file(_fileName);

		//for livesubscrion process
		if (listenLiveRequestFlag)
		{
			std::string rabbit_ip = confReader.getValue < std::string >(
				FIN::FIN_CONST::RABBIT_SUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RABBIT_IP,
				FIN::FIN_CONST::EMPTY_STRING);

			int rabbit_port = confReader.getValue < FIN::INT_32 >(
				FIN::FIN_CONST::RABBIT_SUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RABBIT_PORT,
				FIN::FIN_CONST::EMPTY_INTEGER);

			std::thread consumer(&FIN::NSE::COM::NseComMd::startRabbitSubscriptionProcess, nseMd , rabbit_ip, rabbit_port);
			consumer.detach();
		}

		//start thread to process
		nseMd->processMarketData(nse_multicast_addr, nse_listen_interface, nse_multicast_port);

		do
		{
			getchar();
		} while (true);
		delete nseMd;
	}
	catch (std::exception& ex)
	{
		LOG_INFO << ex.what();
	}
	catch (...)
	{
		LOG_INFO << "UNCAUGHT EXCEPTION";
	}
	
	return 0;
}

