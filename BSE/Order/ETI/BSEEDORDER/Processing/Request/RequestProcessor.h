#pragma once
#ifndef REQUEST_PROCESSOR_H
#define REQUEST_PROCESSOR_H
#include <string>
#include <iostream>
#include "../../HeaderFile/ETILayouts.h"
#include "../../HeaderFile/Fix_StringParser.h"
#include "../../HeaderFile/SessionSettings.h"
#include "../../CommonFile/CustodianReader.h"
#include "../../Processing/Response/ResponseProcessor.h"
#include "../../LogProcess/LogProcess.h"

using namespace FIN;
struct Request_Structure 
{
	//set default value
	Request_Structure()
	{
		//DeleteOrderSingleRequestT default value
		//MSG Header
		CancelReq.MessageHeaderIn.BodyLen = sizeof(DeleteOrderSingleRequestT);
		CancelReq.MessageHeaderIn.TemplateID = TID_DELETEORDERSINGLEREQUEST;
		strcpy(CancelReq.MessageHeaderIn.NetworkMsgID, "");
		strcpy(CancelReq.MessageHeaderIn.Pad2, "");
		//Body part
		CancelReq.MarketSegmentID = 0x80000000;
		CancelReq.RegulatoryID = 0;
		strcpy(CancelReq.Pad4, "");


		//ModifyOrderSingleRequestT default value
		// MSG Header
		ReplaceReq.MessageHeaderIn.BodyLen = sizeof(ModifyOrderSingleRequestT);
		ReplaceReq.MessageHeaderIn.TemplateID = TID_MODIFYORDERSINGLEREQUEST;
		strcpy(ReplaceReq.MessageHeaderIn.NetworkMsgID, "");
		strcpy(ReplaceReq.MessageHeaderIn.Pad2, "");
		//Body part
		ReplaceReq.MaxPricePercentage = 0;
		ReplaceReq.Filler1 = 0xFFFFFFFFFFFFFFFF;
		ReplaceReq.Filler2 = 0xFFFFFFFF;
		ReplaceReq.MaxShow = 0;
		ReplaceReq.ExpireDate = 0xFFFFFFFF;
		ReplaceReq.MarketSegmentID = 0x80000000;
		ReplaceReq.RegulatoryID = 0;
		ReplaceReq.Filler4 = 0xFFFF;
		strcpy(ReplaceReq.PartyIDTakeUpTradingFirm, "");
		strcpy(ReplaceReq.PartyIDOrderOriginationFirm, "");
		strcpy(ReplaceReq.PartyIDBeneficiary, "");
		strcpy(ReplaceReq.Account, FIN::A1_ACC.c_str());
		strcpy(ReplaceReq.CPCode, "");
		strcpy(ReplaceReq.FreeText3, "");
		strcpy(ReplaceReq.ClientCode, "");
		ReplaceReq.ApplSeqIndicator = 0;
		ReplaceReq.PriceValidityCheckType = 0;
		ReplaceReq.ExecInst = 2;
		ReplaceReq.RolloverFlag = 0xFF;
		ReplaceReq.TradingSessionSubID = 0xFF;
		ReplaceReq.TradingCapacity = 1;
		strcpy(ReplaceReq.DeltaQtyFlag, "");
		ReplaceReq.PositionEffect[0] = 'C';
		strcpy(ReplaceReq.PartyIDLocationID, "");
		strcpy(ReplaceReq.CustOrderHandlingInst, "");
		strcpy(ReplaceReq.RegulatoryText, "");
		strcpy(ReplaceReq.Pad4, "");


		//NewOrderSinlgeRequestT
		//MSG HEADER 
		OrderReq.MessageHeaderIn.BodyLen = sizeof(NewOrderSingleRequestT);
		OrderReq.MessageHeaderIn.TemplateID = TID_NEWORDERSINGLEREQUEST;
		strcpy(OrderReq.MessageHeaderIn.NetworkMsgID, "");
		strcpy(OrderReq.MessageHeaderIn.Pad2, "");
		//Body Part
		OrderReq.MaxPricePercentage = 0;
		OrderReq.Filler1 = 0xFFFFFFFFFFFFFFFF;
		OrderReq.Filler2 = 0xFFFFFFFF;
		OrderReq.MaxShow = 0;
		OrderReq.ExpireDate = 0xFFFFFFFF;
		OrderReq.MarketSegmentID = 0x80000000;
		OrderReq.RegulatoryID = 0xFFFFFFFF;
		OrderReq.Filler4 = 0xFFFF;
		strcpy(OrderReq.ContraBroker, "");
		strcpy(OrderReq.PartyIDOrderOriginationFirm, "");
		strcpy(OrderReq.PartyIDBeneficiary, "");
		strcpy(OrderReq.Account, A1_ACC.c_str());
		strcpy(OrderReq.CPCode, "");
		strcpy(OrderReq.FreeText3, "");
		memset(OrderReq.ClientCode, NULL, 12);
		OrderReq.ApplSeqIndicator = 0;
		OrderReq.PriceValidityCheckType = 0;
		OrderReq.ExecInst = 2;
		OrderReq.STPCFlag = 1;
		OrderReq.RolloverFlag = 0xFF;
		OrderReq.TradingSessionSubID = 0xFF;
		OrderReq.TradingCapacity = 1;
		strcpy(OrderReq.PositionEffect, "C");
		strcpy(OrderReq.PartyIDLocationID, "");
		strcpy(OrderReq.CustOrderHandlingInst, "");
		strcpy(OrderReq.RegulatoryText, "");

	}
	int requestType = 5;
	DeleteOrderSingleRequestT CancelReq;
	ModifyOrderSingleRequestT ReplaceReq;
	NewOrderSingleRequestT OrderReq;

};

class Request_Processor 
{
public:
	Request_Processor();
	Request_Processor(SessionSettings* conf);
	~Request_Processor();
	Request_Structure req_struct;
	Request_Structure processRequest(OMSRequest*, uint32_t);
	uint32_t sequence_number;
	SessionSettings *confDetails;
	Response_Processor res;
	CustodianReader custodian_reader;
	 std::string OWN_FREE_TEXT1 = "OWN";
	 LogProcess* log;

private:

	//FixStringParser parser;
	void processNewRequest(OMSRequest* msg);
	void processModifyRequest(OMSRequest* msg);
	void processCancleRequest(OMSRequest* msg);

};



#endif