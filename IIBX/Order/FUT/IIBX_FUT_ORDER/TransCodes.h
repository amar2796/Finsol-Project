#ifndef TRANSCODES_H
#define TRANSCODES_H
namespace FIN 
{	
	enum MSGTYPE {
		Logon_Message_R = 100,
		LogOff_Message_R = 101,
		Change_Password_Message_R = 102,
		Add_Order_Message_R = 103,
		Modify_Order_Message_R = 104,
		Delete_Order_Message_R = 105,
		Download_Product_Message_R = 106,
		Download_Contract_Message_R = 107,
		Download_Pending_Order_Message_R = 108,
		Download_Stoploss_Order_Message_R = 109,
		Download_Trade_Message_R = 110,
		Download_Returned_Orders_Message_R = 111,
		Download_Returned_Stoploss_Order_R = 112,
		Download_Market_Summary_Message_R = 113,
		Download_Client_Wise_Margin_MTM_Message_R = 114,
		Download_Client_Wise_Open_Position_Message_R = 115,
		Download_Market_Picture_BBO_Msg_R = 116,
		Trade_Notification_Message_R = 117,
		Stoploss_Converted_Notification_R = 118,
		Limit_Violation_Notification_Msg_R = 119,
		IOC_Order_Kill_Notification_Msg_R = 120,
		Order_Return_Due_To_Self_Match_R = 121,
		Member_Action_Notification_Msg_R = 129,
		Client_Action_Notification_Msg_R = 130,
		Trade_Notification_Broadcast_Message_R = 122,
		Dpr_Hit_Notification_Broadcast_Message_R = 123,
		Dpr_Relaxed_Notification_Broadcast_Message_R = 124,
		Session_Change_Notification_Broadcast_Message_R = 125,
		CTS_Time_Broadcast_Message_R = 126,
		Market_Picture_Broadcast_Message_R = 127,
		Technical_Failure_Message_R = 128,
	};
}
#endif // TRANSCODES_H