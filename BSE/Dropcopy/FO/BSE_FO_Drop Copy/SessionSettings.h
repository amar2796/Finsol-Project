#pragma once
#ifndef SESSIONSETTINGS_H
#define SESSIONSETTINGS_H
#include<iostream>
#include<string>
class SessionSettings
{
public:

	std::string TradingSessionIdPath;
	int PartitionId;
	std::string ExchangeIp;
	long ExchangePort;
	std::string ExchangeResponseIp;
	long ExchangeResponsePort;
	std::string udp_senderIp;
	int udp_rcverPort;
	int udp_senderPort;
	int listenPort;
	std::string algoId;
	unsigned int SessionId;
	uint32_t heartbeat;
	std::string Password;
	std::string DefaultCstmApplVerID;
	std::string ApplUsageOrders;
	std::string ApplUsageQuotes;
	std::string OrderRoutingIndicator;
	std::string ApplicationSystemName;
	std::string ApplicationSystemVersion;
	std::string  ApplicationSystemVendor;
	unsigned int Username;
	long long SenderLocationId;
	std::string log_path;

	std::string rabbitMqIP;
	int rabbitMqPort;
	std::string ExchangeNumber;

	int oms_channel;

	bool _nanoLogEnabled;

	int throttleLimit_msg;
	int throttleLimit_time;
	std::string newPassword;
	int changePassword;

	std::string contractFilePath;
	std::string dropCopyPath;
	std::string certificatePath;

	int partitionID;
};
#endif