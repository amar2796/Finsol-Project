#include "RequestProcess.h"

RequestProcess::RequestProcess(SessionSettings* s, boost::asio::io_service& ioService):ExchangeConnection(s, ioService)
{
	sessionSetting = s;
}


void RequestProcess::ConnectToExchange_GatewayRequest()
{
	Gateway_Req.MessageHeaderIn.BodyLen = sizeof(GatewayRequestT);
	Gateway_Req.MessageHeaderIn.TemplateID = TID_GATEWAYREQUEST;
	strcpy_s(Gateway_Req.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(Gateway_Req.MessageHeaderIn.Pad2, "0");
	Gateway_Req.RequestHeader.MsgSeqNum = 1;
	Gateway_Req.RequestHeader.SenderSubID = 0;
	Gateway_Req.PartyIDSessionID = sessionSetting->SessionId;
	strcpy_s(Gateway_Req.DefaultCstmApplVerID, sessionSetting->DefaultCstmApplVerID.c_str());
	strcpy_s(Gateway_Req.Password, sessionSetting->Password.c_str());
	strcpy_s(Gateway_Req.Pad6, "0");

	if (sendToSSL((const char*)&Gateway_Req, sizeof(Gateway_Req)))
		isGatewayRequestSend = true;
	else
		isGatewayRequestSend = false;

	log->logPrint(Gateway_Req);
}

void RequestProcess::SendSessionRegistrationRequest()
{
	//this is MsgHeader part
	_sRegRequest.MessageHeaderIn.BodyLen = sizeof(SessionRegistrationRequestT);
	_sRegRequest.MessageHeaderIn.TemplateID = TID_SESSION_REGISTRATION_REQUEST;
	strcpy_s(_sRegRequest.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(_sRegRequest.MessageHeaderIn.Pad2, "0");

	//this is RequestHeader
	_sRegRequest.RequestHeader.MsgSeqNum = 1;
	_sRegRequest.RequestHeader.SenderSubID = sessionSetting->Username;

	//this is MessageBody
	_sRegRequest.PartyIDSessionID = sessionSetting->SessionId;
	strcpy_s(_sRegRequest.Pad4, "0");
	_sRegRequest.Filler1 = 0;

	if (sendToTarget((const char*)&_sRegRequest, _sRegRequest.MessageHeaderIn.BodyLen))
	{
		std::cout << "Session Registraion Request" << std::endl;
		log->logPrint(_sRegRequest);
	}
}

void RequestProcess::ConnectToExchange_LogonRequest()
{
	Logon_Req.MessageHeaderIn.BodyLen = sizeof(Logon_Req);
	Logon_Req.MessageHeaderIn.TemplateID = TID_LOGONREQUEST;
	strcpy_s(Logon_Req.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(Logon_Req.MessageHeaderIn.Pad2, "0");
	Logon_Req.RequestHeader.MsgSeqNum = 2;							
	Logon_Req.RequestHeader.SenderSubID = 0;
	Logon_Req.HeartBtInt = sessionSetting->HeartBt;
	Logon_Req.PartyIDSessionID = sessionSetting->SessionId;
	strcpy_s(Logon_Req.DefaultCstmApplVerID, sessionSetting->DefaultCstmApplVerID.c_str());			
	strcpy_s(Logon_Req.Password, sessionSetting->Password.c_str());
	strcpy(Logon_Req.ApplUsageOrders, sessionSetting->ApplUsageOrders.c_str());
	strcpy(Logon_Req.ApplUsageQuotes, sessionSetting->ApplUsageQuotes.c_str());
	strcpy(Logon_Req.OrderRoutingIndicator, sessionSetting->OrderRoutingIndicator.c_str());
	strcpy_s(Logon_Req.ApplicationSystemName, sessionSetting->ApplicationSystemName.c_str());
	strcpy_s(Logon_Req.ApplicationSystemVersion, sessionSetting->ApplicationSystemVersion.c_str());
	strcpy_s(Logon_Req.FIXEngineVersion, "0");
	strcpy_s(Logon_Req.FIXEngineName, "0");
	strcpy_s(Logon_Req.FIXEngineVendor, "0");
	strcpy_s(Logon_Req.ApplicationSystemVendor, sessionSetting->ApplicationSystemVendor.c_str());
	strcpy_s(Logon_Req.Pad3, "0");
	if(sendToExchange((unsigned char*)&Logon_Req, sizeof(Logon_Req)))
		std::cout << "Session logon request send" << std::endl;
	

	log->logPrint(Logon_Req);
}

void RequestProcess::userLogon()
{
	User_Logon_Req.MessageHeaderIn.BodyLen = sizeof(UserLoginRequestT);
	User_Logon_Req.MessageHeaderIn.TemplateID = TID_USERLOGINREQUEST;
	strcpy_s(User_Logon_Req.MessageHeaderIn.NetworkMsgID, "");
	strcpy_s(User_Logon_Req.MessageHeaderIn.Pad2, "");
	User_Logon_Req.RequestHeader.MsgSeqNum = 3;
	User_Logon_Req.RequestHeader.SenderSubID = 0;
	User_Logon_Req.Username = sessionSetting->Username;
	strcpy_s(User_Logon_Req.Password, sessionSetting->Password.c_str());
	strcpy_s(User_Logon_Req.Pad4, "");

	sendToExchange((unsigned char*)&User_Logon_Req, sizeof(User_Logon_Req));
	
	std::cout << "Send User Logon" << std::endl;
	log->logPrint(User_Logon_Req);
}

int RequestProcess::GenerateHeartBeat()
{
	HeartbeatT HeartBT_Req;
	HeartBT_Req.MessageHeaderIn.BodyLen = sizeof(HeartBT_Req);
	HeartBT_Req.MessageHeaderIn.TemplateID = TID_HEARTBEAT;
	strcpy_s(HeartBT_Req.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(HeartBT_Req.MessageHeaderIn.Pad2, "0");
	sendToExchange((unsigned char*)&HeartBT_Req, sizeof(HeartBT_Req));
	LOG_INFO << "HEARTBEAT ";
	return 0;
}

void RequestProcess::sessionPasswordChange()
{
	_sPasswordChangeReq.MessageHeaderIn.BodyLen = sizeof(SessionPasswordChangeRequestT);
	_sPasswordChangeReq.MessageHeaderIn.TemplateID = TID_SESSIONPASSWORDCHANGEREQUEST;
	strcpy_s(_sPasswordChangeReq.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(_sPasswordChangeReq.MessageHeaderIn.Pad2, "0");
	_sPasswordChangeReq.RequestHeader.MsgSeqNum = 0;							//seq
	_sPasswordChangeReq.RequestHeader.SenderSubID = sessionSetting->Username;
	_sPasswordChangeReq.PartyIDSessionID = sessionSetting->SessionId;
	strcpy_s(_sPasswordChangeReq.Password, sessionSetting->Password.c_str());
	strcpy_s(_sPasswordChangeReq.NewPassword, sessionSetting->newPassword.c_str());
	strcpy_s(_sPasswordChangeReq.Pad4, "0");
	sendToExchange((unsigned char*)&_sPasswordChangeReq, sizeof(_sPasswordChangeReq));

	log->logPrint(_sPasswordChangeReq);
}

void RequestProcess::userPasswordChange()
{
	_uPasswordChangeReq.MessageHeaderIn.BodyLen = sizeof(UserPasswordChangeRequestT);
	_uPasswordChangeReq.MessageHeaderIn.TemplateID = TID_USERPASSWORDCHANGEREQUEST;
	strcpy_s(_uPasswordChangeReq.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(_uPasswordChangeReq.MessageHeaderIn.Pad2, "0");
	_uPasswordChangeReq.RequestHeader.MsgSeqNum = 0;							//seq
	_uPasswordChangeReq.RequestHeader.SenderSubID = sessionSetting->Username;
	_uPasswordChangeReq.Username = sessionSetting->Username;
	strcpy_s(_uPasswordChangeReq.Password, sessionSetting->Password.c_str());
	strcpy_s(_uPasswordChangeReq.NewPassword, sessionSetting->newPassword.c_str());
	strcpy_s(_uPasswordChangeReq.Pad4, "0");
	sendToExchange((unsigned char*)&_uPasswordChangeReq, sizeof(_uPasswordChangeReq));

	log->logPrint(_uPasswordChangeReq);
}