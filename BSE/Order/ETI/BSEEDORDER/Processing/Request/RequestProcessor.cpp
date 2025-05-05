#include "RequestProcessor.h"

Request_Processor::Request_Processor()
{
}

Request_Processor::Request_Processor(SessionSettings* conf)
{
	sequence_number = 0;
	confDetails = conf;
	//req_struct = {};
}

Request_Processor::~Request_Processor()
{

}

Request_Structure Request_Processor::processRequest(OMSRequest* msg, uint32_t seq)
{
	try
	{
		sequence_number = seq;
		
		
		//OMSRequest* msg = (OMSRequest*)data;
		/*auto start = std::chrono::high_resolution_clock::now();
		log->logPrint(msg);
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		std::cout << "Time taken by logPrint: " << duration << " microseconds" << std::endl;*/

		//parser.loadString(req);

		switch (msg->msgType)
		{
		case FIN::Fix_MsgType_NEW_ORDER:
			processNewRequest(msg);
			break;

		case FIN::Fix_MsgType_MODIFY_ORDER:
			processModifyRequest(msg);
			break;

		case FIN::Fix_MsgType_CANCEL_ORDER:
			processCancleRequest(msg);
			break;


		default:
			break;
		}


	}
	catch (std::exception& ex) {

	}
	
	return req_struct;
}

void Request_Processor::processNewRequest(OMSRequest* msg)
{
	try
	{
		req_struct.requestType = 0;

		//REQUEST HEADER
		req_struct.OrderReq.RequestHeader.MsgSeqNum = sequence_number;
		req_struct.OrderReq.RequestHeader.SenderSubID = confDetails->Username;

		//Body Part
		req_struct.OrderReq.OrdType = msg->OrdType;

		if (req_struct.OrderReq.OrdType == 2)
		{
			req_struct.OrderReq.Price = msg->Price;
			req_struct.OrderReq.StopPx = 0x8000000000000000;
		}
		else if (req_struct.OrderReq.OrdType == 4)
		{
			req_struct.OrderReq.Price = msg->Price;
			req_struct.OrderReq.StopPx = msg->StopPx;
		}
		else
		{
			req_struct.OrderReq.StopPx = 0x8000000000000000;
			req_struct.OrderReq.Price = 0x8000000000000000;
		}
		req_struct.OrderReq.SenderLocationID = confDetails->SenderLocationId;
		req_struct.OrderReq.ClOrdID = msg->ClOrdID;
		req_struct.OrderReq.MessageTag = (int32_t)sequence_number;
		req_struct.OrderReq.OrderQty = msg->OrderQty;
		req_struct.OrderReq.SimpleSecurityID = msg->SimpleSecurityID;

		req_struct.OrderReq.Side = msg->Side;

		auto timeInforce = msg->TimeInForce;

		if (timeInforce == 0)
			req_struct.OrderReq.TimeInForce = 7;
		else
			req_struct.OrderReq.TimeInForce = msg->TimeInForce;

		strcpy(req_struct.OrderReq.AlgoID, confDetails->algoId.c_str());

		std::string ClientCode(msg->ClientCode);
		//auto ctcl = ClientCode;													//ClientCode
		if (ClientCode == Fix_Account_OWN || ClientCode == Fix_Account_PRO)
		{
			req_struct.OrderReq.AccountType = FIN::OWN;
			memcpy(req_struct.OrderReq.ClientCode, OWN_FREE_TEXT1.c_str(), OWN_FREE_TEXT1.length());
		}
		else
		{
			if (custodian_reader.custodian.find(ClientCode) == custodian_reader.custodian.end()) {
				req_struct.OrderReq.AccountType = FIN::CLIENT;

			}
			else {
				auto paticipant = custodian_reader.custodian[ClientCode];
				req_struct.OrderReq.AccountType = FIN::CLIENT;
				memcpy(req_struct.OrderReq.CPCode, paticipant.c_str(), paticipant.length());

			}
			memcpy(req_struct.OrderReq.ClientCode, ClientCode.c_str(), ClientCode.length());

		}


		//res.request_clorderID[sequence_number] = req_struct.OrderReq.ClOrdID;
		res.request_clorderID_exchangeOrderID[sequence_number].first = req_struct.OrderReq.ClOrdID;
	}
	catch (std::exception& ex) {

	}
}

