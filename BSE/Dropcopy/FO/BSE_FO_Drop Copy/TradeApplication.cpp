#include"TradeApplication.h"



TradeApplication::TradeApplication(SessionSettings* s, boost::asio::io_service& ioService) :ExchangeConnection(s, ioService)
{
	//read contract file and store
	readLinesFromFile(s->contractFilePath);

	//read live trading session id
	storeTradingSessionID(s->TradingSessionIdPath);

	settingOrder = s;
	_st = new SlidingWindowThrottling(1000	, 1000);

	std::thread heartbeat_handler(&TradeApplication::initiateHeartbeat, this, settingOrder->heartbeat);
	heartbeat_handler.detach();

	countSessionId = 0;

	addPartitionID1 = s->partitionID + 1;

}



void TradeApplication::readLinesFromFile(const std::string fileName) {
	try
	{
		cout << "reading contract file. wait..." << endl;
		std::ifstream inputFile(fileName);
		if (!inputFile.is_open()) {
			std::cerr << "Unable to open the contract file " << fileName << "\n";
			LOG_INFO << "Unable to open the contract file ";
		}
		else {

			std::string line;

			while (std::getline(inputFile, line)) {
				std::istringstream ss(line);

				std::string token;
				std::vector<std::string> tokens;
				int count = 0;
				while (std::getline(ss, token, ',')) {
					switch (count)
					{
					case 0:
					case 2:
					case 4:
					case 5:
					case 6:
					case 16:
					case 18:
					case 75:
						tokens.push_back(token);
					}
					count++;
					//tokens.push_back(token);
				}
				int securityNumber;
				if (firstLine) {
					securityNumber = 0;
					firstLine = false;
				}
				else {
					if (!tokens.empty()) {
						securityNumber = std::stoi(tokens.front());
					}
				}
				contractFileData.push_back(make_pair(securityNumber, tokens));

			}
			inputFile.close();
		}
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_readLinesFromFile]" << ex.what();
	}
}

std::string TradeApplication::findValue(int64_t keyToFind, int pos) {
	
	try
	{
		std::string val;

		for (const auto& pair : contractFileData) {
			if (pair.first == keyToFind) {
				val = pair.second[pos];
				return val;
			}
		}

		return "notfound";
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_findValue]" << ex.what();
	}

}

void TradeApplication::GenerateSequenceNum()
{
	++SeqNum;
}

void TradeApplication::GenerateApllSeqNum()
{
	++ApllSeq;
}

