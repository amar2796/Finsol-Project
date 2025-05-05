#include "Exchange_Processor.h"


FIN::ETI::Exchange_Processor::Exchange_Processor(boost::asio::io_service& ioService) :ExchangeConnection(FIN::FIN_CONST::FIN_CONFIG_FILE, ioService),
_omsSender(ioService)
{
	try
	{
		//updateConfigFile("H21yuyoyu13");
		heartbeat = config_param.getValue<uint32_t>("SESSION.HeartBeat", 0);
		DefaultCstmApplVerID = config_param.getValue<std::string>("SESSION.DefaultCstmApplVerID", "");
		userId = config_param.getValue<uint32_t>("SESSION.UserId", 0);
		password = config_param.getValue<std::string>("SESSION.Password", "");
		confirmationPort = config_param.getValue<int>("OMS_CONNECTION.ListenPort", 0);
		sender_port = config_param.getValue<int>("UDP_CONNECTION.SendPort", 0);
		udp_sender_ip = config_param.getValue<std::string>("UDP_CONNECTION.SendIP", "");
		rcv_port = config_param.getValue<int>("UDP_CONNECTION.RcvPort", 0);
		rabbit_udp = config_param.getValue<int>("OMS_COMM.OMS_Channel", 0);
		terminalInfo = config_param.getValue<uint64_t>("SESSION.TerminalInfo", 0);
		SMPF = config_param.getValue<uint8_t>("SESSION.SMPF", 0);
		persistent = config_param.getValue<uint8_t>("SESSION.Persistent", 0);
		//newPassword = config_param.getValue<std::string>("SESSION.NewPassword", "");
		strategyID = config_param.getValue<int>("SESSION.StrategyID", 0);
		//if (strategyID == 0) strategyID = ((uint64_t)0xffffffffffffffffUL);
		trade_applSeq.seq_path = _path.getCurrentPath() + FIN::FIN_CONST::APPL_SEQ_PATH;
		trade_applSeq.ReadFileForApllSeq();
		/*std::thread t(&Logger::processQueue, &logger);
		t.detach();*/
		//Password Encryption process
		publicKeyFilePath = _path.getCurrentPath() + FIN::FIN_CONST::PEMFILE;
		unencryptedPassword = password;
		password = _passwordEncryption.encryptPassword(password, publicKeyFilePath);
		LOG_INFO << "PASSWORD unencrypted used:" << unencryptedPassword;
		//Read Custodian from File
		std::string custFilePath = _path.getCurrentPath() + FIN::FIN_CONST::CUSTODIAN_FILE;
		std::cout << "[INFO ]custodian path " << custFilePath << std::endl;
		readCustodian(custFilePath);

		std::thread heartbeat_handler(&Exchange_Processor::initiateHeartbeat, this, heartbeat);
		heartbeat_handler.detach();
	}
	catch (std::exception& ex)
	{
		CONSOLE_LOG << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}

}

void FIN::ETI::Exchange_Processor::handleMessage(unsigned char* data, UINT16 templateID, int len = 0)
{
	//if (templateID == 10023) return;
	decrypt((data + 8), len - 8);

	//memset(mapDecrypted, 0, 1000);
	//memcpy(mapDecrypted, data, 8);
	//memcpy(mapDecrypted + 8, decryptedtext, decryptedtext_len);

	memmove(decryptedtext + 8, decryptedtext, len); // +1 for null terminator
	memcpy(decryptedtext, data, 8);




	//auto dycryptedData1 = (ConnectionGatewayResponseT*)decryptedtext;

	//auto dycryptedData = (ConnectionGatewayResponseT1*)decryptedtext;

	switch (templateID)
	{
	case TID_CONNECTION_GATEWAY_RESPONSE:onMessage((ConnectionGatewayResponseT*)decryptedtext); break;
	case TID_LOGON_RESPONSE:onMessage((LogonResponseT*)decryptedtext); break;
	case TID_HEARTBEAT_NOTIFICATION:onMessage((HeartbeatNotificationT*)decryptedtext); break;
	case TID_USER_LOGIN_RESPONSE:onMessage((UserLoginResponseT*)decryptedtext); break;
	case TID_NEW_ORDER_NR_RESPONSE:onMessage((NewOrderNRResponseT*)decryptedtext); break;
	case TID_NEW_ORDER_RESPONSE: onMessage((NewOrderResponseT*)decryptedtext); break;
	case TID_REJECT:
	{
		if (exchangeDone)
			onMessage((RejectT*)decryptedtext);
		else
			onMessage((RejectT*)data);


		break;
	}
	case TID_DELETE_ORDER_NR_RESPONSE: onMessage((DeleteOrderNRResponseT*)decryptedtext); break;
	case TID_DELETE_ORDER_RESPONSE: onMessage((DeleteOrderResponseT*)decryptedtext); break;
	case TID_MODIFY_ORDER_NR_RESPONSE: onMessage((ModifyOrderNRResponseT*)decryptedtext); break;
	case TID_MODIFY_ORDER_RESPONSE: onMessage((ModifyOrderResponseT*)decryptedtext); break;
	case TID_ORDER_EXEC_RESPONSE: onMessage((OrderExecResponseT*)decryptedtext); break;
	case TID_ORDER_EXEC_NOTIFICATION: onMessage((OrderExecNotificationT*)decryptedtext); break;
	case TID_TRADING_SESSION_STATUS_BROADCAST: onMessage((TradingSessionStatusBroadcastT*)decryptedtext); break;
	case TID_SUBSCRIBE_RESPONSE: onMessage((SubscribeResponseT*)decryptedtext); break;
	case TID_SESSION_PASSWORD_CHANGE_RESPONSE: onMessage((SessionPasswordChangeResponseT*)decryptedtext); break;
	case TID_LOGOUT_RESPONSE: onMessage((LogoutResponseT*)decryptedtext); break;
	case TID_FORCED_LOGOUT_NOTIFICATION: onMessage((ForcedLogoutNotificationT*)mapDecrypted); break;
	case TID_FORCED_USER_LOGOUT_NOTIFICATION: onMessage((ForcedUserLogoutNotificationT*)mapDecrypted); break;
	case TID_ORDER_EXEC_REPORT_BROADCAST: onMessage((OrderExecReportBroadcastT*)decryptedtext); break;
	case TID_TRADE_BROADCAST: onMessage((TradeBroadcastT*)decryptedtext); break;
	case TID_RETRANSMIT_RESPONSE: onMessage((RetransmitResponseT*)decryptedtext); break;
	case TID_DELETE_ORDER_BROADCAST:onMessage((DeleteOrderBroadcastT*)decryptedtext); break;
	case TID_RETRANSMIT_ME_MESSAGE_RESPONSE:onMessage((RetransmitMEMessageResponseT*)decryptedtext); break;
	case TID_MARKET_STATUS_NOTIFICATION: onMessage((MarketStatusNotificationT*)decryptedtext); break;
	case TID_NEWS_BROADCAST: onMessage((NewsBroadcastT*)decryptedtext); break;
	case TID_DELETE_ALL_ORDER_BROADCAST: onMessage((DeleteAllOrderBroadcastT*)decryptedtext); break;
	case TID_TRADE_ENHANCEMENT_NOTIFICATION: onMessage((TradeEnhancementNotificationT*)decryptedtext); break;
	case TID_DELETE_ALL_ORDER_QUOTE_EVENT_BROADCAST: onMessage((DeleteAllOrderQuoteEventBroadcastT*)decryptedtext); break;
	default:
		LOG_INFO << "NO PROCESSING FOR THIS TEMPLATEID " << templateID;
		std::cout << "NO PROCESSING FOR THIS TEMPLATEID " << templateID << std::endl;
		break;
	}

}

