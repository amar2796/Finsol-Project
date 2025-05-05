#include "ExchangeProcess.h"
namespace FIN {
	namespace NSE {
		namespace TRIMMED {
			namespace COM {

				ExchangeProcess::ExchangeProcess(ConfigParameter* configDetail)
					: TRIMMED::Connection(CONSTANT::NSECOM_ORDER_CONFIG, configDetail)
				{
					configDetails = configDetail;
				}

				void ExchangeProcess::sendSystemUpdate()
				{
					MS_SYSTEM_INFO_REQ sysInfo(configDetails->trader_id);
					sendToTarget_Encrypt(sysInfo);

					std::cout << "[INFO] sending SYSTEM_INFO_REQUEST" << std::endl;
					std::string log;
					sysInfo.getString(log);
					LOG_INFO << "[INFO] sending SYSTEM_INFO_REQUEST " << log;
				}

				void ExchangeProcess::startCommunication()
				{
					LOG_INFO << "Starting Communication with exchange";
					_seqNu = 0;
					sendGatewayRequest();
				}

				void ExchangeProcess::sendGatewayRequest()
				{
					GR_REQUEST grReq(configDetails->trader_id, configDetails->boxId, configDetails->brokerId);
					sendToTarget_SSL(grReq);
					std::string log;
					grReq.getString(log);
					std::cout << "[INFO] send GR_REQUEST" << std::endl;
					LOG_INFO << "[INFO] send GR_REQUEST " << log;
				}

				void ExchangeProcess::sendBoxLoginRequest(const std::string& key, const std::string& ip, const int port)
				{
					BOX_SIGN_ON_REQUEST_IN box_login(configDetails->trader_id, configDetails->boxId, configDetails->brokerId);

					box_login.setSessionKey(key);
					std::string log;
					box_login.getString(log);
					if (sendToTarget_Encrypt(box_login, true))
					{
						std::cout << "[INFO] send BOX_SIGN_ON_REQUEST" << std::endl;
						LOG_INFO << "[INFO] send BOX_SIGN_ON_REQUEST " << log;
					}
				}
				void ExchangeProcess::sendSecureBoxLogin_Req(const std::string& sessionKey, const std::string& ip, const int port)
				{
					CloseSSLSocket();
					connectSocket(ip, port);
					isSSLComplted = true;

					std::cout << "[INFO] NOW SSL COMPLETED , NORMAL TCP INTITIATE" << std::endl;
					LOG_INFO << "[INFO] NOW SSL COMPLETED , NORMAL TCP INTITIATE";

					SECURE_BOX_REGISTRATION_REQUEST_IN secure_req(configDetails->trader_id, configDetails->boxId, configDetails->brokerId);
					sendToTarget(secure_req);

					LOG_INFO << "[INFO] send SECURE_BOX_REGISTRATION_REQUEST" + sessionKey;
					std::cout << "[INFO] send SECURE_BOX_REGISTRATION_REQUEST" << std::endl;
				}

				void ExchangeProcess::sendLogonMesage()
				{
					SIGNON_REQUETS signOnReq(configDetails->trader_id);
					signOnReq.setUserID(configDetails->trader_id);
					signOnReq.setPassword(configDetails->current_password);
					std::string currDate = getCurrentDate();
					if (configDetails->isPasswordResetMannually)
					{
						std::cout << "[INFO] Reset password manually" << std::endl;
						LOG_INFO << "[INFO] Reset password manually";
						if (configDetails->changePassword)
						{
							signOnReq.setNewPassword(configDetails->newPassword);
							updateConfigFile(configDetails->current_password, configDetails->newPassword, currDate);
						}
					}
					else 
					{ 
						int dateDiff = daysBetweenDates(currDate, configDetails->passwordResetDate);
						if (dateDiff >= 14) 
						{
							std::cout << "[INFO] Reset password" << std::endl;
							LOG_INFO << "[INFO] Reset password";

							//generateNewPassword-> password should contain 1 upper case letter,1 lower case letter,1 numeral and 1 special character from the list @#$%&*/\.
							std::string newPassword = generate_password(8);
							signOnReq.setNewPassword(newPassword);
							updateConfigFile(newPassword, newPassword, currDate);
						}
					}
					signOnReq.setBrokerID(configDetails->brokerId);
					signOnReq.setVersionNumber(configDetails->versionNumber);
					signOnReq.setShowIndex('T');
					sendToTarget_Encrypt(signOnReq);
					
					std::string log;
					signOnReq.getString(log);
					std::cout << "[INFO] sending SIGNON_REQUET" << std::endl;
					LOG_INFO << "[INFO] sending SIGNON_REQUET " << log;
				}

