#include "Logger.h"

Logger::Logger()
{
}

//void Logger::info(LoggerObject obj, int16_t type)
//{
//	obj.type = type;
//	loggingQueue.push(obj);
//}

//void Logger::processQueue()
//{
//	//while (true)
//	//{
//	//	try
//	//	{
//	//		LoggerObject loggerObj;
//
//	//		loggingQueue.pop(loggerObj);
//	//		switch (loggerObj.type)
//	//		{
//
//	//		case 1:
//	//			LogConnectionGatewayRequest(&loggerObj.connGwReq);
//	//			break;
//	//		case 2:
//	//			LogConnectionGatewayResponse(&loggerObj.connGwRes);
//	//			break;
//	//		case 3:
//	//			LogLogonRequestT(loggerObj.logonRequest);
//	//			break;
//	//		case 4:
//	//			LogLogonResponseT(loggerObj.logonResponse);
//	//			break;
//	//		case 5:
//	//			LogUserLoginRequestT(loggerObj.userLogonRequest);
//	//			break;
//	//		case 6:
//	//			LogUserLoginResponseT(loggerObj.userLogonResponse);
//	//			break;
//	//		case 7:
//	//			LogNewRequest(loggerObj.newOrder /*GetReadableTime(loggerObj._currentLogTime)*/);
//	//			break;
//	//		case 8:
//	//			LogLeanNewOrderResponse(loggerObj.newLeanRes);
//	//			break;
//	//		case 9:
//	//			LogLeanNewOrderResponse(loggerObj.newLeanRes);
//	//			break;
//	//		case 10:
//	//			LogModifyOrderSingleRequestT(&loggerObj.modReq);
//	//			break;
//	//		case 11:
//	//			LogLeanModifyResponse(loggerObj.modRes);
//	//			break;
//	//		case 12:
//	//			LogStandardModResponse(loggerObj.standModRes);
//	//			break;
//	//		case 13:
//	//			LogDeleteSingleRequest(loggerObj.canReq);
//	//			break;
//	//		case 14:
//	//			LogDeleteLeanResponse(loggerObj.canRes);
//	//			break;
//	//		case 15:
//	//			LogDeleteStandardResponse(loggerObj.standCanRes);
//	//			break;
//	//		case 16:
//	//			LogOrderExecResponseT(loggerObj.trdRes);
//	//			break;
//	//		case 17:
//	//			LogOrderExecNotificationT(loggerObj.trdNotification);
//	//			break;
//	//		case 18:
//	//			LogOrderExecReportBroadcastT(loggerObj.executionReport);
//	//			break;
//	//		case 19:
//	//			LogTradeBroadcastT(loggerObj.executionBroad);
//	//			break;
//	//		case 20:
//	//			LogRejectT(loggerObj.reject);
//	//			break;
//
//	//		default:
//	//			break;
//	//		}
//	//	}
//	//	catch (std::exception& ex)
//	//	{
//	//		CONSOLE_LOG << ex.what() << std::endl;
//	//		LOG_INFO << ex.what();
//	//	}
//	//}
//}

void Logger::LogLogonRequestT(LogonRequestT* request)
{
	LOG_INFO << LOGONREQUEST
		<< request->MessageHeaderIn.TemplateID
		<< comma << request->ApplicationSystemName
		<< comma << request->ApplicationSystemVendor
		<< comma << request->ApplicationSystemVersion
		<< comma << request->ApplUsageOrders
		<< comma << request->ApplUsageQuotes
		<< comma << request->DefaultCstmApplVerID
		<< comma << request->EncryptedDataMessageSize
		<< comma << request->FIXEngineName
		<< comma << request->FIXEngineVendor
		<< comma << request->HeartBtInt
		<< comma << request->PartyIDSessionID
		<< comma << std::string(request->Password, request->EncryptedDataMessageSize);
}

