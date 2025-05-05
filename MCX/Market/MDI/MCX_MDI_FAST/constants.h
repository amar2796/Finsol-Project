#ifndef FIN_MD_NSETBT_CONSTANTS
#define FIN_MD_NSETBT_CONSTANTS
#define GLOG_NO_ABBREVIATED_SEVERITIES
#include <string>
#include <iostream>
#include "Nanolog.hpp"
#include "ConfigDetails.h"
#include "defines.h"
#include <boost/exception/diagnostic_information.hpp> 
#include <boost/exception_ptr.hpp> 
#include <fstream>
//using namespace std;
namespace FIN
{
	namespace FIN_CONST
	{
		//! making some constants so that if there is any changes we need to change only at one place not everywhere.
		static const std::string pipe = "||";
		static const std::string EMPTY_STRING = "";
		static const int EMPTY_INTEGER = 0;
		static const std::string FIN_CONFIG_SGX_TBT_FILE = "./conf/sgx-tbt.conf";
		static const std::string FIN_CONFIG_TBT_FILE = "./conf/nse-tbt.conf";
		static const std::string FIN_CONFIG_PROCESS_MONITOR_FILE = "./conf/processMonitor.conf";
		static const std::string FIN_CONFIG_PUBLISH_FILE = "./conf/publisher.conf";
		static const std::string FIN_TOKEN_FILE = "./conf/tokenFile.txt";
		static const std::string FILE_NAME = "FileName";
		static const std::string ACTIVE_STREAMS = "ACTIVE-STREAMS";
		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string STREAM_COUNT = "total_no_of_streams";
		static const std::string ENABLE_FLAG = "ENABLE";
		static const std::string NSETBT_MARKET = "TBT-FO-STREAM-1";
		static const std::string SEP_DOT = ".";
		static const std::string PMRY_FEED_ADDR = "multicast_feed_addr";
		static const std::string PMRY_FEED_PORT = "multicast_feed_port";
		static const std::string PMRY_FEED_INTRFCE = "multicast_feed_interface";
		static const std::string SEC_FEED_ADDR = "secondary_feed_addr";
		static const std::string SEC_FEED_PORT = "secondary_feed_port";
		static const std::string SEC_FEED_INTRFCE = "secondary_feed_interface";
		static const std::string RCVRY_CHNL_ADDR = "recovery_chnl_addr";
		static const std::string RCVRY_CHNL_PORT = "recovery_chnl_port";

		static const std::string SGX_UNICAST_IP = "unicast-ip";
		static const std::string SGX_UNICAST_PORT = "unicast-port";

		static const std::string SGX_PUBLISH_IP = "publish-ip";
		static const std::string SGX_PUBLISH_PORT = "publish-port";

		static const std::string LOGGING = "LOGGING";
		
		static const std::string RABBITLIVESUBSCRIPTION = "RABBITMQ_FOR_LIVE_SUBSCRIPTION";
		static const std::string RABBITLISTENSUBSCRIPTION = "RABBITMQ_FOR_LISTEN_SUBSCRIPTION";
		static const std::string ISEnableLestionRequest = "IsEnable";
		static const std::string ISLiveRequest = "IsEnable";
		static const std::string Exchange = "Exchange";
		static const std::string RabbitIP = "RabbitIP";
		static const std::string RabbitPort = "RabbitPort";
		static const std::string GetTbt_Token = "GetTbt_Token";