				void ExchangeProcess::sendMessageDownloadHandle(int currStream)
				{
					MS_MESSAGE_DOWNLOAD  downloadReq(configDetails->trader_id);
					downloadReq.setStreamNo(currStream);

					if (sendToTarget_Encrypt(downloadReq))
					{
						std::cout << "[INFO] Message Download start stream number : " << currStream << std::endl;
						LOG_INFO << "[INFO] Message Download start stream number : " << currStream;
					}
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

					switch (hdr->getTransCode())
					{
					case TransCode_GR_RESPONSE:
					{
						auto gwRsp = (GR_RESPONSE*)data;

						std::string log;
						gwRsp->getString(log);
						LOG_INFO << "[INFO] receive GR_RESPONSE : " + log;
						std::cout << "[INFO] receive GR_RESPONSE" << std::endl;

						this->gwRsp.setCryptographic_Key(gwRsp->getCryptographic_Key());
						this->gwRsp.setCryptographic_IV(gwRsp->getCryptographic_IV());
						this->gwRsp.setSessionKey(gwRsp->getSessionKey());
						if (hdr->getErrorCode() == 0)
						{

							std::string ip;
							int port;
							if (configDetails->useSecondaryIp)
							{
								ip = configDetails->connectHost2;
								port = configDetails->connectPort2;
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
							std::cout << "[INFO] GR_RESPONSE REJECT, Error code : " << gwRsp->getErrorCode() << std::endl;
							LOG_INFO << "[INFO] GR_RESPONSE REJECT, Error code : " << gwRsp->getErrorCode();
						}
						return;
					}
					break;
					case TransCode_SECURE_BOX_OUT:
					{
						MESSAGE_HEADER* secure = (MESSAGE_HEADER*)data;
						std::string log;
						secure->getString(log);
						std::cout << "[INFO] receive SECURE_BOX_REGISTRATION_RESPONSE" << std::endl;
						LOG_INFO << "[INFO] receive SECURE_BOX_REGISTRATION_RESPONSE " << log;

						isSecureBoxDone = true;

						sendBoxLoginRequest(gwRsp.getSessionKey(), gwRsp.getIpAddress(), gwRsp.getPort());
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
						
						std::cout << "[INFO] receive BOX_SIGN_OFF, Error Code : " << secure->getErrorCode() << "\n[INFO] send SINGOFF" << std::endl;
						LOG_INFO << "[INFO] BOX_SIGN_OFF ERROR " << log;
						LOG_INFO << "Closing exchange connection.";

						MS_SIGNOFF logout(configDetails->trader_id);
						if (isSecureBoxDone)
							sendToTarget_Encrypt(logout);
						else
							sendToTarget_SSL(logout);
					}
					break;
					case TransCode_SIGN_OFF_REQUEST_OUT:
					{
						FIN::setConsoleColor(4);
						std::cout << "Successfully logout." << std::endl;
						FIN::setConsoleColor(7);
						LOG_INFO << "Successfully logout.";
						
						stopConnection();
					}
					break;
					case TransCode_BOX_SIGN_ON_REQUEST_OUT:
					{
						auto box_out = (BOX_SIGN_ON_REQUEST_OUT*)(decryptedtext);
						
						if (box_out->getErrorCode() == 0)
						{
							std::string log;
							box_out->getString(log);
							std::cout << "[INFO] receive BOX_SIGN_ON_RESPONSE" << std::endl;
							LOG_INFO << "[INFO] receive BOX_SIGN_ON_RESPONSE " << log;

							sendLogonMesage();
						}
					}
					break;
					case TransCode_SIGN_ON_REQUEST_OUT:
					{
						SIGNON_CONFIRMATION* signOnResp = (SIGNON_CONFIRMATION*)decryptedtext;
						if (signOnResp->getErrorCode() == 0)
						{
							std::cout << "[INFO] receive SIGN_ON_RESPONSE" << std::endl;
							std::string log;
							signOnResp->getString(log);
							LOG_INFO << "[INFO] receive SIGN_ON_RESPONSE " << log;

							sendSystemUpdate();
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
						if (infoData->getErrorCode() == 0)
						{
							std::cout << "[INFO] receive SYSTEM_INFO_RESPONSE" << std::endl;
							std::string log;
							infoData->getString(log);
							LOG_INFO << "[INFO] receive SYSTEM_INFO_RESPONSE " << log;
							onLogon();
							

							//wait for oms connection
							while (!getOmsConnectionStatus())
							{
								std::this_thread::sleep_for(std::chrono::seconds(1));
								std::cout << "Waiting for oms connection to send Message Download" << std::endl;
							}

							streamCounter = infoData->getStreamNo();
							currStream = 1;
							LOG_INFO << "StreamTotal " << streamCounter << " current stream:" << currStream;

							sendMessageDownloadHandle(currStream);
						}
						else
						{
							LOG_INFO << "[INFO] receive SYSTEM_INFO_RESPONSE error";
							std::cout << "[INFO] receive SYSTEM_INFO_RESPONSE error" << std::endl;
						}
					}
					break;
					case TransCode_HEADER_RECORD:
					{
						LOG_INFO << "Message Download process for a current stream : " << currStream;

					} 
					break;
					case TransCode_MESSAGE_RECORD:
					{
						LOG_INFO << "MESSAGE RECORD " << TransCode_MESSAGE_RECORD << "Rcvd Length " << hdr->getMessageLength() << " MSGPassing Length " << hdr->getMessageLength() - sizeof(MESSAGE_HEADER);

						//send inner header like actual data
						messageDownloadHandle((char*)decryptedtext + sizeof(MESSAGE_HEADER), (hdr->getMessageLength() - sizeof(MESSAGE_HEADER)));

					}
					break;
					case TransCode_TRAILER_RECORD:
					{
						//_isMessageDownload = false;
						//std::cout<<"Trailer Record\n";
						LOG_INFO << "Message Download completed this stream : " << currStream;
						if (currStream < streamCounter)
						{
							currStream++;
							
							LOG_INFO << "StreamTotal : " << streamCounter << " current stream : " << currStream;
							
							sendMessageDownloadHandle(currStream);
						}
						else
						{
							//checking which order not getting any response then cancel 
							if (!orderHistoryIsEmpty())
							{
								for (auto ClOrdId : getClOrdIdInstance())
								{
									rejectMessage(ClOrdId, "unknown order");
								}
							}

							setExchangeIsUPStatus(true);
							std::cout << "[INFO] Message Download completed." << std::endl;
							LOG_INFO << "[INFO] Message Download completed.";
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
