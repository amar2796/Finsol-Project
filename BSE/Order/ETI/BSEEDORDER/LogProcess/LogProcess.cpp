#include "LogProcess.h"

void LogProcess::logPrint(NewOrderNRResponseT* msg)
{
	LOG_INFO << NEWORDERNRRESPONSE_T 
		<< msg->NRResponseHeaderME.getString()
		<< comma << msg->MessageHeaderOut.TemplateID
		<< comma << CLORDER_ID << msg->ClOrdID
		<< comma << msg->ExecID
		<< comma << msg->ExecRestatementReason
		<< comma << EXE_STATUS << msg->ExecType[0]
		<< comma << ORDER_ID << msg->OrderID
		<< comma << ORDER_STATUS << msg->OrdStatus[0]
		<< comma << (unsigned)msg->ProductComplex
		<< comma << msg->SecurityID
		<< comma << (double)msg->PriceMkToLimitPx / FIN::price_multiplier;
}

void LogProcess::logPrint(ModifyOrderNRResponseT* response)
{
	LOG_INFO << MODIFYORDERNRRESPONSET 
		<< response->NRResponseHeaderME.getString()
		<< comma << response->MessageHeaderOut.TemplateID
		<< comma << CLORDER_ID << response->ClOrdID
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
		<< comma << (double)response->PriceMkToLimitPx / FIN::price_multiplier;
}

void LogProcess::logPrint(LogonRequestT request)
{
	LOG_INFO << LOGONREQUEST
		<< request.MessageHeaderIn.TemplateID
		<< comma << request.ApplicationSystemName
		<< comma << request.ApplicationSystemVendor
		<< comma << request.ApplicationSystemVersion
		<< comma << request.ApplUsageOrders
		<< comma << request.ApplUsageQuotes
		<< comma << request.DefaultCstmApplVerID
		<< comma << request.FIXEngineName
		<< comma << request.FIXEngineVendor
		<< comma << request.HeartBtInt
		<< comma << request.PartyIDSessionID
		<< comma << std::string(request.Password, LEN_PASSWORD);
}

