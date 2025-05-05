#ifndef FIN_MD_NSETBT_CONSTANTS
#define FIN_MD_NSETBT_CONSTANTS
#include <string>

namespace FIN
{
	namespace FIN_CONST
	{
		const uint16_t BroadCastMsgType_Invalid = 0;
		const uint16_t BroadCastMsgType_MarketData = 1;
		//! making some constants so that if there is any changes we need to only change in one place not everywhere.
		//static const std::string EMPTY_STRING = "";
		static const int EMPTY_INTEGER = 0;
		static const std::string NSEFO = "NSEFO";
		static const std::string NSECM = "NSECM";
		static const std::string NSECD = "NSECD";
		const float NSE_FO_MULTIPLIER = 100;
		const float NSE_CDS_MULTIPLIER = 10000000;
		const float NSE_CM_MULTIPLIER = 100;
		const float NSE_COM_MULTIPLIER = 10000;
		//static const std::string FIN_CONFIG_FILE = "./conf/finMarketGw.conf";
		static const std::string FIN_CONFIG_TBT_FILE = "./conf/nse-tbt.conf";
		static const std::string FIN_CONFIG_PUBLISH_FILE = "./conf/publisher.conf";
		static const std::string FIN_TOKEN_FILE = "./conf/tokenFile.txt";
		//static const std::string FILE_NAME = "FileName";
		static const std::string ACTIVE_STREAMS = "ACTIVE-STREAMS";
		//static const std::string PUBLISHER = "PUBLISHER";
		static const std::string STREAM_COUNT = "total_no_of_streams";
		static const std::string ENABLE_FLAG = "ENABLE";
		static const std::string NSETBT_MARKET = "TBT-FO-STREAM-1";
		//static const std::string SEP_DOT = ".";
		static const std::string PMRY_FEED_ADDR = "primary_feed_addr";
		static const std::string PMRY_FEED_PORT = "primary_feed_port";
		static const std::string PMRY_FEED_INTRFCE = "primary_feed_interface";
		static const std::string SEC_FEED_ADDR = "secondary_feed_addr";
		static const std::string SEC_FEED_PORT = "secondary_feed_port";
		static const std::string SEC_FEED_INTRFCE = "secondary_feed_interface";
		static const std::string RCVRY_CHNL_ADDR = "recovery_chnl_addr";
		static const std::string RCVRY_CHNL_PORT = "recovery_chnl_port";
		static const int TBT_MAX_PACKET_LENGTH = 65535;
		static const int SHORT_MAX = 32767;
		static const char S = 'S';
		static const char E = 'E';
		static const int DEPTH = 5;
		static const int DEPTH_SIZE = 5;

	}
}

#endif