void Logger::LogLogonResponseT(LogonResponseT* response)
{
	LOG_INFO << LOGONRESPONSE_T << response->MessageHeaderOut.TemplateID
		<< comma << (unsigned)response->DaysLeftForPasswordExpiry
		<< comma << response->DefaultCstmApplVerID
		<< comma << (unsigned)response->GraceLoginsLeft
		<< comma << response->HeartBtInt
		<< comma << response->LastLoginIP
		<< comma << response->LastLoginTime
		<< comma << response->ThrottleDisconnectLimit
		<< comma << response->ThrottleNoMsgs
		<< comma << response->ThrottleTimeInterval
		<< comma << (unsigned)response->TradSesMode;
}

void Logger::LogConnectionGatewayRequest(Derivatives::ConnectionGatewayRequestT* request)
{

	LOG_INFO << CONNECTIONGATEWAYREQUEST_T
		<< comma << request->MessageHeaderIn.BodyLen
		<< comma << request->MessageHeaderIn.NetworkMsgID
		<< comma << request->MessageHeaderIn.Pad2
		<< comma << request->MessageHeaderIn.TemplateID
		<< comma << request->RequestHeader.MsgSeqNum
		<< comma << request->RequestHeader.SenderSubID
		<< comma << request->DefaultCstmApplVerID
		<< comma << request->EncryptedDataMessageSize
		<< comma << request->Filler7
		<< comma << request->Pad2
		<< comma << request->PartyIDSessionID
		<< comma << std::string(request->Password, request->EncryptedDataMessageSize);
}

void Logger::LogConnectionGatewayResponse(Derivatives::ConnectionGatewayResponseT* resp)
{
	LOG_INFO << CONNECTIONGATEWAYRESPONSE_T
		<< resp->GatewayID
		<< comma << resp->GatewaySubID
		<< comma << resp->Pad6
		<< comma << resp->SecondaryGatewayID
		<< comma << resp->SecondaryGatewaySubID
		<< comma << (unsigned)resp->SessionMode
		<< comma << (unsigned)resp->TradSesMode;
}

void Logger::LogMessageHeaderInCompT(MessageHeaderInCompT* request)
{
	LOG_INFO << MESSAGEHEADERINCOMP_T
		<< request->BodyLen
		<< comma << request->NetworkMsgID
		<< comma << request->Pad2
		<< comma << request->TemplateID;
}

void Logger::LogRequestHeaderCompT(RequestHeaderCompT* request)
{

	LOG_INFO << REQUESTHEADERCOMP_T << request->MsgSeqNum
		<< comma << request->SenderSubID;
}

//void Logger::LogMessageHeaderOutCompT(MessageHeaderOutDerivativesCompT* request)
//{
//
//	LOG_INFO << MESSAGEHEADEROUTDERIVATIVESCOMP_T << request->BodyLen
//		<< comma << request->Pad2
//		<< comma << request->TemplateID;
//}

//void Logger::LogResponseHeaderMECompT(ResponseHeaderMEDerivativesCompT* request)
//{
//	LOG_INFO << RESPONSEHEADERMEDERIVATIVESCOMP_T << request->MsgSeqNum
//		<< comma << (unsigned)request->ApplID
//		<< comma << request->ApplMsgID
//		<< comma << request->SendingTime;
//}

void Logger::LogUserLoginRequestT(UserLoginRequestT* req)
{
	LOG_INFO << USERLOGINREQUEST_T << req->MessageHeaderIn.TemplateID
		<< comma << req->EncryptedDataMessageSize
		<< comma << std::string(req->Password, req->EncryptedDataMessageSize)
		<< comma << req->Username;
}

void Logger::LogUserLoginResponseT(UserLoginResponseT* res)
{
	LOG_INFO << USERLOGINRESPONSE_T << res->MessageHeaderOut.TemplateID
		<< comma << (unsigned)res->DaysLeftForPasswordExpiry
		<< comma << (unsigned)res->GraceLoginsLeft
		<< comma << res->LastLoginTime;

}