void FIN::ETI::Exchange_Processor::onMessage(ConnectionGatewayResponseT* gwRes)
{


	uint32_t dynamicIp = gwRes->GatewayID;

	struct in_addr ip_addr;
	ip_addr.s_addr = dynamicIp;
	auto new_primaryip = inet_ntoa(ip_addr);

	host_port = gwRes->GatewaySubID;
	host_ip = new_primaryip;


	uint32_t ip = gwRes->GatewayID;
	unsigned char octet[4] = { 0,0,0,0 };
	for (int i = 0; i < 4; i++)
	{
		octet[i] = (ip >> (i * 8)) & 0xFF;
	}
	sprintf((char*)host_ip.c_str(), "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);

	std::cout << "[INFO ] connection gateway response" << std::endl;

	std::cout << "[INFO ][ DYNAMIC IP AND PORT ] "
		<< host_ip << " " << host_port << std::endl;
	LOG_INFO << "[ DYNAMIC IP AND PORT ] " << host_ip << " " << std::to_string(host_port);
	_gateway_request_done = true;


	//logger.logger_response.connGwRes = *gwRes;
	//logger.info(logger.logger_response, 2);

	logger.LogConnectionGatewayResponse(gwRes);

	closeSocket();
	connectSocket();


	std::copy(std::begin(gwRes->Key), std::end(gwRes->Key), std::begin(gatewayResponse.Key));
	std::copy(std::begin(gwRes->IVVector), std::end(gwRes->IVVector), std::begin(gatewayResponse.IVVector));
	exchangeDone = true;

	sendLogonRequest();
	_gateway_request_done = false;
}
void FIN::ETI::Exchange_Processor::onMessage(LogonResponseT* gwLogonRes)
{


	logger.LogLogonResponseT(gwLogonRes);
	if (gwLogonRes->DaysLeftForPasswordExpiry <= 1)
	{
		sendPasswordChange();
		return;
	}

	isConnected = true;
	slidingThrottel = new SlidingWindowThrottling(gwLogonRes->ThrottleNoMsgs, gwLogonRes->ThrottleTimeInterval);
	std::cout << "[INFO ] Session logon done" << std::endl;

	sendUserLogon();
	//exchangeDone = true;
}
void FIN::ETI::Exchange_Processor::onMessage(HeartbeatNotificationT* heartbeat)
{
	LOG_INFO << "Heartbet from exchange : TemplateID " << heartbeat->MessageHeaderOut.TemplateID;
	sendHeartbeat();
	resetHeartbeat = true;

}
void FIN::ETI::Exchange_Processor::onMessage(UserLoginResponseT* userLogon)
{

	logger.LogUserLoginResponseT(userLogon);
	std::cout << "[INFO ] User logon done ready for transaction" << std::endl;

	isLoggedOn = true;
	isSendDeleteBroadCastMsg = false;			//This is for first recovery complete then send DeleteBroadcast
	partitionID = 1;
	isFirstPartition = true;
	isFirst = true;
	_omsSender.startConnection();

	//subscribe
	sendSubscriptionRequest(1);  // Trade listener
	sendSubscriptionRequest(2);
	sendSubscriptionRequest(5); // Listener Data 
}
void FIN::ETI::Exchange_Processor::onMessage(NewOrderNRResponseT* _nrResponse)
{
	_structConvertion._orderSequence.erase(_nrResponse->NRResponseHeaderME.MsgSeqNum);
	_structConvertion.lastUpdatedTime.insert(std::make_pair(_nrResponse->OrderID, _nrResponse->NRResponseHeaderME.SendingTime));

	sendResponseToOMS((char*)_nrResponse, _nrResponse->MessageHeaderOut.BodyLen, 0, _nrResponse->OrderID);

	logger.LogLeanNewOrderResponse(_nrResponse);

}
void FIN::ETI::Exchange_Processor::onMessage(NewOrderResponseT* statndardNewRes)
{
	_structConvertion._orderSequence.erase(statndardNewRes->ResponseHeaderME.MsgSeqNum);
	_structConvertion.lastUpdatedTime.insert(std::make_pair(statndardNewRes->OrderID, statndardNewRes->LstUpdtTime));

	sendResponseToOMS((char*)statndardNewRes, statndardNewRes->MessageHeaderOut.BodyLen, 0, statndardNewRes->OrderID);
	logger.LogStandardNewOrderResponse(statndardNewRes);
}
void FIN::ETI::Exchange_Processor::onMessage(RejectT* rejectRes)
{

	logger.LogRejectT(rejectRes);

	auto find_clorderid = _structConvertion._orderSequence.find(rejectRes->NRResponseHeaderME.MsgSeqNum);
	if (find_clorderid != _structConvertion._orderSequence.end())
	{
		rejectRes->NRResponseHeaderME.SendingTime = find_clorderid->second;

		sendResponseToOMS((char*)rejectRes, rejectRes->MessageHeaderOut.BodyLen, 8, rejectRes->NRResponseHeaderME.SendingTime);
		_structConvertion._orderSequence.erase(find_clorderid);
	}
	else
	{
		std::string rejectionText(rejectRes->VarText, rejectRes->VarTextLen);
		if ((rejectionText.find("password has been expired") != std::string::npos) || (rejectionText.find("Password has been expired") != std::string::npos) || (rejectionText.find("Password needs to be change") != std::string::npos)) {
			sendPasswordChange();
		}
	}
	std::cout << "[INFO ][REJECTION ] : " << rejectRes->VarText << " Reason : " << rejectRes->SessionRejectReason << " ID : " << rejectRes->NRResponseHeaderME.SendingTime << std::endl;
}
void FIN::ETI::Exchange_Processor::onMessage(DeleteOrderNRResponseT* cancelRes)
{
	_structConvertion.lastUpdatedTime.erase(cancelRes->OrderID);

	sendResponseToOMS((char*)cancelRes, cancelRes->MessageHeaderOut.BodyLen, 4, cancelRes->OrderID);
	logger.LogDeleteLeanResponse(cancelRes);


}
void FIN::ETI::Exchange_Processor::onMessage(DeleteOrderResponseT* cancelRes)
{
	_structConvertion.lastUpdatedTime.erase(cancelRes->OrderID);

	sendResponseToOMS((char*)cancelRes, cancelRes->MessageHeaderOut.BodyLen, 4, cancelRes->OrderID);

	logger.LogDeleteStandardResponse(cancelRes);
}
void FIN::ETI::Exchange_Processor::onMessage(ModifyOrderNRResponseT* modRes)
{

	_structConvertion._orderSequence.erase(modRes->NRResponseHeaderME.MsgSeqNum);



	if (_structConvertion.lastUpdatedTime.find(modRes->OrderID) != _structConvertion.lastUpdatedTime.end())
		_structConvertion.lastUpdatedTime[modRes->OrderID] = modRes->NRResponseHeaderME.SendingTime;

	sendResponseToOMS((char*)modRes, modRes->MessageHeaderOut.BodyLen, 5, modRes->OrderID);

	logger.LogLeanModifyResponse(modRes);

}
void FIN::ETI::Exchange_Processor::onMessage(ModifyOrderResponseT* modRes)
{
	_structConvertion._orderSequence.erase(modRes->ResponseHeaderME.MsgSeqNum);

	if (_structConvertion.lastUpdatedTime.find(modRes->OrderID) != _structConvertion.lastUpdatedTime.end())
		_structConvertion.lastUpdatedTime[modRes->OrderID] = modRes->LstUpdtTime;


	sendResponseToOMS((char*)modRes, modRes->MessageHeaderOut.BodyLen, 5, modRes->OrderID);

	logger.LogStandardModResponse(modRes);

}
void FIN::ETI::Exchange_Processor::onMessage(OrderExecNotificationT* bookExecution)
{

	if (bookExecution->OrdStatus[0] == '1')
	{
		if (_structConvertion.lastUpdatedTime.find(bookExecution->OrderID) != _structConvertion.lastUpdatedTime.end())
			_structConvertion.lastUpdatedTime[bookExecution->OrderID] = bookExecution->LstUpdtTime;
	}
	else
	{
		_structConvertion.lastUpdatedTime.erase(bookExecution->OrderID);
	}

	auto trades = IsEligibleToSend(bookExecution);
	if (trades.size() > 0)
	{
		for (size_t i = 0; i < trades.size(); i++)
		{
			sendResponseToOMS((char*)&trades[i], sizeof(trades[i]), 2, trades[i].OrderID);
			trades[i].log();
		}

	}

	logger.LogOrderExecNotificationT(bookExecution);



}
void FIN::ETI::Exchange_Processor::onMessage(OrderExecResponseT* bookExecution)
{
	if (bookExecution->OrdStatus[0] == '1')
	{
		if (_structConvertion.lastUpdatedTime.find(bookExecution->OrderID) != _structConvertion.lastUpdatedTime.end())
			_structConvertion.lastUpdatedTime[bookExecution->OrderID] = bookExecution->LstUpdtTime;
	}
	else
	{
		_structConvertion.lastUpdatedTime.erase(bookExecution->OrderID);
	}
	auto trades = IsEligibleToSend(bookExecution);
	if (trades.size() > 0)
	{
		for (size_t i = 0; i < trades.size(); i++)
		{
			sendResponseToOMS((char*)&trades[i], sizeof(trades[i]), 2, trades[i].OrderID);
			trades[i].log();

		}

	}

	logger.LogOrderExecResponseT(bookExecution);

}
void FIN::ETI::Exchange_Processor::onMessage(TradingSessionStatusBroadcastT* trdaingStateEvent)
{
	LOG_INFO << " Trading Session status MarketSegmentID= "
		<< " MarketSegmentID =" << trdaingStateEvent->MarketSegmentID << "\n"
		<< " Pad7 =" << trdaingStateEvent->Pad7 << "\n"
		<< "RefApplLastMsgID =" << trdaingStateEvent->RefApplLastMsgID << "\n"
		<< "TradeDate =" << trdaingStateEvent->TradeDate << "\n"
		<< "TradSesEvent =" << (unsigned)trdaingStateEvent->TradSesEvent << "\n"
		<< "Partition = " << trdaingStateEvent->RBCHeaderME.PartitionID;
}
void FIN::ETI::Exchange_Processor::onMessage(ThrottleUpdateNotificationT* throttleRes)
{
	LOG_INFO << "Throttle notification received " << throttleRes->ThrottleNoMsgs << " in " << throttleRes->ThrottleTimeInterval << " ms";
	slidingThrottel = new SlidingWindowThrottling(throttleRes->ThrottleNoMsgs, throttleRes->ThrottleTimeInterval);
}
void FIN::ETI::Exchange_Processor::onMessage(SessionPasswordChangeResponseT* passChange)
{
	LOG_INFO << "Password change done " << passChange->MessageHeaderOut.TemplateID;
	password = newPassword;
	unencryptedPassword = unencryptedNewPassword;
	updateConfigFile(unencryptedNewPassword);
	closeSocket();
}
void FIN::ETI::Exchange_Processor::onMessage(LogoutResponseT* logout)
{
	LOG_INFO << "Logout response " << logout->MessageHeaderOut.TemplateID;

}
void FIN::ETI::Exchange_Processor::onMessage(ForcedUserLogoutNotificationT* forcedLogout)
{
	LOG_INFO << "ForceUserLogout " << forcedLogout->MessageHeaderOut.TemplateID;
	std::cout << "[INFO ] Forced User Logout " << std::endl;

}
void FIN::ETI::Exchange_Processor::onMessage(ForcedLogoutNotificationT* forcedLogout)
{
	LOG_INFO << "ForceLogout " << forcedLogout->MessageHeaderOut.TemplateID;
	std::string error_msg(forcedLogout->VarText, forcedLogout->VarTextLen);
	LOG_INFO << "Reason " << error_msg;
	std::cout << "[INFO ] ForceUserLogout " << error_msg << std::endl;


}
void FIN::ETI::Exchange_Processor::onMessage(OrderExecReportBroadcastT* orderExec)
{
	/*logger.logger_response.executionReport = *orderExec;
	logger.info(logger.logger_response, 18);*/
	auto cacnelResponse = IsCanceled(orderExec);
	if (cacnelResponse.OrderID != 0)
	{
		sendResponseToOMS((char*)&cacnelResponse, sizeof(cacnelResponse), 7, orderExec->OrderID);

	}

	logger.LogOrderExecReportBroadcastT(orderExec);

}
void FIN::ETI::Exchange_Processor::onMessage(TradeBroadcastT* tradeRecovery)
{
	//LOG_INFO << "TradeBroadcastT: TotNumTradeReports " << tradeRecovery->TotNumTradeReports << " ApplSeqNum " 
	//	<< tradeRecovery->RBCHeader.ApplSeqNum << " endSeqNumber " << endSeqNumber << " Partition " << tradeRecovery->RBCHeader.PartitionID;

	if (IsEligibleToSend(tradeRecovery))
	{
		sendResponseToOMS((char*)tradeRecovery, tradeRecovery->MessageHeaderOut.BodyLen, 2, tradeRecovery->OrderID);

	}


	trade_applSeq.applSeq.push(tradeRecovery->RBCHeader.ApplSeqNum);


	logger.LogTradeBroadcastT(tradeRecovery);
	//LOG_INFO << "test : " << "clordid=" << tradeRecovery->ClOrdID << ",price=" << tradeRecovery->Price << "," << tradeRecovery->LastPx << "," << tradeRecovery->SideLastPx;

	//std::cout << tradeRecovery->RBCHeader.ApplSeqNum << " : " << lastSeqNumber << " : " << tradeRecovery->RBCHeader.PartitionID << std::endl;

	if (tradeRecovery->RBCHeader.ApplSeqNum == endSeqNumber)
	{
		if (tradeRecovery->RBCHeader.ApplSeqNum != lastSeqNumber)
		{
			sendRetransmissionTrade(tradeRecovery->RBCHeader.ApplSeqNum + 1, tradeRecovery->RBCHeader.PartitionID);
		}
		else if (tradeRecovery->RBCHeader.ApplSeqNum == lastSeqNumber && partitionID < 2)
		{
			sendRetransmissionTrade(1, ++partitionID);
		}
	}
}

void FIN::ETI::Exchange_Processor::onMessage(RetransmitMEMessageResponseT* response)
{
	LOG_INFO << "RetransmitMEMessageResponseT Template ID =" << response->MessageHeaderOut.TemplateID << " Total count =" << response->ApplTotalMessageCount
		<< " RefApplLastMsgID= " << response->RefApplLastMsgID << " ApplEndMsgID = " << response->ApplEndMsgID;



}
void FIN::ETI::Exchange_Processor::onMessage(NewsBroadcastT* response)
{
	LOG_INFO << "NewsBroadcastT Template ID =" << response->MessageHeaderOut.TemplateID << " News =" << std::string(response->VarText, response->VarTextLen);

	// Disable news sending to OMS .
	if (!rabbit_udp)
		_omsRabbitSender.sendToOMS((char*)response, response->MessageHeaderOut.BodyLen, 10, 0);
	else
		oms_udp_sender.sendToOMS((char*)response, response->MessageHeaderOut.BodyLen, 10, 0);

}
void FIN::ETI::Exchange_Processor::onMessage(DeleteAllOrderBroadcastT* response)
{
	if (isSendDeleteBroadCastMsg)
	{
		LOG_INFO << "DeleteAllOrderBroadcastT Template ID =" << response->MessageHeaderOut.TemplateID << " MassCancellation Reason  =" << response->MassActionReason
			<< " OrderID = " << response->OrderID << " OrderSide = " << std::to_string(response->OrdType) << " noOfAffectedOrder =" << response->NoNotAffectedOrders;
		if (response->NoNotAffectedOrders > 0)
		{
			for (int i = 0; i < response->NoNotAffectedOrders; i++)
			{
				LOG_INFO << " NotAffectedOrderID =" << response->NotAffectedOrdersGrp[i].NotAffectedOrderID << " NotAffOrigClOrdID  =" << response->NotAffectedOrdersGrp[i].NotAffOrigClOrdID;
			}
		}
		response->RegulatoryID = userId;
		sendResponseToOMS((char*)response, response->MessageHeaderOut.BodyLen, 4, response->OrderID);
		std::cout << "[INFO ] DeleteAllOrderBroadcastT reason " << std::to_string(response->MassActionReason) << " : " << response->OrderID << std::endl;
	}
}
void FIN::ETI::Exchange_Processor::onMessage(TradeEnhancementNotificationT* response)
{
	LOG_INFO << "TradeEnhancementNotificationT Template ID =" << response->MessageHeaderOut.TemplateID << " OrderID  =" << response->OrderID;
}
void FIN::ETI::Exchange_Processor::onMessage(RetransmitResponseT* retransmitResponse)
{
	endSeqNumber = retransmitResponse->ApplEndSeqNum;
	lastSeqNumber = retransmitResponse->RefApplLastSeqNum;
	if (isFirstPartition)
	{
		if (retransmitResponse->ApplTotalMessageCount == 0)
			tradeCount = retransmitResponse->ApplTotalMessageCount;
		else
			tradeCount = retransmitResponse->RefApplLastSeqNum;

		isFirstPartition = false;
	}

	/*if (retransmitResponse->ApplTotalMessageCount == 0)
	{
		if (lastSeqNumber != 0xffffffffffffffffUL)
		{
			trade_applSeq.ApllSeq = lastSeqNumber;
			trade_applSeq.writeintofile(lastSeqNumber);
		}
		else
		{
			trade_applSeq.ApllSeq = 0;
			trade_applSeq.writeintofile(0);
		}


	}*/
	LOG_INFO << "Retransmit response \n TemplateID " << retransmitResponse->MessageHeaderOut.TemplateID << "\n"
		<< "ApplEndSeqNum=" << retransmitResponse->ApplEndSeqNum << "\n"
		<< "ApplTotalMessageCount=" << retransmitResponse->ApplTotalMessageCount << "\n"
		<< "RefApplLastSeqNum=" << retransmitResponse->RefApplLastSeqNum << "\n";

	if (endSeqNumber == lastSeqNumber && partitionID == 2 && !isSendDeleteBroadCastMsg)
	{
		std::cout << "[INFO] TradeRecovery Completed , Total trade : in partition 1 : " << tradeCount << " and partitiotn 2 : " << retransmitResponse->ApplTotalMessageCount << "\n" << "Now Remaining pending order will be cancelled if any ." << std::endl;
		LOG_INFO << "[INFO] TradeRecovery Completed , Total trade : in partition 1 : " << tradeCount << " and partiiotn 2 : " << retransmitResponse->ApplTotalMessageCount << "Now Remaining pending order will be cancelled if any .";

		isSendDeleteBroadCastMsg = true;
		sendDeleteAllBroadcastMsg();
	}
	else if (retransmitResponse->ApplTotalMessageCount == 0 && endSeqNumber == lastSeqNumber && partitionID < 2)
	{
		sendRetransmissionTrade(1, ++partitionID);
	}
}
void FIN::ETI::Exchange_Processor::onMessage(DeleteOrderBroadcastT* cancelBroadcast)
{
	LOG_INFO << " TemplateID = " << cancelBroadcast->MessageHeaderOut.TemplateID
		<< " ClOrdID = " << cancelBroadcast->ClOrdID
		<< " OrderId = " << cancelBroadcast->OrderID;

}
void FIN::ETI::Exchange_Processor::onMessage(MarketStatusNotificationT* marketStatus)
{
	LOG_INFO << " MarketStatusNotificationT Template ID = " << marketStatus->MessageHeaderOut.TemplateID
		<< " Status = " << marketStatus->Status
		<< " TradingSessionID = " << marketStatus->TradingSessionID;
}

void FIN::ETI::Exchange_Processor::onMessage(DeleteAllOrderQuoteEventBroadcastT* response)
{
	LOG_INFO << "DeleteAllOrderQuoteEventBroadcastT Template ID =" << response->MessageHeaderOut.TemplateID << " SecurityID =" << response->SecurityID;

	sendResponseToOMS((char*)response, response->MessageHeaderOut.BodyLen, 10, response->MessageHeaderOut.TemplateID);
}

void FIN::ETI::Exchange_Processor::initiateHeartbeat(int heartbeat)
{
	std::cout << "[INFO ]Heart beat handler initiated " << std::endl;
	LOG_INFO << "Heart beat handler initiated ";
	while (true)
	{
		Sleep(heartbeat);
		if (!resetHeartbeat)
		{
			if (isConnected)
				sendHeartbeat();
		}
		resetHeartbeat = false;
	}
}
void FIN::ETI::Exchange_Processor::startCommunication()
{
	if (!_gateway_request_done)
	{
		//	count = 0;
		LOG_INFO << "Starting Communication with exchange";
		_seqNu = 0;
		sendGatewayRequest();

	}

}

void FIN::ETI::Exchange_Processor::restartOmsConnections()
{

	std::cout << "[INFO ]Restarting OMS Connection";
	_omsSender.init(confirmationPort);

	if (!rabbit_udp)
	{
		std::string configPath = _path.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
		RabbitMqConsumer::connect(configPath); //consumer	connection
		_omsRabbitSender.connect(configPath); //producer connection		
	}
	else
	{
		disposeSocket();
		init_rcv(rcv_port);
		oms_udp_sender.init(udp_sender_ip, sender_port);

	}

	_omsSender.accept();
	_omsSender.startOmsHealthThread();
}

void FIN::ETI::Exchange_Processor::sendGatewayRequest()
{
	ConnectionGatewayRequestT grReq;


	host_ip = config_param.getValue<std::string>("SESSION.HostIP", "");
	host_port = config_param.getValue<int>("SESSION.HostPORT", 0);


	// Message header 

	grReq.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(grReq), TID_CONNECTION_GATEWAY_REQUEST);



	// Body part 
	memset(grReq.Pad2, NULL, 2);
	//grReq.Pad2[0] = '0x00';
	memset(grReq.DefaultCstmApplVerID, NULL, 30);
	memcpy(grReq.DefaultCstmApplVerID, DefaultCstmApplVerID.c_str(),
		DefaultCstmApplVerID.length());
	grReq.PartyIDSessionID = userId;
	memset(grReq.Filler7, NULL, 32);
	memset(grReq.Password, NULL, 344);
	memcpy(grReq.Password, password.c_str(), password.length());
	grReq.EncryptedDataMessageSize = password.length();



	// Request header
	grReq.RequestHeader.MsgSeqNum = (_seqNu += 1);
	grReq.RequestHeader.SenderSubID = grReq.PartyIDSessionID;




	auto buffer_Data = boost::asio::buffer(&grReq, grReq.MessageHeaderIn.BodyLen);
	sendToTarget(buffer_Data);


	logger.LogConnectionGatewayRequest(&grReq);


	/*logger.logger_request.connGwReq = grReq;
	logger.info(logger.logger_request, 1);*/


	logger.LogConnectionGatewayRequest(&grReq);
	GenrateIvKey();

	std::copy(std::begin(key), std::end(key), std::begin(gatewayResponse.Key));
	std::copy(std::begin(IV), std::end(IV), std::begin(gatewayResponse.IVVector));

	//initializeLib((unsigned char*)key.c_str(), (unsigned char*)IV.c_str());

}

