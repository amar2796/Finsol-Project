#include "ResponseProcessor.h"


Response_Processor::Response_Processor()
{
	fixString = "";
}

vector<OMSResponse*> Response_Processor::IsEligibleToSend(OrderExecNotificationT* response)
{
	vector<OMSResponse*> result;
	try
	{
		for (int i = 0; i < response->NoFills; i++)
		{
			if (tradeRecord.size() <= 0 || tradeRecord.find(response->OrderID) == tradeRecord.end())
			{
				tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);

				//OMSResponse* trade;
				res->SetDefaultValues();
				res->setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);
				result.emplace_back(res);
			}
			else
			{
				auto it = std::find(tradeRecord[response->OrderID].begin(), tradeRecord[response->OrderID].end(), response->FillsGrp[i].FillExecID);

				if (it == tradeRecord[response->OrderID].end())
				{
					tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);

					//OMSResponse* trade;
					res->SetDefaultValues();
					res->setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);

					result.emplace_back(res);
				}
			}

		}

	}
	catch (std::exception& ex)
	{
		LOG_INFO << ex.what();
	}
	return result;
}

vector<OMSResponse*> Response_Processor::IsEligibleToSend(OrderExecResponseT* response)
{
	vector<OMSResponse*> result;
	try
	{
		for (int i = 0; i < response->NoFills; i++)
		{
			if (tradeRecord.size() <= 0 || tradeRecord.find(response->OrderID) == tradeRecord.end())
			{
				tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);
				
				//OMSResponse* trade;
				res->SetDefaultValues();
				res->setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);

				result.emplace_back(res);
			}
			else
			{
				auto it = std::find(tradeRecord[response->OrderID].begin(), tradeRecord[response->OrderID].end(), response->FillsGrp[i].FillExecID);

				if (it == tradeRecord[response->OrderID].end())
				{


					tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);
					
					//OMSResponse* trade;
					res->SetDefaultValues();
					res->setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);

					result.emplace_back(res);
				}
			}

		}

	}
	catch (std::exception& ex)
	{
		LOG_INFO << ex.what();
	}
	return result;
}

OMSResponse* Response_Processor::IsEligibleToSend(NewOrderNRResponseT* response)
{
	res->SetDefaultValues();
	res->header = response->MessageHeaderOut;
	res->OrderID = response->OrderID;
	res->ClOrdID = response->ClOrdID;
	res->OrdStatus[0] = response->OrdStatus[0];
	res->ExecType[0] = response->ExecType[0];
	return res;
}

OMSResponse* Response_Processor::IsEligibleToSend(ModifyOrderNRResponseT* response)
{
	res->SetDefaultValues();
	res->header = response->MessageHeaderOut;
	res->OrderID = response->OrderID;
	res->ClOrdID = response->ClOrdID;
	res->OrigClOrdID = response->OrigClOrdID;
	res->CumQty = response->CumQty;
	res->CxlQty = response->CxlQty;
	res->OrdStatus[0] = response->OrdStatus[0];
	res->ExecType[0] = response->ExecType[0];
	return res;
}

OMSResponse* Response_Processor::IsEligibleToSend(DeleteOrderNRResponseT* response)
{
	res->SetDefaultValues();
	res->header = response->MessageHeaderOut;
	res->OrderID = response->OrderID;
	res->ClOrdID = response->ClOrdID;
	res->OrigClOrdID = response->OrigClOrdID;
	res->CumQty = response->CumQty;
	res->CxlQty = response->CxlQty;
	res->OrdStatus[0] = response->OrdStatus[0];
	res->ExecType[0] = response->ExecType[0];
	return res;
}

OMSResponse* Response_Processor::IsEligibleToSend(RejectT* response)
{
	res->SetDefaultValues();
	res->header = response->MessageHeaderOut;
	return res;
}