void Logger::LogNewRequest(NewOrderSingleRequestT* newOrder)
{
	LOG_INFO << NEWORDERSINGLEREQUEST_T
		<< newOrder->MessageHeaderIn.BodyLen
		<< comma << newOrder->MessageHeaderIn.NetworkMsgID
		<< comma << newOrder->MessageHeaderIn.Pad2
		<< comma << newOrder->MessageHeaderIn.TemplateID
		<< comma << newOrder->RequestHeader.MsgSeqNum
		<< comma << newOrder->RequestHeader.SenderSubID
		<< comma << newOrder->Account
		<< comma << unsigned(newOrder->AccountType)
		<< comma << (unsigned)newOrder->ApplSeqIndicator
		<< comma << CLORDER_ID << newOrder->ClOrdID
		<< comma << newOrder->CPCode
		<< comma << newOrder->CustOrderHandlingInst
		<< comma << newOrder->DisclosedQty
		<< comma << newOrder->Echo
		<< comma << (unsigned)newOrder->ExecInst
		<< comma << newOrder->ExpireDate
		<< comma << newOrder->MarketProtectionPercentage
		<< comma << newOrder->MarketSegmentID
		<< comma << ORDER_QUANTITY << newOrder->OrderQty / FIN::qty_multipler
		<< comma << ORDER_TYPE << unsigned(newOrder->OrdType)
		<< comma << newOrder->PositionEffect
		<< comma << PRICE << ((double)(newOrder->Price)) / FIN::price_multiplier
		<< comma << (unsigned)newOrder->PriceValidityCheckType
		<< comma << newOrder->RegulatoryID
		<< comma << SIDE << (unsigned)newOrder->Side
		<< comma << newOrder->SimpleSecurityID
		<< comma << (unsigned)newOrder->SMPFOrderIdentifier
		<< comma << STOP_PRICE << newOrder->StopPx
		<< comma << newOrder->StrategyID
		<< comma << newOrder->StrategyTriggerSeqNo
		<< comma << newOrder->TargetPartyIDSessionID
		<< comma << newOrder->TerminalInfo
		<< comma << TIME_IN_FORCE << (unsigned)newOrder->TimeInForce
		<< comma << (unsigned)newOrder->TradingCapacity
		<< comma << (unsigned)newOrder->TradingSessionSubID
		<< comma << std::string(newOrder->FreeText1);
}
void Logger::LogLeanNewOrderResponse(NewOrderNRResponseT* newLeanRes)
{
	LOG_INFO << NEWORDERNRRESPONSE_T << newLeanRes->MessageHeaderOut.TemplateID
		<< comma << newLeanRes->NRResponseHeaderME.RequestTime
		<< comma << newLeanRes->NRResponseHeaderME.Reserve1
		<< comma << newLeanRes->NRResponseHeaderME.Reserve2
		<< comma << newLeanRes->NRResponseHeaderME.Reserve3
		<< comma << newLeanRes->NRResponseHeaderME.SendingTime
		<< comma << CLORDER_ID << newLeanRes->ClOrdID
		<< comma << (unsigned)newLeanRes->CrossedIndicator
		<< comma << newLeanRes->ExecID
		<< comma << newLeanRes->ExecRestatementReason
		<< comma << EXE_STATUS << newLeanRes->ExecType[0]
		<< comma << ORDER_ID << newLeanRes->OrderID
		<< comma << ORDER_STATUS << newLeanRes->OrdStatus[0]
		<< comma << (unsigned)newLeanRes->ProductComplex
		<< comma << newLeanRes->SecurityID
		<< comma << (double)newLeanRes->PriceMkToLimitPx / FIN::price_multiplier
		<< comma << (unsigned)newLeanRes->TransactionDelayIndicator
		<< comma << (unsigned)newLeanRes->Triggered;
}

