#pragma once
#ifndef SESSIONSETTINGS_H
#define SESSIONSETTINGS_H
#include <string>
#include "configReader.h"
#include "rabbitmqConsumer.h"
#include "Nanolog.hpp"
#include "CurrentPath.h"

class ConfigParameter
{
public:
	ConfigParameter();
	void ReadConfigurationParametersFromFile(FIN::ConfigReader& confReader);
	void print();

	bool udp_rabbit;
	int throttle_limit;
	int throttle_time_windows;
	int tcp_connfirmation_port;
	std::string unicast_ip;
	std::string nic_ip;
	int request_port;
	int response_port;
	std::string rabbit_ip;
	int rabbit_port;
	int exch_number;
	int trader_id;
	std::string log_path;
	std::string order;
	bool isPasswordResetMannually;
	std::string current_password;
	std::string newPassword;
	std::string passwordResetDate;
	bool changePassword;
	std::string connectHost;
	int connectPort;
	bool useSecondaryIp;
	std::string connectHost2;
	int connectPort2;
	std::string brokerId;
	int boxId;
	int versionNumber;
	int branchId;
	std::string terminalInfo;
	std::string account;
	std::string pan;
	std::string algo_id;

};
#endif