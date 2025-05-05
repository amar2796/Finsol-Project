#pragma once
#ifndef FIN_CONTRACT_FILE_H
#define FIN_CONTRACT_FILE_H
#include "configReader.h"
#include "CurrentPath.h"
#include <unordered_map>
#include <iostream>
#include <string>

using namespace std;
using namespace FIN;

class ReadContract
{
public:
	bool directoryExists(const std::string& path)
	{
		struct stat sb;
		if (stat(path.c_str(), &sb) != 0)
		{
			LOG_INFO << "Given path does not exist " << path;
			//CONSOLE_INFO_LOG("Given path does not exist " + path);
			return false;
		}
		return true;


	}

	void Read()
	{
		try
		{
			if (directoryExists(contractPath))
			{
				struct stat sb;
				if (stat(contractPath.c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR))
				{
					std::ifstream inputFile(contractPath);

					if (!inputFile.is_open()) {
						return;
					}

					std::string line;
					while (std::getline(inputFile, line)) {

						if (line != "")
						{

							std::vector<std::string> dataArray;
							std::stringstream ss(line);
							std::string item;

							while (std::getline(ss, item, '|')) {
								dataArray.push_back(item);
							}

							if (dataArray.size() > 0)
								contractDetails[dataArray[0]] = dataArray;


						}
					}

					inputFile.close();

				}
			}
			else
			{
				LOG_INFO << "Contract file not found";
			}
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
	}

	ReadContract()
	{
		ConfigReader _conf;
		_conf.loadData(_currentPath.getCurrentPath() + FIN::CONSTANT::NSE_DROPCOPY_CONFIG);

		contractPath = _conf.getValue<std::string>(FIN::CONSTANT::LOGGING + FIN::CONSTANT::SEP_DOT + "CONTRACT_DIR", FIN::CONSTANT::EMPTY_STRING);
		Read();
	}
	std::unordered_map<std::string, std::vector<string>> contractDetails;
private:
	CurrentPath	_currentPath;
	std::string contractPath;
};




#endif