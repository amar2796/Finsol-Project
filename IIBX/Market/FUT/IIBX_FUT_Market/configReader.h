#ifndef FIN_COMMON_CONFIG_READER
#define FIN_COMMON_CONFIG_READER

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <fstream>	
#include <unordered_set>
#include<vector>
#include "defines.h"
namespace FIN {

	//! load and parsing config file ,can extract key value pair from file
	class ConfigReader
	{
		public : 
			ConfigReader () 
			{
			}

			void loadData( const std::string &configFile )  
			{
				m_fileName = configFile ;	
				try
				{				
					boost::property_tree::ini_parser::read_ini( m_fileName.c_str(), _pt ) ;
				}
				catch(std::exception & e){
					DEVLOG(e.what());
				}
			}

			template < typename T > 
				T getValue( const std::string &key, const T& defaultValue  )
				{
					return _pt.get< T > ( key, defaultValue ) ;
				}
			template < typename T > 
				void setValue( const  std::string &key, const T& value  )
				{
			      _pt.put( key, value ) ;
				}
				
				void write(const std::string &configFile)
				{
					boost::property_tree::ini_parser::write_ini(configFile.c_str(),_pt);
				}
		private :

			std::string m_fileName ;
			boost::property_tree::ptree _pt ;
	};
	
	class SecurityFileHandler
	{
		public :
		
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
		
		bool symbolForSubscription( const std::string& symbol ) 
		{
			
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
		}
		
		bool symbolForUnSubscription(const std::string& symbol)
		{

			std::vector<std::string> symVec;
			std::vector<std::string>::iterator itr;

			std::fstream myfile(_fileName, std::ios::in);
			std::string line;
			if(_symbolsInFile.find(symbol) == _symbolsInFile.end())
			{
				std::cout<<symbol<<" is not in file \n";
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

		std::unordered_set < std::string > _symbolsInFile ;
		private :
		
		std::unordered_set < std::string >::iterator _itr ;
		std::string _fileName ;
	};	

}

#endif
