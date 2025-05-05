#ifndef FIN_CONNECTION_STATUS
#define FIN_CONNECTION_STATUS

#include <iostream>
#include "configReader.h"

namespace FIN {


	enum class ConnectionStatus
	{
		Con_INVALID,

		Con_CONNECTING,
		Con_CONNECTED,
		Con_DOWNLOADING,
		Con_LOGGED_IN,
		Con_LOGGED_OUT,
		Con_DISCONNECTED,
		Con_DISABLED
	};

	static const std::string strConnectionStatus_INVALID = "ConnectionStatus_INVALID     ";
	static const std::string strConnectionStatus_CONNECTING = "ConnectionStatus_CONNECTING  ";
	static const std::string strConnectionStatus_DOWNLOADING = "ConnectionStatus_DOWNLOADING ";
	static const std::string strConnectionStatus_LOGGED_IN = "ConnectionStatus_LOGGED_IN   ";
	static const std::string strConnectionStatus_LOGGED_OUT = "ConnectionStatus_LOGGED_OUT  ";
	static const std::string strConnectionStatus_CONNECTED = "ConnectionStatus_CONNECTED   ";
	static const std::string strConnectionStatus_DISCONNECTED = "ConnectionStatus_DISCONNECTED";
	static const std::string strConnectionStatus_DISABLED = "ConnectionStatus_DISABLED    ";

	std::string getName(ConnectionStatus cs);
	bool isConnected(ConnectionStatus cs);

	class ConnectionParam
	{
	public:

		template < typename T >
		T getParam(const std::string& key, const T& defaultValue)
		{
			return _confReader.getValue<T>(key, defaultValue);
		}

		std::string getParamsString()
		{
			std::string str = _market + "| " + _connectHost + ":" + std::to_string(_connectPort);
			return str;
		}

		std::string getConnectHost() { return _connectHost; }
		std::string getNicAddress() { return NicIp; }

		ConnectionStatus getConnectionStatus() const
		{
			return _connectionStatus;
		}

		void setConnectionStatus(const ConnectionStatus conStat)
		{
			_connectionStatus = conStat;
		}

		std::string getLogFileName()
		{
			std::string str = _market + "-" + std::to_string(time(NULL)) + ".txt";
			return str;
		}

		void readConfig(const std::string& fileName)
		{
			_confReader.loadData(fileName);
			std::string isMarket = "SESSION.MARKET";
			std::string isPassword = "SESSION.CURRENT_PASSWORD";
			std::string isNewPassword = "SESSION.NEW_PASSWORD";
			std::string isConnectHost = "SESSION.CONNECTHOST";
			std::string isTraderId = "SESSION.TRADERID";
			std::string isConnectPort = "SESSION.CONNECTPORT";
			std::string isBrokerId = "SESSION.BROKERID";
			std::string isChangePassword = "SESSION.CHANGEPASSWORD";
			std::string isBoxId = "SESSION.BOXID";
			std::string isConnectHost2 = "SESSION.CONNECTHOST2";
			std::string isConnectPort2 = "SESSION.CONNECTPORT2";
			std::string isVersionNumber = "SESSION.VERSIONNUMBER";
			std::string isBranchId = "SESSION.BRANCHID";
			std::string isTerminalInfo = "SESSION.TERMINALINFO";
			std::string isAccount = "SESSION.ACCOUNT";
			std::string isResetPasswordMannually = "SESSION.IS_RESET_PASSWORD_MANNUALLY";
			std::string isPasswordResetDate = "SESSION.PASSWORD_RESET_DATE";
			std::string isNicAddress = "SESSION.NIC_IP";

			_market = getParam < std::string >(isMarket, "");
			_password = getParam < std::string >(isPassword, "");
			_newPassword = getParam < std::string >(isNewPassword, "");
			_connectHost = getParam < std::string >(isConnectHost, "");
			_traderId = getParam < int >(isTraderId, 0);
			_connectPort = getParam < int >(isConnectPort, 0);
			_brokerId = getParam < std::string >(isBrokerId, "");
			_changePassword = getParam < bool >(isChangePassword, false);
			_boxId = getParam <int>(isBoxId, 0);
			_connectHost2 = getParam <std::string>(isConnectHost2, "");
			_connectPort2 = getParam <int>(isConnectPort2, 0);
			_versionNumber = getParam <int>(isVersionNumber, 0);
			_branchId = getParam <int>(isBranchId, 0);
			_terminalInfo = getParam <std::string>(isTerminalInfo, "");
			_account = getParam <std::string>(isAccount, "");
			_passwordResetDate = getParam <std::string>(isPasswordResetDate, "");
			NicIp = getParam <std::string>(isNicAddress, "");

			_isPasswordResetMannually = getParam <bool>(isResetPasswordMannually, false);
		}

		void print()
		{
			std::cout << "**************** CONFIG **************\n";
			std::cout << "** Market---------- " << _market << "\n";
			std::cout << "** IsPasswordResetMannualy---------- " << _isPasswordResetMannually << "\n";
			std::cout << "** PasswordRestDate---------- " << _passwordResetDate << "\n";
			std::cout << "** Password-------- " << _password << "\n";
			std::cout << "** NewPassword----- " << _newPassword << "\n";
			std::cout << "** NicIP----- " << NicIp << "\n";
			std::cout << "** ConnectHost----- " << _connectHost << "\n";
			std::cout << "** ConnectPort----- " << _connectPort << "\n";
			std::cout << "** BrokerId-------- " << _brokerId << "\n";
			std::cout << "** ChangePassword-- " << _changePassword << "\n";
			std::cout << "** BoxId----------- " << _boxId << "\n";
			std::cout << "** ConnectHost2---- " << _connectHost2 << "\n";
			std::cout << "** ConnectPort2---- " << _connectPort2 << "\n";
			std::cout << "** Version Number-- " << _versionNumber << "\n";
			std::cout << "** BranchId-------- " << _branchId << "\n";
			std::cout << "** Terminal Info--- " << _terminalInfo << "\n";
			std::cout << "** Account--------- " << _account << "\n";
			std::cout << "**************************************\n\n";
		}

		std::string _market;
		std::string _password;
		std::string _connectHost;
		int _traderid;
		int _connectPort;
		int _boxId;
		std::string _connectHost2;
		int _connectPort2;
		int _versionNumber;
		int _branchId;
		std::string _terminalInfo;
		std::string _account;
		bool _isPasswordResetMannually;
		std::string _passwordResetDate;
		std::string NicIp;

		///std::string _market     ;
		///std::string _password   ;
		///std::string _connectHost;
		///int _traderid  ;
		///int _connectPort; 

		ConfigReader _confReader;
		ConnectionStatus _connectionStatus;


		ConnectionParam()
		{
			_market = "";
			_password = "";
			_newPassword = "";
			_connectHost = "";
			_traderId = 0;
			_connectPort = 0;
			_brokerId = "";

			_changePassword = false;
		}

		int getConnectPort() { return _connectPort; }
		std::string _newPassword;
		std::string _brokerId;
		int _traderId;
		bool _changePassword;

	};
}

#endif
