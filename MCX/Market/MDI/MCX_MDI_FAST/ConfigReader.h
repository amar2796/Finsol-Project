#ifndef FIN_CONFIG_READER_H
#define FIN_CONFIG_READER_H
#include "constants.h"

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
}
#endif