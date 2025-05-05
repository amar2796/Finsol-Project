#include "ExchangeProcess.h"
#include <thread>
namespace FIN {
	namespace NSE {
		namespace TRIMMED {
			namespace FO {

				ExchangeProcess::ExchangeProcess(boost::asio::io_service& ioService)
					: TRIMMED::Connection(CONSTANT::NSE_FO_ORDER_CONFIG, ioService)
				{
				}

				void ExchangeProcess::sendSystemUpdate()
				{
					LOG_INFO << "Send System Info";
					MS_SYSTEM_INFO_REQ sysInfo(getConnectionParam()._traderId);
					//sendToTarget(sysInfo);
					sendToTarget_Encrypt(sysInfo);

				}

				void ExchangeProcess::startCommunication()
				{
					LOG_INFO << "Starting Communication with exchange";
					_seqNu = 0;
					sendGatewayRequest();
				}

				void ExchangeProcess::sendGatewayRequest()
				{
					LOG_INFO << "Sending Gateway Router Request";
					GR_REQUEST grReq(getConnectionParam()._traderId, getConnectionParam()._boxId, getConnectionParam()._brokerId);
					sendToTarget_SSL(grReq);
				}

				void ExchangeProcess::sendBoxLoginRequest(const std::string& key, const std::string& ip, const int port)
				{
					BOX_SIGN_ON_REQUEST_IN box_login(getConnectionParam()._traderId, getConnectionParam()._boxId, getConnectionParam()._brokerId);

					box_login.setSessionKey(key);
					std::string log;
					box_login.getString(log);
					sendToTarget_Encrypt(box_login, true);
				}
				void ExchangeProcess::sendSecureBoxLogin_Req(const std::string& sessionKey, const std::string& ip, const int port)
				{
					std::cout << "[INFO] NOW SSL COMPLETED , NORMAL TCP INTITIATE" << std::endl;
					LOG_INFO << "[INFO] NOW SSL COMPLETED , NORMAL TCP INTITIATE";

					getConnectionParam()._connectHost = ip;
					getConnectionParam()._connectPort = port;

					CloseSSLSocket();
					connectSocket();
					isSSLComplted = true;

					SECURE_BOX_REGISTRATION_REQUEST_IN secure_req(getConnectionParam()._traderId, getConnectionParam()._boxId, getConnectionParam()._brokerId);

					DEVLOG("Sending secure Login Request " + sessionKey);

					sendToTarget(secure_req);
				}

				void ExchangeProcess::sendLogonMesage()
				{
					SIGNON_REQUETS signOnReq(getConnectionParam()._traderId);
					signOnReq.setUserID(getConnectionParam()._traderId);
					signOnReq.setPassword(getConnectionParam()._password);

					std::string currDate = getCurrentDate();
					if (getConnectionParam()._isPasswordResetMannually)
					{
						if (getConnectionParam()._changePassword)
						{
							signOnReq.setNewPassword(getConnectionParam()._newPassword);
							updateConfigFile(getConnectionParam()._password, getConnectionParam()._newPassword, currDate);
						}
					}
					else { //Automatically reset password on every 14th day
						//Calculate the differnce between Today and the password reset date.
						int dateDiff = daysBetweenDates(currDate, getConnectionParam()._passwordResetDate);
						if (dateDiff >= 14) {
							//generateNewPassword-> password should contain 1 upper case letter,1 lower case letter,1 numeral and 1 special character from the list @#$%&*/\.
							std::string newPassword = generate_password(8);
							signOnReq.setNewPassword(newPassword);
							updateConfigFile(newPassword, newPassword, currDate);
							//std::cout<<"Generated Password:"<<newPassword<<" Date:"<<currDate<<std::endl;
						}
					}

					signOnReq.setBrokerID(getConnectionParam()._brokerId);
					signOnReq.setVersionNumber(getConnectionParam()._versionNumber);
					signOnReq.setShowIndex('T');
					sendToTarget_Encrypt(signOnReq);

					//sendToTarget(signOnReq);
				}

