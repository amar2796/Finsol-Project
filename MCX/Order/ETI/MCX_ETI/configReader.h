#ifndef FIN_CONFIG_READER_H
#define FIN_CONFIG_READER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <mutex>
#include <fstream>
#include <unordered_set>

#include "common.h"
#include "path.h"

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

	class SecurityFileHandler
	{
		public :
		SecurityFileHandler () 
		{
		}

		void init()
		{
			std::lock_guard < std::mutex > lock ( _mutex ) ;
			_confReader.loadData (_currentPath.getCurrentPath()+FIN_CONST::FIN_CONFIG_FILE ) ;
			std::string fileName = _confReader.getValue < std::string >( 
					FIN_CONST::MD_SUBSCRIPTION + FIN_CONST::SEP_DOT + FIN_CONST::FILE_NAME , 
					FIN_CONST::EMPTY_STRING 
					) ;
			registerFile ( fileName ) ;
		}

		bool addSymbolForSubscription( const std::string &symbol ) 
		{
			std::lock_guard < std::mutex > lock ( _mutex ) ;
			if ( _symbolsInFile.insert ( symbol ).second )
			{
				std::ofstream myfile (_fileName, std::ios::app);
				if (myfile.is_open())
				{
					myfile << symbol << std::endl;
					myfile.close();
					return true ;
				}
			}

			return false ;
		}

		bool symbolForUnSubscription(const std::string &symbol)
		{

			std::vector<std::string> symVec;
			std::vector< std::string >::iterator itr;

			std::fstream myfile(_fileName, std::ios::in);
			std::string line;
			//std::lock_guard < std::mutex > lock(_mutex);
			_symbolsInFile.erase(symbol.substr(0, symbol.find(',')).c_str());

			if (myfile.is_open())
			{

				while (getline(myfile, line))
				{
					if (line != symbol)
					{
						symVec.push_back(line.c_str());

					}

				}
				myfile.close();

			}

			std::fstream myFile(_fileName, std::ios::out);
			if (myFile.is_open())
			{

				for (itr = symVec.begin(); itr != symVec.end(); itr++)
				{
					myFile << (*itr) << "\n";
				}
				myFile.close();
				return true;

			}


			return false;
		}

		bool isSymbolSubscribed( const std::string& symbol )
		{
			//std::lock_guard < std::mutex > lock ( _mutex ) ;
			return _symbolsInFile.find ( symbol ) != _symbolsInFile.end() ;
		}

		std::unordered_set< std::string > getAllSubscribedSymbols()
		{
			std::lock_guard < std::mutex > lock ( _mutex ) ;
			return _symbolsInFile ;
		}
		/*
		bool symbolSetUpdation(const std::string& symbol )
		{
			std::lock_guard < std::mutex > lock(_mutex);

		}
		*/
		private :
		
		void registerFile ( const std::string &fileName ) 
		{
			_fileName = fileName ;
			std::string line;
			std::ifstream myfile ( _fileName );
			if (myfile.is_open())
			{
				while ( getline (myfile,line) )
				{
					_symbolsInFile.insert (line.substr(0, line.find(',')).c_str()) ;
				}
				myfile.close();
			}
		}

		
		std::string _fileName ;

		std::unordered_set < std::string > _symbolsInFile ;
		std::unordered_set < std::string >::iterator _itr ;

		std::mutex _mutex ;
		ConfigReader _confReader ;
		CurrentPath _currentPath;
	};
}

#endif
