#include "connection.h"
#include <chrono>
#include "CurrentPath.h"

namespace FIN {
	namespace NSE {
		namespace TRIMMED {

			Connection::Connection(const std::string& fileName, boost::asio::io_service& ioService)
				: ConnectionTemplate(ioService),
				_ioService(ioService),
				_timer(_ioService)
				//_tm( 0 )
			{
				CurrentPath _currPath;
				_configFileName = _currPath.getCurrentPath() + fileName;
				//_configFileName = _currPath.getCurrentPath() + FIN::CONSTANT::NSE_FO_ORDER_CONFIG;
				getConnectionParam().readConfig(_configFileName);
				getConnectionParam().print();
				/*	_timer.expires_from_now(boost::posix_time::seconds(20));
					_timer.async_wait(boost::bind(&Connection::handleTimer, this, boost::asio::placeholders::error));*/
				_heartBeatMsg_ = new HEARTBEAT_MESSAGE(getConnectionParam()._traderId);
				_connected = false;
				_isMessageDownload = false;
			}


			void Connection::start()
			{
				while (1)
				{
					ConnectSSLSocket();
					_connected = true;
					startCommunication();

					while (1)
					{
						try
						{
							if (isSSLComplted)
								receiveFromTarget();
							else
								receiveFromTarget_SSL();
						}
						catch (std::exception& e)
						{
							DEVLOG("Exception Rcvd : " << e.what());
							break;
						}
					}

					isSSLComplted = false;
					isSecureBoxDone = false;
					_connected = false;
					initializeDone = false;

					//send request to oms disconnect connection
					disconnectOmsConn();

					std::cout << "Reconnecting after 10 Seconds" << std::endl;
					LOG_INFO << "Reconnecting after 10 Seconds";
					getConnectionParam().readConfig(_configFileName);
					Sleep(10000);
				}
			}

			void Connection::stop()
			{
			}

			void Connection::scheduleHeartbeat()
			{
			}

			/*void Connection::handleTimer(const boost::system::error_code& ec)
			{
				if (!ec)
				{
					if (isSecureBoxDone)
						sendToTarget_Encrypt(*_heartBeatMsg);
					_timer.expires_from_now(boost::posix_time::seconds(20));
					_timer.async_wait(boost::bind(&Connection::handleTimer, this, boost::asio::placeholders::error));
				}
			}*/
			void Connection::encrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv)
			{
				if (!(*ctx = EVP_CIPHER_CTX_new()))
				{
					std::cout << "Error in encrypt initialization " << std::endl;
					LOG_INFO << "Error in encrypt initialization  ";
					ERR_print_errors_fp(stderr);
				}
				if (1 != EVP_EncryptInit_ex(*ctx, EVP_aes_256_gcm(), NULL, key, iv))
				{
					std::cout << "Error in encrypt initialization EVP_EncryptInit_ex" << std::endl;
					LOG_INFO << "Error in encrypt initialization EVP_EncryptInit_ex";
					ERR_print_errors_fp(stderr);
				}
				std::cout << "[INFO] Encrypt Context initialize " << std::endl;
			}

