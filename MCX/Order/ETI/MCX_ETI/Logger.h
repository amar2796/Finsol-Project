#pragma once
#ifndef FIN_LOGGER_H
#define FIN_LOGGER_H

#include "Nanolog.hpp"
#include <chrono>
#include "threadSafeQueue.h"
#include <time.h>
#include "constants.h"
#include "ETI.h"
#include <sstream>
using namespace FIN;
using namespace Derivatives;

//typedef struct
//{
//	int16_t type;
//	//std::chrono::time_point<std::chrono::system_clock> _currentLogTime;
//	ConnectionGatewayRequestT connGwReq;//1
//	ConnectionGatewayResponseT connGwRes;//2
//	LogonRequestT    logonRequest;//3
//	LogonResponseT logonResponse;//4
//	UserLoginRequestT userLogonRequest;//5
//	UserLoginResponseT userLogonResponse;//6
//	NewOrderSingleRequestT newOrder;//7
//	NewOrderNRResponseT newLeanRes;//8
//	NewOrderResponseT standadrRes;//9
//	ModifyOrderSingleRequestT modReq;//10
//	ModifyOrderNRResponseT modRes;//11
//	ModifyOrderResponseT standModRes;//12
//	DeleteOrderSingleRequestT canReq;//13
//	DeleteOrderNRResponseT canRes;//14
//	DeleteOrderResponseT standCanRes;//15
//	OrderExecResponseT   trdRes;//16
//	OrderExecNotificationT trdNotification;//17
//	OrderExecReportBroadcastT executionReport;//18
//	TradeBroadcastT executionBroad; //19
//	RejectT reject; //20
//}LoggerObject;
class Logger
{
public:
	Logger();
	/*LoggerObject logger_request;
	LoggerObject logger_response;

	void info(LoggerObject obj, int16_t type);




	ThreadSafeQueue<LoggerObject> loggingQueue;
	void processQueue();*/

	// Logon request and response 
	void LogLogonRequestT(LogonRequestT* request);
	void LogLogonResponseT(LogonResponseT* response);

	// Connection gw req and res 
	void LogConnectionGatewayRequest(Derivatives::ConnectionGatewayRequestT* request);

	void LogConnectionGatewayResponse(Derivatives::ConnectionGatewayResponseT* resp);


	// req header and message header 
	void LogMessageHeaderInCompT(MessageHeaderInCompT* request);

	void LogRequestHeaderCompT(RequestHeaderCompT* request);


	// response header and message header 
	/*void LogMessageHeaderOutCompT(MessageHeaderOutDerivativesCompT* request);
	void LogResponseHeaderMECompT(ResponseHeaderMEDerivativesCompT* request);
*/


	// req and res user logon
	void LogUserLoginRequestT(UserLoginRequestT* req);
	void LogUserLoginResponseT(UserLoginResponseT* res);

	// new request and responses 
	void LogNewRequest(NewOrderSingleRequestT* newOrder);
	void LogLeanNewOrderResponse(NewOrderNRResponseT* newLeanRes);
	void LogStandardNewOrderResponse(NewOrderResponseT* newStandard);


	// Modify request and responses 
	void LogModifyOrderSingleRequestT(ModifyOrderSingleRequestT* request);

	void LogLeanModifyResponse(ModifyOrderNRResponseT* response);
	void LogStandardModResponse(ModifyOrderResponseT* response);


	// Delete request and responses

	void LogDeleteSingleRequest(DeleteOrderSingleRequestT* request);

	void LogDeleteLeanResponse(DeleteOrderNRResponseT* response);

	void LogDeleteStandardResponse(DeleteOrderResponseT* response);


	// All trade reports 
	void LogOrderExecResponseT(OrderExecResponseT* response);
	void LogOrderExecNotificationT(OrderExecNotificationT* response);
	void LogOrderExecReportBroadcastT(OrderExecReportBroadcastT* response);
	void LogTradeBroadcastT(TradeBroadcastT* response);

	// reject response 

	void LogRejectT(RejectT* rejRes);



private:

};


#endif