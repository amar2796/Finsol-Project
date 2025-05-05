#ifndef FIN_COMMON_H
#define FIN_COMMON_H
#include <string>
#include "defines.h"
#include "Nanolog.hpp"
namespace FIN {

	static const std::string SEP_DOT = ".";
	static const std::string EMPTY_STRING = "";
	static const int EMPTY_INTEGER = 0;

	static const std::string SESSION = "SESSION";
	static const std::string isOrder = "ORDERTYPE";
	static const std::string isPassword = "CURRENT_PASSWORD";
	static const std::string isNewPassword = "NEW_PASSWORD";
	static const std::string isConnectHost = "CONNECTHOST";
	static const std::string isTraderId = "TRADERID";
	static const std::string isConnectPort = "CONNECTPORT";
	static const std::string isBrokerId = "BROKERID";
	static const std::string isChangePassword = "CHANGEPASSWORD";
	static const std::string isBoxId = "BOXID";
	static const std::string nicIP = "NIC_IP";
	static const std::string isConnectHost2 = "CONNECTHOST2";
	static const std::string isConnectPort2 = "CONNECTPORT2";
	static const std::string isVersionNumber = "VERSIONNUMBER";
	static const std::string isBranchId = "BRANCHID";
	static const std::string isTerminalInfo = "TERMINALINFO";
	static const std::string isAccount = "ACCOUNT";
	static const std::string isResetPasswordMannually = "IS_RESET_PASSWORD_MANNUALLY";
	static const std::string isPasswordResetDate = "PASSWORD_RESET_DATE";
	static const std::string UDP_RABBIT = "UDP_RABBIT";
	static const std::string USE_SECONDARY_IP = "USE_SECONDARY_IP";
	static const std::string DEFAULT = "DEFAULT";
	static const std::string THROTTLE_LIMIT = "THROTTLE_LIMIT";
	static const std::string THROTTLE_TIME_WINDOW = "THROTTLE_TIME_WINDOW";
	static const std::string TCP_CONNFIRMATION_PORT = "TCP_CONNFIRMATION_PORT";
	static const std::string PAN = "PAN";
	static const std::string ALGO_ID = "ALGO_ID";
	static const std::string defPAN = "PAN_EXEMPT";
	static const std::string ZERO = "0";							//this is only for algoid is not set

	static const std::string OMS = "OMS";
	static const std::string UNICAST_IP = "UNICAST_IP";
	static const std::string REQUEST_PORT = "REQUEST_PORT";
	static const std::string RESPONSE_PORT = "RESPONSE_PORT";

	static const std::string RABBIT = "RABBIT";
	static const std::string RABBIT_IP = "RABBIT_IP";
	static const std::string RABBIT_PORT = "RABBIT_PORT";
	static const std::string EXCHANGE_NUMBER = "EXCHANGE_NUMBER";

	static const std::string LOGGING = "LOGGING";
	static const std::string LogPath = "LogPath";


	namespace CONSTANT
	{
		static const std::string SEP_DOT = ".";
		static const std::string EMPTY_STRING = "";
		static const std::string MD_SUBSCRIPTION = "MD_SUBSCRIPTION";
		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string LME_MARKET = "LME";
		static const std::string FILE_NAME = "FileName";
		static const std::string NSECOM_ORDER_CONFIG = "NSECOM_ORDER_CONFIG.conf";
		static const std::string LOGGING = "LOGGING";
		static const std::string NSE_ENCRYPTION_FILE = "certificate.pem";
	}
}

#endif
