#ifndef FIN_CONFIG_READER_H
#define FIN_CONFIG_READER_H
#include "constants.h"
#include "Nanolog.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <fstream>
#include <unordered_set>

namespace FIN
{

	class ConfigReader
	{
	public:
		ConfigReader()
		{

		}

		void loadData(const std::string& configFile)
		{
			try
			{

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

		void readTokenFile(const std::string& fileName)
		{
			try
			{
				_fileName = fileName;
				std::string line;
				std::ifstream myfile(_fileName);
				if (myfile.is_open())
				{
					while (getline(myfile, line))
					{
						_symbolsInFile.insert(std::stoi(line));
					}
					myfile.close();
				}
				else {
					std::cout << "Token file not open." << std::endl;
				}
			}
			catch (const std::exception& e)
			{
				LOG_INFO << e.what();
			}
		}
		std::unordered_set < int32_t > _symbolsInFile;
	private:

		//std::unordered_set < std::string >::iterator _itr;
		std::string _fileName;
	};
}
#endif