void LogProcess::logPrint(LogonResponseT* response)
{
	LOG_INFO << LOGONRESPONSE_T << response->MessageHeaderOut.TemplateID
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

void LogProcess::logPrint(GatewayRequestT request)
{

	LOG_INFO << GATEWAYREQUEST_T
		<< comma << request.MessageHeaderIn.BodyLen
		<< comma << request.MessageHeaderIn.NetworkMsgID
		<< comma << request.MessageHeaderIn.Pad2
		<< comma << request.MessageHeaderIn.TemplateID
		<< comma << request.RequestHeader.MsgSeqNum
		<< comma << request.RequestHeader.SenderSubID
		<< comma << request.DefaultCstmApplVerID
		<< comma << request.PartyIDSessionID
		<< comma << std::string(request.Password, LEN_PASSWORD);
}

void LogProcess::logPrint(SessionRegistrationRequestT msg)
{
	LOG_INFO << SESSIONREGISTRATIONREQUEST_T
		<< comma << msg.MessageHeaderIn.BodyLen
		<< comma << msg.MessageHeaderIn.NetworkMsgID
		<< comma << msg.MessageHeaderIn.Pad2
		<< comma << msg.MessageHeaderIn.TemplateID
		<< comma << msg.RequestHeader.MsgSeqNum
		<< comma << msg.RequestHeader.SenderSubID
		<< comma << msg.PartyIDSessionID;

}

void LogProcess::logPrint(GatewayResponseT* resp)
{
	LOG_INFO << GATEWAYRESPONSE_T
		<< resp->GatewayID
		<< comma << resp->GatewaySubID
		<< comma << resp->Pad6
		<< comma << resp->SecondaryGatewayID
		<< comma << resp->SecondaryGatewaySubID
		<< comma << (unsigned)resp->SessionMode
		<< comma << (unsigned)resp->TradSesMode;
}

void LogProcess::logPrint(UserLoginRequestT req)
{
	LOG_INFO << USERLOGINREQUEST_T << req.MessageHeaderIn.TemplateID
		<< comma << req.Username
		<< comma << std::string(req.Password, LEN_PASSWORD);
}

void LogProcess::logPrint(SessionPasswordChangeRequestT msg)
{
	LOG_INFO << SESSIONPASSWORDCHANGEREQUEST_t
		<< comma << msg.MessageHeaderIn.BodyLen
		<< comma << msg.MessageHeaderIn.NetworkMsgID
		<< comma << msg.MessageHeaderIn.Pad2
		<< comma << msg.MessageHeaderIn.TemplateID
		<< comma << msg.RequestHeader.MsgSeqNum
		<< comma << msg.RequestHeader.SenderSubID
		<< comma << msg.PartyIDSessionID
		<< comma << std::string(msg.Password, LEN_PASSWORD)
		<< comma << std::string(msg.NewPassword, LEN_NEW_PASSWORD);
}

void LogProcess::logPrint(UserPasswordChangeRequestT msg)
{
	LOG_INFO << USERPASSWORDCHANGEREQUEST_T
		<< comma << msg.MessageHeaderIn.BodyLen
		<< comma << msg.MessageHeaderIn.NetworkMsgID
		<< comma << msg.MessageHeaderIn.Pad2
		<< comma << msg.MessageHeaderIn.TemplateID
		<< comma << msg.RequestHeader.MsgSeqNum
		<< comma << msg.RequestHeader.SenderSubID
		<< comma << msg.Username
		<< comma << std::string(msg.Password, LEN_PASSWORD)
		<< comma << std::string(msg.NewPassword, LEN_NEW_PASSWORD);
}

void LogProcess::logPrint(UserLoginResponseT* res)
{
	LOG_INFO << USERLOGINRESPONSE_T << res->MessageHeaderOut.TemplateID
		<< comma << (unsigned)res->DaysLeftForPasswdExpiry
		<< comma << (unsigned)res->GraceLoginsLeft
		<< comma << res->LastLoginTime;

}

void LogProcess::logPrint(NewOrderSingleRequestT* newOrder)
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
		<< comma << (unsigned)newOrder->ExecInst
		<< comma << newOrder->ExpireDate
		<< comma << newOrder->MarketSegmentID
		<< comma << ORDER_QUANTITY << newOrder->OrderQty / FIN::qty_multipler
		<< comma << ORDER_TYPE << unsigned(newOrder->OrdType)
		<< comma << newOrder->PositionEffect
		<< comma << PRICE_TAG << ((double)(newOrder->Price)) / FIN::price_multiplier
		<< comma << (unsigned)newOrder->PriceValidityCheckType
		<< comma << newOrder->RegulatoryID
		<< comma << SIDE << (unsigned)newOrder->Side
		<< comma << newOrder->SimpleSecurityID
		<< comma << STOP_PRICE << newOrder->StopPx
		<< comma << TIME_IN_FORCE << (unsigned)newOrder->TimeInForce
		<< comma << (unsigned)newOrder->TradingCapacity
		<< comma << (unsigned)newOrder->TradingSessionSubID;
}

void LogProcess::logPrint(ModifyOrderSingleRequestT* request)
{
	LOG_INFO << MODIFYORDERSINGLEREQUEST_T
		<< comma << request->Account
		<< comma << (unsigned)request->AccountType
		<< comma << (unsigned)request->ApplSeqIndicator
		<< comma << request->ClOrdID
		<< comma << request->CPCode
		<< comma << request->CustOrderHandlingInst
		<< comma << (unsigned)request->ExecInst
		<< comma << request->ExpireDate
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
		<< comma << request->TargetPartyIDSessionID
		<< comma << (unsigned)request->TimeInForce
		<< comma << (unsigned)request->TradingCapacity
		<< comma << (unsigned)request->TradingSessionSubID;
}

void LogProcess::logPrint(DeleteOrderSingleRequestT* request)
{
	LOG_INFO << DELETEORDERSINGLEREQUEST_T << request->MessageHeaderIn.TemplateID
		<< comma << request->OrderID
		<< comma << request->ClOrdID
		<< comma << request->OrigClOrdID
		<< comma << request->MessageTag
		<< comma << request->MarketSegmentID
		<< comma << request->SimpleSecurityID
		<< comma << request->TargetPartyIDSessionID
		<< comma << request->RegulatoryID;
}

