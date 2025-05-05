#pragma once
#ifndef FIN_EXCHANGE_CONNECTION_H
#define FIN_EXCHANGE_CONNECTION_H
#include "ETI.h"
#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include "configReader.h"
#include "Nanolog.hpp"
#include "CommonReqStruct.h"
#include <openssl/ossl_typ.h>
#include <openssl/evp.h>
#include <openssl/err.h>



using namespace boost::asio;
using namespace FIN::FIN_CONST;
using namespace Derivatives;
class ExchangeConnection
{
public:

	void stop();
	void start();
	ExchangeConnection(const std::string& fileName, boost::asio::io_service& ioService);
	
	
	
	//void initializeLib();


	void encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* ciphertext, int* ciphertext_len);
	int decrypt(unsigned char* ciphertext, int ciphertext_len);

	unsigned char ciphertext[1000];
	int ciphertext_len = 0;

	unsigned char decryptedtext[1000]; // Make sure this is large enough to hold the decrypted data
	int decryptedtext_len = 0;



	unsigned char mapDecrypted[1000]; // Make sure this is large enough to hold the decrypted data


	EVP_CIPHER_CTX* ctx_encrypt = EVP_CIPHER_CTX_new(); // NULL;
	EVP_CIPHER_CTX* ctx_decrypt = EVP_CIPHER_CTX_new(); // NULL;
	ConnectionGatewayResponseT gatewayResponse;

protected:
	void scheduleHeartbeat();
	void connectSocket();
	bool sendToTarget(char* data, size_t len);
	bool sendToTarget(ConnectionGatewayRequestT req, size_t len);
	bool sendToTarget(boost::asio::mutable_buffers_1 _asio_buffer);
	bool sendEncryptedTarget(unsigned char*, int len);

	void receiveFromTarget();
	void closeSocket();
	virtual void handleMessage(unsigned char* data, UINT16 templateID, int len) = 0;
	Derivatives::HeartbeatT* _heartBeatMsg;

	boost::asio::io_service& _ioService;
	std::string host_ip;
	int host_port;
	std::string nic_ip;


	unsigned char _hash[16];
	char _data[65535];
	unsigned int _seqNu;
	std::mutex _mutex;
	int len;
	bool _isMessageDownload;
	int streamCounter, currStream;
	FIN::ConfigReader config_param;
	boost::shared_ptr < boost::asio::ip::tcp::socket > m_socket;
	bool _connected;
	bool _gateway_request_done;
	bool exchangeDone = false;
private:
	std::string _configFileName;
	FIN::CurrentPath _currentPath;


	void decrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv);
	void encrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv);





};




#endif


