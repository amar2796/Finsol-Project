#ifndef FIN_CONFIG_READER_H
#define FIN_CONFIG_READER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <mutex>
#include <fstream>
#include <unordered_set>

#include "common.h"
#include "path.h"
#include <unordered_map>
#include "Nanolog.hpp"

namespace FIN {

	class ConfigReader
	{
	public:
		ConfigReader()
		{
		}

		void loadData(const std::string& configFile)
		{
			_fileName = configFile;
			boost::property_tree::ini_parser::read_ini(_fileName.c_str(), _pt);
		}

		template < typename T >
		T getValue(const std::string& key, const T& defaultValue)
		{
			return _pt.get< T >(key, defaultValue);
		}
	private:

		std::string _fileName;
		boost::property_tree::ptree _pt;
	};

	class SecurityFileHandler
	{
	public:
		SecurityFileHandler()
		{
		}

		/*void init()
		{
			std::lock_guard < std::mutex > lock(_mutex);
			std::string fileName = _currentPath.getCurrentPath() + FIN_CONST::SUBSCRIBED_TOKEN_FILE_PATH;
			std::string f_fileName = _currentPath.getCurrentPath() + FIN_CONST::FILTTERED_SYMBOL_FILE_PATH;
			registerFile(fileName);
			ReadFiltteredSymbol(f_fileName);
		}*/

		bool addSymbolForSubscription(const std::string& line, std::vector<std::string>& vect_securityId)
		{
			bool isNoSymbolsInFile = false;
			if (_securityIdMap.size() <= 0) {
				isNoSymbolsInFile = true;
			}
			std::lock_guard < std::mutex > lock(_mutex);

			if (_securityIdMap.find(vect_securityId[0]) == _securityIdMap.end())
			{
				std::ofstream myfile(_fileName, std::ios::app);
				if (myfile.is_open())
				{
					_securityIdMap[vect_securityId[0]] = vect_securityId;
					if (!isNoSymbolsInFile)
						myfile << "\n";
					myfile << line;
					myfile.close();
					return true;
				}
			}


			return false;
		}
		bool symbolForUnSubscription(const std::string& symbol)
		{

			std::vector<std::string> symVec;
			std::vector< std::string >::iterator itr;

			std::fstream myfile(_fileName, std::ios::in);
			std::string line;
			//std::lock_guard < std::mutex > lock(_mutex);
			_securityIdMap.erase(symbol);

			if (myfile.is_open())
			{

				while (getline(myfile, line))
				{
					if (line.find(symbol) == std::string::npos && line != "")
					{
						symVec.push_back(line.c_str());

					}

				}
				myfile.close();

			}

			std::ofstream deleteData(_fileName, std::ios::trunc);
			deleteData.close();

			if (symVec.size() > 0)
			{
				std::fstream myFile(_fileName, std::ios::out);
				if (myFile.is_open())
				{

					for (itr = symVec.begin(); itr != symVec.end(); itr++)
					{
						myFile << (*itr);
						if (symVec.size() > 1)
							myFile << "\n";
					}
					myFile.close();
					return true;

				}
			}
			else
			{
				return true;
			}

			return false;
		}
		bool isSymbolSubscribed(const std::string& symbol)
		{
			//std::lock_guard < std::mutex > lock ( _mutex ) ;
			return _securityIdMap.find(symbol) != _securityIdMap.end();
		}
		std::unordered_map <std::string, std::vector<std::string>> getAllSubscribedSymbols()
		{
			std::lock_guard < std::mutex > lock(_mutex);
			return _securityIdMap;
		}
		std::unordered_set< std::string > getAllFilteredSymbols()
		{
			std::lock_guard < std::mutex > lock(_mutex);
			return _filttered;
		}


		std::unordered_set < std::string > _filttered;
		std::unordered_map < std::string, std::vector<std::string> > _securityIdMap;
		char delimiter = ',';
	private:

		void registerFile(const std::string& fileName)
		{
			_fileName = fileName;
			std::string line;
			std::ifstream myfile(_fileName);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					if (line != "")
					{
						std::vector<std::string> result;
						std::stringstream string_stream(line);
						std::string item;

						while (std::getline(string_stream, item, delimiter)) {
							result.push_back(item);
						}
						_securityIdMap.insert(std::pair<std::string, std::vector<std::string>>(result[0], result));
					}
				}
				myfile.close();
			}
		}
		void ReadFiltteredSymbol(const std::string& fileName)
		{
			std::string line;
			std::ifstream myfile(fileName);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					_filttered.insert(line.c_str());
				}
				myfile.close();
			}


		}


		std::string _fileName;
		std::unordered_set < std::string >::iterator _itr;

		std::mutex _mutex;
		ConfigReader _confReader;
		CurrentPath _currentPath;
	};
}

#endif

