#pragma once
#ifndef FIN_COMMON_H
#define FIN_COMMON_H

#include <iostream>
#include "Nanolog.hpp"
namespace FIN
{
	namespace FIN_CONST
	{
		//! making some constants so that if there is any changes we need to only change in one place not everywhere.
		static const std::string EMPTY_STRING = "";		
		static const std::string FIN_CONFIG_FILE = "\\conf\\iibx-order.conf";
		static const std::string SEP_DOT = ".";

		static const std::string SESSION = "SESSION";
		static const std::string HOST_IP = "HostIP";
		static const std::string HOST_PORT = "HostPORT";
		static const std::string MEMBER_ID = "ClearingMemberCode";
		static const std::string TRADER_ID = "TraderMemberCode";
		static const std::string DEALER_CODE = "DealerCode";
		static const std::string CTCL_CODE = "CTCLTerminalId";
		static const std::string ENABLE_PASS_CHANGE = "PasswordChangeRequired";
		static const std::string PASSWORD = "Password";
		static const std::string ALGO_ID = "AlgoId";
		static const std::string THROTTLE_MSG_COUNT = "ThrottleMsgCount";
		static const std::string THROTTLE_MSG_SEC = "ThrottleMsgSec";

		static const std::string CONTRACT_DOWNLOAD = "CONTRACT_DOWNLOAD";
		static const std::string ENABLE_CON_DOWN = "IsEnable";
		static const std::string PUBLISH_IP = "Publish_ip";
		static const std::string PUBLISH_PORT = "Publish_Port";

		static const std::string OMS_COMM = "OMS_COMM";
		static const std::string IS_RABBIT_UDP = "RABBIT_UDP";
		static const std::string EXCHANGE_NUM = "ExchangeNumber";

		static const std::string OMS_CONNECTION = "OMS_CONNECTION";
		static const std::string OMS_LISTEN_PORT = "ListenPort";

		static const std::string RMQ_CONNECTION = "RMQ_CONNECTION";
		static const std::string RABBIT_IP = "rabbitMqIP";
		static const std::string RABBIT_PORT = "rabbitMqPort";

		static const std::string UDP_CONNECTION = "UDP_CONNECTION";
		static const std::string UDP_SEND_IP = "SendIP";
		static const std::string UDP_SEND_PORT = "SendPort";
		static const std::string UDP_RCV_PORT = "RcvPort";

		static const std::string LOGGING = "LOGGING";
		static const std::string LOG_PATH = "LogPath";
	}



}

#endif 