void TradeApplication::ConnectToExchange_GatewayRequest()
{
	GatewayRequestT Gateway_Req;
	Gateway_Req.MessageHeaderIn.BodyLen = 96;
	Gateway_Req.MessageHeaderIn.TemplateID = TID_GATEWAY_REQUEST;
	strcpy_s(Gateway_Req.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(Gateway_Req.MessageHeaderIn.Pad2, "0");
	//GenerateSequenceNum();
	Gateway_Req.RequestHeader.MsgSeqNum = 1;
	Gateway_Req.RequestHeader.SenderSubID = NO_VALUE_UINT;
	Gateway_Req.PartyIDSessionID = settingOrder->SessionId;
	strcpy_s(Gateway_Req.DefaultCstmApplVerID, settingOrder->DefaultCstmApplVerID.c_str());
	strcpy_s(Gateway_Req.Password, settingOrder->Password.c_str());
	strcpy_s(Gateway_Req.Pad6, "0");

	if (sendToSSL((const char*)&Gateway_Req, sizeof(Gateway_Req)))
	{
		isGatewayRequestSend = true;
		cout << "[GATEWAY REQUEST SEND TO EXCHANGE ]" << endl;
		LOG_INFO << "[GATEWAY REQUEST SEND TO EXCHANGE ] ";
		//std::cout << "msg send to exchange" << std::endl;
	}
	else
	{
		isGatewayRequestSend = false;
		std::cout << "SSL not connect" << std::endl;
	}
}

void TradeApplication::SendSessionRegistrationRequest()
{
	SeqNum = 0;
	SessionRegistrationRequestT _sRegRequest;
	//this is MsgHeader part

	_sRegRequest.MessageHeaderIn.BodyLen = sizeof(SessionRegistrationRequestT);
	_sRegRequest.MessageHeaderIn.TemplateID = 10053;
	strcpy_s(_sRegRequest.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(_sRegRequest.MessageHeaderIn.Pad2, "0");

	//this is RequestHeader
	_sRegRequest.RequestHeader.MsgSeqNum = (SeqNum += 1);
	_sRegRequest.RequestHeader.SenderSubID = sessionSetting->Username;

	//this is MessageBody
	_sRegRequest.PartyIDSessionID = sessionSetting->SessionId;
	strcpy_s(_sRegRequest.Pad4, "0");
	_sRegRequest.Filler1 = 0;

	if (sendToTarget((const char*)&_sRegRequest, _sRegRequest.MessageHeaderIn.BodyLen))
	{
		std::cout << "Session Registraion Request" << std::endl;
		//log->logPrint(_sRegRequest);
	}
}

void TradeApplication::ConnectToExchange_LogonRequest()
{
	LogonRequestT Logon_Req;
	Logon_Req.MessageHeaderIn.BodyLen = 280;
	Logon_Req.MessageHeaderIn.TemplateID = TID_LOGON_REQUEST;
	strcpy_s(Logon_Req.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(Logon_Req.MessageHeaderIn.Pad2, "0");
	//GenerateSequenceNum();
	Logon_Req.RequestHeader.MsgSeqNum = (SeqNum += 1);
	Logon_Req.RequestHeader.SenderSubID = 0;
	Logon_Req.HeartBtInt = settingOrder->heartbeat;
	Logon_Req.PartyIDSessionID = settingOrder->SessionId;
	strcpy_s(Logon_Req.DefaultCstmApplVerID, settingOrder->DefaultCstmApplVerID.c_str());
	strcpy_s(Logon_Req.Password, settingOrder->Password.c_str());
	strcpy(Logon_Req.ApplUsageOrders, settingOrder->ApplUsageOrders.c_str());
	strcpy(Logon_Req.ApplUsageQuotes, settingOrder->ApplUsageQuotes.c_str());
	strcpy(Logon_Req.OrderRoutingIndicator, settingOrder->OrderRoutingIndicator.c_str());
	strcpy_s(Logon_Req.ApplicationSystemName, settingOrder->ApplicationSystemName.c_str());
	strcpy_s(Logon_Req.ApplicationSystemVersion, settingOrder->ApplicationSystemVersion.c_str());
	strcpy_s(Logon_Req.FIXEngineVersion, "0");
	strcpy_s(Logon_Req.FIXEngineName, "0");
	strcpy_s(Logon_Req.FIXEngineVendor, "0");
	strcpy_s(Logon_Req.ApplicationSystemVendor, settingOrder->ApplicationSystemVendor.c_str());
	strcpy_s(Logon_Req.Pad3, "0");

	if (sendToExchange((unsigned char*)&Logon_Req, sizeof(Logon_Req)))
	{
		cout << "[SEESION LOGON REQUEST TO EXCHANGE ]" << endl;
		LOG_INFO << "[SEESION LOGON REQUEST TO EXCHANGE ]";
	}
}

void TradeApplication::userLogon()
{
	UserLoginRequestT User_Logon_Req;
	User_Logon_Req.MessageHeaderIn.BodyLen = 64;
	User_Logon_Req.MessageHeaderIn.TemplateID = TID_USER_LOGIN_REQUEST;
	strcpy_s(User_Logon_Req.MessageHeaderIn.NetworkMsgID, "0");
	strcpy_s(User_Logon_Req.MessageHeaderIn.Pad2, "0");
	//GenerateSequenceNum();
	User_Logon_Req.RequestHeader.MsgSeqNum = (SeqNum += 1);
	User_Logon_Req.RequestHeader.SenderSubID = NO_VALUE_UINT;
	User_Logon_Req.Username = settingOrder->Username;
	strcpy_s(User_Logon_Req.Password, settingOrder->Password.c_str());
	strcpy_s(User_Logon_Req.Pad4, "0");

	sendToExchange((unsigned char*)&User_Logon_Req, sizeof(User_Logon_Req));

	cout << "[USER LOGON REQUEST ]" << endl;
	LOG_INFO<<"[USER LOGON REQUEST ]";
}

void TradeApplication::initiateHeartbeat(uint32_t heartbeat)
{
	
	try
	{
		while (true)
		{
			Sleep(heartbeat);
			if (isConnected)
			{
				GenerateHeartBeat();
				//cout << "sendHeartbeat" << endl;
			}
		}
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_initiateHeartbeat]" << ex.what();
	}
}


void TradeApplication::GenerateHeartBeat()
{
	try
	{
		HeartbeatT HeartBT_Req;
		HeartBT_Req.MessageHeaderIn.BodyLen = 16;
		HeartBT_Req.MessageHeaderIn.TemplateID = TID_HEARTBEAT;
		strcpy_s(HeartBT_Req.MessageHeaderIn.NetworkMsgID, "0");
		strcpy_s(HeartBT_Req.MessageHeaderIn.Pad2, "0");

		sendToExchange((unsigned char*)&HeartBT_Req, sizeof(HeartBT_Req));

		LOG_INFO << "[Heartbeat send ]";
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_GenerateHeartBeat]" << ex.what();
	}
}

void TradeApplication::disableConnectionVariable()
{
	isSSLConnect = false;
	isGatewayRequestSend = false;
	isConnected = false;
	_loggedIn = false;
}

void TradeApplication::connectToBSEExchange()
{
	while (1)
	{
		try
		{
			initializeSSLSocket();
			if (isSSLConnect)
			{
				ConnectToExchange_GatewayRequest();
				if (isGatewayRequestSend)
				{
					std::cout << "Gateway Request send successfully." << std::endl;
					if (!readFromSSL())
						throw std::runtime_error("Failed to read from SSL");

					CloseSSLSocket();
					createSocket();
					SendSessionRegistrationRequest();

					try
					{
						while (1)
						{
							if (!receiveFromTarget())
								throw std::runtime_error("Failed to read from receiving");
						}
					}
					catch (const std::exception& e)
					{
						closeConnectionSocket();
						LOG_INFO << "Exception rcvd during receiveFromTarget. " << e.what();
						throw std::runtime_error("Gateway request failed to send1");
					}
				}
				else
				{
					throw std::runtime_error("Gateway request failed to send");
				}
			}
			else
			{
				throw std::runtime_error("SSL connection failed");
			}
		}
		catch (const std::exception& e)
		{
			std::cout << "Exchange disconnect. \nReconnecting after 10 Seconds : " << e.what() << std::endl;
			LOG_INFO << "Exception rcvd in main loop. " << e.what();

			disableConnectionVariable();
			std::this_thread::sleep_for(std::chrono::seconds(10));
			continue;
		}
	}
}

//void TradeApplication::startCommunication()
//{
//	if (!_gateway_request_done)
//	{
//		LOG_INFO << "Starting Communication with exchange";
//		_seqNu = 0;
//		ConnectToExchange_GatewayRequest();
//	}
//}


void TradeApplication::storeTradingSessionID(std::string path) {
	try {

		std::ifstream file(path);

		if (!file.is_open()) {
			std::cerr << "Error opening file: " << path << std::endl;
			LOG_INFO << "Error trading session id store opening file: ";
		}
		else {
			std::string line;
			std::unordered_set<int> uniqueSessionIDs; // Store unique session IDs

			while (std::getline(file, line)) {
				line.erase(std::remove_if(line.begin(), line.end(), [](unsigned char c) { return std::isspace(c); }), line.end());

				if (!line.empty()) {
					int sessionID = std::stoi(line);
					if (uniqueSessionIDs.find(sessionID) == uniqueSessionIDs.end()) {
						allTradingSessionID.push_back(sessionID);
						uniqueSessionIDs.insert(sessionID);
					}
					else {
						LOG_INFO << "Duplicate trading session ID found: " << sessionID;
					}
				}
			}

			file.close();
		}
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_storeTradingSessionID]" << ex.what();
	}
}