void Request_Processor::processModifyRequest(OMSRequest* msg)
{
	try
	{
		req_struct.requestType = 1;

		//Req Header
		req_struct.ReplaceReq.RequestHeader.MsgSeqNum = sequence_number;
		req_struct.ReplaceReq.RequestHeader.SenderSubID = confDetails->Username;
		
		// Body Start
		req_struct.ReplaceReq.OrderID = msg->OrderID;
		req_struct.ReplaceReq.ClOrdID = msg->ClOrdID;
		req_struct.ReplaceReq.OrigClOrdID = msg->OrigClOrdID;


		req_struct.ReplaceReq.OrdType = msg->OrdType;

		if (req_struct.ReplaceReq.OrdType == 2)
		{
			req_struct.ReplaceReq.Price = msg->Price;
			req_struct.ReplaceReq.StopPx = 0x8000000000000000;
		}
		else if (req_struct.ReplaceReq.OrdType == 4)
		{
			req_struct.ReplaceReq.Price = msg->Price;
			req_struct.ReplaceReq.StopPx = msg->StopPx;
		}
		else
		{
			req_struct.ReplaceReq.StopPx = 0x8000000000000000;
			req_struct.ReplaceReq.Price = 0x8000000000000000;
		}

		req_struct.ReplaceReq.SenderLocationID = confDetails->SenderLocationId;
		req_struct.ReplaceReq.ActivityTime = res.activity_time[req_struct.ReplaceReq.OrderID];

		req_struct.ReplaceReq.MessageTag = sequence_number;
		req_struct.ReplaceReq.OrderQty = msg->OrderQty;
		req_struct.ReplaceReq.SimpleSecurityID = msg->SimpleSecurityID;
		req_struct.ReplaceReq.TargetPartyIDSessionID = confDetails->SessionId;

		req_struct.ReplaceReq.Side = msg->Side;

		auto timeInforce = msg->TimeInForce;
		if (timeInforce == 0)
			req_struct.ReplaceReq.TimeInForce = 7;
		else
			req_struct.ReplaceReq.TimeInForce = msg->TimeInForce;

		strcpy(req_struct.ReplaceReq.AlgoID, confDetails->algoId.c_str());

		//auto ctcl = parser.get(Fix_Account);
		std::string ClientCode(msg->ClientCode);
		if (ClientCode == Fix_Account_OWN || ClientCode == Fix_Account_PRO)
		{
			req_struct.ReplaceReq.AccountType = FIN::OWN;
			memcpy(req_struct.ReplaceReq.ClientCode, OWN_FREE_TEXT1.c_str(), OWN_FREE_TEXT1.length());

		}
		else
		{
			if (custodian_reader.custodian.find(ClientCode) == custodian_reader.custodian.end()) {
				req_struct.ReplaceReq.AccountType = FIN::CLIENT;
			}
			else {
				auto paticipant = custodian_reader.custodian[ClientCode];
				req_struct.ReplaceReq.AccountType = FIN::CLIENT;
				memcpy(req_struct.ReplaceReq.CPCode, paticipant.c_str(), paticipant.length());
			}
			memcpy(req_struct.ReplaceReq.ClientCode, ClientCode.c_str(), ClientCode.length());
			//strcpy(req_struct.ReplaceReq.FreeText1, ctcl.c_str());

		}

		//	res.request_clorderID[sequence_number] = req_struct.ReplaceReq.OrderID;
		res.request_clorderID_exchangeOrderID[sequence_number].first = req_struct.ReplaceReq.ClOrdID;
		res.request_clorderID_exchangeOrderID[sequence_number].second = req_struct.ReplaceReq.OrderID;
	}
	catch (std::exception& ex) {

	}
}

