#ifndef FIN_MD_NSETBT_CONSTANTS
#define FIN_MD_NSETBT_CONSTANTS
#include <string>

namespace FIN
{
	namespace FIN_CONST
	{
		//! making some constants so that if there is any changes we need to change only at one place not everywhere.
		static const std::string EMPTY_STRING = "";
		static const std::string DOT = ".";
		static constexpr int EMPTY_INTEGER = 0;
		static const std::string FIN_CONFIG_FILE = "conf\\iibx-market.conf";
		static const std::string FIN_TOKEN_FILE = "conf\\tokenFile.txt";
		static const std::string LOGGING = "LOGGING";
		static const std::string LogPath = "LogPath";
		static const std::string IIBXEXCHANGEDETAILS = "IIBX-EXCHANGE-DETAILS";
		static const std::string Exchange_Number = "Exchange_Number";
		static const std::string tcp_ip = "tcp_ip";
		static const std::string tcp_port = "tcp_port";
		static const std::string listen_interface = "listen_interface";
		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string Log_Enabled = "Log_Enabled";
		/*static const std::string LOGGING = "LOGGING";
		static const std::string LOGGING = "LOGGING";*/

		static constexpr int DEPTH_SIZE = 5; // change 5 from 1 		
    }
}

#endif
