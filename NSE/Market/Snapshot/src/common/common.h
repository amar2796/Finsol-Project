#ifndef FIN_COMMON_H
#define FIN_COMMON_H
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <string>
#include "defines.h"
//#include "glog\logging.h"
#include "../logging/Nanolog.hpp"
#include <iostream>
#include <string>
#include "marketDataMessage.h"
namespace FIN
{
	namespace FIN_CONST
	{
		static const std::string SEP_DOT = ".";
		static const std::string EMPTY_STRING = "";
		static const std::string FIN_CONFIG_FILE = "./conf/nse-snapshot.conf";
		static const std::string MD_SUBSCRIPTION = "MD_SUBSCRIPTION";
		static const std::string RABBIT_SUBSCRIPTION = "RABBITMQ_FOR_LIVE_SUBSCRIPTION";
		static const std::string NSE_CDS_MULTICAST_RECEIVER = "NSE_CDS_MULTICAST_RECEIVER";
		static const std::string NSE_COM_MULTICAST_RECEIVER = "NSE_COM_MULTICAST_RECEIVER";
		static const std::string NSE_CM_MULTICAST_RECEIVER = "NSE_CM_MULTICAST_RECEIVER";
		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string LME_MARKET = "LME";
		static const std::string FILE_NAME = "FileName";
		static const std::string LMEepochFile = "./conf/epoch.txt";
		static const std::string LMESymbolVolume = "./conf/SymbolVolumeFile.txt";
		static const std::string LMETradeSeqFile = "./conf/TradeSeriesAndSeqNo.txt";
		static const std::string NSE_LISTEN_INTERFACE = "nse_listen_interface";
		static const std::string NSE_MULTICAST_ADDR = "nse_multicast_addr";
		static const std::string NSE_MULTICAST_PORT = "nse_multicast_port";
		static const std::string RABBIT_IP = "RabbitIP";
		static const std::string RABBIT_PORT = "RabbitPort";
		static const std::string LISTENTOLIVEREQUEST = "ListenToLiveRequest";
		static const std::string GET_TOKEN = "GetTbt_Token";
		static const std::string CONSOLE_LOGGING  = "LOGGING";
		static const std::string CONSOLE = "console";
	}


}

#endif
