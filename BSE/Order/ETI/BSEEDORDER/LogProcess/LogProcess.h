#pragma once
#include "../HeaderFile/ETILayouts.h"
#include "../NanoLog/Nanolog.hpp"
#include "../HeaderFile/Common.h"
#include <sstream>

class LogProcess
{
public:
	LogProcess() = default;
	

	void logPrint(NewOrderNRResponseT* msg);
	void logPrint(ModifyOrderNRResponseT* response);
	void logPrint(LogonRequestT request);
	void logPrint(LogonResponseT* response);
	void logPrint(GatewayRequestT request);
	void logPrint(SessionRegistrationRequestT msg);
	void logPrint(GatewayResponseT* resp);
	void logPrint(DeleteOrderSingleRequestT* request);
	void logPrint(DeleteOrderNRResponseT* response);
	void logPrint(OrderExecReportBroadcastT* response);
	void logPrint(RejectT* rejRes);
	void logPrint(SessionRegistrationResponseT* msg);
	void logPrint(TradingSessionStatusBroadcastT* msg);
	void logPrint(ForcedLogoutNotificationT* msg);
	void logPrint(GwOrderAcknowledgementT* msg);
	//void logPrint(OMSResponse msg);
	void logPrint(OMSResponse* msg);
	void logPrint(UserLoginRequestT req);
	void logPrint(SessionPasswordChangeRequestT msg);
	void logPrint(UserPasswordChangeRequestT msg);
	void logPrint(UserLoginResponseT* res);
	void logPrint(NewOrderSingleRequestT* newOrder);
	void logPrint(ModifyOrderSingleRequestT* request);
	void logPrint(OMSRequest* msg);

};
