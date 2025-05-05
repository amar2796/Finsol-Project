#pragma once
#include "../../Exchange/ExchangeConnection.h"
#include "../../HeaderFile/ETILayouts.h"
#include "../../HeaderFile/SessionSettings.h"
#include "../../LogProcess/LogProcess.h"

class RequestProcess : public ExchangeConnection
{
public:
	RequestProcess(SessionSettings* s, boost::asio::io_service& ioService);
	bool isGatewayRequestSend = false;
	SessionSettings* sessionSetting;
	LogProcess* log;
	GatewayRequestT Gateway_Req;
	SessionRegistrationRequestT _sRegRequest;
	LogonRequestT Logon_Req;
	UserLoginRequestT User_Logon_Req;
	//HeartbeatT HeartBT_Req;
	SessionPasswordChangeRequestT _sPasswordChangeReq;
	UserPasswordChangeRequestT _uPasswordChangeReq;

	void ConnectToExchange_GatewayRequest();
	void SendSessionRegistrationRequest();
	void ConnectToExchange_LogonRequest();
	void userLogon();
	int GenerateHeartBeat();
	void sessionPasswordChange();
	void userPasswordChange();
};