void FIN::ETI::Exchange_Processor::sendLogonRequest()
{
	_seqNu = 0;
	LogonRequestT Logon_Req;

	// Message header 

	Logon_Req.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(Logon_Req), TID_LOGON_REQUEST);

	//Logon_Req.MessageHeaderIn.BodyLen = sizeof(Logon_Req);
	//Logon_Req.MessageHeaderIn.TemplateID = TID_LOGON_REQUEST;
	//memset(Logon_Req.MessageHeaderIn.NetworkMsgID, NULL, 8);
	//memset(Logon_Req.MessageHeaderIn.Pad2, NULL, 2);

	// Body 


	Logon_Req.HeartBtInt = heartbeat;
	Logon_Req.PartyIDSessionID = userId;

	memset(Logon_Req.DefaultCstmApplVerID, NULL, 30);
	memcpy(Logon_Req.DefaultCstmApplVerID, DefaultCstmApplVerID.c_str(),
		DefaultCstmApplVerID.length());
	Logon_Req.EncryptedDataMessageSize = password.length();
	memset(Logon_Req.Password, NULL, 344);
	memcpy(Logon_Req.Password, password.c_str(), password.length());

	memcpy(Logon_Req.ApplUsageOrders, config_param.getValue<std::string>("SESSION.ApplUsageOrders", "").c_str(), 1);
	memcpy(Logon_Req.ApplUsageQuotes, config_param.getValue<std::string>("SESSION.ApplUsageQuotes", "").c_str(), 1);
	memcpy(Logon_Req.OrderRoutingIndicator, config_param.getValue<std::string>("SESSION.OrderRoutingIndicator", "").c_str(), 1);

	memset(Logon_Req.FIXEngineName, NULL, 30);
	memcpy(Logon_Req.FIXEngineName, config_param.getValue<std::string>("SESSION.FIXEngineName", "").c_str(),
		config_param.getValue<std::string>("SESSION.FIXEngineName", "").length());


	memset(Logon_Req.FIXEngineVersion, NULL, 30);
	memcpy(Logon_Req.FIXEngineVersion, config_param.getValue<std::string>("SESSION.FIXEngineVersion", "").c_str(),
		config_param.getValue<std::string>("SESSION.FIXEngineVersion", "").length());


	memset(Logon_Req.FIXEngineVendor, NULL, 30);
	memcpy(Logon_Req.FIXEngineVendor, config_param.getValue<std::string>("SESSION.FIXEngineVendor", "").c_str(),
		config_param.getValue<std::string>("SESSION.FIXEngineVendor", "").length());

	memset(Logon_Req.ApplicationSystemName, NULL, 30);
	memcpy(Logon_Req.ApplicationSystemName, config_param.getValue<std::string>("SESSION.ApplicationSystemName", "").c_str(),
		config_param.getValue<std::string>("SESSION.ApplicationSystemName", "").length());


	memset(Logon_Req.ApplicationSystemVersion, NULL, 30);
	memcpy(Logon_Req.ApplicationSystemVersion, config_param.getValue<std::string>("SESSION.ApplicationSystemVersion", "").c_str(),
		config_param.getValue<std::string>("SESSION.ApplicationSystemVersion", "").length());


	memset(Logon_Req.ApplicationSystemVendor, NULL, 30);
	memcpy(Logon_Req.ApplicationSystemVendor, config_param.getValue<std::string>("SESSION.ApplicationSystemVendor", "").c_str(),
		config_param.getValue<std::string>("SESSION.ApplicationSystemVendor", "").length());
	memset(Logon_Req.Pad7, NULL, 7);
	memset(Logon_Req.Filler7, NULL, 32);


	// Request header 
	Logon_Req.RequestHeader.MsgSeqNum = (_seqNu += 1);
	Logon_Req.RequestHeader.SenderSubID = Logon_Req.PartyIDSessionID;




	sendEncryptedTarget((unsigned char*)&Logon_Req, sizeof(LogonRequestT));

	//	encrypt(ctx_encrypt, ((unsigned char*)&Logon_Req) + 24, sizeof(LogonRequestT) - 24, (unsigned char*)ciphertext, &ciphertext_len);
		//memmove(ciphertext + 24, ciphertext, sizeof(LogonRequestT)); // +1 for null terminator
		//memcpy(ciphertext, ((unsigned char*)&Logon_Req), 24);



		//auto buffer_Data = boost::asio::buffer(&Logon_Req, Logon_Req.MessageHeaderIn.BodyLen);
	//auto buffer_Data = boost::asio::buffer(ciphertext, ciphertext_len + 24);


	//sendToTarget(buffer_Data);

	//logger.logger_request.logonRequest = Logon_Req;
	//logger.info(logger.logger_request, 3);

	logger.LogLogonRequestT(&Logon_Req);

}