void Logger::LogStandardNewOrderResponse(NewOrderResponseT* newStandard)
{
	LOG_INFO << NEWORDERRESPONSE_T
		<< comma << newStandard->ResponseHeaderME.RequestTime
		<< comma << newStandard->ResponseHeaderME.Reserve1
		<< comma << newStandard->ResponseHeaderME.Reserve2
		<< comma << newStandard->ResponseHeaderME.Reserve3
		<< comma << newStandard->ResponseHeaderME.SendingTime
		<< comma << newStandard->ClOrdID
		<< comma << newStandard->OrderID
		<< comma << newStandard->ExecRestatementReason
		<< comma << newStandard->ExecType[0]
		<< comma << newStandard->OrdStatus[0]
		<< comma << newStandard->PriceMkToLimitPx
		<< comma << newStandard->SimpleSecurityID
		<< comma << (double)newStandard->PriceMkToLimitPx / FIN::price_multiplier
		<< comma << newStandard->TrdRegTSEntryTime;
}
void Logger::LogModifyOrderSingleRequestT(ModifyOrderSingleRequestT* request)
{
	LOG_INFO << MODIFYORDERSINGLEREQUEST_T
		<< comma << request->Account
		<< comma << (unsigned)request->AccountType
		<< comma << (unsigned)request->ApplSeqIndicator
		<< comma << request->ClOrdID
		<< comma << request->CPCode
		<< comma << request->CustOrderHandlingInst
		<< comma << request->DisclosedQty
		<< comma << request->Echo
		<< comma << (unsigned)request->ExecInst
		<< comma << request->ExpireDate
		<< comma << request->LstUpdtTime
		<< comma << request->MarketSegmentID
		<< comma << request->MaxPricePercentage
		<< comma << request->OrderID
		<< comma << request->OrderQty / FIN::qty_multipler
		<< comma << (unsigned)request->OrdType
		<< comma << request->OrigClOrdID
		<< comma << request->PositionEffect
		<< comma << (double)request->Price / FIN::price_multiplier
		<< comma << (unsigned)request->PriceValidityCheckType
		<< comma << request->RegulatoryID
		<< comma << (unsigned)request->Side
		<< comma << request->SimpleSecurityID
		<< comma << request->StopPx
		<< comma << request->StrategyID
		<< comma << request->StrategyTriggerSeqNo
		<< comma << request->TargetPartyIDSessionID
		<< comma << request->TerminalInfo
		<< comma << (unsigned)request->TimeInForce
		<< comma << (unsigned)request->TradingCapacity
		<< comma << (unsigned)request->TradingSessionSubID
		<< comma << std::string(request->FreeText1);
}

void Logger::LogLeanModifyResponse(ModifyOrderNRResponseT* response)
{
	LOG_INFO << MODIFYORDERNRRESPONSET << response->NRResponseHeaderME.RequestTime
		<< comma << response->NRResponseHeaderME.RequestTime
		<< comma << response->NRResponseHeaderME.Reserve1
		<< comma << response->NRResponseHeaderME.Reserve2
		<< comma << response->NRResponseHeaderME.Reserve3
		<< comma << response->NRResponseHeaderME.SendingTime
		<< comma
		<< CLORDER_ID << response->ClOrdID
		<< comma << FILL_QTY << response->CumQty / FIN::qty_multipler
		<< comma << CXL_QTY << response->CxlQty / FIN::qty_multipler
		<< comma << response->ExecID
		<< comma << response->ExecRestatementReason
		<< comma << EXE_STATUS << response->ExecType[0]
		<< comma << LEAVE_QTY << response->LeavesQty / FIN::qty_multipler
		<< comma << ORDER_ID << response->OrderID
		<< comma << ORDER_STATUS << response->OrdStatus[0]
		<< comma << ORG_ID << response->OrigClOrdID
		<< comma << response->SecurityID
		<< comma << (double)response->PriceMkToLimitPx / FIN::price_multiplier
		<< comma << (unsigned)response->TransactionDelayIndicator;
}

