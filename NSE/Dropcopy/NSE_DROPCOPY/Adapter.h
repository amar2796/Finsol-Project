#ifndef FIN_ADAPTER
#define FIN_ADAPTER
#include "connectionBuilder.h"
#include <boost/asio.hpp>
#include "string"
#include <algorithm>
#include "configReader.h"
#include "CurrentPath.h"
#include "DropFile.h"
#include "ReadContractFile.h"

#include <unordered_map>

#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
namespace FIN {

	class Adapter :public ConnectionBuilder
	{
	public:
		Adapter(boost::asio::io_service& ioService);
		bool init();
		void stop();
		void start();
		void initCSVStoring(std::string, std::string);
		CurrentPath _currentPath;
		std::string confFileName;
		std::string nnfSubscriptionFileName;//Stores NNf fiels based on nnf field present in file send to oms
		SecurityFileHandler _nnfFieldSubscriptionHandler;
		INT64 traderId;
		bool isLoggingEnable;
		std::string NSE_SymbolName;

		string dropCopyString = "";

		long long trade_count = 0;
		long long Unique_trade_count = 0;

		std::unordered_map<long, std::pair< std::string, std::string>> cache;


		// opens an existing csv file or creates a new file.
	protected:
		bool isStoreCSV;
		std::string CSVPath;
		std::string csvfileName;
		std::fstream csv_fout;
	private:
		DropFile drop_file;
		//ReadContract contract_Read;

		void onLogon();
		bool isRMQEnable;
		int counter = 0;
		boost::asio::io_service& _ioService;

		//RabbitmqConnector _sendDataRabbitmq;		
		volatile bool _loggedIn;
		void onMessage(std::string msg);
		void onMessageStringSend(FIN::TRADE_CONFIRMATION* msg);
		void onMessageToTxt(FIN::TRADE_CONFIRMATION* msg);
		void onMessageToNewTxt(FIN::TRADE_CONFIRMATION* msg, std::string status);

		std::string paddingSpace(const std::string& input, size_t totalWidth, bool isRight);
		std::string ExpiryConverter(long expiry);
		std::string GetNseSymbol(std::string strikePrice, std::string&, std::string&);
		std::string CheckStrike(long, string);
		std::string CheckOptionType(string);

		void onMessage(FIN::TRADE_CONFIRMATION* msg);
		std::string GetTransactTime(long long);

		std::string GetCurrentDate();
		std::string storeCurrentDate;
		// Function to remove all spaces from a given string
		std::string removeSpaces(std::string str)
		{
			str.erase(remove(str.begin(), str.end(), ' '), str.end());
			return str;
		}
		std::string trimAfterSpace(std::string str)
		{

			std::string trimmed;
			int i = 0;
			for (i = 0;i < str.length();i++)
			{
				if (str[i] == ' ' || str[i] == '\0')
				{
					break;
				}
				trimmed += str[i];
			}
			//LOG_INFO << "TRIMED" << trimmed<< " i="<<i;
			return trimmed;
		}
		std::string trimNNFFieldTo12Digit(double nnfField) {
			std::string _nnf = std::to_string((INT64)nnfField);//First remove decimal and convert to string
			std::string result = _nnf.substr(0, _nnf.length() - 3);//Now trim last three digits
			return result;
		}
	};
}
#endif