void FIN::ETI::Exchange_Processor::sendHeartbeat()
{
	HeartbeatT heartB;

	heartB.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(heartB), TID_HEARTBEAT);

	/*heartB.MessageHeaderIn.BodyLen = sizeof(heartB);
	memset(heartB.MessageHeaderIn.NetworkMsgID, NULL, 8);
	memset(heartB.MessageHeaderIn.Pad2, NULL, 2);
	heartB.MessageHeaderIn.TemplateID = TID_HEARTBEAT;*/

	auto buffer_Data = boost::asio::buffer(&heartB, heartB.MessageHeaderIn.BodyLen);


	if (sendToTarget(buffer_Data))
		LOG_INFO << "HEART BEAT SENT  : " << heartB.MessageHeaderIn.TemplateID;
	else
		LOG_INFO << "HEART BEAT NOT SENT  : " << heartB.MessageHeaderIn.TemplateID;


}

void FIN::ETI::Exchange_Processor::sendUserLogon()
{
	UserLoginRequestT user_logon;

	// Message header


	user_logon.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(user_logon), TID_USER_LOGIN_REQUEST);

	/*user_logon.MessageHeaderIn.BodyLen = sizeof(user_logon);
	memset(user_logon.MessageHeaderIn.NetworkMsgID, NULL, 8);
	memset(user_logon.MessageHeaderIn.Pad2, NULL, 2);
	user_logon.MessageHeaderIn.TemplateID = TID_USER_LOGIN_REQUEST;*/

	// Body 

	user_logon.EncryptedDataMessageSize = password.length();
	user_logon.Username = userId;
	memset(user_logon.Password, NULL, 344);
	memcpy(user_logon.Password, password.c_str(), password.length());
	memset(user_logon.Filler7, 0, 32);

	// Request header 

	user_logon.RequestHeader.MsgSeqNum = (_seqNu += 1);
	user_logon.RequestHeader.SenderSubID = user_logon.Username;

	//auto buffer_Data = boost::asio::buffer(&user_logon, user_logon.MessageHeaderIn.BodyLen);


	//memset(ciphertext, 0, 1000);
	//ciphertext_len = 0;

	//encrypt(((unsigned char*)&user_logon) + 24, sizeof(UserLoginRequestT) - 24, (unsigned char*)ciphertext, &ciphertext_len);
	//memmove(ciphertext + 24, ciphertext, sizeof(UserLoginRequestT) + 24); // +1 for null terminator
	//memcpy(ciphertext, ((unsigned char*)&user_logon), 24);


	//auto buffer_Data = boost::asio::buffer(ciphertext, sizeof(UserLoginRequestT));
	sendEncryptedTarget((unsigned char*)&user_logon, sizeof(UserLoginRequestT));

	//sendToTarget(buffer_Data);

	/*logger.logger_request.userLogonRequest = user_logon;
	logger.info(logger.logger_request, 5);*/


	logger.LogUserLoginRequestT(&user_logon);


}
int64_t FIN::ETI::Exchange_Processor::floatToInt64Multiplied(double dVal, int64_t multiplier)
{
	if (multiplier == 0) {
		// Handle division by zero (or multiplier being zero) appropriately.
		return 0;
	}

	double offset = 0.0000000001; // Adjust the offset as needed.
	if (dVal >= 0) {
		return static_cast<int64_t>(std::round((dVal + offset) * multiplier));
	}
	else {
		return static_cast<int64_t>(std::round((dVal - offset) * multiplier));
	}
}
void FIN::ETI::Exchange_Processor::sendNewOrder()
{
	try
	{
		NewOrderSingleRequestT _newOrder;

		// Message header
		_newOrder.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(_newOrder), TID_NEW_ORDER_SINGLE_REQUEST);

		// request header 
		_newOrder.RequestHeader = _common_struct.setRequestHeader((_seqNu += 1), userId);

		// body 

		_newOrder.OrdType = std::stoi(_fixStringParser.get(Fix_OrdType));
		_newOrder.StopPx = 0x8000000000000000L;
		_newOrder.Price = 0x8000000000000000L;

		if (_newOrder.OrdType == FIN::Fix_OrdType_Limit)
			_newOrder.Price = floatToInt64Multiplied(std::stod(_fixStringParser.get(FIN::Fix_Price)), FIN::price_multiplier);//(std::stod(_fixStringParser.get(FIN::Fix_Price)))* FIN::price_multiplier;
		if (_newOrder.OrdType == FIN::Fix_OrdType_Stop_Limit)
			_newOrder.StopPx = floatToInt64Multiplied(std::stod(_fixStringParser.get(FIN::Fix_Stop_Price)), FIN::price_multiplier);// (std::stod(_fixStringParser.get(FIN::Fix_Stop_Price))) * FIN::price_multiplier;
		_newOrder.MarketProtectionPercentage = 0x8000000000000000L;



		/*	if (_fixStringParser.get("9227") != "" && _fixStringParser.get("9227") != "0") {
				_newOrder.TerminalInfo = std::stoll(_fixStringParser.get("9227"));
			}
			else {
				_newOrder.TerminalInfo = terminalInfo;
			}*/
		_newOrder.TerminalInfo = _fixStringParser.getOmsTerminalINfo(terminalInfo);

		_newOrder.ClOrdID = std::stoll(_fixStringParser.get(FIN::Fix_ClOrdID));

		_structConvertion._orderSequence.insert(std::make_pair(_seqNu, _newOrder.ClOrdID));

		_newOrder.StrategyID = strategyID;
		if (strategyID == 0)_newOrder.StrategyTriggerSeqNo = strategyID;
		else _newOrder.StrategyTriggerSeqNo = _seqNu;
		_newOrder.Filler1 = 0;
		_newOrder.TargetPartyIDSessionID = userId;
		_newOrder.Echo = _seqNu;
		_newOrder.OrderQty = std::stoll(_fixStringParser.get(FIN::Fix_OrdQty)) * FIN::qty_multipler;
		_newOrder.DisclosedQty = 0;
		_newOrder.ExpireDate = 0xffffffff;
		_newOrder.MarketSegmentID = std::stol(_fixStringParser.get(FIN::Fix_Symbol));
		_newOrder.SimpleSecurityID = std::stol(_fixStringParser.get(FIN::Fix_SecurityID));
		_newOrder.RegulatoryID = 0xffffffff;
		_newOrder.Filler4 = 0xffff;
		memset(_newOrder.PartyIDTakeUpTradingFirm, NULL, 5);
		memset(_newOrder.PartyIDOrderOriginationFirm, NULL, 7);
		memset(_newOrder.PartyIDBeneficiary, NULL, 9);
		memset(_newOrder.FreeText1, NULL, 12);
		memset(_newOrder.CPCode, NULL, 12);
		if (_fixStringParser.get(FIN::Fix_Account) == "OWN" || _fixStringParser.get(FIN::Fix_Account) == "PRO") {
			_newOrder.AccountType = FIN::OWN;
			std::string freeTxt1 = "*";
			freeTxt1 += _fixStringParser.get(Fix_Account).c_str();
			freeTxt1 += "*";
			memcpy(_newOrder.FreeText1, freeTxt1.c_str(), freeTxt1.length());

		}
		else {

			auto ctcl = _fixStringParser.get(FIN::Fix_Account);

			if (custodian.find(ctcl) == custodian.end()) {
				_newOrder.AccountType = FIN::CLIENT;
				//memcpy(_newOrder.FreeText1, _fixStringParser.get(Fix_Account).c_str(), _fixStringParser.get(Fix_Account).length());			
			}
			else {
				_newOrder.AccountType = 5;
				//memcpy(_newOrder.FreeText1, custodian.c_str(), custodian.length());
				memcpy(_newOrder.CPCode, custodian[ctcl].c_str(), custodian[ctcl].length());
			}
			memcpy(_newOrder.FreeText1, _fixStringParser.get(Fix_Account).c_str(), _fixStringParser.get(Fix_Account).length());
		}

		_newOrder.Side = (uint8_t)std::stoi(_fixStringParser.get(Fix_Side));


		_newOrder.PriceValidityCheckType = 0;
		_newOrder.TimeInForce = stoi(_fixStringParser.get(Fix_TimeInForce));
		if (_newOrder.TimeInForce == 1 || _newOrder.TimeInForce == 6)
		{
			_newOrder.ApplSeqIndicator = FIN::STANDARD_ORDER;
			_newOrder.ExecInst = FIN::PERSISTENT;
		}
		else if (persistent == 0)
		{
			_newOrder.ApplSeqIndicator = 0;
			_newOrder.ExecInst = FIN::NON_PERSISTENT;
		}
		else
		{
			_newOrder.ApplSeqIndicator = FIN::STANDARD_ORDER;
			_newOrder.ExecInst = FIN::PERSISTENT;

		}

		_newOrder.SMPFOrderIdentifier = SMPF;
		_newOrder.Filler5 = 0xff;
		_newOrder.TradingSessionSubID = 0xff;
		_newOrder.TradingCapacity = 1;
		memcpy(_newOrder.Account, FIN::ACCOUNT.c_str(), 2);
		memset(_newOrder.PositionEffect, FIN::POSSITION_EFFECT, 1);
		memset(_newOrder.PartyIDLocationID, NULL, 2);
		memset(_newOrder.CustOrderHandlingInst, NULL, 1);
		memset(_newOrder.UserReferenceText, NULL, 20);
		//memset(_newOrder.FreeText1, NULL, 12);
		/*std::string freeTxt1 = "*";
		freeTxt1 += _fixStringParser.get(Fix_Account).c_str();
		freeTxt1 += "*";
		memcpy(_newOrder.FreeText1, freeTxt1.c_str(), freeTxt1.length());*/
		//memcpy(_newOrder.FreeText1, _fixStringParser.get(Fix_Account).c_str(), _fixStringParser.get(Fix_Account).length());	
		memset(_newOrder.FreeText3, NULL, 12);


		//auto buffer_Data = boost::asio::buffer(&_newOrder, _newOrder.MessageHeaderIn.BodyLen);
			//sendToTarget(buffer_Data);
		sendEncryptedTarget((unsigned char*)&_newOrder, sizeof(NewOrderSingleRequestT));


		logger.LogNewRequest(&_newOrder);


		/*logger.logger_request.newOrder = _newOrder;
		logger.info(logger.logger_request, 7);*/
	}
	catch (std::exception& ex)
	{
		CONSOLE_LOG << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}

}

