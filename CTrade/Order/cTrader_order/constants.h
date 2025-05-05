#ifndef FIN_ORDER_GW_CONSTATNTS_H
#define FIN_ORDER_GW_CONSTATNTS_H

#include <string>
#include <unordered_map>

namespace FIN {

    typedef std::unordered_map< std::string , std::string > FixKeyValue ;
    const std::string Fix_PositionEffect = "77";
    const std::string Fix_Account           = "1"   ;
    const std::string Fix_AvgPX             = "6"   ;
    const std::string Fix_ClOrdID           = "11"  ;
	const std::string Fix_Currency			= "15"	;
    const std::string Fix_CumQty            = "14"  ;
    const std::string Fix_ExecID            = "17"  ;
    const std::string Fix_LastPx            = "31"  ;
    const std::string Fix_LastShares        = "32"  ;
    const std::string Fix_MsgType           = "35"  ;
    const std::string Fix_OrderID           = "37"  ;
    const std::string Fix_OrdQty            = "38"  ;
    const std::string Fix_OrdStatus         = "39"  ;
    const std::string Fix_OrdType           = "40"  ;
    const std::string Fix_OrigClOrdID       = "41"  ;
    const std::string Fix_Price             = "44"  ;
    const std::string Fix_SecurityID        = "48"  ;
    const std::string Fix_SenderCompID      = "49"  ;
    const std::string Fix_SenderSubID       = "50"  ;
    const std::string Fix_Side              = "54"  ;
    const std::string Fix_Symbol            = "55"  ;
    const std::string Fix_TargetSubID       = "57"  ;
    const std::string Fix_Text              = "58"  ;
    const std::string Fix_TimeInForce       = "59"  ;
    const std::string Fix_TransactTime      = "60"  ;
    const std::string Fix_StopPX            = "99"  ;
    const std::string Fix_ExecType          = "150" ;
    const std::string Fix_LeavesQty         = "151" ;
    const std::string Fix_SecurityType      = "167" ;
    const std::string Fix_MaturityMonthYear = "200" ;
    const std::string Fix_OptionType        = "201" ;
    const std::string Fix_StrikePrice       = "202" ;
    const std::string Fix_CustomerOrFirm    = "204" ;
    const std::string Fix_MaturityDate      = "205" ;
    const std::string Fix_CxlRejResponseTo  = "434" ;
    const std::string Fix_Product           = "460" ;

    const std::string Fix_Expirytime = "126";
    const std::string Fix_ExpiryDate = "432";
 
    // STRING VALUES FOR FIX KEYS
    const std::string Fix_SecurityType_FUT        = "FUT" ;
    
    const std::string Fix_CustomerOrFirm_Customer = "0" ;
    const std::string Fix_CustomerOrFirm_Firm     = "1" ;

    const std::string Fix_Account_OWN = "OWN" ;
    const std::string Fix_Account_PRO = "PRO" ;
    
    const std::string Fix_OrdType_Limit = "2" ;

    const char Fix_Side_Buy = '1' ;
    const char Fix_Side_Sell = '2' ;

    const char Fix_OptionType_PutEuropean  = '0' ;
    const char Fix_OptionType_CallEuropean = '1' ;
    const char Fix_OptionType_CallAmerican = '2' ;
    const char Fix_OptionType_PutAmerican  = '3' ;
 
    const char Fix_Product_IDX = '7' ;
    const char Fix_Product_STK = '5' ;
    const char Fix_Product_CUR = '4' ;

    const char Fix_MsgType_NEW_ORDER = 'D' ;
    const char Fix_MsgType_CANCEL_ORDER = 'F' ;
    const char Fix_MsgType_MODIFY_ORDER = 'G' ;

    const char Fix_TimeInForce_DAY = '0' ;
    const char Fix_TimeInForce_IOC = '3' ;

}

#endif