bool TradeApplication::receiveFromTarget()
{
	try
	{
		memset(buffer, 0, 65535);
		int dataRcvd = m_socket->read_some(boost::asio::buffer(buffer, 65535));
		//LOG_INFO << "RCVED PACKET " << dataRcvd;
		//std::cout << dataRcvd << std::endl;
		MessageHeaderOutCompT* hdr = nullptr;
		int pos = 0;
		int length = 0;
		while (pos < dataRcvd)
		{
			hdr = (MessageHeaderOutCompT*)(buffer + pos);
			length = hdr->BodyLen;
			if (length <= 0)
			{
				LOG_INFO << " Packet length " << length << " total packet " << dataRcvd << " pos " << pos;

				int leftbyte = dataRcvd - pos;
				memcpy(buffer, (char*)buffer + pos, leftbyte);
				dataRcvd = leftbyte;
				dataRcvd += m_socket->read_some(boost::asio::buffer(buffer + leftbyte, 65535 - leftbyte));
				pos = 0;

				continue;
			}
			else if (pos + length > dataRcvd)
			{
				LOG_INFO << " Packet length is greater than  datarecvd len  " << length << " total packet " << dataRcvd << " pos " << pos;

				int needMoreBytes = (pos + length) - dataRcvd;
				memcpy(buffer, (char*)buffer + pos, (dataRcvd - pos));
				dataRcvd = dataRcvd - pos;
				dataRcvd += m_socket->read_some(boost::asio::buffer(buffer + dataRcvd, needMoreBytes));
				pos = 0;

				continue;
			}
			HandleData(buffer + pos, hdr->TemplateID, hdr->BodyLen);

			pos += hdr->BodyLen;
		}
		return true;
	}
	catch (const std::exception& ex)
	{
		std::cout << "receive exception " << ex.what() << std::endl;
		return false;
	}
	return false;
}

void TradeApplication::HandleData(char* data, uint16_t templateID, int length)
{
	//cout <<"TemplateID : " << templateID << endl;
	char* resMsg = nullptr;
	bool memoryAllocated = false;

	//decrypt msg
	if (isSecureBoxDone)
	{
		memset(decryptedtext, 0, 1000);
		decryptedtext_len = 0;
		performDecrypt(((unsigned char*)data + 8), (length - 8), decryptedtext, &decryptedtext_len);

		resMsg = new char[8 + decryptedtext_len];
		std::memcpy(resMsg, data, 8);
		std::memcpy(resMsg + 8, decryptedtext, decryptedtext_len);

	}
	else
	{
		resMsg = data;
	}
	try
	{
		switch (templateID)
		{
		//case TID_GATEWAY_RESPONSE						:onMessage((GatewayResponseT*)data); break;
		case TID_SESSION_REGISTRATION_RESPONSE			:onMessage((SessionRegistrationResponseT*)data);	break;
		case TID_LOGON_RESPONSE							:onMessage((LogonResponseT*)resMsg); break;
		case TID_USER_LOGIN_RESPONSE					:onMessage((UserLoginResponseT*)resMsg); break;
		case TID_SUBSCRIBE_RESPONSE						:onMessage((SubscribeResponseT*)resMsg); break;
		case TID_RETRANSMIT_RESPONSE					:onMessage((RetransmitResponseT*)resMsg); break;
		case TID_TRADE_BROADCAST						:onMessage((TradeBroadcastT*)resMsg); break;
		case TID_REJECT									:onMessage((RejectT*)resMsg); break;
		case TID_LOGOUT_RESPONSE						:onMessage((LogoutResponseT*)resMsg); break;
		case TID_USER_LOGOUT_RESPONSE					:onMessage((UserLogoutResponseT*)resMsg); break;
		case TID_HEARTBEAT_NOTIFICATION					:onMessage((HeartbeatNotificationT*)resMsg); break;
		case TID_RISK_COLLATERAL_ALERT_ADMIN_BROADCAST	:onMessage((RiskCollateralAlertAdminBroadcastT*)resMsg); break;
		case TID_FORCED_LOGOUT_NOTIFICATION				:onMessage((ForcedLogoutNotificationT*)resMsg); break;
		
		default:
			cout << "TEMPLATE ID NOT FAOUND " << templateID << endl;
			std::string myString(data);
			LOG_INFO << "TEMPLATE ID NOT FAOUND " << templateID<<" : " << myString;
			break;
		}

	}
	catch (std::exception& ex)
	{
		LOG_INFO << "[exception_HandleData]" << ex.what();
	}
	if (memoryAllocated)
	{
		delete[] resMsg;  // Free memory if allocated
	}
}

void TradeApplication::onMessage(SessionRegistrationResponseT* msg)
{
	std::string sessionStatus(msg->Status);
	std::string sessionText(msg->VarText);
	//logPrint(msg);
	if (sessionStatus == "Y")
	{
		std::cout << "Session Registration Response" << std::endl;
		ConnectToExchange_LogonRequest();
		isSecureBoxDone = true;
	}
	else
	{
		LOG_INFO << "Status : " << sessionStatus << " Reason : " << sessionText;
		std::cout << "Session Registration error : " << sessionText << std::endl;
	}
}

void TradeApplication::onMessage(LogonResponseT* lgRes)
{
	cout << "[SESSION LOGON RESPONSE]" << endl;
	LOG_INFO << "[SESSION LOGON RESPONSE] "/*<<lgRes->getString()*/;
	isConnected = true;
	LOG_INFO << lgRes->ThrottleNoMsgs << " " << lgRes->ThrottleTimeInterval << " " << lgRes->ThrottleDisconnectLimit;
	_st = new SlidingWindowThrottling(lgRes->ThrottleNoMsgs - 1, lgRes->ThrottleTimeInterval);
	userLogon();
}