void Logger::LogStandardModResponse(ModifyOrderResponseT* response)
{
	LOG_INFO << MODIFYORDERRESPONSET << response->MessageHeaderOut.TemplateID
		<< comma
		<< response->ResponseHeaderME.RequestTime
		<< comma << response->ResponseHeaderME.RequestTime
		<< comma << response->ResponseHeaderME.Reserve1
		<< comma << response->ResponseHeaderME.Reserve2
		<< comma << response->ResponseHeaderME.Reserve3
		<< comma
		<< response->ResponseHeaderME.SendingTime
		<< comma << response->ClOrdID
		<< comma << response->CumQty / FIN::qty_multipler
		<< comma << response->CxlQty / FIN::qty_multipler
		<< comma << response->ExecID
		<< comma << response->ExecRestatementReason
		<< comma << response->ExecType[0]
		<< comma << response->LeavesQty / FIN::qty_multipler
		<< comma << response->OrderID
		<< comma << response->OrdStatus[0]
		<< comma << response->OrigClOrdID
		<< comma << (double)response->PriceMkToLimitPx / FIN::price_multiplier
		<< comma << response->SecurityID;
}

void Logger::LogDeleteSingleRequest(DeleteOrderSingleRequestT* request)
{
	LOG_INFO << DELETEORDERSINGLEREQUEST_T << request->MessageHeaderIn.TemplateID
		<< comma << request->ClOrdID
		<< comma << request->Echo
		<< comma << request->MarketSegmentID
		<< comma << request->OrderID
		<< comma << request->OrigClOrdID
		<< comma << request->SimpleSecurityID
		<< comma << request->StrategyID
		<< comma << request->StrategyTriggerSeqNo
		<< comma << request->TargetPartyIDSessionID;

}

void Logger::LogDeleteLeanResponse(DeleteOrderNRResponseT* response)
{
	LOG_INFO << DELETEORDERNRRESPONSET << response->MessageHeaderOut.TemplateID
		<< comma << response->NRResponseHeaderME.RequestTime
		<< comma << response->NRResponseHeaderME.Reserve1
		<< comma << response->NRResponseHeaderME.Reserve2
		<< comma << response->NRResponseHeaderME.Reserve3
		<< comma << response->NRResponseHeaderME.SendingTime
		<< comma << response->ClOrdID
		<< comma << response->CumQty / FIN::qty_multipler
		<< comma << response->CxlQty / FIN::qty_multipler
		<< comma << response->ExecID
		<< comma << response->ExecRestatementReason
		<< comma << response->ExecType[0]
		<< comma << response->OrderID
		<< comma << response->OrdStatus[0]
		<< comma << response->OrigClOrdID
		<< comma << response->SecurityID
		<< comma << (unsigned)response->TransactionDelayIndicator;
}

void Logger::LogDeleteStandardResponse(DeleteOrderResponseT* response)
{
	LOG_INFO << DELETEORDERRESPONSE_T << response->MessageHeaderOut.TemplateID
		<< comma << response->ResponseHeaderME.RequestTime
		<< comma << response->ResponseHeaderME.Reserve1
		<< comma << response->ResponseHeaderME.Reserve2
		<< comma << response->ResponseHeaderME.Reserve3
		<< comma << response->ResponseHeaderME.SendingTime
		<< comma << response->ResponseHeaderME.PartitionID
		<< comma << response->ClOrdID
		<< comma << response->CumQty / FIN::qty_multipler
		<< comma << response->CxlQty / FIN::qty_multipler
		<< comma << response->ExecID
		<< comma << response->ExecRestatementReason
		<< comma << response->ExecType[0]
		<< comma << response->OrderID
		<< comma << response->OrdStatus[0]
		<< comma << response->OrigClOrdID
		<< comma << response->SecurityID;
}

