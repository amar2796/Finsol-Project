#pragma once
#ifndef RESPONSE_PROCESSOR_H
#define RESPONSE_PROCESSOR_H
#include <string>
#include <iostream>
#include "../../HeaderFile/ETILayouts.h"
#include <unordered_map>
#include "../../NanoLog/Nanolog.hpp"

using namespace std;

class Response_Processor
{
public:
	std::unordered_map<uint64_t, uint64_t> activity_time;
	std::unordered_map < uint32_t, std::pair< uint64_t, uint64_t> > request_clorderID_exchangeOrderID;
	Response_Processor();
	OMSResponse* res;
	vector<OMSResponse*> IsEligibleToSend(OrderExecNotificationT* response);
	vector<OMSResponse*> IsEligibleToSend(OrderExecResponseT* response);
	OMSResponse* IsEligibleToSend(NewOrderNRResponseT* response);
	OMSResponse* IsEligibleToSend(ModifyOrderNRResponseT* response);
	OMSResponse* IsEligibleToSend(DeleteOrderNRResponseT* response);
	OMSResponse* IsEligibleToSend(RejectT* response);
	OMSResponse* IsEligibleToSend(OrderExecReportBroadcastT* response);
	/*bool IsEligibleToSend(TradeBroadcastT* response);
	OMSResponse* processAckConfirm(GwOrderAcknowledgementT*);
	OMSResponse* processConfirmResponse(NewOrderNRResponseT*);
	OMSResponse* processImmediateExecution(OrderExecResponseT*);
	OMSResponse* processExecution(OrderExecNotificationT*);
	OMSResponse* processModifyResponse(ModifyOrderNRResponseT*);
	OMSResponse* processCancelResponse(DeleteOrderNRResponseT*);
	OMSResponse* processRejectResponse(RejectT*);
	OMSResponse* processSelfTradeCancel(OrderExecReportBroadcastT*);*/
	~Response_Processor();

private:
	std::string fixString;
	OMSResponse* _omsRes;
	
	std::string BEGIN_STRING = "8=FIX.4.2";
	std::string SOH = "\x1";

	unordered_map<  uint64_t, vector< uint32_t>> tradeRecord;





};



#endif