void TradeApplication::onMessage(UserLoginResponseT* ulRes)
{
	cout << "[USER LOGON RESPONSE]" << endl;
	LOG_INFO << "[USER LOGON RESPONSE] Successfully Logged IN with Exchange. \nNow, start download recovery trade.";
	std::cout << "Successfully Logged IN with Exchange." << endl << "Now, starting download recovery trade." << std::endl;
	_loggedIn = true;

	tradeSessionCount = 0;
	checkSubscription = true;
	pId = 1;

	if (getSize() >= 1) {
		val = allTradingSessionID[tradeSessionCount];
		int pid = pId;

		_st->checkThrottleLimit();
		sendSubscriptionRequest(1, val);	//Subscribe SessionID

		_st->checkThrottleLimit();
		LOG_INFO << "RetransRequest_trade : " << "Partition : " << pid << " SessionID :" << val;
		RetransRequest_trade(0, pId++, val);		//, pId
	}
	//cout << "send request partition : " << pId << endl;
}
void TradeApplication::onMessage(SubscribeResponseT* subRes)
{
	countSessionId++;

	cout << "[Subscribe Response]" << endl;
	LOG_INFO << "Subscribe Response";

	if (allTradingSessionID.size() == countSessionId) {
		std::cout << "Total " << countSessionId << " SessionID subscribe sucessfully." << std::endl;
	}
}
void TradeApplication::onMessage(RetransmitResponseT* retRes)
{
	//std::cout << "[Retransmit Response]" << std::endl;
	LOG_INFO << "Retransmit Response ApplTotalMessageCount : " << retRes->ApplTotalMessageCount << " pId : " << pId;
	if (pId == 1 && endSeqNumber == lastSeqNumber && getSize() == tradeSessionCount) {
		if (checkSubscription) {
			//int s = getSize();
			//for (int i = 0; i < s; i++) {
			//	uint32_t val = allTradingSessionID[i];
			//	_st->checkThrottleLimit();
			//	sendSubscriptionRequest(1, val);	//Trade Data
			//}
			checkSubscription = false;
			std::cout << "Recovery completed. Now, streaming live trade..." << std::endl;
			LOG_INFO << "Recovery completed.";
		}
	}

	if (checkSubscription) {
		//assion value endSeqNumbar , lastSeqNumbar for recovery
		endSeqNumber = retRes->ApplEndSeqNum;
		lastSeqNumber = retRes->RefApplLastSeqNum;

		/*cout << "endSeqNumber : " << endSeqNumber << endl;
		cout << "lastSeqNumber : " << lastSeqNumber << endl;
		cout << "pId : " << pId << endl;
		cout << "trade Session Id : " << allTradingSessionID[tradeSessionCount] << endl;
		cout << "trade Session Id size : " << allTradingSessionID.size() << endl;*/

		if (endSeqNumber == lastSeqNumber && pId <= sessionSetting->partitionID) {
			//cout << "send request on retransmit (10009)" << endl;
			if (getSize() != tradeSessionCount) {
				if (pId == 1)
				{
					_st->checkThrottleLimit();
					sendSubscriptionRequest(1, allTradingSessionID[tradeSessionCount]);	//Subscribe SessionID
				}
				_st->checkThrottleLimit();
				val = allTradingSessionID[tradeSessionCount];
				RetransRequest_trade(0, pId++, allTradingSessionID[tradeSessionCount]);
			}
			//cout << "send request partition : " << pId << endl;
		}
		if (pId == addPartitionID1) {
			if (getSize() != tradeSessionCount) {
				++tradeSessionCount;
			}
			pId = 1;
		}

		/*cout <<"ApplEndSeqNum : " << retRes->ApplEndSeqNum << endl;
		cout <<"RefApplLastSeqNum : " << retRes->RefApplLastSeqNum << endl;
		cout <<"ApplTotalMessageCount : " << retRes->ApplTotalMessageCount << endl;*/
	}
}
void TradeApplication::onMessage(TradeBroadcastT* tradeBroadcast)
{
	// Check if the securityid exists
	//auto sec_it = packet_loss_check.find(tradeBroadcast->RootPartyIDSessionID);
	//if (sec_it != packet_loss_check.end()) {
	//	// SecurityID exists, now check if the partition exists
	//	auto& inner_map = sec_it->second;  // Access the inner map for partitions
	//	auto part_it = inner_map.find(tradeBroadcast->RBCHeader.PartitionID);

	//	if (part_it != inner_map.end()) {
	//		// Both securityid and partition exist, check for missing sequence numbers
	//		if ((part_it->second + 1 != tradeBroadcast->RBCHeader.ApplSeqNum) && tradeBroadcast->RBCHeader.ApplSeqNum != 1) {
	//			// Detected a missing sequence
	//			std::cout << "Missing trade partition: " << tradeBroadcast->RBCHeader.PartitionID
	//				<< " : seq: " << tradeBroadcast->RBCHeader.ApplSeqNum
	//				<< " : SecurityID: " << tradeBroadcast->RootPartyIDSessionID << std::endl;
	//		}
	//		// Update sequence number to current ApplSeqNum
	//		part_it->second = tradeBroadcast->RBCHeader.ApplSeqNum;
	//	}
	//	else {
	//		// Partition does not exist, insert it with the sequence number
	//		inner_map[tradeBroadcast->RBCHeader.PartitionID] = tradeBroadcast->RBCHeader.ApplSeqNum;  // Insert new partition with sequence number
	//	}
	//}
	//else {
	//	// SecurityID does not exist, insert it with the partition and sequence number
	//	packet_loss_check[tradeBroadcast->RootPartyIDSessionID][tradeBroadcast->RBCHeader.PartitionID] = tradeBroadcast->RBCHeader.ApplSeqNum;  // Insert new securityid and partition
	//}



	//auto result = packet_loss_check.find(tradeBroadcast->RBCHeader.PartitionID);
	//if (result != packet_loss_check.end()) {
	//	if ((result->second) + 1 != tradeBroadcast->RBCHeader.ApplSeqNum && tradeBroadcast->RBCHeader.ApplSeqNum != 1)
	//	{
	//		LOG_INFO << "Missing trade partition:" << tradeBroadcast->RBCHeader.PartitionID <<" : seq: "<<tradeBroadcast->RBCHeader.ApplSeqNum << " : OrderID : " << tradeBroadcast->OrderID;
	//		cout << "Missing trade partition:" << tradeBroadcast->RBCHeader.PartitionID << " : seq: " << tradeBroadcast->RBCHeader.ApplSeqNum << " : OrderID : " << tradeBroadcast->OrderID  << endl;
	//	}
	//	result->second = tradeBroadcast->RBCHeader.ApplSeqNum;
	//}
	//else {
	//	packet_loss_check[tradeBroadcast->RBCHeader.PartitionID] = tradeBroadcast->RBCHeader.ApplSeqNum;    
	//}


	//tradeCount++;
	//std::cout << "Trade  : time : " << GetTransactTime(tradeBroadcast->TransactTime) << std::endl;
	LOG_INFO <<"Trade " << " SecurityID : " << tradeBroadcast->SecurityID
		<< ", SessionID : " << tradeBroadcast->RootPartyIDSessionID
		<< ", PartitionID : " << tradeBroadcast->RBCHeader.PartitionID
		<< ", Side : " << tradeBroadcast->Side
		<< ", Price : " << convetPriceToDecimal(tradeBroadcast->LastPx)
		<< ", Time : " << GetTransactTime(tradeBroadcast->TransactTime)
		<< ", OrderID : " << tradeBroadcast->OrderID;

	onMessageToTxt(tradeBroadcast);

	//cout << "ApplSeqNum : " << tradeBroadcast->RBCHeader.ApplSeqNum << endl;
	//cout << "lastSeqNumber : " << lastSeqNumber << endl;
	//cout << "PartitionID : " << tradeBroadcast->RBCHeader.PartitionID << endl;
	//cout << "endSeqNumber : " << endSeqNumber << endl;
	//cout << "SessionID " << tradeBroadcast->RootPartyIDSessionID << endl;

	if (checkSubscription) {
		applSeqNum = tradeBroadcast->RBCHeader.ApplSeqNum;
		if (applSeqNum == endSeqNumber)
		{

			if (applSeqNum < lastSeqNumber) {
				applSeqNum += 1;
				_st->checkThrottleLimit();
				RetransRequest_trade(applSeqNum, tradeBroadcast->RBCHeader.PartitionID, tradeBroadcast->RootPartyIDSessionID);	//, pId
				//cout<<"applSeqNum "<< applSeqNum<<" : " << tradeBroadcast->RBCHeader.PartitionID << " " << tradeBroadcast->RootPartyIDSessionID << endl;
			}
		}
	}
}
void TradeApplication::onMessage(RejectT* rejT)
{
	std::cout << "[REJECT REASON ]" << rejT->SessionRejectReason << " : " << rejT->VarText << std::endl;
	LOG_INFO << "[REJECT REASON] " << rejT->SessionRejectReason << " : " << rejT->VarText;

	//check for business unit rejection (trader id rejected)
	std::string_view errorMsg(rejT->VarText, rejT->VarTextLen);
	if (rejT->SessionRejectReason == ErrorForBUCode && errorMsg == ErrorForBU)
	{
		FIN::setConsoleColor(4);
		std::cout << "Your session id : " << val << " is already deleted by exchanged." << std::endl;
		FIN::setConsoleColor(7);
		LOG_INFO << "Your session id : " << val << " is already deleted by exchanged.";

		//remove from file
		delete_securityid_from_file(std::to_string(val));

		//remove from memory
		remove_securityid_from_memory(val);

		//reset partition
		pId = 1;

		//check next iteration for retransmit request
		if (getSize() != tradeSessionCount)
		{
			_st->checkThrottleLimit();
			sendSubscriptionRequest(1, val);	//Subscribe SessionID

			_st->checkThrottleLimit();
			val = allTradingSessionID[tradeSessionCount];
			RetransRequest_trade(0, 1, val);		//(seqnum, partitionid, sessionid)
		}

		////handle if session is last in memory then subscribe all session id
		//if (pId == 1 && endSeqNumber == lastSeqNumber && getSize() == tradeSessionCount) {
		//	if (checkSubscription) {
		//		int s = getSize();
		//		for (int i = 0; i < s; i++) {
		//			val = allTradingSessionID[i];
		//			_st->checkThrottleLimit();
		//			sendSubscriptionRequest(1, val);	//Trade Data
		//		}
		//		checkSubscription = false;
		//	}
		//}

		return;
	}

	if(rejT->SessionRejectReason!=100)
		sendLogoutRequest();
}

