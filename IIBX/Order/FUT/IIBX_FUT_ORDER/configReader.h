#ifndef FIN_CONFIG_READER_H
#define FIN_CONFIG_READER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <mutex>
#include <fstream>
#include <unordered_set>

#include "common.h"
#include "CurrentPath.h"

namespace FIN {

	class ConfigReader
	{
		public : 
			ConfigReader () 
			{
			}

			void loadData( const std::string &configFile )  
			{
				_fileName = configFile ;		
				boost::property_tree::ini_parser::read_ini( _fileName.c_str(), _pt ) ;
			}

			template < typename T > 
				T getValue( const std::string &key, const T& defaultValue  )
				{
					return _pt.get< T > ( key, defaultValue ) ;
				}
		private :

			std::string _fileName ;
			boost::property_tree::ptree _pt ;
	};
}

#endif
