#pragma once
#ifndef FIN_EXCHANGE_CONNECTION_H
#define FIN_EXCHANGE_CONNECTION_H
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "configReader.h"
#include "messages.h"
#include "color.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>

using boost::asio::ip::tcp;
class ExchangeConnection
{
public:	
	void stop();
	void start();
	ExchangeConnection(const std::string& fileName, boost::asio::io_service& ioService);
	void init();
protected:
	bool createTCPConnection();
	std::string escapeNonPrintableChars(const std::string& input);
	void generateKeyandIV(std::string);
	void scheduleHeartbeat();
	void connectSocket();
	int getFirstTwoByteLength(char* data);
	bool sendToTarget(std::string data, size_t len);
	/*bool initializeEncryption(const unsigned char* key, const unsigned char* iv);
	int performEncrypt(unsigned char* plaintext, int plaintext_len);
	bool initializeDecryption(const unsigned char* key, const unsigned char* iv);
	int performDecrypt(unsigned char* ciphertext, int ciphertext_len);*/
	bool encrypt(const unsigned char* plaintext, int plaintext_len, const unsigned char* key, const unsigned char* iv, unsigned char* ciphertext, int& ciphertext_len);
	bool decrypt(const unsigned char* ciphertext, int ciphertext_len, const unsigned char* key, const unsigned char* iv, unsigned char* plaintext, int& plaintext_len);
	void receiveFromTarget();
	void closeSocket();
	//virtual void handleMessage(char* data) = 0;	
	virtual void handleMessage(std::string&data, int len) = 0;	
	//HeartbeatT* _heartBeatMsg;

	boost::asio::io_service& _ioService;
	std::string host_ip;
	int host_port;
	std::string host_ip_res;
	int host_port_res;
	std::string password;
	unsigned char _hash[16];
	char c[65535];
	char _data[65535];
	unsigned int _seqNu;
	std::mutex _mutex;
	int len;		
	FIN::ConfigReader config_param;
	boost::shared_ptr < boost::asio::ip::tcp::socket > m_socket;
	bool _connected = false;
	bool _gateway_request_done;
	bool firstMsgReceive = false;
public:
	std::string _configFileName;
	FIN::CurrentPath _currentPath;

	std::string key;
	std::string IV;

	// Creating different contexts for encryption and decryption
	/*EVP_CIPHER_CTX* encrypt;
	EVP_CIPHER_CTX* decrypt;*/

	unsigned char ciphertext[65355];
	unsigned char decryptedtext[65355]; // Make sure this is large enough to hold the decrypted data
	int decryptedtext_len = 0;

	int ciphertext_len = 0;
};




#endif