void TradeApplication::remove_securityid_from_memory(uint64_t id)
{
	auto it = std::find(allTradingSessionID.begin(), allTradingSessionID.end(), id);

	if (it != allTradingSessionID.end())
	{
		allTradingSessionID.erase(it);
		LOG_INFO << "Removed security id from memory : " << id;
	}
	else {
		LOG_INFO << "Security id not found" << id;
	}
}

void TradeApplication::delete_securityid_from_file(std::string security_id)
{
	try
	{
		std::vector<std::string> symVec;
		std::vector< std::string >::iterator itr;

		std::fstream myfile(settingOrder->TradingSessionIdPath, std::ios::in);
		std::string line;

		int countLine = 0;

		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				if (line != security_id && !line.empty())
				{
					symVec.push_back(line);
					countLine++;
				}
			}
			myfile.close();
		}

		std::ofstream myFile(settingOrder->TradingSessionIdPath, std::ios::trunc);
		if (symVec.size() > 0)
		{
			if (myFile.is_open())
			{

				for (itr = symVec.begin(); itr != symVec.end(); itr++)
				{
					--countLine;
					myFile << (*itr);
					if (symVec.size() > 0 && countLine != 0)
						myFile << "\n";
				}
				myFile.close();

			}
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Unable to read Token file : " << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}
}