void Logger::LogOrderExecResponseT(OrderExecResponseT* response)
{
	std::ostringstream res;

	res << ORDEREXECRESPONSE_T
		<< comma << response->MessageHeaderOut.TemplateID
		<< comma << response->ResponseHeaderME.PartitionID
		<< comma << response->ResponseHeaderME.RequestTime
		<< comma << response->ResponseHeaderME.SendingTime
		<< comma << response->ClOrdID
		<< comma << response->CumQty / FIN::qty_multipler
		<< comma << response->CxlQty / FIN::qty_multipler
		<< comma << response->DisclosedQty / FIN::qty_multipler
		<< comma << response->ExecID
		<< comma << response->ExecRestatementReason
		<< comma << response->ExecType[0]
		<< comma << response->LeavesQty / FIN::qty_multipler
		<< comma << response->MarketSegmentID
		<< comma << response->OrderID
		<< comma << response->SecurityID
		<< comma << response->OrigClOrdID
		<< comma << (unsigned)response->Side
		<< comma << response->StrategyID
		<< comma << response->StrategySequenceNumber
		<< comma << response->TrdRegTSEntryTime
		<< comma << (unsigned)response->NoFills;

	for (int i = 0; i < response->NoFills; i++)
	{
		res << comma << response->FillsGrp[i].FillExecID
			<< comma << response->FillsGrp[i].FillMatchID
			<< comma << response->FillsGrp[i].FillPx / FIN::price_multiplier
			<< comma << response->FillsGrp[i].FillQty / FIN::qty_multipler;
	}

	LOG_INFO << res.str();
	//LOG_INFO << ORDEREXECRESPONSE_T << response.MessageHeaderOut.TemplateID << " "
	//	<< "ClOrdID =" << response.ClOrdID << " "
	//	<< "CumQty =" << response.CumQty / FIN::qty_multipler << " "
	//	<< "CxlQty =" << response.CxlQty / FIN::qty_multipler << " "
	//	<< "DisclosedQty =" << response.DisclosedQty / FIN::qty_multipler << " "
	//	<< "ExecID =" << response.ExecID << " "
	//	<< "ExecRestatementReason =" << response.ExecRestatementReason << " "
	//	<< "ExecType =" << response.ExecType << " "
	//	<< "LeavesQty =" << response.LeavesQty / FIN::qty_multipler << " "
	//	<< "MarketSegmentID=" << response.MarketSegmentID << " "
	//	<< "OrderID =" << response.OrderID << " "
	//	<< "SecurityID =" << response.SecurityID << " "
	//	<< "OrigClOrdID =" << response.OrigClOrdID << " "
	//	<< "Side =" << (unsigned)response.Side << " "
	//	<< "StrategyID =" << response.StrategyID << " "
	//	<< "StrategySequenceNumber =" << response.StrategySequenceNumber << " "
	//	<< "TrdRegTSEntryTime =" << response.TrdRegTSEntryTime << " "
	//	<< "NoFills =" << (unsigned)response.NoFills << " ";
	//for (int i = 0; i < response.NoFills; i++)
	//{
	//	LOG_INFO << "FillExecID =" << response.FillsGrp[i].FillExecID << " "
	//		<< "FillMatchID =" << response.FillsGrp[i].FillMatchID << " "
	//		<< "FillPx =" << response.FillsGrp[i].FillPx / FIN::price_multiplier << " "
	//		<< "FillQty =" << response.FillsGrp[i].FillQty / FIN::qty_multipler << " ";
	//}

}