void Request_Processor::processCancleRequest(OMSRequest* msg)
{
	try
	{
		req_struct.requestType = 2;

		//Request Header
		req_struct.CancelReq.RequestHeader.MsgSeqNum = sequence_number;
		req_struct.CancelReq.RequestHeader.SenderSubID = confDetails->Username;

		//Body part
		req_struct.CancelReq.OrderID = msg->OrderID;
		req_struct.CancelReq.ClOrdID = msg->ClOrdID;
		req_struct.CancelReq.OrigClOrdID = msg->OrigClOrdID;
		req_struct.CancelReq.ActivityTime = res.activity_time[req_struct.CancelReq.OrderID];
		req_struct.CancelReq.MessageTag = sequence_number;
		req_struct.CancelReq.SimpleSecurityID = msg->SimpleSecurityID;
		req_struct.CancelReq.TargetPartyIDSessionID = confDetails->SessionId;
		strcpy(req_struct.CancelReq.AlgoID, confDetails->algoId.c_str());

		res.request_clorderID_exchangeOrderID[sequence_number].first = req_struct.CancelReq.ClOrdID;
		res.request_clorderID_exchangeOrderID[sequence_number].second = req_struct.CancelReq.OrderID;
	}
	catch (std::exception& ex) {

	}
}



//void Request_Processor::processNewRequest()
//{
//	try
//	{
//		req_struct.requestType = 0;
//
//		//MSG HEADER 
//		req_struct.OrderReq.MessageHeaderIn.BodyLen = sizeof(NewOrderSingleRequestT);
//		req_struct.OrderReq.MessageHeaderIn.TemplateID = TID_NEWORDERSINGLEREQUEST;
//		strcpy(req_struct.OrderReq.MessageHeaderIn.NetworkMsgID, "");
//		strcpy(req_struct.OrderReq.MessageHeaderIn.Pad2, "");
//		//
//
//		//REQUEST HEADER
//		req_struct.OrderReq.RequestHeader.MsgSeqNum = sequence_number;
//		req_struct.OrderReq.RequestHeader.SenderSubID = confDetails->Username;
//		//
//
//		req_struct.OrderReq.OrdType = stoi(parser.get(Fix_OrdType));
//
//		if (req_struct.OrderReq.OrdType == 2)
//		{
//			req_struct.OrderReq.Price = stoll(parser.get(Fix_Price));
//			req_struct.OrderReq.StopPx = 0x8000000000000000;
//		}
//		else if (req_struct.OrderReq.OrdType == 4)
//		{
//			req_struct.OrderReq.Price = stoll(parser.get(Fix_Price));
//			req_struct.OrderReq.StopPx = stoll(parser.get(Fix_TriggerPrice));
//		}
//		else
//		{
//			req_struct.OrderReq.StopPx = 0x8000000000000000;
//			req_struct.OrderReq.Price = 0x8000000000000000;
//		}
//		req_struct.OrderReq.MaxPricePercentage = 0;
//		req_struct.OrderReq.SenderLocationID = confDetails->SenderLocationId;
//		req_struct.OrderReq.ClOrdID = (unsigned)stoll(parser.get(Fix_ClOrdID));
//		req_struct.OrderReq.Filler1 = 0xFFFFFFFFFFFFFFFF;
//		req_struct.OrderReq.Filler2 = 0xFFFFFFFF;
//		req_struct.OrderReq.MessageTag = (int32_t)sequence_number;
//		req_struct.OrderReq.OrderQty = stoi(parser.get(Fix_OrdQty));
//		req_struct.OrderReq.MaxShow = 0;
//		req_struct.OrderReq.ExpireDate = 0xFFFFFFFF;
//		req_struct.OrderReq.MarketSegmentID = 0x80000000;
//		req_struct.OrderReq.SimpleSecurityID = (unsigned)stoll(parser.get(Fix_SecurityID));
//		req_struct.OrderReq.RegulatoryID = 0xFFFFFFFF;
//		req_struct.OrderReq.Filler4 = 0xFFFF;
//		strcpy(req_struct.OrderReq.ContraBroker, "");
//		strcpy(req_struct.OrderReq.PartyIDOrderOriginationFirm, "");
//		strcpy(req_struct.OrderReq.PartyIDBeneficiary, "");
//		strcpy(req_struct.OrderReq.Account, A1_ACC.c_str());
//		strcpy(req_struct.OrderReq.CPCode, "");
//		strcpy(req_struct.OrderReq.FreeText3, "");
//		memset(req_struct.OrderReq.ClientCode, NULL, 12);
//
//
//		req_struct.OrderReq.ApplSeqIndicator = 0;
//		req_struct.OrderReq.Side = stoi(parser.get(Fix_Side));
//		req_struct.OrderReq.PriceValidityCheckType = 0;
//
//		auto timeInforce = stoi(parser.get(Fix_TimeInForce));
//
//		if (timeInforce == 0)
//			req_struct.OrderReq.TimeInForce = 7;
//		else
//			req_struct.OrderReq.TimeInForce = stoi(parser.get(Fix_TimeInForce));
//
//		//req_struct.OrderReq.TimeInForce = stoi(parser.get(Fix_TimeInForce));
//
//		req_struct.OrderReq.ExecInst = 2;
//		req_struct.OrderReq.STPCFlag = 1;
//		//req_struct.OrderReq.Filler5 = 0xFF;
//		req_struct.OrderReq.RolloverFlag = 0xFF;
//		req_struct.OrderReq.TradingSessionSubID = 0xFF;
//		req_struct.OrderReq.TradingCapacity = 1;
//
//		strcpy(req_struct.OrderReq.PositionEffect, "C");
//		strcpy(req_struct.OrderReq.PartyIDLocationID, "");
//		strcpy(req_struct.OrderReq.CustOrderHandlingInst, "");
//		strcpy(req_struct.OrderReq.RegulatoryText, "");
//		strcpy(req_struct.OrderReq.AlgoID, confDetails->algoId.c_str());
//
//		auto ctcl = parser.get(Fix_Account);													//ClientCode
//		if (ctcl == Fix_Account_OWN || ctcl == Fix_Account_PRO)
//		{
//			req_struct.OrderReq.AccountType = FIN::OWN;
//			memcpy(req_struct.OrderReq.ClientCode, OWN_FREE_TEXT1.c_str(), OWN_FREE_TEXT1.length());
//		}
//		else
//		{
//			if (custodian_reader.custodian.find(ctcl) == custodian_reader.custodian.end()) {
//				req_struct.OrderReq.AccountType = FIN::CLIENT;
//
//			}
//			else {
//				auto paticipant = custodian_reader.custodian[ctcl];
//				req_struct.OrderReq.AccountType = FIN::CLIENT;
//				memcpy(req_struct.OrderReq.CPCode, paticipant.c_str(), paticipant.length());
//
//			}
//			memcpy(req_struct.OrderReq.ClientCode, ctcl.c_str(), ctcl.length());
//
//		}
//
//
//		//res.request_clorderID[sequence_number] = req_struct.OrderReq.ClOrdID;
//		res.request_clorderID_exchangeOrderID[sequence_number].first = req_struct.OrderReq.ClOrdID;
//	}
//	catch (std::exception& ex) {
//
//	}
//}
//
//void Request_Processor::processModifyRequest()
//{
//	try
//	{
//		req_struct.requestType = 1;
//
//
//		// MSG Header
//		req_struct.ReplaceReq.MessageHeaderIn.BodyLen = sizeof(ModifyOrderSingleRequestT);
//		req_struct.ReplaceReq.MessageHeaderIn.TemplateID = TID_MODIFYORDERSINGLEREQUEST;
//		strcpy(req_struct.ReplaceReq.MessageHeaderIn.NetworkMsgID, "");
//		strcpy(req_struct.ReplaceReq.MessageHeaderIn.Pad2, "");
//		//
//
//		//Req Header
//		req_struct.ReplaceReq.RequestHeader.MsgSeqNum = sequence_number;
//		req_struct.ReplaceReq.RequestHeader.SenderSubID = confDetails->Username;
//		//
//
//		// Body Start
//		req_struct.ReplaceReq.OrderID = stoll(parser.get(Fix_OrderID));
//		req_struct.ReplaceReq.ClOrdID = stoll(parser.get(Fix_ClOrdID));
//		req_struct.ReplaceReq.OrigClOrdID = stoll(parser.get(Fix_OrigClOrdID));
//
//
//		req_struct.ReplaceReq.OrdType = stoi(parser.get(Fix_OrdType));
//
//		if (req_struct.ReplaceReq.OrdType == 2)
//		{
//			req_struct.ReplaceReq.Price = stoll(parser.get(Fix_Price));
//			req_struct.ReplaceReq.StopPx = 0x8000000000000000;
//		}
//		else if (req_struct.ReplaceReq.OrdType == 4)
//		{
//			req_struct.ReplaceReq.Price = stoll(parser.get(Fix_Price));
//			req_struct.ReplaceReq.StopPx = stoll(parser.get(Fix_TriggerPrice));
//		}
//		else
//		{
//			req_struct.ReplaceReq.StopPx = 0x8000000000000000;
//			req_struct.ReplaceReq.Price = 0x8000000000000000;
//		}
//
//		req_struct.ReplaceReq.MaxPricePercentage = 0;
//		req_struct.ReplaceReq.SenderLocationID = confDetails->SenderLocationId;
//		req_struct.ReplaceReq.ActivityTime = res.activity_time[req_struct.ReplaceReq.OrderID];
//		req_struct.ReplaceReq.Filler1 = 0xFFFFFFFFFFFFFFFF;
//		req_struct.ReplaceReq.Filler2 = 0xFFFFFFFF;
//
//		req_struct.ReplaceReq.MessageTag = sequence_number;
//		req_struct.ReplaceReq.OrderQty = stoi(parser.get(Fix_OrdQty));
//		req_struct.ReplaceReq.MaxShow = 0;
//		req_struct.ReplaceReq.ExpireDate = 0xFFFFFFFF;
//		req_struct.ReplaceReq.MarketSegmentID = 0x80000000;
//		req_struct.ReplaceReq.SimpleSecurityID = stoll(parser.get(Fix_SecurityID));
//		req_struct.ReplaceReq.TargetPartyIDSessionID = confDetails->SessionId;
//		req_struct.ReplaceReq.RegulatoryID = 0;
//
//		req_struct.ReplaceReq.Filler4 = 0xFFFF;
//
//
//		strcpy(req_struct.ReplaceReq.PartyIDTakeUpTradingFirm, "");
//		strcpy(req_struct.ReplaceReq.PartyIDOrderOriginationFirm, "");
//		strcpy(req_struct.ReplaceReq.PartyIDBeneficiary, "");
//		strcpy(req_struct.ReplaceReq.Account, FIN::A1_ACC.c_str());
//
//		strcpy(req_struct.ReplaceReq.CPCode, "");
//		strcpy(req_struct.ReplaceReq.FreeText3, "");
//		strcpy(req_struct.ReplaceReq.ClientCode, "");
//		req_struct.ReplaceReq.ApplSeqIndicator = 0;//stoi(b.ApplSeqIndicator);
//		req_struct.ReplaceReq.Side = stoi(parser.get(Fix_Side));
//		req_struct.ReplaceReq.PriceValidityCheckType = 0;// stoi(b.PriceValidityCheckType);
//
//		auto timeInforce = stoi(parser.get(Fix_TimeInForce));
//		if (timeInforce == 0)
//			req_struct.ReplaceReq.TimeInForce = 7;
//		else
//			req_struct.ReplaceReq.TimeInForce = stoi(parser.get(Fix_TimeInForce));
//
//		//req_struct.ReplaceReq.TimeInForce = stoi(parser.get(Fix_TimeInForce));
//		req_struct.ReplaceReq.ExecInst = 2;//stoi(b.ExecInst);
//		//req_struct.ReplaceReq.Filler5 = 0xFF;
//		req_struct.ReplaceReq.RolloverFlag = 0xFF;
//		req_struct.ReplaceReq.TradingSessionSubID = 0xFF;
//		req_struct.ReplaceReq.TradingCapacity = 1;//stoi(b.TradingCapacity);
//		strcpy(req_struct.ReplaceReq.DeltaQtyFlag, "");
//		req_struct.ReplaceReq.PositionEffect[0] = 'C';
//		strcpy(req_struct.ReplaceReq.PartyIDLocationID, "");
//		strcpy(req_struct.ReplaceReq.CustOrderHandlingInst, "");
//		//strcpy(req_struct.ReplaceReq.CustOrderHandlingInst, "");
//		strcpy(req_struct.ReplaceReq.RegulatoryText, "");
//		strcpy(req_struct.ReplaceReq.AlgoID, confDetails->algoId.c_str());
//
//		auto ctcl = parser.get(Fix_Account);
//		if (ctcl == Fix_Account_OWN || ctcl == Fix_Account_PRO)
//		{
//			req_struct.ReplaceReq.AccountType = FIN::OWN;
//			memcpy(req_struct.ReplaceReq.ClientCode, OWN_FREE_TEXT1.c_str(), OWN_FREE_TEXT1.length());
//
//		}
//		else
//		{
//			if (custodian_reader.custodian.find(ctcl) == custodian_reader.custodian.end()) {
//				req_struct.ReplaceReq.AccountType = FIN::CLIENT;
//			}
//			else {
//				auto paticipant = custodian_reader.custodian[ctcl];
//				req_struct.ReplaceReq.AccountType = FIN::CLIENT;
//				memcpy(req_struct.ReplaceReq.CPCode, paticipant.c_str(), paticipant.length());
//			}
//			memcpy(req_struct.ReplaceReq.ClientCode, ctcl.c_str(), ctcl.length());
//			//strcpy(req_struct.ReplaceReq.FreeText1, ctcl.c_str());
//
//		}
//
//
//		strcpy(req_struct.ReplaceReq.Pad4, "");
//
//		//	res.request_clorderID[sequence_number] = req_struct.ReplaceReq.OrderID;
//		res.request_clorderID_exchangeOrderID[sequence_number].first = req_struct.ReplaceReq.ClOrdID;
//		res.request_clorderID_exchangeOrderID[sequence_number].second = req_struct.ReplaceReq.OrderID;
//	}
//	catch (std::exception& ex) {
//
//	}
//}
//
//void Request_Processor::processCancleRequest()
//{
//	try
//	{
//		req_struct.requestType = 2;
//
//
//		//MSG Header
//		req_struct.CancelReq.MessageHeaderIn.BodyLen = sizeof(DeleteOrderSingleRequestT);
//		req_struct.CancelReq.MessageHeaderIn.TemplateID = TID_DELETEORDERSINGLEREQUEST;
//		strcpy(req_struct.CancelReq.MessageHeaderIn.NetworkMsgID, "");
//		strcpy(req_struct.CancelReq.MessageHeaderIn.Pad2, "");
//
//		//Request Header
//		req_struct.CancelReq.RequestHeader.MsgSeqNum = sequence_number;
//		req_struct.CancelReq.RequestHeader.SenderSubID = confDetails->Username;
//
//		//Body part
//		req_struct.CancelReq.OrderID = stoll(parser.get(Fix_OrderID));
//		req_struct.CancelReq.ClOrdID = stoll(parser.get(Fix_ClOrdID));
//		req_struct.CancelReq.OrigClOrdID = stoll(parser.get(Fix_OrigClOrdID));
//		req_struct.CancelReq.ActivityTime = res.activity_time[req_struct.CancelReq.OrderID];
//		req_struct.CancelReq.MessageTag = sequence_number;
//		req_struct.CancelReq.MarketSegmentID = 0x80000000;
//		req_struct.CancelReq.SimpleSecurityID = stoll(parser.get(Fix_SecurityID));
//		req_struct.CancelReq.TargetPartyIDSessionID = confDetails->SessionId;
//		req_struct.CancelReq.RegulatoryID = 0;
//		strcpy(req_struct.CancelReq.AlgoID, confDetails->algoId.c_str());
//		strcpy(req_struct.CancelReq.Pad4, "");
//
//		//res.request_clorderID[sequence_number] = req_struct.CancelReq.OrderID;
//		res.request_clorderID_exchangeOrderID[sequence_number].first = req_struct.CancelReq.ClOrdID;
//		res.request_clorderID_exchangeOrderID[sequence_number].second = req_struct.CancelReq.OrderID;
//	}
//	catch (std::exception& ex) {
//
//	}
//}

