#ifndef FIN_COMMON_H
#define FIN_COMMON_H
#include <string>
#include "defines.h"
#include "Nanolog.hpp"
namespace FIN {	namespace CONSTANT {
		
	static const std::string SEP_DOT = ".";
	static const std::string EMPTY_STRING = "";
	static const std::string FIN_CONFIG_FILE = "./conf/finMarketGw.conf";
	static const std::string MD_SUBSCRIPTION = "MD_SUBSCRIPTION";
	static const std::string PUBLISHER = "PUBLISHER";
	static const std::string LME_MARKET = "LME";
	static const std::string FILE_NAME = "FileName";		
	static const std::string NSE_DROPCOPY_CONFIG = "NSE_DROP_COPY_CONFIG.txt";
	
	static const std::string NSE_DROPCOPY_SUBSCRIPTION = "terminalInfo.txt";
	static const std::string LOGGING = "LOGGING";

	}

#define CONSOLE_INFO_LOG( ARG )\
	std::cout << "[INFO] " << ARG<< std::endl
#define CONSOLE_ERROR_LOG( ARG )\
	std::cout << "[ERROR] " << ARG<< std::endl

}

#endif