				void ExchangeProcess::disconnectOmsConn()
				{
					sendRequestDisconnectOmsConn();
				}

				void ExchangeProcess::handleMessage(char* data, int value)
				{
					MESSAGE_HEADER* hdr = (MESSAGE_HEADER*)data;
					if (isSecureBoxDone)
					{
						//std::cout << "This should be decrypt " << hdr->getTransCode() << " " << value << std::endl;
						memset(decryptedtext, 0, 128);
						decryptedtext_len = 0;
						// std::cout << "This should be decrypt " << hdr->getTransCode() << " " << value << std::endl;
						decrypt(ctx_decrypt, (unsigned char*)data, value, decryptedtext, &decryptedtext_len);
						hdr = (MESSAGE_HEADER*)decryptedtext;
						//std::cout << "This should be decrypt " << hdr->getTransCode() << " " << value << std::endl;
					}
					/*else
						std::cout << "This should not be  decrypt " << hdr->getTransCode() << " " << value << std::endl;*/

					switch (hdr->getTransCode())
					{
					case TransCode_GR_RESPONSE:
					{

						auto gwRsp = (GR_RESPONSE*)data;
						this->gwRsp.setCryptographic_Key(gwRsp->getCryptographic_Key());
						this->gwRsp.setCryptographic_IV(gwRsp->getCryptographic_IV());
						this->gwRsp.setSessionKey(gwRsp->getSessionKey());
						if (hdr->getErrorCode() == 0)
						{

							std::string ip;
							int port;
							if (getConnectionParam().getParam<bool>("SESSION.USE_SECONDARY_IP", true))
							{
								ip = getConnectionParam()._connectHost2;
								port = getConnectionParam()._connectPort2;
							}
							else
							{
								ip = gwRsp->getIpAddress();
								port = gwRsp->getPort();
							}

							sendSecureBoxLogin_Req(gwRsp->getSessionKey(), ip, port);
						}
						else
						{
							gwRsp->print();
						}
						std::string log;
						gwRsp->getString(log);
						LOG_INFO << log;
						std::cout << log << std::endl;
						return;
					}
					break;
					case TransCode_BOX_SIGN_OFF:
					{
						MS_BOX_SIGN_OFF* secure;
						if (isSecureBoxDone)
							secure = (MS_BOX_SIGN_OFF*)decryptedtext;
						else
							secure = (MS_BOX_SIGN_OFF*)data;

						std::string log;
						secure->getString(log);
						LOG_INFO << log;
						std::cout << "[INFO ] BOX SIGN OUT, Error Code : " << secure->getErrorCode() << "\nClosing exchange connection..." << std::endl;
						LOG_INFO << "[INFO ] BOX SIGN OUT ERROR" << log;
						LOG_INFO << "Closing exchange connection.";
					}
					break;
					case TransCode_SECURE_BOX_OUT:
					{
						MESSAGE_HEADER* secure = (MESSAGE_HEADER*)data;
						std::string log;
						secure->getString(log);
						LOG_INFO << log;
						std::cout << "[INFO ] " << log << std::endl;
						LOG_INFO << "[INFO ] " << log;

						isSecureBoxDone = true;

						// encrypt_EVP_aes_256_gcm_init(&ctx_encrypt, (unsigned char *)gwRsp.getCryptographic_Key(), (unsigned char *)gwRsp.getCryptographic_IV());
						// decrypt_EVP_aes_256_gcm_init(&ctx_decrypt, (unsigned char *)gwRsp.getCryptographic_Key(), (unsigned char *)gwRsp.getCryptographic_IV());

						// sendLogonMesage();
						sendBoxLoginRequest(gwRsp.getSessionKey(), gwRsp.getIpAddress(), gwRsp.getPort());
						return;
					}
					case TransCode_BOX_SIGN_ON_REQUEST_OUT:
					{
						auto box_out = (BOX_SIGN_ON_REQUEST_OUT*)(decryptedtext);
						// length = hdr->getLength();
						std::string log;
						box_out->getString(log);
						std::cout << log << std::endl;

						if (hdr->getErrorCode() == 0)
						{
							sendLogonMesage();
						}
					}
					break;
					case TransCode_SIGN_ON_REQUEST_OUT:
					{
						SIGNON_CONFIRMATION* signOnResp = (SIGNON_CONFIRMATION*)decryptedtext;
						if (signOnResp->getErrorCode() == 0)
						{
							sendSystemUpdate();
							onLogon();
						}
						else
						{
							((MS_ERROR_RESPONSE*)decryptedtext)->print();
						}
					}
					break;
					case TransCode_SYSTEM_INFORMATION_OUT:
					{
						MS_SYSTEM_INFO_DATA* infoData = (MS_SYSTEM_INFO_DATA*)decryptedtext;

						while (!getOmsConnectionStatus())
						{
							std::this_thread::sleep_for(std::chrono::seconds(1));
							std::cout << "Waiting for oms connection to send Message Download" << std::endl;
						}

						std::cout << "Message Download start." << std::endl;
						LOG_INFO << "Message Download start.";

						MS_MESSAGE_DOWNLOAD  downloadReq(getConnectionParam()._traderId);
						streamCounter = infoData->getStreamNo();
						currStream = 1;
						//std::string s = std::to_string(currStream);
						LOG_INFO << "StreamTotal " << streamCounter << " current stream:" << currStream;
						//std::string s=infoData->getAlphaChar();
						// downloadReq.setAlphaChar(s);
						downloadReq.setStreamNo(currStream);
						downloadReq.setSequenceNumber(0);
						//downloadReq.print();
						sendToTarget_Encrypt(downloadReq);
					}
					break;
					case TransCode_HEADER_RECORD:
					{
						//_isMessageDownload = true;

						//std::cout<<"Inside Header Record(handle message)\n";
						LOG_INFO << "Message Download process for a current stream : " << currStream;

					} break;
					case TransCode_MESSAGE_RECORD:
					{
						LOG_INFO << "MESSAGE RECORD " << TransCode_MESSAGE_RECORD << "Rcvd Length " << hdr->getMessageLength() << " MSGPassing Length " << hdr->getMessageLength() - sizeof(MESSAGE_HEADER);

						//send inner header like actual data
						messageDownloadHandle((char*)decryptedtext + sizeof(MESSAGE_HEADER), (hdr->getMessageLength() - sizeof(MESSAGE_HEADER)));

					}break;
					case TransCode_TRAILER_RECORD:
					{
						//_isMessageDownload = false;
						//std::cout<<"Trailer Record\n";
						LOG_INFO << "Message Download completed this stream : " << currStream;
						if (currStream < streamCounter)
						{
							MS_MESSAGE_DOWNLOAD  downloadReq(getConnectionParam()._traderId);
							currStream++;
							LOG_INFO << "StreamTotal : " << streamCounter << " current stream : " << currStream;
							//std::string s=infoData->getAlphaChar();
							downloadReq.setStreamNo(currStream);
							downloadReq.setSequenceNumber(0);
							//                        downloadReq.print() ;
							sendToTarget_Encrypt(downloadReq);
						}
						else
						{
							//check not response order
							if (!orderHistoryIsEmpty())
							{
								while (!orderHistoryIsEmpty())
								{
									rejectMessage(getClOrderid(), "unknown order");
								}
							}

							setExchangeIsUPStatus(true);
							std::cout << "Message Download completed." << std::endl;
							LOG_INFO << "Message Download completed.";
						}
					}break;

					case TransCode_ORDER_ERROR: onMessage((ORDER_ERROR_RESPONSE*)decryptedtext); break;
					case TransCode_ORDER_CONFIRMATION_TR: onMessage((ORDER_NEW_CONFIRMATION_TR*)decryptedtext); break;
					case TransCode_TRADE_CONFIRMATION: onMessage((MS_TRADE_CONFIRM*)decryptedtext); break;
					case TransCode_TRADE_CONFIRMATION_TR: onMessage((MS_TRADE_CONFIRM_TR*)decryptedtext); break;
					case TransCode_ORDER_CXL_CONFIRMATION_TR: onMessage((ORDER_CXL_CONFIRMATION_TR*)decryptedtext); break;
					case TransCode_ORDER_MOD_CONFIRMATION_TR: onMessage((ORDER_MOD_CONFIRMATION_TR*)decryptedtext); break;
					case TransCode_ORDER_MOD_REJECT: onMessage((ORDER_MODIFICATION_ERROR*)decryptedtext); break;
					case TransCode_ORDER_CANCEL_REJECT: onMessage((ORDER_CANCELATION_ERROR*)decryptedtext); break;
					case TransCode_ORDER_CONFIRMATION: onMessage((ORDER_CONFIRM_RESPONSE*)decryptedtext); break;
					case TransCode_ORDER_CANCEL_CONFIRMATION:onMessage((ORDER_CANCEL_CONFIRMATION*)decryptedtext); break;
					case TransCode_BATCH_ORDER_CANCEL:onMessage((ORDER_CANCEL_CONFIRMATION*)decryptedtext); break;
					case TransCode_ORDER_MOD_CONFIRMATION: onMessage((ORDER_MODIFICATION_CONFIRMATION*)decryptedtext); break;
					default:
					{
						LOG_INFO << "Transaction Code Not Handled for Execution: " << hdr->getTransCode();
					}
					}
					printMessages((char*)decryptedtext);

				}
				std::string strReq;
				std::string strResp;
#define LOG_FO_RESPONSE(RESP)   strResp.clear();    (((FO::RESP *)(data))->getString(strResp));    LOG_INFO << "[EX_RCV_MSG_DWN] " << strResp;
#define LOG_FO_REQUEST(REQ)     strReq.clear();     (((FO::REQ *)(data - sizeof(DC_PACKET_WRAPPER)))->getString(strReq)); LOG_INFO << "[EX_SND_MSG_DWN] " << strReq;


