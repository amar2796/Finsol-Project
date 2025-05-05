#pragma once
#ifndef FIN_DROP_FILE_H
#define FIN_DROP_FILE_H
#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <synchapi.h>
#include "ReadConfigurationFile.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>
#include "common.h"
#include <vector>
#include "ThreadSafeQueue.h"
#include <unordered_map>
#include <boost/date_time.hpp>
#include "Nanolog.hpp"
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

			char buffer[512];
			GetModuleFileNameA(NULL, buffer, 512);     // windows operation so will also made generice for both os.
			std::string path(buffer);
			size_t slashPosition1 = path.rfind("\\");
			path = path.substr(0, slashPosition1 + 1);

			_conf.loadData(path + FIN::FIN_CONFIG_FILE);

			filePath = _conf.getValue<std::string>("DROPCOPY.DropCopyFilePath", "");
			if (directoryExists(filePath))
			{
				filePath += "\\EQD_ITRTM_2102_" + getCurrentDate() + ".csv";
				ReadFile_flush();
				std::cout << "TRADE REPORT FILE INITIALIZED, WILL APPEAR AFTER TRADE CAPTURE RECEIVES FIRST TRADE REPORT" << std::endl;
			}

			std::thread t1(&DropFile::ProcessQueue, this);
			t1.detach();
			// Open a file for both reading and writing
			file.open(filePath, std::ios::in | std::ios::out | std::ios::app);

			if (!file.is_open()) {
				std::cout << "Failed to open the file!" + filePath << std::endl;
				LOG_INFO << "Failed to open the file!" + filePath;
				return;
			}


		}

		~DropFile()
		{

		}
		bool IsDuplicateTrade(unsigned int trade)
		{
			if (!firstReport)
			{
				std::cout << "STARTED WRITING IN FILE " + filePath << std::endl;
				firstReport = true;
			}
			/*auto it = std::find(tradeList.begin(),
				tradeList.end(), trade);*/
			if (tradeList.find(trade) != tradeList.end())
			{
				LOG_INFO << "Duplicate Trade found";
				return true;
			}
			else
			{
				return false;
			}
		}


	private:
		string dropCopyString = "";
		ThreadSafeQueue<std::string> tradeMessageQ;
		std::string filePath;
		std::unordered_set<unsigned int> tradeList;

		bool firstReport = false;
		std::fstream file;

		std::string getCurrentDate() {

			boost::gregorian::date currentDate = boost::gregorian::day_clock::local_day();

			return boost::gregorian::to_iso_string(currentDate);
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

		void ReadFile_flush()
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

			}
			catch (std::exception& ex)
			{
				std::cout << ex.what();
			}
		}

		bool directoryExists(const std::string& path)
		{
			struct stat sb;
			if (stat(path.c_str(), &sb) != 0)
			{
				LOG_INFO << "Given path does not exist " + path;
				std::cout << "Given path does not exist " + path << std::endl;
				return false;
			}
			return true;


		}

		int count = 0;
		int target = 100;

		void writeInFile(std::string& msg)
		{
			count++;
			file << msg << std::endl;
			file.flush();
			if (target == count) {
				target += 100;
				file.close();
				file.open(filePath, std::ios::in | std::ios::out | std::ios::app);
			}

		}

		void ProcessQueue()
		{
			try
			{

				while (true)
				{
					auto trade = tradeMessageQ.front();
					writeInFile(trade);
					tradeMessageQ.pop();
				}

			}
			catch (std::exception& ex)
			{
				std::cout << ex.what();
			}
		}


	};

}
#endif