		static const std::string Enable_Log = "Enable_Log";
		static const int TBT_MAX_PACKET_LENGTH = 70000;
		static const int SHORT_MAX = 32767;
		static const char S = 'S';
		static const char E = 'E';
		static const double NSE_MULTIPLIER_FO = 100.00;
		static const double NSE_MULTIPLIER_CD = 10000000.00;
		static const double NSE_MULTIPLIER_CM = 100.00;
		static const int DEPTH_SIZE = 5;
		static const std::string FILE_UDP_READ = "./conf/tbt_recorded_data_13_2_stream_1.txt";
		static const std::string FILE_TCP_READ = "./conf/tbt_recorded_data_13_2_stream_1.txt";
		const uint16_t Exchange_CME = 0;
		const uint16_t Exchange_LME = 1;
		const uint16_t NSECD = 7;
		const uint16_t NSEFO = 8;
		const uint16_t NSECM = 9;
		const uint16_t BroadCastMsgType_Invalid = 0;
		const uint16_t BroadCastMsgType_MarketData = 1;

		static const int OrderbookDirectoryLen = 128; // "R"
		static const int TickSizeLen = 24; // "L"
		static const int AddOrderLen = 36; // "A"
		static const int OrderExecuteLen = 51; // "E"
		static const int OrderExecutePriceLen = 57; // "C"
		static const int OrderReplaceLen = 35; // "U"
		static const int OrderDeleteLen = 17; // "D"
		static const int TradeMessageIdentifierLen = 49; // "P"
		static const int PacketHeaderLen = 20; // "Packet header len"
		static const int MessgaeByteLen = 3; // "MEssage byte len"
		static const int SecondsLen = 4; // "Seconds len"
		static const int orderStateLen = 28; // O
		static const int CombinationOrderBooklegLen = 17;//M
		static const int SystemEventLen = 5; //S
		static const int EquilibriumPriceUpdateLen = 52; // Z


		static const std::string FIN_CONFIG_FILE = "config.conf";
		static const std::string SNAPSHOT = "SNAPSHOT";
		static const std::string INCREMENTAL = "INCREMENTAL";
		static const std::string PUBISHER = "PUBLISHER";
		static const std::string MARKET_TYPE = "MARKET_TYPE";
		static const std::string FAST_TEMPLATE = "fast_template\\MDIFastTemplates-1.1.xml";
		static const std::string TOKEN = "Tokens.txt";
		static const std::string FILLTEREDSYMBOL = "filltered_symbols.txt";

		static const std::string multicast_addr = "Multicast_Addr";
		static const std::string multicast_addr_B = "Multicast_Addr_B";
		static const std::string multicast_port = "Multicast_Port";
		static const std::string multicast_port_B = "Multicast_Port_B";

		static const std::string Interface_Addr = "Interface_Addr";

		static const std::string SNAP_INCR = "SNAP_INCR";

		static const uint64_t EXCHANGE_LAST = 1;
		static const uint64_t OPEN_PRICE = 2;
		static const uint64_t HIGH_PRICE = 4;
		static const uint64_t LOW_PRICE = 8;
		static const uint64_t OFFICIAL_CLOSING_PRICE = 16;
		static const uint64_t LAST_AUCTION_PRICE = 32;
		static const uint64_t OUT_OF_SEQUENCE_ETH = 64;
		static const uint64_t PREVIOUS_CLOSING_PRICE = 128;
		static const uint64_t LIFE_HIGH = 8192;
		static const uint64_t LIFE_LOW = 16384;
		static const uint16_t tradeConditonSetSize = 8;
		static const uint64_t trade_con[tradeConditonSetSize] = { 1, 2, 4, 8, 16, 128, 8192, 16384 };

		static const std::string Use_Multicast = "UseUDP_Multicast";
		static const std::string UDP_MULTICAST_PUBLISHER = "UDP-MULTICAST-PUBLISHER";
		static const std::string Multicast_addr = "Multicast_addr";
		static const std::string Multicast_port = "Multicast_port";
		static const std::string Listen_interface = "Listen_interface";

		static const std::string Use_Unicast = "UseUDP_Unicast";
		static const std::string UDP_UNICAST_PUBLISHER = "UDP-UNICAST-PUBLISHER";
		static  const std::string Unicast_Addr = "Unicast_Addr";
		static const std::string Unicast_Port = "Unicast_Port";
	}
}

#endif
