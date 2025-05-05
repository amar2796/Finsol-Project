#ifndef FIN_NSE_TRIMMED_CONNECTION_H
#define FIN_NSE_TRIMMED_CONNECTION_H

#include "connectionTemplate.h"
#include "throttling.h"
#include "messages.h"
#include <fstream>
#include <openssl/md5.h>
#include <sstream>
#include <cmath>
#include <time.h>
namespace FIN
{
	namespace NSE
	{
		namespace TRIMMED
		{

			class Connection : public ConnectionTemplate
			{
			public:
				void stop();
				void start();
				Connection(const std::string& fileName, boost::asio::io_service& ioService);

				void encrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv);
				void encrypt(EVP_CIPHER_CTX* ctx, unsigned char* plaintext, int plaintext_len, unsigned char* ciphertext, int* ciphertext_len);
				void decrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv);
				int decrypt(EVP_CIPHER_CTX* ctx, unsigned char* ciphertext, int ciphertext_len, unsigned char* plaintext, int* plaintext_len);

				EVP_CIPHER_CTX* ctx_encrypt = EVP_CIPHER_CTX_new(); // NULL;
				EVP_CIPHER_CTX* ctx_decrypt = EVP_CIPHER_CTX_new(); // NULL;

				unsigned char ciphertext[1000];
				int ciphertext_len = 0;

				unsigned char decryptedtext[1000]; // Make sure this is large enough to hold the decrypted data
				int decryptedtext_len = 0;
				bool initializeDone = false;
			protected:
				void scheduleHeartbeat();
				// void handleTimer(const boost::system::error_code& ec);

				bool sendToTarget(DC_PACKET_WRAPPER& data);
				bool sendToTarget_Encrypt(DC_PACKET_WRAPPER& data, bool isBoxLogin= false);



				bool sendToTarget_SSL(DC_PACKET_WRAPPER& data);

				void receiveFromTarget();
				void receiveFromTarget_SSL();


				virtual void handleMessage(char* data, int vlue = 0) = 0;
				virtual void disconnectOmsConn() = 0;

				virtual void onLogon() = 0;
				virtual void startCommunication() = 0;

				void updateConfigFile(std::string password, std::string newPassword, std::string date);
				int daysBetweenDates(std::string date1, std::string date2);
				std::string getCurrentDate();
				//Below three functions used to generate random password                
				int selectArray();
				int getKey();
				std::string generate_password(int length);

				HEARTBEAT_MESSAGE* _heartBeatMsg_;
				FIN::NSE::TRIMMED::FO::GR_RESPONSE gwRsp;


				boost::asio::io_service& _ioService;
				boost::asio::deadline_timer _timer;

				unsigned char _hash[16];
				char _data[65535];
				unsigned int _seqNu;
				std::mutex _mutex;
				int len;
				bool _isMessageDownload;
				int streamCounter, currStream;

			private:
				bool _connected;
				std::string _configFileName;

				// SlidingWindowThrottling *_st ;

				std::string _fileName;
				std::ofstream _logFile;

				// TEST_TIME_STAMP _tm ;

				// void writeLog( char *msg, size_t size )
				//{
				//     // Make this log writting Async!!!!
				//     if( !_logFile.is_open() )
				//     {
				//         _logFile.open( _fileName.c_str() ) ;
				//     }

				//    _logFile.write( msg, size ) ;
				//    _logFile << std::flush ;
				//}

				void writeLog(char* msg, size_t size)
				{
					// Make this log writting Async!!!!
					if (!_logFile.is_open())
					{
						_logFile.open(_fileName.c_str(), std::ios::out | std::ios::binary);
					}
					_logFile.write(msg, size);
					_logFile.flush();
				}

				// struct timeval currentTime;
				// void writTimestamp()
				//{
				//     _tm.setTimeStamp( getMicrotime() ) ;
				//     writeLog(  ( char *) & _tm , _tm.getLength()  ) ;
				// }

				// long getMicrotime(){
				//     gettimeofday(&currentTime, NULL);
				//     return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
				// }

				virtual void printMessages(char* data) {}
			};
		}
	}
}

#endif