			void Connection::encrypt(EVP_CIPHER_CTX* ctx, unsigned char* plaintext, int plaintext_len, unsigned char* ciphertext, int* ciphertext_len)
			{
				// std::cout << "[INFO ] encrypt address " << (void *)ctx << " global " << (void *)ctx_encrypt << " value " << ctx_encrypt << std::endl;
				int len;
				if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
				{
					std::cout << "Error in encrypt  " << std::endl;
					LOG_INFO << "Error in encrypt  ";
					ERR_print_errors_fp(stderr); // Print OpenSSL error stack
				}
				*ciphertext_len = len;
				// std::cout << "Encrypted len " << len << " " << *ciphertext_len << std::endl;
			}
			void Connection::decrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv)
			{
				if (!(*ctx = EVP_CIPHER_CTX_new()))
				{
					std::cout << "Error in decrypt initialization " << std::endl;
					LOG_INFO << "Error in decrypt initialization  ";
					ERR_print_errors_fp(stderr);
				}

				if (1 != EVP_DecryptInit_ex(*ctx, EVP_aes_256_gcm(), NULL, key, iv))
				{
					std::cout << "Error in decrypt EVP_DecryptInit_ex " << std::endl;
					LOG_INFO << "Error in decrypt EVP_DecryptInit_ex  ";
					ERR_print_errors_fp(stderr);
				}
				std::cout << "[INFO] Decrypt Context initialize " << std::endl;
				LOG_INFO << "Decrypt Context initialize   ";
			}
			int Connection::decrypt(EVP_CIPHER_CTX* ctx, unsigned char* ciphertext, int ciphertext_len, unsigned char* plaintext, int* plaintext_len)
			{
				// std::cout << "decrypt address " << (void *)ctx << " global " << (void *)ctx_decrypt << " value " << ctx_decrypt << std::endl;

				int len;
				if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
				{
					std::cout << " [INFO] Error in decrypt  " << std::endl;
					LOG_INFO << "Error in decrypt  ";
					ERR_print_errors_fp(stderr); // Print OpenSSL error stack
				}

				*plaintext_len = len;
				//	std::cout << "Decrypt len " << len << " " << *plaintext_len << std::endl;

				return 0;
			}
			bool Connection::sendToTarget_SSL(DC_PACKET_WRAPPER& request)
			{
				try
				{
					if (!_connected || ssl == NULL)
						return false;

					MD5_CTX ctx;
					MD5_Init(&ctx);
					MD5_Update(&ctx, (uint8_t*)&request + sizeof(DC_PACKET_WRAPPER), request.getLength() - sizeof(DC_PACKET_WRAPPER));
					MD5_Final(_hash, &ctx);

					std::string str(_hash, _hash + sizeof _hash / sizeof _hash[0]);

					request.setMD5Checksum(str);
					request.setSequenceNo(++_seqNu);
					auto bytSent = SSL_write(ssl, &request, request.getLength());
					if (bytSent < 1)
					{
						LOG_INFO << " SSL REQUEST NOT SENT " << bytSent;
						std::cout << " [INFO] SSL REQUEST NOT SENT " << bytSent << std::endl;
					}
					printMessages((char*)&request + sizeof(DC_PACKET_WRAPPER));
					// printMessages( (char *) &request ) ;
				}
				catch (std::exception& ex)
				{
					LOG_INFO << ex.what();
				}
				return false;
			}
			bool Connection::sendToTarget(DC_PACKET_WRAPPER& request)
			{
				if (!_connected) return  false;

				MD5_CTX ctx;
				MD5_Init(&ctx);
				MD5_Update(&ctx, (uint8_t*)&request + sizeof(DC_PACKET_WRAPPER), request.getLength() - sizeof(DC_PACKET_WRAPPER));
				MD5_Final(_hash, &ctx);

				std::string str(_hash, _hash + sizeof _hash / sizeof _hash[0]);

				request.setMD5Checksum(str);
				request.setSequenceNo(++_seqNu);
				boost::asio::write(*m_socket, boost::asio::buffer(&request, request.getLength()));


				//writTimestamp() ;
				//writeLog( ( char *) &request , request.getLength() ) ;
				printMessages((char*)&request + sizeof(DC_PACKET_WRAPPER));
				//printMessages( (char *) &request ) ;

				return false;
			}

			bool Connection::sendToTarget_Encrypt(DC_PACKET_WRAPPER& request, bool isBoxLogin)
			{
				if (!isSecureBoxDone)
					return false;

				// std::cout << " key " << gwRsp.getCryptographic_Key() << " IV  " << gwRsp.getCryptographic_IV() << " initialize  " << initializeDone << std::endl;
				if (isBoxLogin)
				{
					OpenSSL_add_all_algorithms();
					encrypt_EVP_aes_256_gcm_init(&ctx_encrypt, (unsigned char*)gwRsp.getCryptographic_Key(), (unsigned char*)gwRsp.getCryptographic_IV());
					decrypt_EVP_aes_256_gcm_init(&ctx_decrypt, (unsigned char*)gwRsp.getCryptographic_Key(), (unsigned char*)gwRsp.getCryptographic_IV());
					initializeDone = true;
				}
				memset(ciphertext, 0, 128);
				ciphertext_len = 0;
				if (!_connected)
					return false;
				// unsigned char _ehash[16];

				MD5_CTX e_ctx;
				MD5_Init(&e_ctx);
				MD5_Update(&e_ctx, (uint8_t*)&request + sizeof(DC_PACKET_WRAPPER), request.getLength() - sizeof(DC_PACKET_WRAPPER));
				MD5_Final(_hash, &e_ctx);

				std::string str(_hash, _hash + sizeof _hash / sizeof _hash[0]);

				request.setMD5Checksum(str);
				request.setSequenceNo(++_seqNu);
				// encrypt_EVP_aes_256_gcm_init(&ctx_encrypt, (unsigned char *)gwRsp.getCryptographic_Key(), (unsigned char *)gwRsp.getCryptographic_IV());

				encrypt(ctx_encrypt, ((unsigned char*)&request + sizeof(DC_PACKET_WRAPPER)), (request.getLength() - sizeof(DC_PACKET_WRAPPER)), ciphertext, &ciphertext_len);

				LOG_INFO << ciphertext_len << " header + encrypted  " << 22 + ciphertext_len;

				request.setLength(22 + ciphertext_len);
				std::vector<unsigned char> buffer(22 + ciphertext_len);
				std::memcpy(buffer.data(), &request, 22);
				std::memcpy(buffer.data() + 22, ciphertext, ciphertext_len);

				// std::cout << "Buffer len  " << buffer.size() << std::endl;

				// boost::asio::write(*m_socket, boost::asio::buffer(&(header + request), header.getLength()));
				auto sendbytes = boost::asio::write(*m_socket, boost::asio::buffer(buffer));
				// std::cout << "Buffer len  " << buffer.size() << " send bytes " << sendbytes << std::endl;

				// boost::asio::write(*m_socket, boost::asio::buffer(&request, request.getLength()));

				// writTimestamp() ;
				// writeLog( ( char *) &request , request.getLength() ) ;
				printMessages((char*)&request + sizeof(DC_PACKET_WRAPPER));
				// printMessages( (char *) &request ) ;

				return false;
			}