OMSResponse* Response_Processor::IsEligibleToSend(OrderExecReportBroadcastT* response)
{
	res->SetDefaultValues();
	res->header = response->MessageHeaderOut;
	res->OrderID = response->OrderID;
	res->ClOrdID = response->ClOrdID;
	res->OrigClOrdID = response->OrigClOrdID;
	res->LeavesQty = response->LeavesQty;
	res->CumQty = response->CumQty;
	res->CxlQty = response->CxlQty;
	res->price = response->Price;
	res->OrdStatus[0] = response->OrdStatus[0];
	res->ExecType[0] = response->ExecType[0];
	return res;
}

Response_Processor::~Response_Processor()
{
}

//OMSSMPFCancel Response_Processor::IsCanceled(OrderExecReportBroadcastT* response)
//{
//	OMSSMPFCancel result{};
//	try
//	{
//		if (response->OrdStatus[0] == '4' || response->ExecType[0] == '4')
//		{
//
//			result.ClOrdID = response->ClOrdID;
//			result.OrderID = response->OrderID;
//			result.CxlQty = response->CxlQty;
//			result.header = response->MessageHeaderOut;
//			result.CumQty = response->CumQty;
//			result.OrigClOrdID = response->OrigClOrdID;
//		}
//	}
//	catch (std::exception& ex)
//	{
//		LOG_INFO << ex.what();
//	}
//	return result;
//}


