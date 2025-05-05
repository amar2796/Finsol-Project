#pragma once
#ifndef TRADE_APPLICATION_H
#define TRADE_APPLICATION_H

// openssl
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <iomanip>

#include <winsock2.h>   
#include "SessionSettings.h"
#include "threadSafeQueue.h"
#include "DropFile.h"
#include "Nanolog.hpp"
#include "readContractFile.h"
#include "ETILayouts.h"
#include "ExchangeConnection.h"
#include "Common.h"
#include "throttling.h"
#include "color.h"

using namespace std;
#define MAXPENDING 5   
#define RCVBUFSIZE 65535
#define RCVBUFSIZEE 312
#define RCVBUFSIZEG 312

#define ErrorForBU "Retransmission of Session not belonging to BU not allowed"
#define ErrorForBUCode 99

class TradeApplication : ExchangeConnection
{
public:
	TradeApplication(SessionSettings* s, boost::asio::io_service& ioService);

	char buffer[RCVBUFSIZE];

	SlidingWindowThrottling* _st;

	DropFile drop_file;

	bool isSecureBoxDone = false;
	bool isSocketConnect = false;
	int msg_header_out_compT_size = sizeof(MessageHeaderOutCompT);
	const std::string Certificate = "Certificate.pem";

	
	void readLinesFromFile(const std::string);
	void storeTradingSessionID(const std::string);
	bool receiveFromTarget();
	int getSize();
	void sendLogoutRequest();
	std::string findValue(int64_t, int);
	void ConnectToExchange_GatewayRequest();
	void SendSessionRegistrationRequest();
	void ConnectToExchange_LogonRequest();
	void userLogon();
	void GenerateHeartBeat();
	void RetransRequest_trade(uint64_t, int, uint32_t);
	void connectToBSEExchange();
	void disableConnectionVariable();
	//void initializeSSLSocket();
	//void startCommunication();
	void GenerateSequenceNum();
	void GenerateApllSeqNum();
	std::string convertDateFormat(std::string);
	std::string GetTransactTime(uint64_t);
	std::string removeTrailingSpaces(const std::string& str);
	std::string convetPriceToDecimal(int64_t val);
	std::string convetStrickToDecimal(std::string val);
	bool findDuplicateTrade(std::string);
	std::string makeStringSideWise(TradeBroadcastT*, int);
	vector<std::string> getValueRespectedKey(int64_t keyToFind);
	void initiateHeartbeat(uint32_t);

	virtual void HandleData(char* data, uint16_t templateID, int len);

	//void onMessage(GatewayResponseT* gwRes);
	void onMessage(SessionRegistrationResponseT* lgRes);
	void onMessage(LogonResponseT* lgRes);
	void onMessage(UserLoginResponseT* ulRes);
	void onMessage(SubscribeResponseT* subRes);
	void onMessage(RetransmitResponseT* retRes);
	void onMessage(TradeBroadcastT* trBrodcastT);
	void onMessage(RejectT* rejT);
	void remove_securityid_from_memory(uint64_t id);
	void delete_securityid_from_file(std::string security_id);
	void onMessage(LogoutResponseT* logRes);
	void onMessage(UserLogoutResponseT* uLogRes);
	void onMessage(HeartbeatNotificationT* hbNotification);
	void onMessage(RiskCollateralAlertAdminBroadcastT* rcaaBroadcast);
	void onMessage(ForcedLogoutNotificationT* flNotification);

	void sendSubscriptionRequest(uint8_t, uint32_t);
	void onMessageToTxt(TradeBroadcastT* msg);
	std::string paddingSpace(const std::string& input, size_t totalWidth, bool isRight);

private:
	SessionSettings* settingOrder;
	std::vector < pair< int, vector<string> > > contractFileData;
	std::vector < uint64_t > allTradingSessionID;
	std::unordered_set<std::string> tradeId;
	std::vector<std::string> tradeValue;

	std::string host_ip;
	uint32_t host_port;
	unsigned int _seqNu;
	bool firstLine = true;
	int tradeSessionCount = 0;
	int pId = 1;
	bool checkSubscription = true;
	bool isConnected = false;
	std::string error_msg;
	std::string error_logout;
	uint64_t applSeqNum;
	unsigned SeqNum = 0;
	unsigned ApllSeq = 0;
	int logout_count = 0;
	uint64_t endSeqNumber;
	uint64_t lastSeqNumber;
	int countLine = 0;
	bool _loggedIn;
	int countSessionId = 0;
	uint32_t val = 0;
	// create ssl
	SSL* ssl;
	SSL_CTX* ctx;
	int sockfd;

	// Creating different contexts for encryption and decryption
	EVP_CIPHER_CTX* encrypt;
	EVP_CIPHER_CTX* decrypt;