			void Connection::receiveFromTarget()
			{

				memset(_data, 0, 65535);
				int dataRcvd = m_socket->read_some(boost::asio::buffer(_data, 65535));
				LOG_INFO << "RCVED PACKET " << dataRcvd;
				DC_PACKET_WRAPPER* hdr = nullptr;
				int pos = 0;
				int length = 0;
				while (pos < dataRcvd)
				{

					hdr = (DC_PACKET_WRAPPER*)(_data + pos);
					length = hdr->getLength();
					if (length <= 0)
					{
						LOG_INFO << " Packet length " << length << " total packet " << dataRcvd << " pos " << pos;

						int leftbyte = dataRcvd - pos;
						memcpy(_data, (char*)_data + pos, leftbyte);
						dataRcvd = leftbyte;
						dataRcvd += m_socket->read_some(boost::asio::buffer(_data + leftbyte, 65535 - leftbyte));
						pos = 0;

						continue;
					}
					else if (pos + length > dataRcvd)
					{
						/*LOG_INFO << " Packet length is greater than  datarecvd len  " << length << " total packet " << dataRcvd << " pos " << pos;
						memcpy(_data, (char*)_data + pos, length);
						dataRcvd = dataRcvd - pos;
						dataRcvd += m_socket->read_some(boost::asio::buffer((char*)_data + dataRcvd, 65535 - dataRcvd));
						pos = 0;*/


						LOG_INFO << " Packet length is greater than  datarecvd len  " << length << " total packet " << dataRcvd << " pos " << pos;

						int needMoreBytes = (pos + length) - dataRcvd;
						memcpy(_data, (char*)_data + pos, (dataRcvd - pos));
						dataRcvd = dataRcvd - pos;
						dataRcvd += m_socket->read_some(boost::asio::buffer((char*)_data + dataRcvd, needMoreBytes));
						pos = 0;

						continue;
					}

					//writTimestamp() ;
					//writeLog( ( char *)_data + pos , hdr->getLength() ) ;
					printMessages(_data + pos + sizeof(DC_PACKET_WRAPPER));
					//handleMessage(_data + pos + sizeof(DC_PACKET_WRAPPER));
					handleMessage(_data + pos + sizeof(DC_PACKET_WRAPPER), (hdr->getLength() - sizeof(DC_PACKET_WRAPPER)));

					pos += hdr->getLength();
				}




			}
			void Connection::receiveFromTarget_SSL()
			{
				if (ssl == NULL)
				{
					Sleep(1000);
					return;
				}
				memset(_data, 0, 65535);
				int dataRcvd = SSL_read(ssl, _data, 65535);
				// LOG_INFO << "RCVED PACKET " << dataRcvd;
				// std::cout << "RCVED PACKET from ssl " << dataRcvd << std::endl;
				DC_PACKET_WRAPPER* hdr = nullptr;
				int pos = 0;
				int length = 0;
				while (pos < dataRcvd)
				{

					hdr = (DC_PACKET_WRAPPER*)(_data + pos);
					length = hdr->getLength();
					if (length <= 0)
					{
						LOG_INFO << " Packet length " << length << " total packet " << dataRcvd << " pos " << pos;

						int leftbyte = dataRcvd - pos;
						memcpy(_data, (char*)_data + pos, leftbyte);
						dataRcvd = leftbyte;
						dataRcvd += SSL_read(ssl, _data + leftbyte, 65535 - leftbyte);
						pos = 0;

						continue;
					}
					else if (pos + length > dataRcvd)
					{
						/*LOG_INFO << " Packet length is greater than  datarecvd len  " << length << " total packet " << dataRcvd << " pos " << pos;
						memcpy(_data, (char*)_data + pos, length);
						dataRcvd = dataRcvd - pos;
						dataRcvd += m_socket->read_some(boost::asio::buffer((char*)_data + dataRcvd, 65535 - dataRcvd));
						pos = 0;*/

						LOG_INFO << " Packet length is greater than  datarecvd len  " << length << " total packet " << dataRcvd << " pos " << pos;

						int needMoreBytes = (pos + length) - dataRcvd;
						memcpy(_data, (char*)_data + pos, (dataRcvd - pos));
						dataRcvd = dataRcvd - pos;
						dataRcvd += SSL_read(ssl, _data + dataRcvd, needMoreBytes);
						pos = 0;

						continue;
					}

					// writTimestamp() ;
					// writeLog( ( char *)_data + pos , hdr->getLength() ) ;
					//printMessages(_data + pos + sizeof(DC_PACKET_WRAPPER));
					handleMessage(_data + pos + sizeof(DC_PACKET_WRAPPER));

					pos += hdr->getLength();
				}
			}