void FIN::ETI::Exchange_Processor::sendModifyOrder()
{
	try
	{
		ModifyOrderSingleRequestT replaceReq;
		replaceReq.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(replaceReq), TID_MODIFY_ORDER_SINGLE_REQUEST);
		replaceReq.RequestHeader = _common_struct.setRequestHeader((_seqNu += 1), userId);
		replaceReq.OrderID = stoull(_fixStringParser.get(Fix_OrderID));
		replaceReq.ClOrdID = stoull(_fixStringParser.get(Fix_ClOrdID));
		replaceReq.OrigClOrdID = NO_VALUE_ULONG;// stoull(_fixStringParser.get(Fix_OrigClOrdID));

		replaceReq.OrdType = std::stoi(_fixStringParser.get(Fix_OrdType));
		replaceReq.StopPx = 0x8000000000000000L;
		replaceReq.Price = 0x8000000000000000L;

		if (replaceReq.OrdType == FIN::Fix_OrdType_Limit)
			replaceReq.Price = floatToInt64Multiplied(std::stod(_fixStringParser.get(FIN::Fix_Price)), FIN::price_multiplier);// (std::stod(_fixStringParser.get(FIN::Fix_Price))) * FIN::price_multiplier;
		if (replaceReq.OrdType == FIN::Fix_OrdType_Stop_Limit)
			replaceReq.StopPx = floatToInt64Multiplied(std::stod(_fixStringParser.get(FIN::Fix_Stop_Price)), FIN::price_multiplier);// (std::stod(_fixStringParser.get(FIN::Fix_Stop_Price))) * FIN::price_multiplier;
		replaceReq.MaxPricePercentage = 0x8000000000000000L;



		replaceReq.TerminalInfo = _fixStringParser.getOmsTerminalINfo(terminalInfo);


		auto find_itr = _structConvertion.lastUpdatedTime.find(replaceReq.OrderID);
		if (find_itr != _structConvertion.lastUpdatedTime.end())
			replaceReq.LstUpdtTime = find_itr->second;// 0xffffffffffffffffUL;
		replaceReq.Filler1 = 0xffffffffffffffffUL;
		replaceReq.Filler2 = 0xffffffff;
		replaceReq.Echo = _seqNu;
		replaceReq.OrderQty = std::stoll(_fixStringParser.get(FIN::Fix_OrdQty)) * FIN::qty_multipler;
		replaceReq.DisclosedQty = 0;
		replaceReq.StrategyID = strategyID;
		if (strategyID == 0)replaceReq.StrategyTriggerSeqNo = strategyID; // 0xffffffffffffffffUL;
		else replaceReq.StrategyTriggerSeqNo = _seqNu;
		replaceReq.ExpireDate = 0xffffffff;
		replaceReq.MarketSegmentID = std::stol(_fixStringParser.get(FIN::Fix_Symbol));
		replaceReq.SimpleSecurityID = std::stol(_fixStringParser.get(FIN::Fix_SecurityID));
		replaceReq.TargetPartyIDSessionID = userId;
		replaceReq.RegulatoryID = 0xffffffff;
		replaceReq.Filler4 = 0xffff;

		_structConvertion._orderSequence.insert(std::make_pair(_seqNu, replaceReq.OrderID));

		memset(replaceReq.PartyIDTakeUpTradingFirm, NULL, 5);
		memset(replaceReq.PartyIDOrderOriginationFirm, NULL, 7);
		memset(replaceReq.PartyIDBeneficiary, NULL, 9);
		memset(replaceReq.CPCode, NULL, 12);
		memset(replaceReq.FreeText1, NULL, 12);
		if (_fixStringParser.get(FIN::Fix_Account) == "OWN" || _fixStringParser.get(FIN::Fix_Account) == "PRO") {
			replaceReq.AccountType = FIN::OWN;
			std::string freeTxt1 = "*";
			freeTxt1 += _fixStringParser.get(Fix_Account).c_str();
			freeTxt1 += "*";
			memcpy(replaceReq.FreeText1, freeTxt1.c_str(), freeTxt1.length());

		}
		else {

			auto ctcl = _fixStringParser.get(FIN::Fix_Account);

			if (custodian.find(ctcl) == custodian.end()) {
				replaceReq.AccountType = FIN::CLIENT;
			}
			else {
				replaceReq.AccountType = 5;
				memcpy(replaceReq.CPCode, custodian[ctcl].c_str(), custodian[ctcl].length());
			}
			memcpy(replaceReq.FreeText1, _fixStringParser.get(Fix_Account).c_str(), _fixStringParser.get(Fix_Account).length());
		}

		replaceReq.Side = stoi(_fixStringParser.get(Fix_Side));
		replaceReq.PriceValidityCheckType = 0;
		replaceReq.TimeInForce = stoi(_fixStringParser.get(Fix_TimeInForce));


		if (replaceReq.TimeInForce == 1 || replaceReq.TimeInForce == 6)
		{
			replaceReq.ApplSeqIndicator = FIN::STANDARD_ORDER;
			replaceReq.ExecInst = FIN::PERSISTENT;
		}
		else if (persistent == 0)
		{
			replaceReq.ApplSeqIndicator = 0;
			replaceReq.ExecInst = FIN::NON_PERSISTENT;
		}
		else
		{
			replaceReq.ApplSeqIndicator = FIN::STANDARD_ORDER;
			replaceReq.ExecInst = FIN::PERSISTENT;

		}
		replaceReq.Filler5 = 0xff;
		replaceReq.TradingSessionSubID = 0xff;
		replaceReq.TradingCapacity = 1;// 0xff;
		memset(replaceReq.Filler6, NULL, 1);

		memset(replaceReq.PositionEffect, FIN::POSSITION_EFFECT, 1);
		memcpy(replaceReq.Account, FIN::ACCOUNT.c_str(), 2);
		memset(replaceReq.PartyIDLocationID, NULL, 2);
		memset(replaceReq.CustOrderHandlingInst, NULL, 1);
		memset(replaceReq.UserReferenceText, NULL, 20);
		memset(replaceReq.FreeText3, NULL, 12);
		memset(replaceReq.Pad4, NULL, 4);



		sendEncryptedTarget((unsigned char*)&replaceReq, sizeof(ModifyOrderSingleRequestT));


		logger.LogModifyOrderSingleRequestT(&replaceReq);

	}
	catch (std::exception& ex)
	{
		CONSOLE_LOG << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}

}


