#include"TradeApplication.h"

//TradeApplication::TradeApplication(SessionSettings* s, boost::asio::io_service& ioService) : RequestProcess(s, ioService)
TradeApplication::TradeApplication(SessionSettings* s, boost::asio::io_service& ioService) : _omsSender(ioService), processor(s),
m_ioService(ioService),RequestProcess(s, ioService), LogProcess()
{
	settingOrder = s;
	
	std::thread heartbeat_handler(&TradeApplication::initiateHeartbeat, this, settingOrder->HeartBt);
	heartbeat_handler.detach();

	std::thread logThread(&TradeApplication::logProcess, this);
	logThread.detach();
}

void TradeApplication::GenerateSequenceNum()
{
	++SeqNum;
}

void TradeApplication::GenerateApllSeqNum()
{
	++ApllSeq;
}

int TradeApplication::writeintofile()
{
	while (true)
	{
		ofstream myfile1;
		Sleep(10);
		myfile1.open(settingOrder->TradeFile_path);
		if (myfile1.is_open())
		{
			myfile1 << ApllSeq;
		}
		myfile1.close();
	}
	return 0;
}

int TradeApplication::readfromfile()
{
	fstream myfile;
	std::string line;

	myfile.open(settingOrder->TradeFile_path);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (line != "")
			{
				ApllSeq = std::atoi(line.c_str());
				std::cout << "[INFO] " << "LAST SEQUENCE NUMBER " << ApllSeq << std::endl;
			}
		}
	}
	myfile.close();
	return 0;
}

