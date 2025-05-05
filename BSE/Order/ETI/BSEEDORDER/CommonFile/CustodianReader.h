#pragma once
#ifndef CUSTODIAN_READER_H
#define CUSTODIAN_READER_H

#include <iostream>
#include "../HeaderFile/defines.h"
//#include "path.h"

#include <unordered_map>
#include <cstdint>
#include <fstream>
using namespace std;

class  CustodianReader
{
public:
	CustodianReader();
	~CustodianReader();
	void readeCustodianFile();
	std::string getCurrentPath();

	std::unordered_map<std::string, std::string>custodian;
	//FIN::CurrentPath _path;

private:
	std::string CUSTODIAN_FILE = "\\custodian.txt";

};





#endif