void FIN::ETI::Exchange_Processor::sendCancelOrder()
{
	try
	{
		DeleteOrderSingleRequestT deleteOrder;
		deleteOrder.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(deleteOrder), TID_DELETE_ORDER_SINGLE_REQUEST);
		deleteOrder.RequestHeader = _common_struct.setRequestHeader((_seqNu += 1), userId);
		deleteOrder.ClOrdID = stoull(_fixStringParser.get(Fix_ClOrdID));
		deleteOrder.OrderID = stoull(_fixStringParser.get(Fix_OrderID));
		deleteOrder.OrigClOrdID = NO_VALUE_ULONG;
		deleteOrder.LstUpdtTime = NO_VALUE_ULONG;
		deleteOrder.StrategyID = strategyID;
		deleteOrder.Echo = _seqNu;
		deleteOrder.MarketSegmentID = stoi(_fixStringParser.get(Fix_Symbol));
		deleteOrder.SimpleSecurityID = stoul(_fixStringParser.get(Fix_SecurityID));
		deleteOrder.TargetPartyIDSessionID = userId;
		deleteOrder.RegulatoryID = NO_VALUE_UINT;
		memset(deleteOrder.Pad4, NULL, 4);
		sendEncryptedTarget((unsigned char*)&deleteOrder, sizeof(DeleteOrderSingleRequestT));
		logger.LogDeleteSingleRequest(&deleteOrder);
	}
	catch (std::exception& ex)
	{
		CONSOLE_LOG << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}

}
void FIN::ETI::Exchange_Processor::onMessage(SubscribeResponseT* response)
{
	LOG_INFO << "SubscribeResponse : Template ID " << response->MessageHeaderOut.TemplateID << " ApplSubId " << response->ApplSubID;
	if (response->ResponseHeader.MsgSeqNum == 5)
	{
		sendRetransmittOrderEvent();
	}

}