void Logger::LogOrderExecNotificationT(OrderExecNotificationT* response)
{

	std::ostringstream res;

	res << ORDEREXECNOTIFICATION_T
		<< comma << response->MessageHeaderOut.TemplateID
		<< comma << response->RBCHeaderME.PartitionID
		<< comma << response->RBCHeaderME.SendingTime

		<< comma << response->ClOrdID
		<< comma << FQ << response->CumQty / FIN::qty_multipler
		<< comma << CQ << response->CxlQty / FIN::qty_multipler
		<< comma << response->ExecID
		<< comma << response->ExecRestatementReason
		<< comma << response->ExecType[0]
		<< comma << LQ << response->LeavesQty / FIN::qty_multipler
		<< comma << response->MarketSegmentID
		<< comma << response->OrderID
		<< comma << response->SecurityID
		<< comma << response->OrigClOrdID
		<< comma << (unsigned)response->Side
		<< comma << response->StrategyID
		<< comma << response->StrategySequenceNumber
		<< comma << response->TerminalInfo
		<< comma << (unsigned)response->NoFills;

	for (int i = 0; i < response->NoFills; i++)
	{

		res << " [ FILL ] " << comma << response->FillsGrp[i].FillExecID
			<< comma << response->FillsGrp[i].FillMatchID
			<< comma << response->FillsGrp[i].FillPx / FIN::price_multiplier
			<< comma << response->FillsGrp[i].FillQty / FIN::qty_multipler;
	}

	LOG_INFO << res.str();
	/*LOG_INFO << " Trade = TemplateID = " << response.MessageHeaderOut.TemplateID << " "
		<< "ClOrdID =" << response.ClOrdID << " "
		<< "CumQty =" << response.CumQty / FIN::qty_multipler << " "
		<< "CxlQty =" << response.CxlQty / FIN::qty_multipler << " "
		<< "ExecID =" << response.ExecID << " "
		<< "ExecRestatementReason =" << response.ExecRestatementReason << " "
		<< "ExecType =" << response.ExecType << " "
		<< "LeavesQty =" << response.LeavesQty / FIN::qty_multipler << " "
		<< "MarketSegmentID =" << response.MarketSegmentID << " "
		<< "OrderID =" << response.OrderID << " "
		<< "SecurityID =" << response.SecurityID << " "
		<< "OrigClOrdID =" << response.OrigClOrdID << " "
		<< "Side =" << (unsigned)response.Side << " "
		<< "StrategyID =" << response.StrategyID << " "
		<< "StrategySequenceNumber =" << response.StrategySequenceNumber << " "
		<< "TerminalInfo =" << response.TerminalInfo << " "
		<< "NoFills =" << (unsigned)response.NoFills << " ";
	for (int i = 0; i < response.NoFills; i++)
	{
		LOG_INFO << "FillExecID =" << response.FillsGrp[i].FillExecID << " "
			<< "FillMatchID =" << response.FillsGrp[i].FillMatchID << " "
			<< "FillPx =" << response.FillsGrp[i].FillPx / FIN::price_multiplier << " "
			<< "FillQty =" << response.FillsGrp[i].FillQty / FIN::qty_multipler << " ";
	}*/
}

void Logger::LogOrderExecReportBroadcastT(OrderExecReportBroadcastT* response)
{

	std::ostringstream res;


	res << ORDEREXECREPORTBROADCAST_T
		<< comma << response->MessageHeaderOut.TemplateID
		<< comma << response->RBCHeaderME.SendingTime
		<< comma << response->RBCHeaderME.PartitionID
		<< comma << response->ClOrdID
		<< comma << FQ << response->CumQty / FIN::qty_multipler
		<< comma << CQ << response->CxlQty / FIN::qty_multipler
		<< comma << response->ExecID
		<< comma << response->ExecRestatementReason
		<< comma << response->ExecType[0]
		<< comma << LQ << response->LeavesQty / FIN::qty_multipler
		<< comma << response->MarketSegmentID
		<< comma << response->OrderID
		<< comma << response->SecurityID
		<< comma << response->OrigClOrdID
		<< comma << (unsigned)response->Side
		<< comma << response->StrategyID
		<< comma << response->StrategySequenceNumber
		<< comma << response->TerminalInfo
		<< comma << (unsigned)response->ApplSeqIndicator
		<< comma << response->Account
		<< comma << (unsigned)response->AccountType
		<< comma << response->CustOrderHandlingInst
		<< comma << (unsigned)response->ExecInst
		<< comma << (unsigned)response->NoFills;
	for (int i = 0; i < response->NoFills; i++)
	{
		res << "[ FILL" << comma
			<< comma << response->FillsGrp[i].FillExecID
			<< comma << response->FillsGrp[i].FillMatchID
			<< comma << response->FillsGrp[i].FillPx / FIN::price_multiplier
			<< comma << response->FillsGrp[i].FillQty / FIN::qty_multipler << " ]";
	}
	LOG_INFO << res.str();


	/*LOG_INFO << "TemplateID = " << response.MessageHeaderOut.TemplateID << " "
		<< "ClOrdID =" << response.ClOrdID << " "
		<< "CumQty =" << response.CumQty / FIN::qty_multipler << " "
		<< "CxlQty =" << response.CxlQty / FIN::qty_multipler << " "
		<< "ExecID =" << response.ExecID << " "
		<< "ExecRestatementReason =" << response.ExecRestatementReason << " "
		<< "ExecType =" << response.ExecType << " "
		<< "LeavesQty =" << response.LeavesQty / FIN::qty_multipler << " "
		<< "MarketSegmentID =" << response.MarketSegmentID << " "
		<< "OrderID =" << response.OrderID << " "
		<< "SecurityID =" << response.SecurityID << " "
		<< "OrigClOrdID =" << response.OrigClOrdID << " "
		<< "Side =" << (unsigned)response.Side << " "
		<< "StrategyID =" << response.StrategyID << " "
		<< "StrategySequenceNumber =" << response.StrategySequenceNumber << " "
		<< "TerminalInfo =" << response.TerminalInfo << " "
		<< "ApplSeqIndicator =" << (unsigned)response.ApplSeqIndicator << " "
		<< "Account =" << response.Account << " "
		<< "AccountType =" << (unsigned)response.AccountType << " "
		<< "CustOrderHandlingInst =" << response.CustOrderHandlingInst << " "
		<< "ExecInst=:" << (unsigned)response.ExecInst << " "

		<< "NoFills =" << (unsigned)response.NoFills << " ";
	for (int i = 0; i < response.NoFills; i++)
	{
		LOG_INFO << "FillExecID =" << response.FillsGrp[i].FillExecID << " "
			<< "FillMatchID =" << response.FillsGrp[i].FillMatchID << " "
			<< "FillPx =" << response.FillsGrp[i].FillPx / FIN::price_multiplier << " "
			<< "FillQty =" << response.FillsGrp[i].FillQty / FIN::qty_multipler << " ";
	}*/
}

