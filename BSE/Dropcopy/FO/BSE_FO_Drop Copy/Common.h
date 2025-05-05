#ifndef COMMON_H
#define COMMON_H
#include <string>
#include <iostream>
#include <unordered_map>
//#include <processenv.h>
//#include <consoleapi2.h>
//#include <WinBase.h>
namespace FIN
{
	const std::string COMMA = ",";
	const std::string BLANK = "";
	// STRING VALUES FOR FIX KEYS
	const std::string Fix_SecurityType_FUT = "FUT";
	const std::string PIPE = " || ";
	const std::string Fix_CustomerOrFirm_Customer = "0";
	const std::string Fix_CustomerOrFirm_Firm = "1";

	const std::string Fix_Account_OWN = "OWN";
	const std::string Fix_Account_PRO = "PRO";

	const uint8_t Fix_OrdType_Limit =2;
	const char FIX_OrderType_stop_loss = '4';

	const char Fix_Side_Buy = '1';
	const char Fix_Side_Sell = '2';

	const char Fix_OptionType_PutEuropean = '0';
	const char Fix_OptionType_CallEuropean = '1';
	const char Fix_OptionType_CallAmerican = '2';
	const char Fix_OptionType_PutAmerican = '3';

	const char Fix_Product_IDX = '7';
	const char Fix_Product_STK = '5';
	const char Fix_Product_CUR = '4';

	const char Fix_MsgType_NEW_ORDER = 'D';
	const char Fix_MsgType_MODIFY_ORDER = 'G';
	const char Fix_MsgType_CANCEL_ORDER = 'F';
	const char Fix_MsgType_MULTI_LEG_ORDER = 'M';
	const char Fix_TimeInForce_DAY = '0';
	const char Fix_TimeInForce_IOC = '3';


	const std::string A1_ACC="A1";
	const std::string Fix_Account = "581";
	const std::string Fix_AvgPX = "6";
	const std::string Fix_ClOrdID = "11";
	const std::string Fix_CumQty = "14";
	const std::string Fix_ExecID = "17";
	const std::string Fix_LastPx = "31";
	const std::string Fix_LastShares = "32";
	const std::string Fix_MsgType = "35";
	const std::string Fix_OrderID = "37";
	const std::string Fix_OrdQty = "38";
	const std::string Fix_OrdStatus = "39";
	const std::string Fix_OrdType = "40";
	const std::string Fix_OrigClOrdID = "41";
	const std::string Fix_Price = "44";
	const std::string Fix_SecurityID = "48";
	const std::string Fix_SenderCompID = "49";
	const std::string Fix_Side = "54";
	const std::string Fix_Symbol = "55";
	const std::string Fix_Text = "58";
	const std::string Fix_TimeInForce = "59";
	const std::string Fix_TransactTime = "60";
	const std::string Fix_ExecType = "150";
	const std::string Fix_LeavesQty = "151";
	const std::string Fix_SecurityType = "167";
	const std::string Fix_MaturityMonthYear = "200";
	const std::string Fix_OptionType = "201";
	const std::string Fix_StrikePrice = "202";
	const std::string Fix_CustomerOrFirm = "204";
	const std::string Fix_MaturityDate = "205";
	const std::string Fix_CxlRejResponseTo = "434";
	const std::string Fix_Product = "460";
	const std::string Fix_Series = "65";
	const std::string Fix_TriggerPrice = "99";
	const std::string Fix_Number_OfLeg = "555";





	typedef std::unordered_map< std::string, std::string > FixKeyValue;
	static const std::string SEP_DOT = ".";
	static const std::string EMPTY_STRING = "";
	static const int EMPTY_INTEGER = 0;
	static const std::string FIN_CONFIG_FILE = "config.conf";
	static const std::string SESSION = "SESSION";
	static const std::string SessionId = "SessionId";
	static const std::string HeartBeat = "HeartBeat";
	static const std::string Password = "Password";
	static const std::string NewPassword = "NewPassword";
	static const std::string ChangePassword = "ChangePassword";
	static const std::string DefaultCstmApplVerID = "DefaultCstmApplVerID";
	static const std::string ApplUsageOrders = "ApplUsageOrders";
	static const std::string ApplUsageQuotes = "ApplUsageQuotes";
	static const std::string OrderRoutingIndicator = "OrderRoutingIndicator";
	static const std::string ApplicationSystemName = "ApplicationSystemName";
	static const std::string ApplicationSystemVersion = "ApplicationSystemVersion";
	static const std::string ApplicationSystemVendor = "ApplicationSystemVendor";
	static const std::string Username = "Username";
	static const std::string SenderLocationId = "SenderLocationId";
	static const std::string ExchangeIP = "ExchangeIP";
	static const std::string ExchangePort = "ExchangePort";
	static const std::string AlgoID = "AlgoID";
	static const std::string ListenPort = "ListenPort";
	static const std::string RMQ_CONNECTION = "RMQ_CONNECTION";
	static const std::string ExchangeNumber = "ExchangeNumber";
	static const std::string OMS_THROTTLE = "OMS_THROTTLE";
	static const std::string THROTTLE_MSG_LIMIT = "THROTTLE_LIMIT";
	static const std::string THROTTLE_TIME_WINDOW = "THROTTLE_TIME_WINDOW";
	static const std::string NanoLogEnabled = "Enable_Nano_Log";

	static const std::string PARTITIONID = "PARTITIONID";
	static const std::string PartitionID = "PartitionID";

	static const std::string CONTRACTFILE = "CONTRACTFILE";
	static const std::string ContractFilePath = "ContractFilePath";
	
	static const std::string TRADINGSESSIONPATH = "TRADINGSESSIONPATH";
	static const std::string TradingSessionIdPath = "TradingSessionIdPath";

	static const std::string DROPCOPY = "DROPCOPY";
	static const std::string DropCopyFilePath = "DropCopyFilePath";

	static const std::string SendIP = "SendIP";
	static const std::string SendPort = "SendPort";
	static const std::string RcvPort = "RcvPort";

	static const std::string LOGGING = "LOGGING";
	static const std::string LogPath = "LogPath";



}



#endif