				void ExchangeProcess::messageDownloadHandle(char* data, int msgLength) //440
				{
					std::string strR;
					int pos = 0, inr_len = 0;
					while (pos < msgLength)
					{
						//Sleep(100);
						//LOG_INFO << " ORDER DOWNLOAD RESPONSE WITH 500 MICRO SECOND SLEEP ";

						MESSAGE_HEADER* hdr = (MESSAGE_HEADER*)(data + pos);
						std::string s2;
						hdr->getString(s2);
						LOG_INFO << "INNER_HDR " << s2;

						inr_len = hdr->getMessageLength();
						switch (hdr->getTransCode())
						{
						case TransCode_SIGN_ON_REQUEST_OUT:
						{
							strR.clear();
							(((SIGNON_CONFIRMATION*)(data + pos))->getString(strR));
							LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;

							//LOG_FO_RESPONSE(SIGNON_CONFIRMATION);
						}
						break;
						case TransCode_GR_RESPONSE:
						{
							strR.clear();
							(((GR_RESPONSE*)(data + pos))->getString(strR));
							LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;

							//LOG_FO_RESPONSE(GR_RESPONSE);
						}
						break;
						case TransCode_ORDER_MOD_CONFIRMATION:
						{
							onMessage((ORDER_MODIFICATION_CONFIRMATION*)(data + pos));
							//strR.clear();
							//(((ORDER_MODIFICATION_CONFIRMATION*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_ORDER_CONFIRMATION:
						{
							onMessage((ORDER_CONFIRM_RESPONSE*)(data + pos));
							//strR.clear();
							//(((ORDER_CONFIRM_RESPONSE*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_TRADE_CONFIRMATION:
						{
							onMessage((MS_TRADE_CONFIRM*)(data + pos));
							//strR.clear();
							//(((MS_TRADE_CONFIRM*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_ORDER_CONFIRMATION_TR:
						{
							onMessage((ORDER_NEW_CONFIRMATION_TR*)(data + pos));
							//strR.clear();
							//(((ORDER_NEW_CONFIRMATION_TR*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;

						case TransCode_ORDER_ERROR:
						{
							onMessage((ORDER_ERROR_RESPONSE*)(data + pos));
							//strR.clear();
							//(((ORDER_ERROR_RESPONSE*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_BOX_SIGN_ON_REQUEST_OUT:
						{
							strR.clear();
							(((MS_BOX_SIGN_OFF*)(data + pos))->getString(strR));
							LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;

							//LOG_FO_RESPONSE(MS_BOX_SIGN_OFF);
						}
						break;
						case TransCode_TRADE_CONFIRMATION_TR:
						{
							onMessage((MS_TRADE_CONFIRM_TR*)(data + pos));
							//strR.clear();
							//(((MS_TRADE_CONFIRM_TR*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_BATCH_ORDER_CANCEL: {}	break;
						case TransCode_ORDER_CANCEL_CONFIRMATION:
						{
							onMessage((ORDER_CANCEL_CONFIRMATION*)(data + pos));
							//strR.clear();
							//(((ORDER_CANCEL_CONFIRMATION*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_ORDER_CXL_CONFIRMATION_TR:
						{
							onMessage((ORDER_CXL_CONFIRMATION_TR*)(data + pos));
							//strR.clear();
							//(((ORDER_CXL_CONFIRMATION_TR*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_ORDER_MOD_CONFIRMATION_TR:
						{
							onMessage((ORDER_MOD_CONFIRMATION_TR*)(data + pos));
							//strR.clear();
							//(((ORDER_MOD_CONFIRMATION_TR*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_ORDER_MOD_REJECT:
						{
							onMessage((ORDER_MODIFICATION_ERROR*)data + pos);
							//strR.clear();
							//(((ORDER_MODIFICATION_ERROR*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_ORDER_CANCEL_REJECT:
						{
							onMessage((ORDER_CANCELATION_ERROR*)data + pos);
							//strR.clear();
							//(((ORDER_CANCELATION_ERROR*)(data + pos))->getString(strR));
							//LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
						}
						break;
						case TransCode_SYSTEM_INFORMATION_OUT:
						{
							strR.clear();
							(((MS_SYSTEM_INFO_DATA*)(data + pos))->getString(strR));
							LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
							//LOG_FO_RESPONSE(MS_SYSTEM_INFO_DATA);
						}
						break;
						case TransCode_PRICE_CONFIRMATION:
						{
							strR.clear();
							(((MS_OE_REQUEST*)(data + pos))->getString(strR));
							LOG_INFO << "[EX_RCV_MSG_DWN] " << strR;
							//LOG_FO_RESPONSE(MS_OE_REQUEST);
						}
						break;
						default:
						{
							LOG_INFO << "Transaction Code Not Handled for Execution:(Inner Header) " << hdr->getTransCode();
						}
						}
						pos += inr_len;
						if (inr_len <= 0) //This is the case when exchange in sending 0 in any message field(i.e. if exchange sent340 bytes of data in which first 240 bytes are casted but other 100 bytes contains 0 then we have to put stop condition here otherwise our code goes infinite
						{
							LOG_INFO << "Inner Header receive 0 length";
							pos = msgLength;
						}
					} //close of while(pos<len)
				}

			} // namespace FO
		}     // namespace TRIMMED
	}         // namespace NSE
} // namespace FIN