//OMSResponse* Response_Processor::processAckConfirm(GwOrderAcknowledgementT* confirm)
//{
//	try
//	{
//		_omsRes->clear();
//		_omsRes->header = confirm->MessageHeaderOut;
//		_omsRes->OrderID = confirm->PrimaryOrderID;
//		_omsRes->ClOrdID = confirm->ClOrdID;
//		
//		
//		/*fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			"37=" + std::to_string(confirm->PrimaryOrderID) + SOH +
//			"11=" + std::to_string(confirm->ClOrdID) + SOH +
//			"17=" + std::to_string(confirm->ClOrdID) + SOH +
//			"39=0" + SOH +
//			"150=0" + SOH;*/
//
//	}
//	catch (std::exception& ex)
//	{
//
//	}
//	return _omsRes;
//
//}
//
//OMSResponse* Response_Processor::processConfirmResponse(NewOrderNRResponseT* confirm)
//{
//
//	try
//	{
//		_omsRes->clear();
//		_omsRes->header = confirm->MessageHeaderOut; 
//		_omsRes->OrderID = confirm->OrderID;
//		_omsRes->ClOrdID = confirm->ClOrdID;
//		_omsRes->FillExecID = confirm->ExecID;
//		_omsRes->OrdStatus[0] = confirm->OrdStatus[0];
//		_omsRes->ExecType[0] = confirm->ExecType[0];
//
//		/*fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			"37=" + std::to_string(confirm->OrderID) + SOH +
//			"11=" + std::to_string(confirm->ClOrdID) + SOH +
//			"48=" + std::to_string(confirm->SecurityID) + SOH +
//			"17=" + std::to_string(confirm->ExecID) + SOH +
//			"39=" + confirm->OrdStatus[0] + SOH +
//			"150=" + confirm->ExecType[0] + SOH +
//			"58=" + std::to_string(confirm->ExecRestatementReason) + SOH;*/
//
//		activity_time[confirm->OrderID] = confirm->ActivityTime;
//		request_clorderID_exchangeOrderID.erase(confirm->NRResponseHeaderME.MsgSeqNum);
//	}
//	catch (std::exception& ex)
//	{
//
//	}
//	return _omsRes;
//}
//
//OMSResponse* Response_Processor::processImmediateExecution(OrderExecResponseT* trade)
//{
//	try
//	{
//		fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			"37=" + std::to_string(trade->OrderID) + SOH +
//			"11=" + std::to_string(trade->ClOrdID) + SOH +
//			"48=" + std::to_string(trade->SecurityID) + SOH +
//			"17=" + std::to_string(trade->ExecID) + SOH +
//			"39=" + trade->OrdStatus[0] + SOH +
//			"150=" + trade->ExecType[0] + SOH +
//			"41=" + std::to_string(trade->OrigClOrdID) + SOH +
//			"1300=" + std::to_string(trade->MarketSegmentID) + SOH +
//			"151=" + std::to_string(trade->LeavesQty) + SOH +
//			"14=" + std::to_string(trade->CumQty) + SOH +
//			"84=" + std::to_string(trade->CxlQty) + SOH +
//			"1362=" + std::to_string(trade->NoFills) + SOH +
//			"38=" + std::to_string(trade->CumQty + trade->LeavesQty) + SOH;
//
//		for (int i = 0; i < trade->NoFills; i++)
//		{
//			fixString += "1364=" + std::to_string(trade->FillsGrp[i].FillPx) + SOH +
//				"1365=" + std::to_string(trade->FillsGrp[i].FillQty) + SOH +
//				"28708=" + std::to_string(trade->FillsGrp[i].FillMatchID) + SOH +
//				"1363=" + std::to_string(trade->FillsGrp[i].FillExecID) + SOH;
//
//		}
//
//		for (int x = 0; x < trade->NoLegExecs; x++)
//		{
//			fixString += "602=" + std::to_string(trade->InstrmntLegExecGrp[x].LegSecurityID) + SOH +
//				"637=" + to_string(trade->InstrmntLegExecGrp[x].LegLastPx) + SOH +
//				"1418=" + to_string(trade->InstrmntLegExecGrp[x].LegLastQty) + SOH +
//				"1893=" + to_string(trade->InstrmntLegExecGrp[x].LegExecID) + SOH +
//				"624=" + to_string(trade->InstrmntLegExecGrp[x].LegSide) + SOH +
//				"25010=" + to_string(trade->InstrmntLegExecGrp[x].NoFillsIndex) + SOH;
//
//		}
//
//		if (trade->OrdStatus[0] == '1')
//		{
//			if (activity_time.find(trade->OrderID) != activity_time.end())
//				activity_time[trade->OrderID] = trade->ActivityTime;
//		}
//		else
//		{
//			activity_time.erase(trade->OrderID);
//
//		}
//
//	}
//	catch (std::exception& ex)
//	{
//
//
//	}
//	return _omsRes;
//}
//
//OMSResponse* Response_Processor::processExecution(OrderExecNotificationT* trade)
//{
//	try
//	{
//		fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			"37=" + std::to_string(trade->OrderID) + SOH +
//			"11=" + std::to_string(trade->ClOrdID) + SOH +
//			"48=" + std::to_string(trade->SecurityID) + SOH +
//			"17=" + std::to_string(trade->ExecID) + SOH +
//			"39=" + trade->OrdStatus[0] + SOH +
//			"150=" + trade->ExecType[0] + SOH +
//			"41=" + std::to_string(trade->OrigClOrdID) + SOH +
//			"1300=" + std::to_string(trade->MarketSegmentID) + SOH +
//			"151=" + std::to_string(trade->LeavesQty) + SOH +
//			"14=" + std::to_string(trade->CumQty) + SOH +
//			"84=" + std::to_string(trade->CxlQty) + SOH +
//			"1362=" + std::to_string(trade->NoFills) + SOH +
//			"38=" + std::to_string(trade->CumQty + trade->LeavesQty) + SOH;
//
//
//		for (int i = 0; i < trade->NoFills; i++)
//		{
//			fixString += "1364=" + std::to_string(trade->FillsGrp[i].FillPx) + SOH +
//				"1365=" + std::to_string(trade->FillsGrp[i].FillQty) + SOH +
//				"28708=" + std::to_string(trade->FillsGrp[i].FillMatchID) + SOH +
//				"1363=" + std::to_string(trade->FillsGrp[i].FillExecID) + SOH;
//
//
//		}
//
//		for (int x = 0; x < trade->NoLegExecs; x++)
//		{
//			fixString += "602=" + std::to_string(trade->InstrmntLegExecGrp[x].LegSecurityID) + SOH +
//				"637=" + to_string(trade->InstrmntLegExecGrp[x].LegLastPx) + SOH +
//				"1418=" + to_string(trade->InstrmntLegExecGrp[x].LegLastQty) + SOH +
//				"1893=" + to_string(trade->InstrmntLegExecGrp[x].LegExecID) + SOH +
//				"624=" + to_string(trade->InstrmntLegExecGrp[x].LegSide) + SOH +
//				"25010=" + to_string(trade->InstrmntLegExecGrp[x].NoFillsIndex) + SOH;
//
//		}
//		if (trade->OrdStatus[0] == '1')
//		{
//			if (activity_time.find(trade->OrderID) != activity_time.end())
//				activity_time[trade->OrderID] = trade->ActivityTime;
//		}
//		else
//		{
//			activity_time.erase(trade->OrderID);
//		}
//
//	}
//	catch (std::exception& ex)
//	{
//
//
//	}
//	return _omsRes;
//}
//
//OMSResponse* Response_Processor::processModifyResponse(ModifyOrderNRResponseT* modify)
//{
//	try
//	{
//		_omsRes->clear();
//		_omsRes->header = modify->MessageHeaderOut;
//		_omsRes->OrderID = modify->OrderID;
//		_omsRes->ClOrdID = modify->ClOrdID;
//		_omsRes->FillExecID = modify->ExecID;
//		_omsRes->ExecType[0] = modify->ExecType[0];
//		_omsRes->OrigClOrdID = modify->OrigClOrdID;
//		_omsRes->LeavesQty = modify->LeavesQty;
//		_omsRes->CumQty = modify->CumQty;
//		_omsRes->CxlQty = modify->CxlQty;
//		_omsRes->OrdStatus[0] = modify->OrdStatus[0];
//
//
//		/*fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			"37=" + std::to_string(modify->OrderID) + SOH +
//			"11=" + std::to_string(modify->ClOrdID) + SOH +
//			"48=" + std::to_string(modify->SecurityID) + SOH +
//			"17=" + std::to_string(modify->ExecID) + SOH +
//			"39=" + modify->ExecType[0] + SOH +
//			"150=" + modify->ExecType[0] + SOH +
//			"41=" + std::to_string(modify->OrigClOrdID) + SOH +
//			"38=" + std::to_string(modify->LeavesQty + modify->CumQty) + SOH +
//			"151=" + std::to_string(modify->LeavesQty) + SOH +
//			"14=" + std::to_string(modify->CumQty) + SOH +
//			"84=" + std::to_string(modify->CxlQty) + SOH +
//			"44=" + std::to_string(modify->PriceMkToLimitPx) + SOH +
//			"236=" + std::to_string(modify->Yield) + SOH +
//			"58=" + std::to_string(modify->ExecRestatementReason) + SOH;*/
//
//		if (activity_time.find(modify->OrderID) != activity_time.end())
//			activity_time[modify->OrderID] = modify->ActivityTime;
//
//		//request_clorderID.erase(modify->NRResponseHeaderME.MsgSeqNum);
//		request_clorderID_exchangeOrderID.erase(modify->NRResponseHeaderME.MsgSeqNum);
//	}
//	catch (std::exception& ex)
//	{
//
//	}
//	return _omsRes;
//}
//
//OMSResponse* Response_Processor::processCancelResponse(DeleteOrderNRResponseT* cancel)
//{
//	try
//	{
//		_omsRes->clear();
//		_omsRes->header = cancel->MessageHeaderOut;
//		_omsRes->OrderID = cancel->OrderID;
//		_omsRes->FillExecID = cancel->ExecID;
//		_omsRes->ExecType[0] = cancel->ExecType[0];
//		_omsRes->OrdStatus[0] = cancel->OrdStatus[0];
//		_omsRes->OrigClOrdID = cancel->OrigClOrdID;
//		_omsRes->CumQty = cancel->CumQty;
//		_omsRes->CxlQty = cancel->CxlQty;
//
//
//		/*fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			"37=" + std::to_string(cancel->OrderID) + SOH +
//			"11=" + std::to_string(cancel->ClOrdID) + SOH +
//			"48=" + std::to_string(cancel->SecurityID) + SOH +
//			"17=" + std::to_string(cancel->ExecID) + SOH +
//			"39=" + cancel->ExecType[0] + SOH +
//			"150=" + cancel->ExecType[0] + SOH +
//			"41=" + std::to_string(cancel->OrigClOrdID) + SOH +
//			"14=" + std::to_string(cancel->CumQty) + SOH +
//			"84=" + std::to_string(cancel->CxlQty) + SOH;*/
//
//		activity_time.erase(cancel->OrderID);
//		//request_clorderID.erase(cancel->NRResponseHeaderME.MsgSeqNum);
//		request_clorderID_exchangeOrderID.erase(cancel->NRResponseHeaderME.MsgSeqNum);
//	}
//	catch (std::exception& ex)
//	{
//
//	}
//	return _omsRes;
//}
//
//OMSResponse* Response_Processor::processRejectResponse(RejectT* reject)
//{
//	try
//	{
//		fixString = BEGIN_STRING + SOH + "35=8" + SOH;
//		string reject_status = "";
//		if (request_clorderID_exchangeOrderID.find(reject->NRResponseHeaderME.MsgSeqNum) != request_clorderID_exchangeOrderID.end())
//		{
//			if (request_clorderID_exchangeOrderID[reject->NRResponseHeaderME.MsgSeqNum].second != 0)
//			{
//				reject_status = "37=" + std::to_string(request_clorderID_exchangeOrderID[reject->NRResponseHeaderME.MsgSeqNum].second) + SOH +
//					"11=" + std::to_string(request_clorderID_exchangeOrderID[reject->NRResponseHeaderME.MsgSeqNum].first) + SOH +
//					"39=9" + SOH + "150=9" + SOH;
//
//			}
//			else
//			{
//				reject_status = "37=0" + SOH +
//					"11=" + std::to_string(request_clorderID_exchangeOrderID[reject->NRResponseHeaderME.MsgSeqNum].first) + SOH +
//					"39=8" + SOH + "150=8" + SOH;
//
//			}
//
//		}
//
//		fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			reject_status +
//			"58=" + reject->VarText + SOH;
//
//	}
//	catch (std::exception& ex)
//	{
//		std::cout << ex.what() << std::endl;
//	}
//	return _omsRes;
//}
//
//OMSResponse* Response_Processor::processSelfTradeCancel(OrderExecReportBroadcastT* cancel)
//{
//	try
//	{
//		_omsRes->clear();
//		_omsRes->header = cancel->MessageHeaderOut;
//		_omsRes->OrderID = cancel->OrderID;
//		_omsRes->ClOrdID = cancel->ClOrdID;
//		_omsRes->ExecType[0] = cancel->ExecType[0];
//		_omsRes->OrdStatus[0] = cancel->OrdStatus[0];
//		_omsRes->OrigClOrdID = cancel->OrigClOrdID;
//		_omsRes->CumQty = cancel->CumQty;
//		_omsRes->CxlQty = cancel->CxlQty;
//
//		/*fixString = BEGIN_STRING + SOH + "35=8" + SOH +
//			"37=" + std::to_string(cancel->OrderID) + SOH +
//			"11=" + std::to_string(cancel->ClOrdID) + SOH +
//			"48=" + std::to_string(cancel->SecurityID) + SOH +
//			"17=" + std::to_string(cancel->ExecID) + SOH +
//			"39=" + cancel->ExecType[0] + SOH +
//			"150=" + cancel->ExecType[0] + SOH +
//			"41=" + std::to_string(cancel->OrigClOrdID) + SOH +
//			"14=" + std::to_string(cancel->CumQty) + SOH +
//			"84=" + std::to_string(cancel->CxlQty) + SOH +
//			"58=Reason Code :" + std::to_string(cancel->ExecRestatementReason) + SOH;*/
//
//		activity_time.erase(cancel->OrderID);
//		//request_clorderID.erase(cancel->NRResponseHeaderME.MsgSeqNum);
//		//request_clorderID_exchangeOrderID.erase(cancel->MessageHeaderOut.MsgSeqNum);
//	}
//	catch (std::exception& ex)
//	{
//
//	}
//	return _omsRes;
//}