void TradeApplication::onMessage(LogoutResponseT* logRes)
{
	cout << "Session Logout Successfully" << endl;
	LOG_INFO << "Session Logout Successfully : " << "Template ID : " << logRes->MessageHeaderOut.TemplateID;
	
	shutdownSocket();
	closeSocket();
	std::cout << "Close Socket Successfully" << std::endl;
	LOG_INFO << "Close Socket Successfully";
}
void TradeApplication::onMessage(UserLogoutResponseT* uLogRes)
{
	cout << "User Logout Response : " << endl;
	LOG_INFO << "User Logout Response : " << "Template ID : " << uLogRes->MessageHeaderOut.TemplateID;
}
void TradeApplication::onMessage(HeartbeatNotificationT* hbNotification)
{
	GenerateHeartBeat();
}
void TradeApplication::onMessage(RiskCollateralAlertAdminBroadcastT* rcaaBroadcast)
{
	//cout << rcaaBroadcast->MessageHeaderOut.TemplateID <<" : " << rcaaBroadcast->VarText<<endl;
	error_msg = rcaaBroadcast->VarText;

	LOG_INFO << "RiskCollateralAlertAdminBroadcastT " << ",template id : " <<
		rcaaBroadcast->MessageHeaderOut.TemplateID
		<< ", Reason : " << error_msg;
}
void TradeApplication::onMessage(ForcedLogoutNotificationT* flNotification)
{
	error_logout = flNotification->VarText;
	cout << flNotification->MessageHeaderOut.TemplateID << " : Reason : " << error_logout << endl;
	LOG_INFO << "flNotification " << ",template id : " <<
		flNotification->MessageHeaderOut.TemplateID
		<< " : Reason : " << error_logout;
}

int TradeApplication::getSize() {
	try
	{
		return allTradingSessionID.size();
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_getSize]" << ex.what();
	}
}

void TradeApplication::sendLogoutRequest()
{
	LogoutRequestT lReq;
	lReq.MessageHeaderIn.BodyLen = sizeof(lReq);
	lReq.MessageHeaderIn.TemplateID = TID_LOGOUT_REQUEST;
	memset(lReq.MessageHeaderIn.NetworkMsgID, NO_VALUE_STR, 8);
	memset(lReq.MessageHeaderIn.Pad2, NO_VALUE_STR, 2);
	lReq.RequestHeader.MsgSeqNum = (SeqNum += 1);
	lReq.RequestHeader.SenderSubID = NO_VALUE_UINT;

	auto buffer_Data = boost::asio::buffer(&lReq, lReq.MessageHeaderIn.BodyLen);
	sendToExchange((unsigned char*)&lReq, sizeof(lReq));

	std::cout << "[Session Logout Request] : "<< std::endl;
	LOG_INFO << "Session Logout Request ";

}

void TradeApplication::sendSubscriptionRequest(uint8_t val, uint32_t Id)
{
	try
	{
		SubscribeRequestT subReq;
		subReq.MessageHeaderIn.BodyLen = 32;
		subReq.MessageHeaderIn.TemplateID = TID_SUBSCRIBE_REQUEST;
		memset(subReq.MessageHeaderIn.NetworkMsgID, NO_VALUE_STR, 8);
		memset(subReq.MessageHeaderIn.Pad2, NO_VALUE_STR, 2);
		subReq.RequestHeader.MsgSeqNum = (SeqNum += 1);
		subReq.RequestHeader.SenderSubID = NO_VALUE_UINT;
		subReq.SubscriptionScope = Id; //settingOrder->TradingSessionIdPath;	//0xFFFFFFFF;
		subReq.RefApplID = val;
		memset(subReq.Pad3, 0x00, 3);

		sendToExchange((unsigned char*)&subReq, sizeof(subReq));

		std::cout << "[Subscribe Request] : " << Id << std::endl;
		LOG_INFO << "Subscribe Request " << Id;
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_sendSubscriptionRequest]" << ex.what();
	}
}

void TradeApplication::RetransRequest_trade(uint64_t start_seq, int pID, uint32_t sessionId)	//, int pID
{
	try
	{
		if (start_seq == 0)
			start_seq = 1;

		RetransmitRequestT RetransmitReq;
		RetransmitReq.MessageHeaderIn.BodyLen = 48;
		RetransmitReq.MessageHeaderIn.TemplateID = TID_RETRANSMIT_REQUEST;
		memset(RetransmitReq.MessageHeaderIn.NetworkMsgID, NO_VALUE_STR, 8);
		memset(RetransmitReq.MessageHeaderIn.Pad2, NO_VALUE_STR, 2);
		//GenerateSequenceNum();
		RetransmitReq.RequestHeader.MsgSeqNum = (SeqNum += 1);
		RetransmitReq.RequestHeader.SenderSubID = NO_VALUE_UINT;
		RetransmitReq.ApplBegSeqNum = start_seq;
		RetransmitReq.ApplEndSeqNum = NO_VALUE_ULONG;
		RetransmitReq.SubscriptionScope = sessionId;		// 0xFFFFFFFF; 
		RetransmitReq.PartitionID = pID;		//settingOrder->PartitionId;
		RetransmitReq.RefApplID = 1;
		strcpy_s(RetransmitReq.Pad1, "");

		sendToExchange((unsigned char*)&RetransmitReq, sizeof(RetransmitReq));

		//cout << "[Retransmit REQUEST] " << endl;
		//cout << "Trading SessionID : " << settingOrder->TradingSessionId << endl;
		LOG_INFO << "Retransmit Request " << "pid: " << pID << " Sessionid: " << sessionId;
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_RetransRequest_trade]" << ex.what();
	}
}


