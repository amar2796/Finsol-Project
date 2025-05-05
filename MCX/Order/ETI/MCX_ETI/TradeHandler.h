#pragma once
#ifndef FIN_TRADE_HANDLER_H
#define FIN_TRADE_HANDLER_H

#include <iostream>
#include <unordered_map>
#include <vector>

#include "ETI.h"
#include "Nanolog.hpp"
#include "constants.h"
#include <sstream>

using namespace std;
using namespace Derivatives;
using namespace FIN;

struct OmsTradeResponse
{
public:
	MessageHeaderOutCompT header;
	//uint64_t SendingTime;
	uint64_t OrderID;
	uint64_t ClOrdID;
	uint64_t OrigClOrdID;
	int64_t LeavesQty;
	int64_t CumQty;
	int64_t CxlQty;
	char OrdStatus[LEN_ORD_STATUS];
	char ExecType[LEN_EXEC_TYPE];
	int64_t FillPx;
	int64_t FillQty;
	int32_t FillExecID;

	void log()
	{

		std::ostringstream res;
		res << OMS_TRADE
			<< comma << MSG_TYPE << header.TemplateID
			<< comma << ORDER_ID << OrderID
			<< comma << CLORDER_ID << ClOrdID
			<< comma << ORG_ID << OrigClOrdID
			<< comma << LEAVE_QTY << LeavesQty / FIN::qty_multipler
			<< comma << FILL_QTY << CumQty / FIN::qty_multipler
			<< comma << CXL_QTY << CxlQty / FIN::qty_multipler
			<< comma << ORDER_STATUS << OrdStatus[0]
			<< comma << EXE_STATUS << ExecType[0]
			<< comma << LAST_PX << FillPx / FIN::price_multiplier
			<< comma << LAST_QTY << FillQty / FIN::qty_multipler
			<< comma << TRADE_ID << FillExecID;
		LOG_INFO << res.str();
	}
	void setTrade(OrderExecNotificationT* response, int32_t tradeId, int64_t lastFillPrice, int64_t lastFillQty)
	{
		header = response->MessageHeaderOut;
		OrderID = response->OrderID;
		OrigClOrdID = response->OrigClOrdID;
		ClOrdID = response->ClOrdID;
		LeavesQty = response->LeavesQty;
		CumQty = response->CumQty;
		CxlQty = response->CxlQty;
		OrdStatus[0] = response->OrdStatus[0];
		ExecType[0] = response->ExecType[0];
		FillPx = lastFillPrice;
		FillQty = lastFillQty;
		FillExecID = tradeId;
	}
	void setTrade(OrderExecResponseT* response, int32_t tradeId, int64_t lastFillPrice, int64_t lastFillQty)
	{
		header = response->MessageHeaderOut;
		OrderID = response->OrderID;
		OrigClOrdID = response->OrigClOrdID;
		ClOrdID = response->ClOrdID;
		LeavesQty = response->LeavesQty;
		CumQty = response->CumQty;
		CxlQty = response->CxlQty;
		OrdStatus[0] = response->OrdStatus[0];
		ExecType[0] = response->ExecType[0];
		FillPx = lastFillPrice;
		FillQty = lastFillQty;
		FillExecID = tradeId;
	}
	void setTrade(TradeBroadcastT* response)
	{
		header = response->MessageHeaderOut;
		OrderID = response->OrderID;
		ClOrdID = response->ClOrdID;
		LeavesQty = response->LeavesQty;
		CumQty = response->CumQty;
		FillPx = response->LastPx;
		FillQty = response->LastQty;
		FillExecID = response->TradeID;
	}

};

struct OMSSMPFCancel
{
public:
	MessageHeaderOutCompT header;
	uint64_t OrderID;
	uint64_t ClOrdID;
	uint64_t OrigClOrdID;
	int64_t LeavesQty;
	int64_t CumQty;
	int64_t CxlQty;

};

class TradeHandler
{

private:
	unordered_map<  uint64_t, vector< uint32_t>> tradeRecord;

public:
	//	bool IsEligibleToSend(OrderExecNotificationT*);
	//	bool IsEligibleToSend(OrderExecResponseT*);
	bool IsEligibleToSend(TradeBroadcastT*);

	vector<OmsTradeResponse> IsEligibleToSend(OrderExecNotificationT*);
	vector<OmsTradeResponse> IsEligibleToSend(OrderExecResponseT*);
	//OmsTradeResponse IsEligibleToSend(TradeBroadcastT*);
	//bool IsCanceled(OrderExecReportBroadcastT*);
	OMSSMPFCancel IsCanceled(OrderExecReportBroadcastT*);
};



#endif