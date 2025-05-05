#pragma once
#ifndef TRADE_APPLICATION_H
#define TRADE_APPLICATION_H

#include <winsock2.h>   
#include <windows.h>
#include <stdio.h>     
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <string>
#include<sstream>
#include<fstream>
#include <ctime>
#include <thread>
#include <map> 
#include <iterator>
#include <time.h>
#include <ws2tcpip.h>
#include <array>
#include "../HeaderFile/throttling.h"
#include "../HeaderFile/SessionSettings.h"
#include "../Exchange/RequestProcess/RequestProcess.h"
#include "../Processing/Response/ResponseProcessor.h"
#include "../Processing/Request/RequestProcessor.h"
#include "../OMS/UDP/UdpRecevier.h"
#include "../OMS/RabbitMQ/RabbitConsumer.h"
#include "../OMS/RabbitMQ/RabbitProducer.h"
#include "../OMS/UDP/UdpSender.h"
#include "../OMS/OMS_Acceptor.h"
#include "../LogProcess/LogProcess.h"
#include "../HeaderFile/LockFreeQueue.h"
//openssl
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <iomanip>


using namespace std;
#define MAXPENDING 5   
#define RCVBUFSIZE 65535
#define RCVBUFSIZEE 312
#define RCVBUFSIZEG 312


class TradeApplication : public RequestProcess, UDP_RECEVIER, RabbitMqConsumer, LogProcess
{
public:
	boost::asio::io_service& m_ioService;
	SessionSettings* settingOrder;
	char buffer[RCVBUFSIZE];

	bool isSecureBoxDone = false;
	bool isSocketConnect = false;

	const std::string Certificate = "Certificate.pem";

	char* resMsg;

	TradeApplication(SessionSettings* s, boost::asio::io_service& ioService);
	
	UDP_Sender oms_udp_sender;
	RabbitMqProducer _omsRabbitSender;
	OmsSenderServer _omsSender;
	Request_Processor processor;
	SlidingWindowThrottling* _st;

	void receiveFromOMS(char* data);
	void startOmsRcv();

	int writeintofile();
	int readfromfile();
	void connectToBSEExchange();
	bool receiveFromTarget();
	void GenerateSequenceNum();
	void GenerateApllSeqNum();
	int OmsResponseSize = sizeof(OMSResponse);

	void initiateHeartbeat(int heartbeat);
	bool isConnected = false;
	int msg_header_out_compT_size = sizeof(MessageHeaderOutCompT);



protected:
	unsigned SeqNum = 2;
	unsigned ApllSeq = 0;
	int logout_count = 0;


	void HandleData(char* data, uint16_t templateID, int len);
	void onMessage(SessionRegistrationResponseT* msg);
	void onMessage(TradingSessionStatusBroadcastT* msg);
	void onMessage(ForcedLogoutNotificationT* msg);
	void onMessage(LogonResponseT* msg);
	void onMessage(UserLoginResponseT* msg);
	void onMessage(LogoutResponseT* msg);
	void onMessage(UserLogoutResponseT* msg);
	void onMessage(HeartbeatNotificationT* msg);
	void onMessage(GwOrderAcknowledgementT* msg);
	void onMessage(NewOrderNRResponseT* msg);
	void onMessage(OrderExecResponseT* msg);
	void onMessage(OrderExecNotificationT* msg);
	void onMessage(ModifyOrderNRResponseT* msg);
	void onMessage(DeleteOrderNRResponseT* msg);
	void onMessage(RejectT* msg);
	void onMessage(OrderExecReportBroadcastT* msg);
	void onMessage(SessionPasswordChangeResponseT* msg);
	void onMessage(UserPasswordChangeResponseT* msg);
	void disableConnectionVariable();
	bool _loggedIn = false;
	bool isFirst=true;
	

	void doOmsConnect();
	void restartOmsConnections();
	void sendToOMS(char* response, size_t size);
	void logProcess();

	//std::queue<std::pair<char*, uint16_t>> logQue;
	
	LockFreeQueue<std::pair<char*, uint16_t>> logQue;	// <msg value, templateID>
	std::atomic<bool> running{ true };

};





#endif;