void TradeApplication::onMessageToTxt(TradeBroadcastT* msg)
{
	if (msg->TransactTime < 0)
	{
		LOG_INFO << "Invalid format";
		return;
	}
	//if (checkSubscription)
	//{
	//	// Check for duplicate trade
	//	std::string securityId = std::to_string(msg->SecurityID);
	//	std::string tradeId = std::to_string(msg->TradeID);
	//	LOG_INFO << securityId << " : " << tradeId;
	//	if (findDuplicateTrade(securityId+tradeId))
	//		return;
	//}
	
	tradeValue = getValueRespectedKey(msg->SecurityID);
	if (tradeValue.size() == 1)
	{
		std::cout << "SecurityID not found in Contract File, Don't write in file : " << msg->SecurityID << std::endl;
		LOG_INFO << "SecurityID not found in Contract File, Don't write in file" << msg->SecurityID;
		tradeValue.clear();
		return;
	}

	//int64_t keyToFind = msg->SecurityID;
	//std::string checkAvailable = findValue(keyToFind, 0);

	//// Check if SecurityID is found in Contract File
	//if (checkAvailable == "notfound") {
	//	std::cout << "SecurityID not found in Contract File, Don't write in file : " << msg->SecurityID << std::endl;
	//	LOG_INFO << "SecurityID not found in Contract File, Don't write in file" << msg->SecurityID;
	//	return;
	//}


	std::stringstream dropCopyStream;

	dropCopyStream << msg->SideTradeID << FIN::COMMA; // Trade Number
	dropCopyStream << GetTransactTime(msg->TransactTime) << FIN::COMMA; // Trade Date Time
	dropCopyStream << "11" << FIN::COMMA; // Trade Status  11
	dropCopyStream << "D" << FIN::COMMA; // Segment Indicator

	dropCopyStream << "O" << FIN::COMMA; // Settlement Type

	dropCopyStream << tradeValue[1] << FIN::COMMA; // Product Type
	dropCopyStream << tradeValue[7] << FIN::COMMA; // Product Code
	dropCopyStream << tradeValue[5] << FIN::COMMA; // Asset Code

	dropCopyStream << convertDateFormat(tradeValue[2]) << FIN::COMMA; // Expiry Date

	dropCopyStream << convetStrickToDecimal(tradeValue[3]) << FIN::COMMA; // Strike Price

	dropCopyStream << tradeValue[4] << FIN::COMMA; // Option Type
	dropCopyStream << tradeValue[6] << FIN::COMMA; // Series Code

	dropCopyStream << makeStringSideWise(msg, msg->Side);

	drop_file.pushToQueue(dropCopyStream.str());
	tradeValue.clear();
}

std::string TradeApplication::makeStringSideWise(TradeBroadcastT* msg, int side) {
	try {
		std::stringstream dropCopyStream;

		if (side == 1) {
			int firstPart = msg->RootPartyIDSessionID / 100000;
			dropCopyStream << firstPart << FIN::COMMA; // Buy Broker

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Broker

			dropCopyStream << convetPriceToDecimal(msg->LastPx) << FIN::COMMA; // Trade Price 

			dropCopyStream << msg->LastQty << FIN::COMMA; // Trade Quantity 

			dropCopyStream << msg->SecurityID << FIN::COMMA; // SecurityID  

			dropCopyStream << msg->SenderLocationID << FIN::COMMA; // Trade Buyer Location ID

			dropCopyStream << firstPart << FIN::COMMA; // Buy CM Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell CM Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Trade Seller Location ID

			dropCopyStream << "DBSBK0000181" << FIN::COMMA; // Buy Custodial Participant

			dropCopyStream << "N" << FIN::COMMA; // Buy Side Confirmation

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Custodial Participant

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Side Confirmation

			dropCopyStream << "U" << FIN::COMMA; // Buy Covered Uncovered Flag

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Covered Uncovered Flag

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Old Custodial Participant

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Old CM Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Old Custodial Participant

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Old CM Code

			int lastPart = msg->RootPartyIDSessionID % 10000;
			dropCopyStream << lastPart << FIN::COMMA; // Trade Buyer Terminal ID

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Trade Seller Terminal ID

			dropCopyStream << msg->OrderID << FIN::COMMA; // Buy Order No = OrderID

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Order No

			if (std::string(msg->ClientCode).substr(0, 3) == VCF)
				clientCode = VCF;
			else
				clientCode = removeTrailingSpaces(std::string(msg->ClientCode));

			dropCopyStream << clientCode << FIN::COMMA; // Buy Client Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Client Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Remarks

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Remarks

			dropCopyStream << "0" << FIN::COMMA; // Buy Position

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Position

			dropCopyStream << msg->PositionEffect << FIN::COMMA; // Buy Proprietor/ Client Flag

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Proprietor/ Client Flag

			dropCopyStream << GetTransactTime(msg->ActivityTime) << FIN::COMMA; // Buy Order Time Stamp 

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Order Time Stamp

			int indicator = msg->AggressorIndicator == 0 ? 1 : 0;
			dropCopyStream << indicator << FIN::COMMA; // Buy Order Active Flag

		}
		else {
			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Broker blank

			int firstPart = msg->RootPartyIDSessionID / 100000;
			dropCopyStream << firstPart << FIN::COMMA; // Sell Broker blank = member id

			dropCopyStream << convetPriceToDecimal(msg->LastPx) << FIN::COMMA; // Trade Price = Price

			dropCopyStream << msg->LastQty << FIN::COMMA; // Trade Quantity = LastQty

			dropCopyStream << msg->SecurityID << FIN::COMMA; // SecurityID  msg->SecurityID

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Trade Buyer Location ID = SenderLocationId

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy CM Code

			dropCopyStream << firstPart << FIN::COMMA; // Sell CM Code

			dropCopyStream << msg->SenderLocationID << FIN::COMMA; // Trade Seller Location ID

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Custodial Participant

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Side Confirmation

			dropCopyStream << "DBSBK0000181" << FIN::COMMA; // Sell Custodial Participant

			dropCopyStream << "N" << FIN::COMMA; // Sell Side Confirmation

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Covered Uncovered Flag

			dropCopyStream << "U" << FIN::COMMA; // Sell Covered Uncovered Flag

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Old Custodial Participant

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Old CM Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Old Custodial Participant

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Old CM Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Trade Buyer Terminal ID

			int lastPart = msg->RootPartyIDSessionID % 10000;
			dropCopyStream << lastPart << FIN::COMMA; // Trade Seller Terminal ID

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Order No

			dropCopyStream << msg->OrderID << FIN::COMMA; // Sell Order No

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Client Code

			if (std::string(msg->ClientCode).substr(0, 3) == VCF)
				clientCode = VCF;
			else
				clientCode = removeTrailingSpaces(std::string(msg->ClientCode));

			dropCopyStream << clientCode << FIN::COMMA; // Sell Client Code

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Remarks

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Sell Remarks

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Position

			dropCopyStream << "0" << FIN::COMMA; // Sell Position

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Proprietor/ Client Flag

			dropCopyStream << msg->PositionEffect << FIN::COMMA; // Sell Proprietor/Client Flag

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Order Time Stamp

			dropCopyStream << GetTransactTime(msg->ActivityTime) << FIN::COMMA; // Sell Order Time Stamp = ActivityTime

			dropCopyStream << FIN::BLANK << FIN::COMMA; // Buy Order Active Flag

			int indicator = msg->AggressorIndicator == 0 ? 1 : 0;
			dropCopyStream << indicator; // Sell Order Active Flag
		}

		return dropCopyStream.str();
	}
	catch (const std::exception& ex) {
		LOG_INFO << "[exception_makeStringSideWise]" << ex.what();
		return "";
	}
}

