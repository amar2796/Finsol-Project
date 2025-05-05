#include "configParameter.h"

ConfigParameter::ConfigParameter()
{
}

void ConfigParameter::ReadConfigurationParametersFromFile(FIN::ConfigReader& confReader)
{
	try
	{
		FIN::CurrentPath _currentPath;

		std::string filePath = _currentPath.getCurrentPath() + FIN::CONSTANT::NSECOM_ORDER_CONFIG;

		LOG_INFO << "************************* CONFIG *********************";
		order = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isOrder,
			FIN::EMPTY_STRING);
		LOG_INFO << "** Order------------------------- " << order;

		throttle_limit = confReader.getValue < int >(
			FIN::DEFAULT + FIN::SEP_DOT + FIN::THROTTLE_LIMIT,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** ThrottleLimit----------------- " << throttle_limit;

		throttle_time_windows = confReader.getValue < int >(
			FIN::DEFAULT + FIN::SEP_DOT + FIN::THROTTLE_TIME_WINDOW,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** ThrottleTimeWindow------------ " << throttle_time_windows;

		isPasswordResetMannually = confReader.getValue < bool >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isResetPasswordMannually,
			false);
		LOG_INFO << "** IsPasswordResetMannualy------- " << isPasswordResetMannually;

		current_password = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isPassword,
			FIN::EMPTY_STRING);
		LOG_INFO << "** CurrentPassword--------------- " <<current_password;

		newPassword = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isNewPassword,
			FIN::EMPTY_STRING);
		LOG_INFO << "** NewPassword------------------- " << newPassword;

		passwordResetDate = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isPasswordResetDate,
			FIN::EMPTY_STRING);
		LOG_INFO << "** PasswordRestDate-------------- " << passwordResetDate;

		changePassword = confReader.getValue < bool >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isChangePassword,
			false);
		LOG_INFO << "** ChangePassword---------------- " << changePassword;

		connectHost = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isConnectHost,
			FIN::EMPTY_STRING);
		LOG_INFO << "** ConnectHost------------------- " << connectHost;

		connectPort = confReader.getValue < int >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isConnectPort,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** ConnectPort------------------- " << connectPort;

		nic_ip=confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::nicIP,
			FIN::EMPTY_STRING);
		LOG_INFO << "** NicIp------------------------- " << nic_ip;

		useSecondaryIp = confReader.getValue < bool >(
			FIN::SESSION + FIN::SEP_DOT + FIN::USE_SECONDARY_IP,
			false);
		LOG_INFO << "** UseSecondaryIp---------------- " << useSecondaryIp;

		connectHost2 = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isConnectHost2,
			FIN::EMPTY_STRING);
		LOG_INFO << "** ConnectHost2------------------ " << connectHost2;

		connectPort2 = confReader.getValue < int >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isConnectPort2,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** ConnectPort2------------------ " << connectPort2;

		trader_id = confReader.getValue < int >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isTraderId,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** TraderID---------------------- " << trader_id;

		brokerId = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isBrokerId,
			FIN::EMPTY_STRING);
		LOG_INFO << "** BrokerId---------------------- " << brokerId;

		boxId = confReader.getValue < int >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isBoxId,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** BoxId------------------------- " << boxId;

		versionNumber = confReader.getValue < int >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isVersionNumber,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** Version Number---------------- " << versionNumber;

		branchId = confReader.getValue < int >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isBranchId,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** BranchId---------------------- " << branchId;

		terminalInfo = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isTerminalInfo,
			FIN::EMPTY_STRING);
		LOG_INFO << "** Terminal Info----------------- " << terminalInfo;

		account = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::isAccount,
			FIN::EMPTY_STRING);
		LOG_INFO << "** Account----------------------- " << account;

		udp_rabbit = confReader.getValue < bool >(
			FIN::SESSION + FIN::SEP_DOT + FIN::UDP_RABBIT,
			false);
		LOG_INFO << "** IsUdpRabbit------------------- " << udp_rabbit;

		unicast_ip = confReader.getValue < std::string >(
			FIN::OMS + FIN::SEP_DOT + FIN::UNICAST_IP,
			FIN::EMPTY_STRING);
		LOG_INFO << "** UnicastIp--------------------- " << unicast_ip;

		request_port = confReader.getValue < int >(
			FIN::OMS + FIN::SEP_DOT + FIN::REQUEST_PORT,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** UnicastRequestPort------------ " << request_port;

		response_port = confReader.getValue < int >(
			FIN::OMS + FIN::SEP_DOT + FIN::RESPONSE_PORT,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** UnicastResponsePort----------- " << response_port;

		rabbit_ip = confReader.getValue < std::string >(
			FIN::RABBIT + FIN::SEP_DOT + FIN::RABBIT_IP,
			FIN::EMPTY_STRING);
		LOG_INFO << "** RabbitIp---------------------- " << rabbit_ip;

		rabbit_port = confReader.getValue < int >(
			FIN::RABBIT + FIN::SEP_DOT + FIN::RABBIT_PORT,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** RabbitPort-------------------- " << rabbit_port;

		exch_number = confReader.getValue < int >(
			FIN::RABBIT + FIN::SEP_DOT + FIN::EXCHANGE_NUMBER,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** ExchangeNumber-------------------- " << exch_number;

		tcp_connfirmation_port = confReader.getValue < int >(
			FIN::DEFAULT + FIN::SEP_DOT + FIN::TCP_CONNFIRMATION_PORT,
			FIN::EMPTY_INTEGER);
		LOG_INFO << "** TcpConnfirmationPort---------- " << tcp_connfirmation_port;

		pan = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::PAN,
			FIN::defPAN);
		LOG_INFO << "** PAN--------------------------- " << pan;

		algo_id = confReader.getValue < std::string >(
			FIN::SESSION + FIN::SEP_DOT + FIN::ALGO_ID,
			FIN::ZERO);
		LOG_INFO << "** ALGO_ID----------------------- " << algo_id;

		log_path = confReader.getValue < std::string >(
			FIN::LOGGING + FIN::SEP_DOT + FIN::LogPath,
			FIN::EMPTY_STRING);
		LOG_INFO << "** LogPath----------------------- " << log_path;

		//write here



		LOG_INFO << "******************************************************";
		//print();
	}
	catch (const std::exception& e)
	{
		std::cout << "some field missing in config file : "<< e.what() << std::endl;
	}
}

void ConfigParameter::print()
{
	std::cout << "************************* CONFIG *********************\n";
	std::cout << "** Order------------------------- " << order << "\n";
	std::cout << "** ThrottleLimit----------------- " << throttle_limit << "\n";
	std::cout << "** ThrottleTimeWindow------------ " << throttle_time_windows << "\n";
	std::cout << "** IsPasswordResetMannualy------- " << isPasswordResetMannually << "\n";
	std::cout << "** CurrentPassword--------------- " << current_password << "\n";
	std::cout << "** NewPassword------------------- " << newPassword << "\n";
	std::cout << "** PasswordRestDate-------------- " << passwordResetDate << "\n";
	std::cout << "** ChangePassword---------------- " << changePassword << "\n";
	std::cout << "** ConnectHost------------------- " << connectHost << "\n";
	std::cout << "** ConnectPort------------------- " << connectPort << "\n";
	std::cout << "** NipIP------------------------- " << nic_ip << "\n";
	std::cout << "** UseSecondaryIp---------------- " << useSecondaryIp << "\n";
	std::cout << "** ConnectHost2------------------ " << connectHost2 << "\n";
	std::cout << "** ConnectPort2------------------ " << connectPort2 << "\n";
	std::cout << "** TraderID---------------------- " << trader_id << "\n";
	std::cout << "** BrokerId---------------------- " << brokerId << "\n";
	std::cout << "** BoxId------------------------- " << boxId << "\n";
	std::cout << "** Version Number---------------- " << versionNumber << "\n";
	std::cout << "** BranchId---------------------- " << branchId << "\n";
	std::cout << "** Terminal Info----------------- " << terminalInfo << "\n";
	std::cout << "** Account----------------------- " << account << "\n";
	std::cout << "** IsUdpRabbit------------------- " << udp_rabbit << "\n";
	std::cout << "** UnicastIp--------------------- " << unicast_ip << "\n";
	std::cout << "** UnicastRequestPort------------ " << request_port << "\n";
	std::cout << "** UnicastResponsePort----------- " << response_port << "\n";
	std::cout << "** RabbitIp---------------------- " << rabbit_ip << "\n";
	std::cout << "** RabbitPort-------------------- " << rabbit_port << "\n";
	std::cout << "** ExchangeNumber---------------- " << exch_number << "\n";
	std::cout << "** TcpConnfirmationPort---------- " << tcp_connfirmation_port << "\n";
	std::cout << "** PAN--------------------------- " << pan << "\n";
	std::cout << "** ALGOID------------------------ " << algo_id << "\n";
	std::cout << "** LogPath----------------------- " << log_path << "\n";

	//write here
	
	std::cout << "******************************************************\n\n";
}