void TradeApplication::initiateHeartbeat(int heartbeat)
{
	//std::cout << "Heart beat handler initiated " << heartbeat << std::endl;
	LOG_INFO << "Heart beat handler initiated ";
	while (true)
	{
		if (!resetHeartbeat)
		{
			Sleep(heartbeat);
			if (isConnected)
				GenerateHeartBeat();
		}
		resetHeartbeat = false;

	}
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
					if (!readFromSSL())
						throw std::runtime_error("Failed to read from SSL");

					CloseSSLSocket();
					createSocket();
					SendSessionRegistrationRequest();

					try
					{
						while (1)
						{
							if(!receiveFromTarget())
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
	//decrypt msg
	if (isSecureBoxDone)
	{
		auto decryptedText_len = performDecrypt(((unsigned char*)data + msg_header_out_compT_size), (length - msg_header_out_compT_size));

		resMsg = new char[msg_header_out_compT_size + decryptedText_len];
		std::memcpy(resMsg, data, msg_header_out_compT_size);
		std::memcpy(resMsg + msg_header_out_compT_size, decryptedtext, decryptedText_len);
	}
	else
	{
		resMsg = data;
	}
	try
	{
		switch (templateID)
		{
		case TID_SESSION_REGISTRATION_RESPONSE:		onMessage((SessionRegistrationResponseT*)data);			break;
		case TID_LOGON_RESPONSE:					onMessage((LogonResponseT*)resMsg);						break;
		case TID_USER_LOGIN_RESPONSE:				onMessage((UserLoginResponseT*)resMsg);					break;
		case TID_LOGOUT_RESPONSE:					onMessage((LogoutResponseT*)resMsg);					break;
		case TID_USER_LOGOUT_RESPONSE:				onMessage((UserLogoutResponseT*)resMsg);				break;
		case TID_FORCED_LOGOUT_NOTIFICATION:		onMessage((ForcedLogoutNotificationT*)resMsg);			break;
		case TID_HEARTBEAT_NOTIFICATION:			onMessage((HeartbeatNotificationT*)resMsg);				break;
		case TID_GW_ORDER_ACKNOWLEDGEMENT:			onMessage((GwOrderAcknowledgementT*)resMsg);			break;
		case TID_NEW_ORDER_NR_RESPONSE:				onMessage((NewOrderNRResponseT*)resMsg);				break;
		case TID_ORDER_EXEC_RESPONSE:				onMessage((OrderExecResponseT*)resMsg);					break;
		case TID_ORDER_EXEC_NOTIFICATION:			onMessage((OrderExecNotificationT*)resMsg);				break;
		case TID_MODIFY_ORDER_NR_RESPONSE:			onMessage((ModifyOrderNRResponseT*)resMsg);				break;
		case TID_DELETE_ORDER_NR_RESPONSE:			onMessage((DeleteOrderNRResponseT*)resMsg);				break;
		case TID_ORDER_EXEC_REPORT_BROADCAST:		onMessage((OrderExecReportBroadcastT*)resMsg);			break;
		case TID_REJECT:							onMessage((RejectT*)resMsg);							break;
		case TID_SESSION_PASSWORD_CHANGE_RESPONSE:	onMessage((SessionPasswordChangeResponseT*)resMsg);		break;
		case TID_USER_PASSWORD_CHANGE_RESPONSE:		onMessage((UserPasswordChangeResponseT*)resMsg);		break;
		case TID_TRADING_SESSION_STATUS_BROADCAST:	onMessage((TradingSessionStatusBroadcastT*)resMsg);		break;
		case TID_RISK_COLLATERAL_ALERT_BROADCAST:															break;
		default:
			cout << "TEMPLATE ID NOT FAOUND " << templateID << endl;
			LOG_INFO << "[INFO] " << "TEMPLATE ID NOT FAOUND " << templateID;
			break;
		}
	}
	catch (std::exception& ex)
	{
		LOG_INFO << "[INFO] " << ex.what();
	}
}

void TradeApplication::onMessage(SessionRegistrationResponseT* msg)
{
	std::string sessionStatus(msg->Status);
	std::string sessionText(msg->VarText);
	logPrint(msg);
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

void TradeApplication::onMessage(TradingSessionStatusBroadcastT* msg)
{
	logPrint(msg);
	isConnected = false;
}

void TradeApplication::onMessage(ForcedLogoutNotificationT* msg)
{
	logPrint(msg);
	std::string error_msg(msg->VarText, msg->VarTextLen);
	std::cout << "[ FORCEDLOGOUTNOTIFICATIONT ] " << error_msg << std::endl;
}

void TradeApplication::onMessage(LogonResponseT* msg)
{
	cout << "Session Logon Response" << endl;
	_st = new FIN::SlidingWindowThrottling(msg->ThrottleNoMsgs, msg->ThrottleTimeInterval);
	logPrint(msg);
	userLogon();
}

void TradeApplication::onMessage(UserLoginResponseT* msg)
{
	logPrint(msg);
	cout << "User Logon Response" << endl;
	_loggedIn = true;
	isConnected = true;
	
	

	if (settingOrder->changePassword == 1) {
		GenerateSequenceNum();
		sessionPasswordChange();
	}
	if (logout_count > 0)
	{
		SeqNum = 3;
		cout << "user Logon Respone" << endl;
	}
	GenerateSequenceNum();
}

void TradeApplication::onMessage(LogoutResponseT* msg)
{
	cout << "Session Logout Response" << endl;
	LOG_INFO << "[ LogoutResponseT ] " << "Session Logout Response";
}

void TradeApplication::onMessage(UserLogoutResponseT* msg)
{
	cout << "User Logout Response" << endl;
	LOG_INFO << "[ UserLogoutResponseT ] " << "User Logout Response";
}

void TradeApplication::onMessage(HeartbeatNotificationT* msg)
{
	GenerateHeartBeat();
}

void TradeApplication::onMessage(GwOrderAcknowledgementT* msg)
{
	logPrint(msg);
}

void TradeApplication::onMessage(NewOrderNRResponseT* msg)
{
	auto response = processor.res.IsEligibleToSend(msg);
	sendToOMS((char*)&response, OmsResponseSize);

	logQue.push(std::make_pair((char*)msg, msg->MessageHeaderOut.TemplateID));
}

void TradeApplication::onMessage(OrderExecResponseT* msg)
{
	auto trades = processor.res.IsEligibleToSend(msg);
	if (trades.size() > 0)
	{
		for (size_t i = 0; i < trades.size(); i++)
		{
			sendToOMS((char*)&trades[i], sizeof(trades[i]));
			
			logQue.push(std::make_pair((char*)trades[i], msg->MessageHeaderOut.TemplateID));
		}
	}
	GenerateApllSeqNum();
}

void TradeApplication::onMessage(OrderExecNotificationT* msg)
{
	auto trades = processor.res.IsEligibleToSend(msg);
	if (trades.size() > 0)
	{
		for (size_t i = 0; i < trades.size(); i++)
		{
			sendToOMS((char*)&trades[i], sizeof(trades[i]));
			
			logQue.push(std::make_pair((char*)trades[i], msg->MessageHeaderOut.TemplateID));
		}
	}
	GenerateApllSeqNum();
}

void TradeApplication::onMessage(ModifyOrderNRResponseT* msg)
{
	auto response = processor.res.IsEligibleToSend(msg);
	sendToOMS((char*)&response, OmsResponseSize);
	
	logQue.push(std::make_pair((char*)msg, msg->MessageHeaderOut.TemplateID));
}

void TradeApplication::onMessage(DeleteOrderNRResponseT* msg)
{
	auto response = processor.res.IsEligibleToSend(msg);
	sendToOMS((char*)&response, OmsResponseSize);
	
	logQue.push(std::make_pair((char*)msg, msg->MessageHeaderOut.TemplateID));
}

void TradeApplication::onMessage(RejectT* msg)
{
	auto response = processor.res.IsEligibleToSend(msg);
	sendToOMS((char*)&response, OmsResponseSize);
	
	logQue.push(std::make_pair((char*)msg, msg->MessageHeaderOut.TemplateID));
	std::cout << "[INFO ]" << msg->SessionRejectReason << " : " << msg->VarText << std::endl;
}

void TradeApplication::onMessage(OrderExecReportBroadcastT* msg)
{
	auto response = processor.res.IsEligibleToSend(msg);
	sendToOMS((char*)&response, OmsResponseSize);
	
	logQue.push(std::make_pair((char*)msg, msg->MessageHeaderOut.TemplateID));
}
 
void TradeApplication::onMessage(SessionPasswordChangeResponseT* msg)
{
	LOG_INFO << "[Session Password Change Response]";
	if (settingOrder->changePassword == 1) {
		GenerateSequenceNum();
		userPasswordChange();
	}
}

void TradeApplication::onMessage(UserPasswordChangeResponseT* msg)
{
	SeqNum = msg->ResponseHeader.MsgSeqNum;
	LOG_INFO << "[User Password Change Response]";
}

void TradeApplication::disableConnectionVariable()
{
	isSSLConnect = false;
	isGatewayRequestSend = false;
	isConnected = false;
	_loggedIn = false;
}

//OMS data process
void TradeApplication::startOmsRcv()
{
	try
	{
		bool first = true;
		while (true)
		{
			if (_loggedIn)
			{
				if (first) {
					doOmsConnect();
					first = false;
				}
				try
				{
					if (!settingOrder->oms_channel)
						StartReceiving();
					else
						rabbitMqConsumer();
				}
				catch (std::exception& e)
				{
					std::cout << "[INFO] " << "Exception : " << e.what() << std::endl;
					restartOmsConnections();
				}
			}
			else
			{
				this_thread::sleep_for(std::chrono::seconds(5));
			}
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		LOG_INFO << ex.what();
		throw;
	}
}

void TradeApplication::doOmsConnect()
{
	FIN_DEVLOG("Initiating OMS Connection");

	if (settingOrder->oms_channel)
	{
		RabbitMqConsumer::rabbit_connect(settingOrder);
		_omsRabbitSender.RabbitMqConnection(settingOrder);
	}
	else
	{
		UDP_RECEVIER::init(settingOrder->udp_rcverPort);
		oms_udp_sender.init(settingOrder->udp_senderIp, settingOrder->udp_senderPort);

	}

	_omsSender.init(settingOrder->listenPort);
	_omsSender.accept();
	_omsSender.startOmsHealthThread();
}

void TradeApplication::restartOmsConnections()
{
	FIN_DEVLOG("Restarting OMS Connection");
	_omsSender.init(settingOrder->listenPort);

	if (settingOrder->oms_channel)
	{
		RabbitMqConsumer::rabbit_connect(settingOrder);
		_omsRabbitSender.RabbitMqConnection(settingOrder);
	}
	else
	{
		UDP_RECEVIER::init(settingOrder->udp_rcverPort);
		oms_udp_sender.init(settingOrder->udp_senderIp, settingOrder->udp_senderPort);

	}

	_omsSender.accept();
	_omsSender.startOmsHealthThread();
}

void TradeApplication::receiveFromOMS(char* data)
{
	try
	{
		_st->checkThrottleLimit();

		OMSRequest* msg = (OMSRequest*)data;

		GenerateSequenceNum();
		auto return_struct = processor.processRequest(msg, SeqNum);
		switch (return_struct.requestType)
		{
		case 0:
			if (sendToExchange((unsigned char*)&return_struct.OrderReq, sizeof(return_struct.OrderReq)))
				logQue.push(std::make_pair((char*)&return_struct.OrderReq, return_struct.OrderReq.MessageHeaderIn.TemplateID));
			break;
		case 1:
			if (sendToExchange((unsigned char*)&return_struct.ReplaceReq, sizeof(return_struct.ReplaceReq)))
				logQue.push(std::make_pair((char*)&return_struct.ReplaceReq, return_struct.ReplaceReq.MessageHeaderIn.TemplateID));
			break;
		case 2:
			if (sendToExchange((unsigned char*)&return_struct.CancelReq, sizeof(return_struct.CancelReq)))
				logQue.push(std::make_pair((char*)&return_struct.CancelReq, return_struct.CancelReq.MessageHeaderIn.TemplateID));

			break;


		default:
			break;
		}
	}
	catch (std::exception& ex)
	{

	}
}

void TradeApplication::sendToOMS(char* msg, size_t len)
{
	if (!settingOrder->oms_channel)
		oms_udp_sender.sendToOMS(msg, len);
	else
		_omsRabbitSender.sendForOMS(msg);
}

void TradeApplication::logProcess() 
{
	LOG_INFO << "Log processing start.";
	while (running) 
	{
		std::pair<char*, uint16_t> value;

		logQue.wait_and_pop(value);

		switch (value.second) 
		{
		case TID_NEWORDERSINGLEREQUEST:
			logPrint((NewOrderSingleRequestT*)value.first);
			break;
		case TID_MODIFY_ORDER_SINGLE_REQUEST:
			logPrint((ModifyOrderSingleRequestT*)value.first);
			break;
		case TID_DELETEORDERSINGLEREQUEST:
			logPrint((DeleteOrderSingleRequestT*)value.first);
			break;
		case TID_NEWORDERNRRESPONSE:
			logPrint((NewOrderNRResponseT*)value.first);
			break;
		case TID_ORDER_EXEC_RESPONSE:
			logPrint((OMSResponse*)value.first);
			break;
		case TID_ORDER_EXEC_NOTIFICATION:
			logPrint((OMSResponse*)value.first);
			break;
		case TID_MODIFY_ORDER_NR_RESPONSE:
			logPrint((ModifyOrderNRResponseT*)value.first);
			break;
		case TID_DELETE_ORDER_NR_RESPONSE:
			logPrint((DeleteOrderNRResponseT*)value.first);
			break;
		case TID_REJECT:
			logPrint((RejectT*)value.first);
			break;
		case TID_ORDER_EXEC_REPORT_BROADCAST:
			logPrint((OrderExecReportBroadcastT*)value.first);
			break;

		default:
			LOG_INFO << "not handle templateID : " << value.second;
			break;
		}
	}
}
