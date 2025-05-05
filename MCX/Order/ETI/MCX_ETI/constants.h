#ifndef FIN_ORDER_GW_CONSTATNTS_H
#define FIN_ORDER_GW_CONSTATNTS_H

#include <string>
#include <unordered_map>

namespace FIN {



#define CONSOLE_LOG std::cout << "[" << __func__ << " - Line " << __LINE__ << "]: "

	const char comma = ',';
	const int64_t price_multiplier = 100000000;
	const int64_t  qty_multipler = 10000;

	const uint8_t CLIENT = 3;
	const uint8_t OWN = 1;
	const uint8_t INSTITUITION = 5;

	const std::string CONNECTIONGATEWAYREQUEST_T = "[ ConnectionGatewayRequestT ]";
	const std::string LOGONREQUEST = "[ LogonRequest ]";
	const std::string LOGONRESPONSE_T = "[ LogonResponseT ]";
	const std::string CONNECTIONGATEWAYRESPONSE_T = "[ ConnectionGatewayResponseT ]";
	const std::string MESSAGEHEADERINCOMP_T = "[ MessageHeaderInCompT ]";
	const std::string REQUESTHEADERCOMP_T = "[ RequestHeaderCompT ]";
	const std::string MESSAGEHEADEROUTDERIVATIVESCOMP_T = "[ MessageHeaderOutDerivativesCompT ]";
	const std::string RESPONSEHEADERMEDERIVATIVESCOMP_T = "[ ResponseHeaderMEDerivativesCompT ]";
	const std::string USERLOGINREQUEST_T = "[ UserLoginRequestT ]";
	const std::string USERLOGINRESPONSE_T = "[ UserLoginResponseT ]";
	const std::string NEWORDERSINGLEREQUEST_T = "[ NewOrderSingleRequestT ]";
	const std::string NEWORDERNRRESPONSE_T = "[ NewOrderNRResponseT ]";
	const std::string NEWORDERRESPONSE_T = "[ NewOrderResponseT ]";
	const std::string MODIFYORDERSINGLEREQUEST_T = "[ ModifyOrderSingleRequestT ]";
	const std::string MODIFYORDERNRRESPONSET = "[ ModifyOrderNRResponseT ]";
	const std::string MODIFYORDERRESPONSET = "[ ModifyOrderResponseT ]";
	const std::string DELETEORDERSINGLEREQUEST_T = "[ DeleteOrderSingleRequestT ]";
	const std::string DELETEORDERNRRESPONSET = "[ DeleteOrderNRResponseT ]";
	const std::string DELETEORDERRESPONSE_T = "[ DeleteOrderResponseT ]";
	const std::string ORDEREXECRESPONSE_T = "[ OrderExecResponseT ]";
	const std::string ORDEREXECNOTIFICATION_T = "[ OrderExecNotificationT ]";
	const std::string ORDEREXECREPORTBROADCAST_T = "[ OrderExecReportBroadcastT ]";
	const std::string TRADEBROADCAST_T = "[ TradeBroadcastT ]";
	const std::string REJECT_T = "[ RejectT ]";


	const std::string OMS_TRADE = "[ OmsTradeResponse ] ";

	const std::string ORDER_QUANTITY = "38=";
	const std::string SIDE = "54=";
	const std::string ORDER_TYPE = "40=";

	const std::string MSG_TYPE = "35=";
	const std::string ORDER_ID = "37=";
	const std::string CLORDER_ID = "11=";
	const std::string ORG_ID = "41=";
	const std::string LEAVE_QTY = "151=";
	const std::string FILL_QTY = "14=";
	const std::string CXL_QTY = "84=";
	const std::string ORDER_STATUS = "39=";
	const std::string EXE_STATUS = "150=";
	const std::string LAST_PX = "31=";
	const std::string LAST_QTY = "32=";
	const std::string TRADE_ID = "17=";
	const std::string PRICE = "44=";
	const std::string STOP_PRICE = "99=";
	const std::string TIME_IN_FORCE = "59=";






	const std::string TERMINAL_TAG = "9227";

	const std::string FQ = " FQ ";
	const std::string LQ = " LQ ";
	const std::string CQ = " CQ ";
	const std::string LAQ = " LAQ ";
	const std::string LAP = " LAP ";










	typedef std::unordered_map< std::string, std::string > FixKeyValue;

	const std::string Fix_Account = "1";
	const std::string Fix_AvgPX = "6";
	const std::string Fix_ClOrdID = "11";
	const std::string Fix_Currency = "15";
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
	const std::string Fix_Stop_Price = "99";
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

	const std::string Fix_FillPx = "1364";
	const std::string Fix_Fill_Qty = "1365";
	const std::string Fix_FillMatchId = "28708";
	const std::string Fix_FillExecId = "1363";

	const std::string Fix_BeginString = "8";
	const std::string Fix_BeginString_Val = "FIX.4.4";
	const std::string Fix_MsgTyp_Val = "8";
	const std::string Fix_MsgTyp_RejVal = "9";
	const std::string Fix_SendTime = "52";
	const std::string Fix_ExecRestatementReason = "378";
	const std::string Fix_NumberFill = "1362";
	const std::string Fix_TerminalInfo = "708";
	const std::string Fix_CrossIndicator = "2523";

	const std::string Fix_TransactioDelayIndicator = "25159";
	const std::string Fix_CheckSum = "10";



	const std::string EXCH_IS_NOT_UP = "Exchange is not up";





	// STRING VALUES FOR FIX KEYS
	const std::string Fix_SecurityType_FUT = "FUT";

	const std::string Fix_CustomerOrFirm_Customer = "0";
	const std::string Fix_CustomerOrFirm_Firm = "1";

	const std::string Fix_Account_OWN = "OWN";
	const std::string Fix_Account_PRO = "PRO";

	const uint8_t Fix_OrdType_Limit = 2;
	const uint8_t Fix_OrdType_Stop_Limit = 4;
	const uint8_t Fix_OrdType_Market = 5;

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

	const char Fix_TimeInForce_DAY = '0';
	const char Fix_TimeInForce_IOC = '3';

	const char POSSITION_EFFECT = 'C';
	const std::string ACCOUNT = "A1";
	const uint8_t NON_PERSISTENT = 2;
	const uint8_t PERSISTENT = 1;
	const uint8_t NORECOVERY_REQUIRED = 0;
	const uint8_t STANDARD_ORDER = 1;

}

#endif
