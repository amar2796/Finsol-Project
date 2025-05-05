#pragma once
#ifndef READFILE_H
#define READFILE_H
#include "../HeaderFile/Common.h"
#include "../HeaderFile/SessionSettings.h"
#include <iostream>



#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <fstream>
#include <unordered_set>

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
			std::cout <<"[INFO] " << "Exception in load data : " << e.what() << std::endl;
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


class ReadConfigurationFile
{
	SessionSettings *s1;
	int SeqNum;
	std::ofstream WriteSeqNumfile;
	std::ifstream readSeqNumfile;

public:

	ReadConfigurationFile();
	ReadConfigurationFile(SessionSettings *s);
	void ReadConfigurationParametersFromFile();

	std::string SeqNumFile(std::string);
};
#endif