	unsigned char key[32] = { 0 };
	unsigned char iv[16] = { 0 };

	unsigned char ciphertext[1000];

	unsigned char decryptedtext[1000]; // Make sure this is large enough to hold the decrypted data
	int decryptedtext_len = 0;

	//bool isSSLConnect = false;
	//bool resetHeartbeat = false; // Change access level if needed

	//const std::string Certificate = "Certificate.pem";
	size_t msg_header_comp_size = sizeof(MessageHeaderInCompT);

	bool isGatewayRequestSend = false;

	std::string VCF = "VCF";
	std::string clientCode = "";

	int addPartitionID1 = 0;

	//std::unordered_map<int, std::unordered_map<int, int>> packet_loss_check;

};
#endif;








//#pragma once
//#ifndef TRADE_APPLICATION_H
//#define TRADE_APPLICATION_H
//
//#include <winsock2.h>   
//#include "SessionSettings.h"
//#include "threadSafeQueue.h"
//#include "throttling.h"
//#include "DropFile.h"
//#include "Nanolog.hpp"
//#include "readContractFile.h"
//#include "ETILayouts.h"
//#include "ExchangeConnection.h"
//#include "Common.h"
//
//using namespace std;
//#define MAXPENDING 5   
//#define RCVBUFSIZE 65535
//#define RCVBUFSIZEE 312
//#define RCVBUFSIZEG 312
//
//class TradeApplication : ExchangeConnection
//{
//public:
//	TradeApplication(SessionSettings* s);
//	void readLinesFromFile(const std::string);
//	void storeTradingSessionID(const std::string);
//	int getSize();
//	void sendLogoutRequest();
//	std::string findValue(int64_t, int);
//	void ConnectToExchange_GatewayRequest();
//	void ConnectToExchange_LogonRequest();
//	void userLogon();
//	void GenerateHeartBeat();
//	void RetransRequest_trade(uint64_t, int, uint32_t);
//	void connectToBSEExchange();
//	void startCommunication();
//	void GenerateSequenceNum();
//	void GenerateApllSeqNum();
//	std::string convertDateFormat(std::string);
//	std::string GetTransactTime(uint64_t);
//	std::string removeTrailingSpaces(const std::string& str);
//	std::string convetPriceToDecimal(int64_t val);
//	std::string convetStrickToDecimal(std::string val);
//	bool findDuplicateTrade(uint32_t);
//	std::string makeStringSideWise(TradeBroadcastT*, int);
//	vector<std::string> getValueRespectedKey(int64_t keyToFind);
//	void initiateHeartbeat(uint32_t);
//
//	virtual void HandleData(char* data, uint16_t templateID);
//
//	void onMessage(GatewayResponseT* gwRes);
//	void onMessage(LogonResponseT* lgRes);
//	void onMessage(UserLoginResponseT* ulRes);
//	void onMessage(SubscribeResponseT* subRes);
//	void onMessage(RetransmitResponseT* retRes);
//	void onMessage(TradeBroadcastT* trBrodcastT);
//	void onMessage(RejectT* rejT);
//	void onMessage(LogoutResponseT* logRes);
//	void onMessage(UserLogoutResponseT* uLogRes);
//	void onMessage(HeartbeatNotificationT* hbNotification);
//	void onMessage(RiskCollateralAlertAdminBroadcastT* rcaaBroadcast);
//	void onMessage(ForcedLogoutNotificationT* flNotification);
//
//	void sendSubscriptionRequest(uint8_t, uint32_t);
//	void onMessageToTxt(TradeBroadcastT* msg);
//	std::string paddingSpace(const std::string& input, size_t totalWidth, bool isRight);
//
//private:
//	SessionSettings* settingOrder;
//	SlidingWindowThrottling* _st;
//	std::vector < pair< int, vector<string> > > contractFileData;
//	std::vector < uint64_t > allTradingSessionID;
//	std::unordered_set<unsigned int> tradeId;
//	std::vector<std::string> tradeValue;
//	DropFile drop_file;
//	
//	std::string host_ip;
//	uint32_t host_port;
//	unsigned int _seqNu;
//	bool firstLine = true;
//	int tradeSessionCount = 0;
//	int pId = 1;
//	bool checkSubscription=true;
//	bool isConnected = false;
//	std::string error_msg;
//	std::string error_logout;
//	uint64_t applSeqNum;
//	unsigned SeqNum = 0;
//	unsigned ApllSeq = 0;
//	int logout_count = 0;
//	uint64_t endSeqNumber;
//	uint64_t lastSeqNumber;
//	int countLine = 0;
//	bool _loggedIn;
//	
//	int countSessionId = 0;
//
//
//};
//#endif;
//
