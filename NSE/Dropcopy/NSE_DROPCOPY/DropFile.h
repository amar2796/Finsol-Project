#pragma once
#ifndef FIN_DROP_FILE_H
#define FIN_DROP_FILE_H
#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include "Nanolog.hpp"
#include <synchapi.h>
#include "configReader.h"
#include "CurrentPath.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
#include "common.h"
#include <vector>
#include "Constants.h"
#include "ThreadSafeQueue.h"
#include <unordered_map>
#include <cstdio>
using namespace std;
namespace FIN {


	class DropFile
	{

	public:
		void pushToQueue(std::string msg)
		{
			tradeMessageQ.push(msg);
		}
		DropFile()
		{

			ConfigReader _conf;
			_conf.loadData(_currentPath.getCurrentPath() + FIN::CONSTANT::NSE_DROPCOPY_CONFIG);

			filePath = _conf.getValue<std::string>(FIN::CONSTANT::LOGGING + FIN::CONSTANT::SEP_DOT + "FILE_STORE_PATH", FIN::CONSTANT::EMPTY_STRING);
			if (directoryExists(filePath))
			{
				filePath += "\\TradeFO" + getCurrentDate() + ".txt";
				ReadFile_Flush();
				CONSOLE_INFO_LOG("TRADE REPORT FILE INITIALIZED, WILL APPEAR AFTER TRADE CAPTURE RECEIVES FIRST TRADE REPORT");
			}

			std::thread t1(&DropFile::ProcessQueue, this);
			t1.detach();
			// Open a file for both reading and writing
			file.open(filePath, std::ios::in | std::ios::out | std::ios::app);

			if (!file.is_open()) {
				CONSOLE_ERROR_LOG("Failed to open the file!" + filePath);
				LOG_INFO << "Failed to open the file!" << filePath;
				return;
			}


		}

		~DropFile()
		{

		}
		bool IsDuplicateTrade(long trade)
		{
			if (!firstReport)
			{
				CONSOLE_INFO_LOG("STARTED WRITING IN FILE " + filePath);
				firstReport = true;
			}
			/*auto it = std::find(tradeList.begin(),
				tradeList.end(), trade);*/
			if (tradeList.find(trade) != tradeList.end())
			{
				return true;
			}
			else
			{
				//	tradeList.insert(trade);
				return false;
			}
		}


	private:
		std::string filePath;
		string dropCopyString = "";
		ThreadSafeQueue<std::string> tradeMessageQ;
		// long long tradeCount_infile = 0;
		CurrentPath	_currentPath;
		//std::vector<long> tradeList;
		std::unordered_set<long> tradeList;

		int count = 0;
		int checkCount = 50;

		bool firstReport = false;
		std::string NSE_SymbolName;
		std::unordered_map<long, std::string> cache;
		std::fstream file;
		std::string getCurrentDate() {
			auto now = std::chrono::system_clock::now();
			std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

			std::tm* localTime = std::localtime(&currentTime);

			if (localTime) {
				std::ostringstream oss;
				//oss << std::put_time(localTime, "%Y-%m-%d");
				oss << std::put_time(localTime, "%d%m%y");


				return oss.str();
			}
			else {
				return "Failed to get the current date.";
			}
		}



		std::string extractFirstIndex(const std::string& input) {
			size_t commaPos = input.find(',');
			if (commaPos != std::string::npos) {
				return input.substr(0, commaPos);
			}
			else {
				return input; // Return the entire string if no comma is found
			}
		}

		void ReadFile_Flush()
		{
			try
			{

				std::ofstream file(filePath.c_str(), std::ios::trunc); // Opens the file for writing and truncates its content

				if (file.is_open()) {
					file.close();
					LOG_INFO << "[INFO ] File flushed " << filePath;
					std::cout << "[INFO ] File flushed " << filePath << std::endl;
				}
				else {
					LOG_INFO << "[INFO ] Unable to opne file " << filePath;
					std::cout << "[INFO ] Unable to opne file " << filePath << std::endl;
				}

				/*if (std::remove(filePath.c_str()) == 0)
				{
					LOG_INFO << "[INFO ] File flushed " << filePath;
					std::cout << "[INFO ] File flushed " << filePath << std::endl;
				}*/
				/*struct stat sb;

				if (stat(filePath.c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR))
				{
					std::ifstream inputFile(filePath);

					if (!inputFile.is_open()) {
						return;
					}

					std::string line;
					while (std::getline(inputFile, line)) {

						if (line != "")
							tradeList.insert(std::stol(extractFirstIndex(line)));
					}

					inputFile.close();

				}*/

			}
			catch (std::exception& ex)
			{
				LOG_INFO << ex.what();
			}
		}