void FIN::ETI::Exchange_Processor::sendRetransmittOrderEvent()
{
	RetransmitMEMessageRequestT RetransmitMessageReq;

	RetransmitMessageReq.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(RetransmitMessageReq), TID_RETRANSMIT_ME_MESSAGE_REQUEST);
	RetransmitMessageReq.RequestHeader = _common_struct.setRequestHeader((_seqNu += 1), userId);
	RetransmitMessageReq.SubscriptionScope = userId; //userId;
	RetransmitMessageReq.PartitionID = 1;
	RetransmitMessageReq.RefApplID = 5;
	memset(RetransmitMessageReq.ApplBegMsgID, NULL, 16);
	memset(RetransmitMessageReq.ApplEndMsgID, NULL, 16);
	memset(RetransmitMessageReq.Pad1, NULL, 1);

	sendEncryptedTarget((unsigned char*)&RetransmitMessageReq, sizeof(RetransmitMEMessageRequestT));

	//auto buffer_Data = boost::asio::buffer(&RetransmitMessageReq, RetransmitMessageReq.MessageHeaderIn.BodyLen);



	//sendToTarget(buffer_Data);

	if (trade_applSeq.ApllSeq > 100)
		trade_applSeq.ApllSeq -= 100;
	else
		trade_applSeq.ApllSeq = 1;

	LOG_INFO << "Retransmit order event TemplateID = " << RetransmitMessageReq.MessageHeaderIn.TemplateID << "\n"
		<< "ApplBegMsgID=" << RetransmitMessageReq.ApplBegMsgID << "\n"
		<< "ApplEndMsgID=" << RetransmitMessageReq.ApplEndMsgID << "\n"
		<< "PartitionID=" << RetransmitMessageReq.PartitionID << "\n"
		<< "RefApplID=" << RetransmitMessageReq.RefApplID << "\n"
		<< "SubscriptionScope=" << RetransmitMessageReq.SubscriptionScope << "\n";

	sendRetransmissionTrade(trade_applSeq.ApllSeq, partitionID);

}
void FIN::ETI::Exchange_Processor::sendRetransmissionTrade(uint64_t start_seq, uint32_t pID)
{
	if (start_seq == 0)
		start_seq = 1;


	RetransmitRequestT recoverTrade;
	recoverTrade.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(recoverTrade), TID_RETRANSMIT_REQUEST);
	recoverTrade.RequestHeader = _common_struct.setRequestHeader((_seqNu += 1), userId);
	recoverTrade.ApplBegSeqNum = start_seq;
	recoverTrade.ApplEndSeqNum = 0xffffffffffffffffUL;
	recoverTrade.SubscriptionScope = userId;
	recoverTrade.PartitionID = pID;
	recoverTrade.RefApplID = 1;
	memset(recoverTrade.Pad1, NULL, 1);


	sendEncryptedTarget((unsigned char*)&recoverTrade, sizeof(RetransmitRequestT));

	//auto buffer_Data = boost::asio::buffer(&recoverTrade, recoverTrade.MessageHeaderIn.BodyLen);


//	sendToTarget(buffer_Data);

	LOG_INFO << "Retransmit trade recovery TemplateID :" << recoverTrade.MessageHeaderIn.TemplateID << "\n"
		<< "ApplBegSeqNum:" << recoverTrade.ApplBegSeqNum << "\n"
		<< "ApplEndSeqNum:" << recoverTrade.ApplEndSeqNum << "\n"
		<< "PartitionID:" << recoverTrade.PartitionID << "\n"
		<< "RefApplID:" << recoverTrade.RefApplID << "\n"
		<< "SubscriptionScope:" << recoverTrade.SubscriptionScope << "\n";
}

void FIN::ETI::Exchange_Processor::sendSubscriptionRequest(uint8_t applId)
{
	SubscribeRequestT subReq;
	subReq.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(subReq), TID_SUBSCRIBE_REQUEST);
	subReq.RequestHeader = _common_struct.setRequestHeader((_seqNu += 1), userId);
	subReq.RefApplID = applId;
	if (applId == 2)
		subReq.SubscriptionScope = 0xffffffff;
	else
		subReq.SubscriptionScope = userId;
	memset(subReq.Pad3, NULL, 3);

	//auto buffer_Data = boost::asio::buffer(&subReq, subReq.MessageHeaderIn.BodyLen);


	//sendToTarget(buffer_Data);
	sendEncryptedTarget((unsigned char*)&subReq, sizeof(SubscribeRequestT));

	LOG_INFO << "Subscription Request TemplateID =" << subReq.MessageHeaderIn.TemplateID << "\n"
		<< "RefApplID=" << subReq.RefApplID << "\n"
		<< "SubscriptionScope=" << subReq.SubscriptionScope << "\n";
}

void FIN::ETI::Exchange_Processor::sendPasswordChange()
{
	SessionPasswordChangeT changePass;

	changePass.MessageHeaderIn = _common_struct.SetMessageHeaderIN(sizeof(changePass), TID_SESSION_PASSWORD_CHANGE);
	changePass.RequestHeader = _common_struct.setRequestHeader((_seqNu += 1), userId);
	newPassword = generate_password(9);//9 is length of password
	unencryptedNewPassword = newPassword;
	newPassword = _passwordEncryption.encryptPassword(newPassword, publicKeyFilePath);
	memcpy(changePass.NewPassword, newPassword.c_str(), newPassword.length());
	memcpy(changePass.Password, password.c_str(), password.length());
	changePass.PartyIDSessionID = userId;
	changePass.Filler2 = 0xffffffff;

	//auto buffer_Data = boost::asio::buffer(&changePass, changePass.MessageHeaderIn.BodyLen);
	//sendToTarget(buffer_Data);

	LOG_INFO << "Sending pass change : TmplateID = " << changePass.MessageHeaderIn.TemplateID << "\n"
		<< "UnEncryptedNewPassword=" << unencryptedNewPassword << "\n"
		<< "NewPassword=" << changePass.NewPassword << "\n"
		<< "PartyIDSessionID=" << changePass.PartyIDSessionID << "\n"
		<< "UnEcryptedPassword=" << unencryptedPassword << "\n"
		<< "Password=" << changePass.Password << "\n";



	//sendToTarget(buffer_Data);

	sendEncryptedTarget((unsigned char*)&changePass, sizeof(SessionPasswordChangeT));

}

void FIN::ETI::Exchange_Processor::GenrateIvKey()
{
	IV = unencryptedPassword;
	while (IV.length() < 12) {
		IV += '0';
	}
	key = unencryptedPassword;
	while (key.length() < 32) {
		key += '0';
	}

}

void FIN::ETI::Exchange_Processor::receiveFromOMS(const std::string& omsInput)
{
	try
	{
		if (omsInput == "H")
			return;

		LOG_INFO << "[FROM_OMS] " << omsInput;
		_fixStringParser.loadString(omsInput);
		if (isLoggedOn)
		{

			slidingThrottel->checkThrottleLimit();


			switch (_fixStringParser.getChar(Fix_MsgType))
			{
			case Fix_MsgType_NEW_ORDER: sendNewOrder();     break;
			case Fix_MsgType_CANCEL_ORDER: sendCancelOrder();  break;
			case Fix_MsgType_MODIFY_ORDER: sendModifyOrder(); break;
			default: std::cout << "Inalid Message Type";
			}
			resetHeartbeat = true;
		}
		else
		{
			RejectT reject = {};
			reject.MessageHeaderOut.BodyLen = sizeof(reject);
			reject.MessageHeaderOut.TemplateID = 10010;
			reject.NRResponseHeaderME.SendingTime = std::stoull(_fixStringParser.get("11"));
			memcpy(reject.VarText, FIN::EXCH_IS_NOT_UP.c_str(), FIN::EXCH_IS_NOT_UP.length());
			reject.VarTextLen = FIN::EXCH_IS_NOT_UP.length();
			onMessage(&reject);
		}
		_fixStringParser.reset();
	}
	catch (std::exception& e)
	{
		std::cout << "Exception :" << e.what() << std::endl;
		LOG_INFO << "Exception :" << e.what();
	}

}



void FIN::ETI::Exchange_Processor::startExchangeConnection()
{

	while (1)
	{
		try
		{
			connectSocket();
			_connected = true;
			startCommunication();
			while (1)
			{
				try
				{
					receiveFromTarget();
				}
				catch (std::exception& e)
				{
					if (isFirst)
					{
						std::cout << "Exchange Disconnected" << std::endl;
						LOG_INFO << "Exchange Disconnected";
					}
					std::cout << "Exception Rcvd : " << e.what() << std::endl;
					LOG_INFO << "Exception Rcvd : " << e.what();
					isConnected = false;
					break;
				}
			}
			std::cout << "[INFO ]Reconnecting after 5 Seconds" << std::endl;
			_connected = false;
			isLoggedOn = false;
			exchangeDone = false;

			_omsSender.stopConnection();
			if (isFirst)
			{
				_omsSender.sendLogoutMsg(_omsSender.logoutMessage());
				_omsSender.closeSocketConnection();
				isFirst = false;
			}

			Sleep(5000);
		}
		catch (std::exception& ex)
		{
			CONSOLE_LOG << ex.what() << std::endl;
			LOG_INFO << ex.what();
		}
	}

}