void LogProcess::logPrint(DeleteOrderNRResponseT* response)
{
	LOG_INFO << DELETEORDERNRRESPONSET 
		<< response->NRResponseHeaderME.getString()
		<< comma << response->MessageHeaderOut.TemplateID
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

void LogProcess::logPrint(OrderExecReportBroadcastT* response)
{

	std::ostringstream res;


	res << ORDEREXECREPORTBROADCAST_T
		<< comma << response->MessageHeaderOut.TemplateID
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


}

void LogProcess::logPrint(RejectT* rejRes)
{
	LOG_INFO << REJECT_T << rejRes->MessageHeaderOut.TemplateID
		<< comma << rejRes->SessionRejectReason
		<< comma << (unsigned)rejRes->SessionStatus
		<< comma << std::string(rejRes->VarText, rejRes->VarTextLen)
		<< comma << rejRes->NRResponseHeaderME.SendingTime;
}

void LogProcess::logPrint(SessionRegistrationResponseT* msg)
{
	std::string error_msg(msg->VarText, msg->VarTextLen);
	LOG_INFO << SESSIONREGISTRATIONRESPONSET << msg->MessageHeaderOut.TemplateID
		<< comma << msg->Status[0]
		<< comma << msg->Pad1[0]
		<< comma << error_msg;
}

void LogProcess::logPrint(TradingSessionStatusBroadcastT* msg)
{
	LOG_INFO << TRADINGSESSIONSTATUSBROADCASTT << msg->MessageHeaderOut.TemplateID
		<< comma << msg->MarketSegmentID
		<< comma << msg->TradeDate
		<< comma << msg->TradSesEvent;
}

void LogProcess::logPrint(ForcedLogoutNotificationT* msg)
{
	std::string error_msg(msg->VarText, msg->VarTextLen);
	LOG_INFO << FORCEDLOGOUTNOTIFICATIONT << msg->MessageHeaderOut.TemplateID
		<< comma << error_msg;
}

void LogProcess::logPrint(GwOrderAcknowledgementT* msg)
{
	LOG_INFO << GWORDERACHNOLEDGEMENTT << msg->MessageHeaderOut.TemplateID
		<< comma << msg->PrimaryOrderID
		<< comma << msg->ClOrdID
		<< comma << msg->MessageTag;
}

void LogProcess::logPrint(OMSResponse* msg)
{
	std::ostringstream res;
	res << OMS_TRADE
		<< comma << MSG_TYPE << msg->header.TemplateID
		<< comma << ORDER_ID << msg->OrderID
		<< comma << CLORDER_ID << msg->ClOrdID
		<< comma << ORG_ID << msg->OrigClOrdID
		<< comma << LEAVE_QTY << msg->LeavesQty / FIN::qty_multipler
		<< comma << FILL_QTY << msg->CumQty / FIN::qty_multipler
		<< comma << CXL_QTY << msg->CxlQty / FIN::qty_multipler
		<< comma << ORDER_STATUS << msg->OrdStatus[0]
		<< comma << EXE_STATUS << msg->ExecType[0]
		<< comma << LAST_PX << msg->FillPx / FIN::price_multiplier
		<< comma << LAST_QTY << msg->FillQty / FIN::qty_multipler
		<< comma << TRADE_ID << msg->FillExecID;
	LOG_INFO << res.str();
}

void LogProcess::logPrint(OMSRequest* msg)
{
	std::ostringstream res;
	res << OMS_REQUEST
		<< comma << ORDER_ID << msg->OrderID
		<< comma << ORG_ID << msg->OrigClOrdID
		<< comma << CLORDER_ID << msg->ClOrdID
		<< comma << PRICE_TAG << msg->Price
		<< comma << STOP_PRICE << msg->StopPx
		<< comma << SECURITY_ID << msg->SimpleSecurityID
		<< comma << ORDER_QUANTITY << msg->OrderQty
		<< comma << SIDE << msg->Side
		<< comma << ORDER_TYPE << msg->OrdType
		<< comma << TIME_IN_FORCE << msg->TimeInForce;
	LOG_INFO << res.str();
}