vector<std::string> TradeApplication::getValueRespectedKey(int64_t keyToFind)
{
	for (const auto& pair : contractFileData) {
		if (pair.first == keyToFind) {
			return pair.second;
		}
	}

	return { "notfound" };
}

bool TradeApplication::findDuplicateTrade(std::string trade)
{
	try
	{
		if (tradeId.find(trade) != tradeId.end())
		{
			LOG_INFO << "Duplicate Trade found : "<< trade;
			return true;
		}
		else
		{
			tradeId.insert(trade);
			return false;
		}
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_findDuplicateTrade]" << ex.what();
	}
}

std::string TradeApplication::convetPriceToDecimal(int64_t val) {
	
	try
	{
		int64_t val1 = val / 10000;
		std::stringstream ss;
		ss << std::fixed << std::setprecision(4) << std::showpoint << static_cast<double>(val1) / 10000.0;

		return ss.str();
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_convetPriceToDecimal]" << ex.what();
	}
}

std::string TradeApplication::convetStrickToDecimal(std::string val) {

	try
	{
		int val1 = std::stoi(val);
		val1 = val1 / 100;
		std::string temp = std::to_string(val1);
		temp += ".0000";
		return temp;
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_convetStrickToDecimal]" << ex.what();
	}
}

std::string TradeApplication::convertDateFormat(std::string inputDate) {
	try
	{
		std::istringstream ss(inputDate);

		int day;
		std::string month;
		int year;

		char dash1, dash2;
		ss >> day >> dash1 >> std::setw(3) >> month >> dash1 >> year >> dash2;

		// Convert month to uppercase using std::locale
		for (char& c : month) {
			c = std::toupper(c, std::locale());
		}

		// Adjust the year if it is in the two-digit format
		if (year >= 0 && year < 100) {
			if (year < 50) {
				year += 2000;
			}
			else {
				year += 1900;
			}
		}

		std::ostringstream result;
		result << std::setw(2) << std::setfill('0') << day << " " << month << " " << std::setw(4) << std::setfill('0') << year;

		return result.str();
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_convertDateFormat]" << ex.what();
	}
}

std::string TradeApplication::GetTransactTime(uint64_t longDate)
{
	try
	{

		time_t timestamp_sec = longDate / 1000000000;

		// Convert timestamp to struct tm for formatting
		struct tm* timeinfo;
		timeinfo = localtime(&timestamp_sec);

		// Map of month names and their abbreviations
		std::map<int, std::string> monthAbbreviations = {
			{0, "JAN"}, {1, "FEB"}, {2, "MAR"}, {3, "APR"},
			{4, "MAY"}, {5, "JUN"}, {6, "JUL"}, {7, "AUG"},
			{8, "SEP"}, {9, "OCT"}, {10, "NOV"}, {11, "DEC"}
		};

		// Construct the formatted string manually
		std::stringstream formattedTimestamp;
		formattedTimestamp << std::setfill('0')
			<< std::setw(2) << timeinfo->tm_mday << " "
			<< monthAbbreviations[timeinfo->tm_mon] << " "
			<< std::setw(4) << timeinfo->tm_year + 1900 << " "
			<< std::setw(2) << timeinfo->tm_hour << ":"
			<< std::setw(2) << timeinfo->tm_min << ":"
			<< std::setw(2) << timeinfo->tm_sec;

		return formattedTimestamp.str();
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_GetTransactTime]" << ex.what();
	}
}

std::string TradeApplication::removeTrailingSpaces(const std::string& str) {
	size_t endpos = str.find_last_not_of(" ");
	if (endpos != std::string::npos) {
		return str.substr(0, endpos + 1);
	}
	return str;
}

std::string TradeApplication::paddingSpace(const std::string& input, size_t totalWidth, bool isRight)
{
	try
	{
		if (input.length() >= totalWidth) {
			return input;
		}
		if (isRight)
			return input + std::string(totalWidth - input.length(), ' ');
		else
			return  std::string(totalWidth - input.length(), ' ') + input;
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "[exception_paddingSpace]" << ex.what();
	}
}