		bool directoryExists(const std::string& path)
		{
			struct stat sb;
			if (stat(path.c_str(), &sb) != 0)
			{
				LOG_INFO << "Given path does not exist " << path;
				CONSOLE_INFO_LOG("Given path does not exist " + path);
				return false;
			}
			return true;


		}

		std::string GetTransactTime(long long timestamp_ns)
		{
			string transacttime = "";
			try
			{
				// Convert nanoseconds to seconds and remaining nanoseconds
				int64_t timestamp_sec = timestamp_ns / 1'000'000'000;
				int64_t remaining_ns = timestamp_ns % 1'000'000'000;

				// Convert the timestamp to a time_point
				std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(timestamp_sec);

				// Add nanoseconds using duration_cast
				tp += std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(remaining_ns));

				// Convert the time_point to a tm structure
				std::time_t tt = std::chrono::system_clock::to_time_t(tp);
				std::tm time_info = *gmtime(&tt); // Use gmtime here

				// Format the time using put_time
				std::stringstream ss;
				ss << std::put_time(&time_info, "%d %b %Y %H:%M:%S");
				transacttime = ss.str();


				for (char& d : transacttime) {
					d = std::toupper(d);
				}

			}
			catch (std::exception& ex)
			{
				LOG_INFO << ex.what();
			}
			return transacttime;
		}
		std::string ExpiryConverter(long seconds) {
			string MaturityMonthYear;
			try {


				if (cache.find(seconds) != cache.end()) {
					// If the result is already cached, return it directly.
					return cache[seconds];
				}

				// Define the base date as January 1, 1980, 00:00:00
				std::tm fromDate = {};
				fromDate.tm_year = 80; // Year since 1900 (1980)
				fromDate.tm_mon = 0;  // Month (January)
				fromDate.tm_mday = 1; // Day
				fromDate.tm_hour = 0; // Hour
				fromDate.tm_min = 0;  // Minute
				fromDate.tm_sec = 0;  // Second

				std::tm dataw_expiry_month = fromDate;
				dataw_expiry_month.tm_sec += seconds;

				std::mktime(&dataw_expiry_month); // Normalize the time structure

				// Format the date as "dd-MMM-yy"
				char buffer[10]; // Room for "dd-MMM-yy" + '\0'
				std::strftime(buffer, sizeof(buffer), "%d%b%Y", &dataw_expiry_month);
				MaturityMonthYear = buffer;
				for (char& c : MaturityMonthYear) {
					c = std::toupper(c);
				}

				cache[seconds] = MaturityMonthYear;

				char sym_date_buffer[6]; // Room for "yyMMM" + '\0'
				std::strftime(sym_date_buffer, sizeof(sym_date_buffer), "%y%b", &dataw_expiry_month);
				NSE_SymbolName = sym_date_buffer;

				for (char& d : NSE_SymbolName) {
					d = std::toupper(d);
				}
			}
			catch (const std::exception& ex) {
				std::cerr << ex.what() << std::endl;
			}
			return MaturityMonthYear;
		}

		std::string GetNseSymbol(std::string strike_price, std::string& symbol, std::string& optionType)
		{
			string localSymbol = "";
			try
			{
				symbol.erase(std::remove_if(symbol.begin(), symbol.end(), ::isspace), symbol.end());

				localSymbol = symbol + NSE_SymbolName + strike_price + optionType;


			}
			catch (const std::exception& ex) {
				std::cerr << ex.what() << std::endl;
			}
			return localSymbol;
		}
		std::string paddingSpace(const std::string& input, size_t totalWidth, bool isRight)
		{
			if (input.length() >= totalWidth) {
				return input;
			}
			if (isRight)
				return input + std::string(totalWidth - input.length(), ' ');
			else
				return  std::string(totalWidth - input.length(), ' ') + input;

		}


		void writeInFile(std::string& msg)
		{
			file << msg << std::endl;
			file.flush();
			//if (count == checkCount)
			//{
			//	file.close();
			//	file.open(filePath, std::ios::in | std::ios::out | std::ios::app);
			//	count = 0;
			//	if (checkCount < 500)
			//	{
			//		checkCount += 100;
			//		//	std::cout << "Check Count " << checkCount << std::endl;
			//	}
			//}

		}
		void ProcessQueue()
		{
			try
			{

				while (true)
				{
					//tradeCount_infile += 1;
					auto trade = tradeMessageQ.front();
					writeInFile(trade);


					tradeMessageQ.pop();

					//count += 1;
					//LOG_INFO << " STORED IN FILE " << tradeCount_infile;
				}

			}
			catch (std::exception& ex)
			{
				LOG_INFO << ex.what();
			}
		}


	};

}
#endif