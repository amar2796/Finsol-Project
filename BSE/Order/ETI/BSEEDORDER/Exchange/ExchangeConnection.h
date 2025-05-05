#pragma once
// openssl
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
#include <iomanip>
#include <boost/asio/io_service.hpp>
#include "../NanoLog/Nanolog.hpp"
#include "../HeaderFile/SessionSettings.h"
#include "../HeaderFile/ETILayouts.h"
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <vector>
#include <cstring>
#include "../LogProcess/LogProcess.h"

using namespace boost::asio;
class ExchangeConnection
{
public: 
    ExchangeConnection(SessionSettings* s, boost::asio::io_service& ioService);
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

    bool isSSLConnect = false;
    bool resetHeartbeat = false; // Change access level if needed

    const std::string Certificate = "Certificate.pem";
    size_t msg_header_comp_size = sizeof(MessageHeaderInCompT);

    SessionSettings* sessionSetting;
    LogProcess* log;

    void initializeSSLSocket();
    bool sendToSSL(const char* msg, size_t len);
    bool readFromSSL();
    void CloseSSLSocket();
    void createSocket();
    void closeConnectionSocket();
    bool initializeEncryption(unsigned char* key, unsigned char* iv);
    int performEncrypt(unsigned char* plaintext, int plaintext_len);
    bool initializeDecryption(unsigned char* key, unsigned char* iv);
    int performDecrypt(unsigned char* ciphertext, int ciphertext_len);
    bool sendToTarget(const char* msg, size_t len);
    bool sendToExchange(unsigned char* msg, size_t len);

    boost::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
    boost::asio::io_service& m_ioService;
};