			// Function returns the number of days between two dates. Date Format:
			int Connection::daysBetweenDates(std::string date1, std::string date2)
			{
				std::stringstream ss(date1 + "-" + date2);
				int year, month, day;
				char hyphen;
				// Parse the first date into seconds
				ss >> year >> hyphen >> month >> hyphen >> day;
				struct tm starttm = { 0, 0, 0, day,
									month - 1, year - 1900 };
				time_t start = mktime(&starttm);
				// Parse the second date into seconds
				ss >> hyphen >> year >> hyphen
					>> month >> hyphen >> day;
				struct tm endtm = { 0, 0, 0, day,
									month - 1, year - 1900 };
				time_t end = mktime(&endtm);
				// Find out the difference and divide it
				// by 86400 to get the number of days
				return abs(end - start) / 86400;
			}
			std::string Connection::getCurrentDate()
			{
				time_t t = time(NULL);
				tm* ltm = localtime(&t);
				std::stringstream date;
				date << 1900 + ltm->tm_year
					<< "-"
					<< 1 + ltm->tm_mon
					<< "-"
					<< ltm->tm_mday
					//  << " "
					//  << 1 + ltm->tm_hour
					//  << ":"
					//  << 1 + ltm->tm_min
					//  << ":"
					//  << 1 + ltm->tm_sec
					;
				return date.str();
			}
			//selectArray is  a utility function that is used to
			//randomly generate a integer in the range 1 to 4 (both inclusive)
			int Connection::selectArray()
			{
				//srand(time(NULL));
				int i = rand() % 5;
				if (i == 0)
					i++;
				return i;
			}

			//getKey() is another utility function that is used to randomly generate
			//an integer in the range 0 to 25 (both inclusive)
			int Connection::getKey()
			{
				/*srand(time(NULL));*/

				int key = rand() % 26;
				return key;
			}
			std::string Connection::generate_password(int length)
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
			void Connection::updateConfigFile(std::string password, std::string newPassword, std::string date)
			{
				std::ifstream file;
				std::vector < std::string > fileData;
				std::string line;
				int positionOfPasswordFiled = 0, posOfNewPassField = 0, posOfDateField = 0;
				file.open(_configFileName);
				int lineNum = 0;
				if (file.is_open()) {
					while (file) {
						getline(file, line);
						fileData.push_back(line);
						if (line.find("CURRENT_PASSWORD") != std::string::npos) {
							positionOfPasswordFiled = lineNum;
						}
						if (line.find("NEW_PASSWORD") != std::string::npos) {
							posOfNewPassField = lineNum;
						}
						if (line.find("PASSWORD_RESET_DATE") != std::string::npos) {
							posOfDateField = lineNum;
						}
						lineNum++;
					}
					file.close();
				}
				// fileData[++positionOfPasswordFiled] = "PASSWORD="+password;																						
				// fileData[++positionOfPasswordFiled] = ("NEWPASSWORD="+newPassword);
				// fileData[++positionOfPasswordFiled] = ("PASSWORD_RESET_DATE="+date);
				fileData[positionOfPasswordFiled] = "CURRENT_PASSWORD=" + password;
				fileData[posOfNewPassField] = ("NEW_PASSWORD=" + newPassword);
				fileData[posOfDateField] = ("PASSWORD_RESET_DATE=" + date);

				std::ofstream fout;
				fout.open(_configFileName);
				if (fout.is_open()) {
					//std::cout<<"FoutOpened Successfully\n";

					for (int i = 0; i < fileData.size(); i++)
					{
						fout << fileData[i] << "\n";
						fout.flush();
					}
				}
				fout.close();
			}


		}
	}
}
