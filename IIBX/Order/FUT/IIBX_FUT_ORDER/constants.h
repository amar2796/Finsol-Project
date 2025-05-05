#ifndef FIN_ORDER_GW_CONSTATNTS_H
#define FIN_ORDER_GW_CONSTATNTS_H

#include <string>
#include <unordered_map>

namespace FIN {


	const int64_t price_multiplier = 10000;
	const int64_t  qty_multipler = 1;

	const std::string OWN = "20";
	const std::string CLIENT = "30";
	const uint8_t SPECIAL_CLIENT = 40;
	const uint8_t INSTITUITION = 90;




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
	const std::string Fix_TriggerPrice = "99";

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
	const std::string Fix_IsinNumber = "1802";

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
	const uint8_t Fix_OrdType_stop_loss = 4;
	const uint8_t Fix_OrdType_Market = 1;

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

	const std::string PIPE = "|";
	const std::string EQUAL = "=";
	const std::string ZERO = "0";
	namespace OMSRESPONSE
	{
		const std::string TEMPLATEID = "TemplateID";
		const std::string ORDERSTATUS = "OrderStatus";
		const std::string ORDERID = "OrderID";
		const std::string ERRORCODE = "ErrorCode"; 
		const std::string TIMESTAMP = "TimeStamp";
		const std::string ORDERNUMBER = "OrderNumber";
		const std::string ORDERTYPE = "OrderType";
		const std::string BUYSELL = "Side";
		const std::string QUANTITY = "Quantity";
		const std::string VALUMEREAMAING = "VolumeRemaining";
		const std::string VOLUMEFILLEDTODAY = "VolumeFilledToday";
		const std::string LASTFILLEDQTY = "LastFilledQty";
		const std::string PRICE = "Price";
		const std::string TRIGGERPRICE = "TriggerPrice";
		const std::string REMARKS = "Remarks";
		const std::string REASONCODE = "ReasonCode";
		const std::string TRADENUMBER = "TrdeNumber";
		const std::string ORDERMESSAGE = "OrderMessage";

		namespace STATUS
		{
			const std::string PENDING = "Pending";
			const std::string PARTIALFILL = "Partial Fill";
			const std::string FILLED = "Filled";
			const std::string CANCELLED = "Cancelled";
			const std::string REPLACED = "Replaced";
			const std::string REJECTED = "Rejected";
		}
		namespace TYPE
		{
			const std::string NORMAL = "Normal Order";
			const std::string STOPLOSS = "Stop Loss Order";
		}
		namespace SIDE
		{
			const std::string BUY = "Buy";
			const std::string Sell = "Sell";
		}
		namespace MSG
		{
			const std::string ORDCONF = "[ORD_CONF]:";
			const std::string ORDREJ = "[ORD_Rej]:";
			const std::string ORDMODCONF = "[ORD_MOD_CONF]:";
			const std::string ORDMODREJ = "[ORD_MOD_Rej]:";
			const std::string ORDDELCONF = "[ORD_DEL_CONF]:";
			const std::string ORDDELREJ = "[ORD_DEL_Rej]:";
			const std::string TRADECONF = "[TRADE_CONF]:";
		}
	}
}

#endif