void Logger::LogTradeBroadcastT(TradeBroadcastT* response)
{
	LOG_INFO << TRADEBROADCAST_T << response->MessageHeaderOut.TemplateID
		<< comma << response->RBCHeader.ApplSeqNum
		<< comma << response->RBCHeader.PartitionID
		<< comma << response->Account
		<< comma << (unsigned)response->AccountType
		<< comma << (unsigned)response->AggressorIndicator
		<< comma << response->ClearingTradePrice
		<< comma << CLORDER_ID << response->ClOrdID
		<< comma << FILL_QTY << response->CumQty / FIN::qty_multipler
		<< comma << response->CustOrderHandlingInst
		<< comma << response->Echo
		<< comma << LEAVE_QTY << response->LeavesQty / FIN::qty_multipler
		<< comma << response->MarketSegmentID
		<< comma << response->MatchDate
		<< comma << (unsigned)response->MatchSubType
		<< comma << (unsigned)response->MatchType
		<< comma << response->OrderCategory
		<< comma << ORDER_ID << response->OrderID
		<< comma << response->CPCode
		<< comma << SIDE << (unsigned)response->Side
		<< comma << LAST_PX << response->SideLastPx / FIN::price_multiplier
		<< comma << LAST_QTY << response->LastQty / FIN::qty_multipler
		<< comma << SIDE << (unsigned)response->OrderSide
		<< comma << ORDER_TYPE << (unsigned)response->OrdType
		<< comma << TRADE_ID << response->TradeID
		<< comma << response->PositionEffect
		<< comma << response->Price / FIN::price_multiplier
		<< comma << response->OrigTradeID
		<< comma << response->RelatedSecurityID
		<< comma << response->TerminalInfo
		<< comma << response->SimpleSecurityID;
}

void Logger::LogRejectT(RejectT* rejRes)
{
	std::string error_msg(rejRes->VarText, rejRes->VarTextLen);
	LOG_INFO << REJECT_T << rejRes->MessageHeaderOut.TemplateID
		<< comma << rejRes->SessionRejectReason
		<< comma << (unsigned)rejRes->SessionStatus
		<< comma << error_msg
		<< comma << rejRes->NRResponseHeaderME.SendingTime;
}