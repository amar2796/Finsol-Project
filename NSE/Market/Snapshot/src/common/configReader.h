#ifndef FIN_CONFIG_READER_H
#define FIN_CONFIG_READER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <mutex>
#include <fstream>
#include <unordered_set>

#include "common.h"
#include<stdlib.h>
namespace FIN {

	class ConfigReader
	{
	public:
		ConfigReader()
		{

		}

		void loadData(const std::string& configFile)
		{
			try {

				_fileName = configFile;
				boost::property_tree::ini_parser::read_ini(_fileName.c_str(), _pt);
			}
			catch (std::exception& e)
			{
				std::cout << "Exception in load data : " << e.what() << std::endl;
			}
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

		void init(std::string configFile)
		{
			_confReader.loadData(configFile.c_str());
			std::string fileName = _confReader.getValue < std::string >(
				FIN_CONST::MD_SUBSCRIPTION + FIN_CONST::SEP_DOT + FIN_CONST::FILE_NAME,
				FIN_CONST::EMPTY_STRING
				);
			_fileName = fileName;
			std::string line;
			std::ifstream myfile(fileName);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					if (line == "")
						continue;
					std::string token = line.substr(0, line.find(',')).c_str();
					if (_tokenFromTBT.find(token) == _tokenFromTBT.end())
						_symbolsInFile.insert(line.substr(0, line.find(',')).c_str());
					else
						std::cout << line <<" : is Already Subscribed in TBT" << std::endl;
				}
				myfile.close();
			}
		}



		bool isSymbolSubscribed(const std::string& symbol)
		{
			return _symbolsInFile.find(symbol) != _symbolsInFile.end();
		}

		std::unordered_set< std::string > getAllSubscribedSymbols()
		{
			return _symbolsInFile;
		}
		std::unordered_map< std::string, std::string > getAllSubscribedSymbolMapping()
		{
			return _symbolmapping;
		}
		bool addSymbolForSubscription(const std::string& symbol)
		{
			bool isNoSymbolsInFile = false;
			if (_symbolsInFile.size() <=0) {
				isNoSymbolsInFile = true;
			}
			if ( _symbolsInFile.insert ( symbol ).second )
			{
				std::ofstream myfile (_fileName, std::ios::app);
				if (myfile.is_open())
				{
					if(!isNoSymbolsInFile)
						myfile << "\n";
					
					myfile << symbol;
					myfile.close();
					return true ;
				}
			}

			return false;
		}

		bool symbolForUnSubscription(const std::string& symbol)
		{
			std::vector<std::string> symVec;
			std::vector<std::string>::iterator itr;

			std::fstream myfile(_fileName, std::ios::in);
			std::string line;

			int countLine = 0;

			if (_symbolsInFile.find(symbol) == _symbolsInFile.end())
			{
				std::cout << symbol << " is not in file \n";
				return false;
			}
			_symbolsInFile.erase(symbol.c_str());
			if (myfile.is_open())
			{

				while (getline(myfile, line))
				{
					if (line != symbol)
					{
						symVec.push_back(line);
						countLine++;
					}

				}
				myfile.close();

			}

			std::fstream myFile(_fileName, std::ios::out);
			if (myFile.is_open())
			{

				for (itr = symVec.begin(); itr != symVec.end(); itr++)
				{
					--countLine;
					myFile << (*itr);
					if (symVec.size() > 0 && countLine != 0)
						myFile << "\n";
				}
				myFile.close();
				return true;

			}


			return false;
		}

		void tokenFromTBT(std::string value)
		{
			_tokenFromTBT.insert(value);
		}

		bool findTokenInTBT(std::string value)
		{
			if (_tokenFromTBT.find(value) != _tokenFromTBT.end())
				return true;

			return false;
		}
	private:

		void registerFile(const std::string& fileName)
		{
			/*
				_fileName = fileName;
				std::string line;
				std::ifstream myfile(_fileName);
				if (myfile.is_open())
				{
					while (getline(myfile, line))
					{
						if (line == "")
							continue;
						_symbolsInFile.insert(line.substr(0, line.find(',')).c_str());
					}
					myfile.close();
				}
				*/
				//_fileName = fileName;
				//std::string line;
				//std::ifstream myfile(_fileName);
				//
				//	if (myfile.is_open())
				//	{
				//		/*while (getline(myfile, line))
				//		{
				//			_symbolsInFile.insert(line);
				//		}*/
				//
				//		while (getline(myfile, line))
				//		{
				//			int position = 0;
				//			if (line == "")continue;
				//			position = line.find("=");
				//			if (position != std::string::npos)
				//			{
				//				auto first_string = line.substr(0, position);
				//				auto second_string = line.substr(position + 1, line.length() - position);
				//				std::cout << "position + 1 : " << position + 1 << "\n line.length() - position : " << line.length() - position << "\n";
				//				_symbolmapping.insert(std::make_pair(first_string, second_string));
				//			}
				//			else
				//				_symbolsInFile.insert(line.substr(0, line.find(',')).c_str());
				//		
				//		}
				//		
				//		myfile.close();
				//		
				//	}
				//
				//
		}


		//std::string _fileName;

		std::unordered_set < std::string > _symbolsInFile;
		std::unordered_map<std::string, std::string> _symbolmapping;
		std::unordered_set < std::string >::iterator _itr;
		std::unordered_set<std::string> _tokenFromTBT;
		
		ConfigReader _confReader;
		std::string _fileName;
	};


}

#endif