void FIN::ETI::Exchange_Processor::startOMSChannel()
{
	bool first = true;
	while (true) {
		if (isLoggedOn)
		{
			if (first)
			{
				doOMSConnect();

				first = false;
			}
			try
			{
				if (!rabbit_udp)
					rabbitMqConsumer();
				else
					StartReceiving();
			}
			catch (std::exception& e)
			{
				restartOmsConnections();
			}
		}
		else
		{
			Sleep(10);
		}
	}
}


void FIN::ETI::Exchange_Processor::doOMSConnect()
{
	try
	{
		std::cout << "[INFO ]Initiating OMS Connection";

		if (!rabbit_udp)
		{
			std::string configPath = _path.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
			RabbitMqConsumer::connect(configPath); //consumer	connection
			_omsRabbitSender.connect(configPath); //producer connection		
		}
		else
		{
			init_rcv(rcv_port);
			oms_udp_sender.init(udp_sender_ip, sender_port);

		}

		_omsSender.init(confirmationPort);
		_omsSender.accept();
		_omsSender.startOmsHealthThread();
	}
	catch (std::exception& ex)
	{
		CONSOLE_LOG << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}

}

//***********************************************************************
// Password Generation and updation in config
//***********************************************************************

//selectArray is  a utility function that is used to
			//randomly generate a integer in the range 1 to 4 (both inclusive)
int FIN::ETI::Exchange_Processor::selectArray()
{
	//srand(time(NULL));
	int i = rand() % 5;
	if (i == 0)
		i++;
	return i;
}

//getKey() is another utility function that is used to randomly generate
//an integer in the range 0 to 25 (both inclusive)
int FIN::ETI::Exchange_Processor::getKey()
{
	/*srand(time(NULL));*/

	int key = rand() % 26;
	return key;
}
std::string FIN::ETI::Exchange_Processor::generate_password(int length)
{
	srand(time(NULL));
	//Intializing result string password as NULL.
	std::string password = "";

	//std::strings whose characters will be used to build password
	std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
	std::string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::string s_symbol = "@#$%&*";
	std::string number = "0123456789";

	//initializing local variables
	int key, count_alphabet = 0, count_ALPHABET = 0, count_number = 0, count_s_symbol = 0;

	//Count will store the length of the password being created,
	//initially this will be zero(0)
	int count = 0;
	while (count < length) {
		// selectArray() function will return a number 1 to 4
		// and will use to select one of the above defined string
		//(i.e alphabet or ALPHABET or s_symbol or number )
		// 1 is for string alphabet
		// 2 is for string ALPHABET
		// 3 is for string number
		// and 4 is for string s_symbol

		int k = selectArray();

		//for the first character of password we are generating as alphabet. so the string that should be selected is either alphabet or 
		//ALPHABET (i.e 1 or 2) following if condition will take care of it.
		if (count == 0) {
			k = k % 3;
			if (k == 0)
				k++;
		}
		switch (k) {
		case 1:
			// following if condition will check if minimum requirement of alphabet
			// character has been fulfilled or not
			// in case it has been fulfilled and minimum requirements of other
			// characters is still left then it will break ;
			if ((count_alphabet == 2) && (count_number == 0 || count_ALPHABET == 0 || count_ALPHABET == 1 || count_s_symbol == 0))
				break;

			key = getKey();
			password = password + alphabet[key];
			count_alphabet++;
			count++;
			break;

		case 2:
			// following if condition will check if minimum requirement of
			// ALPHABET character has been fulfilled or not
			// in case it has been fulfilled and minimum requirements of
			// other characters is still left then it will break ;
			if ((count_ALPHABET == 2) && (count_number == 0 || count_alphabet == 0 || count_alphabet == 1 || count_s_symbol == 0))
				break;
			key = getKey();
			password = password + ALPHABET[key];
			count_ALPHABET++;
			count++;
			break;

		case 3:
			// following if condition will check if minimum requirement
			// of Numbers  has been fulfilled or not
			// in case it has been fulfilled and minimum requirements of
			// other characters is still left then it will break ;
			if ((count_number == 1) && (count_alphabet == 0 || count_alphabet == 1 || count_ALPHABET == 1 || count_ALPHABET == 0 || count_s_symbol == 0))
				break;

			key = getKey();
			key = key % 10;
			password = password + number[key];
			count_number++;
			count++;
			break;

		case 4:
			// following if condition will check if minimum requirement of
			// Special symbol character has been fulfilled or not
			// in case it has been fulfilled and minimum requirements of
			// other characters is still left then it will break ;
			if ((count_s_symbol == 1) && (count_alphabet == 0 || count_alphabet == 1 || count_ALPHABET == 0 || count_ALPHABET == 1 || count_number == 0))
				break;

			key = getKey();
			key = key % 6;
			password = password + s_symbol[key];
			count_s_symbol++;
			count++;
			break;
		}
	}
	return password;
}
void FIN::ETI::Exchange_Processor::updateConfigFile(std::string password_)
{
	std::ifstream file;
	std::vector < std::string > fileData;
	std::string line;
	int positionOfPasswordFiled = 0, posOfNewPassField = 0, posOfDateField = 0;
	std::string confPath = _path.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
	file.open(confPath);
	int lineNum = 0;
	if (file.is_open()) {
		while (file) {
			getline(file, line);
			fileData.push_back(line);
			if (line.find("Password=") != std::string::npos) {
				positionOfPasswordFiled = lineNum;
			}
			lineNum++;
		}
		file.close();
	}
	fileData[positionOfPasswordFiled] = "Password=" + password_;
	/*fileData[posOfNewPassField] = ("NEW_PASSWORD=" + newPassword);
	fileData[posOfDateField] = ("PASSWORD_RESET_DATE=" + date);*/

	std::ofstream fout;
	fout.open(confPath);
	if (fout.is_open()) {
		//std::cout<<"FoutOpened Successfully\n";

		for (int i = 0; i < fileData.size() - 1; i++)
		{
			fout << fileData[i] << "\n";
			fout.flush();
		}
		fout.close();
	}
}
void FIN::ETI::Exchange_Processor::readCustodian(std::string fileName)
{
	std::ifstream file;
	std::string line;

	file.open(fileName);
	if (file.is_open()) {
		while (getline(file, line))
		{

			if (line != "")
			{
				size_t pos = line.find(':');

				if (pos != std::string::npos) {
					std::string key = line.substr(0, pos);
					std::string value = line.substr(pos + 1);

					LOG_INFO << key << " : " << value;
					std::cout << "[INFO ]Custodian data : " << key << " : " << value << std::endl;
					custodian[key] = value;
				}
			}
		}

		file.close();
	}

}
void FIN::ETI::Exchange_Processor::sendResponseToOMS(char* response, int bodyLen, int messageType, uint64_t orderId)
{
	if (!rabbit_udp)
		_omsRabbitSender.sendToOMS(response, bodyLen, messageType, orderId);
	else
		oms_udp_sender.sendToOMS(response, bodyLen, messageType, orderId);
}

void FIN::ETI::Exchange_Processor::sendDeleteAllBroadcastMsg()
{

	DeleteAllOrderBroadcastT response;

	//MessageHeaderOut
	response.MessageHeaderOut.BodyLen = sizeof(DeleteAllOrderBroadcastT);
	response.MessageHeaderOut.TemplateID = TID_DELETE_ALL_ORDER_BROADCAST;

	//RBCHeaderME
	response.RBCHeaderME.SendingTime = 0;
	response.RBCHeaderME.PartitionID = 0;
	std::strcpy(response.RBCHeaderME.ApplMsgID, "0");
	response.RBCHeaderME.ApplID = 0;
	response.RBCHeaderME.ApplResendFlag = 0;
	response.RBCHeaderME.LastFragment = 0;

	//MessageBody
	response.MassActionReportID = 0;
	response.MarketSegmentID = 0;
	response.TargetPartyIDSessionID = 0;
	response.NoNotAffectedOrders = 0;			//It's show number of repeating group
	response.MassActionReason = 6;				//6 = Session loss or logout
	response.ExecInst = 1;						//1 = Persistent Order(FIX value-H)


	onMessage(&response);
	/*sendResponseToOMS((char*)&response, response.MessageHeaderOut.BodyLen, 4, TID_DELETE_ALL_ORDER_BROADCAST);
	std::cout << "[INFO ] DeleteAllOrderBroadcastT reason " << std::to_string(response.MassActionReason) << std::endl;
	LOG_INFO << "send DeleteAllBroadcast";*/
}



