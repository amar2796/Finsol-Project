#include "Adapter.h"
namespace FIN {
	namespace IIBX {
		Adapter::Adapter(boost::asio::io_service& ioService):ExchangeConnection(FIN::FIN_CONST::FIN_CONFIG_FILE, ioService),
		_omsSender(ioService)
		{
			init();
			//isIECConnectionRes = false;
			throttlemsg = config_param.getValue<int>(FIN::FIN_CONST::SESSION+FIN::FIN_CONST::SEP_DOT+FIN::FIN_CONST::THROTTLE_MSG_COUNT, 0);
			throttlesec = config_param.getValue<int>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::THROTTLE_MSG_SEC, 0);
			seqNumber = 0;
			slidingThrottel= new SlidingWindowThrottling(throttlemsg, throttlesec);					//5 msg/sec
		}	

		void Adapter::init()
		{
			config_param.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
			tMemCode = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::TRADER_ID, "");
			cMemCode = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::MEMBER_ID, "");
			dealerCode = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::DEALER_CODE, "");
			ctclCode = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::CTCL_CODE, "");

			password = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::PASSWORD, "");
			
			confirmationPort = config_param.getValue<int>(FIN::FIN_CONST::OMS_CONNECTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::OMS_LISTEN_PORT, 0);
			sender_port = config_param.getValue<int>(FIN::FIN_CONST::UDP_CONNECTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::UDP_SEND_PORT, 0);
			udp_sender_ip = config_param.getValue<std::string>(FIN::FIN_CONST::UDP_CONNECTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::UDP_SEND_IP, "");
			rcv_port = config_param.getValue<int>(FIN::FIN_CONST::UDP_CONNECTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::UDP_RCV_PORT, 0);
			rabbit_udp = config_param.getValue<int>(FIN::FIN_CONST::OMS_COMM + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::IS_RABBIT_UDP, 0);

			isPasswordChange = config_param.getValue<bool>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::ENABLE_PASS_CHANGE, false);
			FIN::CurrentPath _currentPath;
			configFileLocation = _currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
			isIECConnectionRes = false;

			algoID = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::ALGO_ID, "");

			//for contract send
			isContractDownload = config_param.getValue<bool>(FIN::FIN_CONST::CONTRACT_DOWNLOAD + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::ENABLE_CON_DOWN, false);
			if (isContractDownload)
			{
				contractIP = config_param.getValue<std::string>(FIN::FIN_CONST::CONTRACT_DOWNLOAD + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::PUBLISH_IP, "");
				contractPort = config_param.getValue<int>(FIN::FIN_CONST::CONTRACT_DOWNLOAD + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::PUBLISH_PORT, 0);
				std::cout << "Connection for contract send for IP : " << contractIP << " PORT : " << contractPort << std::endl;
				contract_send = new ConnectionForContract(contractIP, contractPort);
			}
			
			isMarketPictureDownload = false;

			LOG_INFO << "Initilize complete Adapter";
		}
		void Adapter::startExchangeConnection()
		{
			while (1)
			{
				try
				{
					init();							//read config every time
					if (createTCPConnection())
					{
						connectSocket();
						
						while (_connected)
						{
							try
							{
								receiveFromTarget();
							}
							catch (std::exception& e)
							{
								if (isFirst)
								{
									FIN::setConsoleColor(4);
									std::cout << "Exchange Disconnected" << std::endl;
									FIN::setConsoleColor(7);

									disableConnection();
									LOG_INFO << "Exchange Disconnected";
								}
								LOG_INFO << "Exception Rcvd : " << __func__ << e.what();
								break;
							}
						}
					}

					std::cout << "Reconnecting after 5 Seconds" << std::endl;

					disableConnection();
					if (isFirst)
					{
						_omsSender.sendLogoutMsg(_omsSender.logoutMessage());
						_omsSender.closeSocketConnection();
						isFirst = false;
					}

					Sleep(5000); // Delay before retrying the connection
				}
				catch (std::exception& e)
				{
					std::cout << "Exception Rcvd : " << __func__ << e.what() << std::endl;
					LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
				}
			}
		}

		void Adapter::disableConnection()
		{
			_connected = false;
			isLoggedOn = false;
			isIECConnectionRes = false;
			closeSocket();
			_omsSender.stopConnection();
		}

		void Adapter::sendLogonRequest()
		{
			std::string msg = getMessageHeaderForLogon("LOGON", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);

			msg = getLoginRequest(msg,password);

			if (sendToTarget(msg, msg.length()))
			{
				std::cout << "Logon request send." << std::endl;
			}
			else
			{
				std::cout << "Exchange not connected." << std::endl;
			}

		}

		void Adapter::sendNewOrder()
		{
			std::string msg = getMessageHeader("ORDER_ADD", seqNumber++, tMemCode, dealerCode, ctclCode);

			msg = getNewOrderReq(msg, seqNumber);
			
			//check algo id logic
			if (!algoID.empty())
				msg.append(FIN::PIPE).append(algoID);
			else
				msg.append(FIN::PIPE).append("0");

			sendToTarget(msg, msg.length());
		}

		void Adapter::sendModifyOrder()
		{
			std::string msg = getMessageHeader("ORDER_MOD", seqNumber++, tMemCode, dealerCode, ctclCode);

			std::string req = getModifyOrderReq(msg, seqNumber);

			sendToTarget(req, req.length());
		}

		void Adapter::sendCancelOrder()
		{
			std::string msg = getMessageHeader("ORDER_DEL", seqNumber++, tMemCode, dealerCode, ctclCode);

			std::string req = getCancelOrderReq(msg, seqNumber);

			sendToTarget(req, req.length());
		}

		void Adapter::sendDownloadPendingOrder()
		{
			std::string msg = getMessageHeaderOnly("DL_PENDING_ORDERS", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);
			sendToTarget(msg, msg.length());
		}
		
		void Adapter::sendDownloadReturnOrderMsg()
		{
			std::string msg = getMessageHeaderOnly("DL_RETURN_ORDERS", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);
			sendToTarget(msg, msg.length());
		}

		void Adapter::sendDownloadTradeMsg()
		{
			std::string msg = getMessageHeaderOnly("DL_TRADE_BOOK", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);
			sendToTarget(msg, msg.length());
		}

		void Adapter::sendDownloadStopLossMsg()
		{
			std::string msg = getMessageHeaderOnly("DL_PENDING_SL_ORDERS", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);
			sendToTarget(msg, msg.length());
		}

		void Adapter::sendDownloadReturnedStopLossMsg()
		{
			std::string msg = getMessageHeaderOnly("DL_RETURN_SL_ORDERS", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);
			sendToTarget(msg, msg.length());
		}
		
		void Adapter::sendChangePassword(std::string newPass)
		{
			std::string msg = getMessageHeaderForLogon("CHANGEPASS", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);

			msg.append(password).append(FIN::PIPE);
			msg.append(newPass);

			sendToTarget(msg, msg.length());
		}

		void Adapter::sendLogOffRequest()
		{
			std::string msg = getMessageHeaderForLogon("LOGOFF", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);

			msg.append(newPassword);

			sendToTarget(msg, msg.length());
		}

		void Adapter::sendContractDownload()
		{
			std::string msg = getMessageHeaderOnly("DL_CONTRACT", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);
			sendToTarget(msg, msg.length());
		}

		void Adapter::sendDownloadMarketPictureBBOMsg()
		{
			std::string msg = getMessageHeaderOnly("DL_MARKET_PICT", seqNumber++, cMemCode, tMemCode, dealerCode, ctclCode);
			sendToTarget(msg, msg.length());
		}

		IECCONNECTION Adapter::parseIECMsg(std::string resp)
		{
			IECCONNECTION msg;
			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			int pos = 0;
			while (getline(f, s, '|')) {
				vect.push_back(s);
			}
			try {
				strncpy(msg.messageType, vect[2].c_str(), sizeof(msg.messageType) - 1);
				strncpy(msg.responseDetails, vect[3].c_str(), sizeof(msg.responseDetails) - 1);
				return msg;
			}
			catch (std::exception& e) {
				std::cout << "Exception in Parse Body:" << __func__ << e.what() << std::endl;
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
			}
		}

		HeaderResponse Adapter::parseHeaderMsg(std::string resp)
		{
			HeaderResponse msg;
			//check order type then process all member
			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|')) 
			{
				vect.push_back(s);
			}
			try 
			{
				if (vect[2] == "MSG_FAILURE")
				{
					strncpy(msg.messageType, vect[2].c_str(), vect[2].length());
					//msg.responseCode = std::stoi(vect[3]);
					if(vect.size() == 4)
						std::memcpy(msg.responseDetails, vect[3].c_str(), vect[3].length());
					else
						std::memcpy(msg.responseDetails, vect[4].c_str(), vect[4].length());
					msg.checkMsgType = 128;
					return msg;
				}
				else if (vect[2] == "N_TRADE_EXECUTED")
				{
					strncpy(msg.messageType, vect[2].c_str(), vect[2].length());
					msg.checkMsgType = 117;
					return msg;
				}
				else if (vect[2] == "N_ORDRET_STPC")
				{
					strncpy(msg.messageType, vect[2].c_str(), vect[2].length());
					msg.checkMsgType = 121;
					return msg;
				}
				else if (vect[2] == "N_LIMIT_VIOLATION")
				{
					strncpy(msg.messageType, vect[2].c_str(), vect[2].length());
					msg.checkMsgType = 119;
					return msg;
				}
				else if (vect[2] == "N_STOPLOSS_CONVERTED")
				{
					strncpy(msg.messageType, vect[2].c_str(), vect[2].length());
					msg.checkMsgType = 118;
					return msg;
				}
				else if (vect[2] == "N_KILL_ORDER")
				{
					strncpy(msg.messageType, vect[2].c_str(), sizeof(msg.messageType) - 1);
					msg.checkMsgType = 120;
					return msg;
				}
				else if (vect[2] == "N_MEMBER_ACTION")
				{
					strncpy(msg.messageType, vect[2].c_str(), sizeof(msg.messageType) - 1);
					msg.checkMsgType = 129;
					return msg;
				}
				else if (vect[2] == "N_CLIENT_ACTION")
				{
					strncpy(msg.messageType, vect[2].c_str(), sizeof(msg.messageType) - 1);
					msg.checkMsgType = 130;
					return msg;
				}
				else if (vect[2] == "LOGON_R")
				{
					msg.checkMsgType = 100;
				}
				else if (vect[2] == "ORDER_ADD_R")
				{
					msg.checkMsgType = 103;
				}
				else if (vect[2] == "ORDER_MOD_R")
				{
					msg.checkMsgType = 104;
				}
				else if (vect[2] == "ORDER_DEL_R")
				{
					msg.checkMsgType = 105;
				}
				else if (vect[2] == "DL_PENDING_ORDERS_R")
				{
					msg.checkMsgType = 108;
				}
				else if (vect[2] == "DL_TRADE_BOOK_R")
				{
					msg.checkMsgType = 110;
				}
				else if (vect[2] == "CHANGEPASS_R")
				{
					msg.checkMsgType = 102;
				}
				else if (vect[2] == "LOGOFF_R")
				{
					msg.checkMsgType = 101;
				}
				else if (vect[2] == "DL_CONTRACT_R")
				{
					msg.checkMsgType = 107;
				}
				else if (vect[2] == "DL_PENDING_SL_ORDERS_R")
				{
					msg.checkMsgType = 109;
				}
				else if (vect[2] == "DL_MARKET_PICT_R")
				{
					msg.checkMsgType = 116;
				}
				else if (vect[2] == "N_LIMIT_VIOLATION")
				{
					msg.checkMsgType = 119;
				}
				else if (vect[2] == "DL_RETURN_ORDERS_R")
				{
					msg.checkMsgType = 111;
				}
				else if (vect[2] == "DL_RETURN_SL_ORDERS_R")
				{
					msg.checkMsgType = 112;
				}

				strncpy(msg.messageType, vect[2].c_str(), sizeof(msg.messageType) - 1);
				msg.messageEcho = std::stoi(vect[3]);
				msg.responseCode = std::stoi(vect[8]);
				std::memcpy(msg.responseDetails, vect[9].c_str(), vect[9].length());

				return msg;
			}
			catch (std::exception& e) {
				std::cout << "Header Parse issue : " << __func__ << e.what() << std::endl;
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
			}
		}

		void Adapter::handleMessage(std::string& msg, int len)	
		{
			//parse msg
			HeaderResponse hres;
			std::string decrypt_string;
			if (isIECConnectionRes)
			{
				decrypt((unsigned char*)msg.c_str() + 2, len, (unsigned char*)key.c_str(), (unsigned char*)IV.c_str(), decryptedtext, decryptedtext_len);
				
				decrypt_string.assign(reinterpret_cast<const char*>(decryptedtext), decryptedtext_len);
				//std::cout << "Recv :" << str << std::endl;
				LOG_INFO << "[FROM EXCHANGE] " << " : " << decrypt_string;
				hres = parseHeaderMsg(decrypt_string);
			}
			else
			{
				IECCONNECTION str = parseIECMsg(msg);
				if (std::strcmp(str.responseDetails, "IEC Connection Sucessful")==0)
				{
					FIN::setConsoleColor(2);
					std::cout << "Connected to IEC Service." << std::endl;
					FIN::setConsoleColor(7);
					LOG_INFO << "Connected to IEC Service.";
					isIECConnectionRes = true;

					sendLogonRequest();
					return;
				}
				else
				{
					std::cout << "IEC Connection error code : " << str.responseDetails << std::endl;
					LOG_INFO << "IEC Connection error code : " << str.responseDetails;
					
					//send logout msg
				}
			}
			switch (hres.checkMsgType)
			{
			case FIN::MSGTYPE::Logon_Message_R:						processLogonMsg(hres);	break;
			case FIN::MSGTYPE::Add_Order_Message_R:					processNewOrderMsg(hres, decrypt_string); break;
			case FIN::MSGTYPE::Modify_Order_Message_R:				processModifyOrderMsg(hres, decrypt_string); break;
			case FIN::MSGTYPE::Delete_Order_Message_R:				processDeleteOrderMsg(hres, decrypt_string); break;
			case FIN::MSGTYPE::Download_Pending_Order_Message_R:	processDownloadPendingOrderMsg(hres, decrypt_string); break;
			case FIN::MSGTYPE::Download_Stoploss_Order_Message_R:	processDownloadStopLossMsg(hres, decrypt_string);	break;
			case FIN::MSGTYPE::Download_Trade_Message_R:			processDownloadTradeMsg(hres, decrypt_string);	break;
			case FIN::MSGTYPE::Order_Return_Due_To_Self_Match_R:	processReturnSelfMatchTradeMsg(hres, decrypt_string); break;
			case FIN::MSGTYPE::Download_Returned_Orders_Message_R:	processDownloadReturnOrderMsg(hres, decrypt_string);	break;
			case FIN::MSGTYPE::Download_Returned_Stoploss_Order_R:	processDownloadReturnStopLossMsg(hres, decrypt_string);	break;
			case FIN::MSGTYPE::Download_Market_Picture_BBO_Msg_R:	processDownloadMarketPictureBBOMsg(hres, decrypt_string);	break;
			case FIN::MSGTYPE::Change_Password_Message_R:			processChangePasswordMsg(hres);	break;
			case FIN::MSGTYPE::LogOff_Message_R:					processLogoffMsg(hres);	break;
			case FIN::MSGTYPE::Download_Contract_Message_R:			processContractMsg(hres, decrypt_string);	break;
			case FIN::MSGTYPE::Technical_Failure_Message_R:			processTechnicalFailure(hres);	break;
			case FIN::MSGTYPE::Trade_Notification_Message_R:		processTradeNotificationMsg(hres, decrypt_string); break;
			case FIN::MSGTYPE::Stoploss_Converted_Notification_R:	processStopLossConvertedNotificationMsg(decrypt_string);	break;
			case FIN::MSGTYPE::Limit_Violation_Notification_Msg_R:	processLimitViolationNotificationMsg(decrypt_string);	break;
			case FIN::MSGTYPE::IOC_Order_Kill_Notification_Msg_R:	processOrderKillNotificationMsg(decrypt_string);	break;
			case FIN::MSGTYPE::Member_Action_Notification_Msg_R:	processMemberActionNotificationMsg(decrypt_string);	break;
			case FIN::MSGTYPE::Client_Action_Notification_Msg_R:	processClientActionNotificationMsg(decrypt_string);	break;

			default:
				std::cout << "Not process msgType : " << hres.messageType << std::endl;
				std::string val(reinterpret_cast<const char*>(decryptedtext, decryptedtext_len));
				LOG_INFO << "Not process msgType : " << val;
				break;
			}
		}	

		void Adapter::processLogonMsg(HeaderResponse hres)
		{
			if (hres.responseCode == 0)
			{
				logonSuccess();

				while (!_omsSender._process)
				{
					std::this_thread::sleep_for(std::chrono::seconds(2));
					std::cout << "Waiting for oms connection to process recovery" << std::endl;
				}

				sendDownloadMarketPictureBBOMsg();

				while (isMarketPictureDownload)
				{
					std::this_thread::sleep_for(std::chrono::seconds(1));
					std::cout << "Waiting for Market Picture BBO" << std::endl;
				}

				sendContractDownload();
				
				if (isPasswordChange)
				{
					newPassword = generateCustomPassword();
					sendChangePassword(newPassword);
				}
				else
				{
					std::cout << "Now Process recovery (Pending/StopLoss/Trade/ReturnOrder/ReturnStopLoss) order" << std::endl;
					LOG_INFO << "Now Process recovery (Pending/StopLoss/Trade/ReturnOrder/ReturnStopLoss) order";

					sendDownloadPendingOrder();
				}
			}
			else
			{
				FIN::setConsoleColor(4);
				std::cout << "Login failed " << hres.responseDetails << std::endl;
				FIN::setConsoleColor(7);
				LOG_INFO << "Login failed " << hres.responseDetails;
			}
		}

		void Adapter::processNewOrderMsg(HeaderResponse hres, std::string msg)
		{
			if (hres.responseCode == 0)
			{
				Order orderDetail;
				OmsResponseOrder* omsResp = exchResp.getOrderEntryConfirm(msg);

				try
				{
					auto itr = storeSeqNumber.find(hres.messageEcho);
					if (itr != storeSeqNumber.end())
					{
						omsResp->ClOrdID = itr->second.first;			//	ClOrdId
						omsResp->price = itr->second.second * priceDevider;	//	Price
						orderDetail.price = itr->second.second;
					}
					else
					{
						std::cout << "Key " << hres.messageEcho << " not found." << std::endl;
					}
				}
				catch (const std::exception& e)
				{
					std::cout << "Exception : " << __func__ << e.what() << std::endl;
					LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
					return;
				}

				//std::memcpy(omsResp->errorMessage, hres.responseDetails, sizeof(hres.responseDetails));

				orderDetail.originalqty = omsResp->quantity;
				orderDetail.pendingqty = omsResp->quantity;
				newEntry(omsResp->orderNumber, orderDetail);

				storeSeqNumber.erase(hres.messageEcho);													//remove for memory

				sendToOms((char*)omsResp);
				//std::cout << omsResp->toString() << std::endl;
				LOG_INFO << "[TO OMS] " << omsResp->toString();
			}
			else
			{
				OmsResponseOrder* omsResp = exchResp.getorderEntryReject(msg);

				try
				{
					auto itr = storeSeqNumber.find(hres.messageEcho);
					if (itr != storeSeqNumber.end())
					{
						omsResp->ClOrdID = itr->second.first;			//	ClOrdId
						omsResp->price = itr->second.second * priceDevider;	//	Price
					}
					else
					{
						std::cout << "Key " << hres.messageEcho << " not found." << std::endl;
					}
				}
				catch (const std::exception& e)
				{
					std::cout << "Exception : " << __func__ << e.what() << std::endl;
					LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
				}

				std::memcpy(omsResp->errorMessage, hres.responseDetails, sizeof(hres.responseDetails));
				storeSeqNumber.erase(hres.messageEcho);													//remove for memory

				sendToOms((char*)omsResp);
				LOG_INFO << "[TO OMS] " << omsResp->toString();

				setConsoleColor(4);
				std::cout << "Rejected -> Response Code : " << hres.responseCode << " : Response Details : " << omsResp->toResponceDetails() << std::endl;
				setConsoleColor(7);
			}
		}

		void Adapter::processModifyOrderMsg(HeaderResponse hres, std::string msg)
		{
			if (hres.responseCode == 0)
			{
				OmsResponseOrder* omsResp = exchResp.getorderModConfirm(msg);

				try
				{
					if (isAvailableOrderId(omsResp->orderNumber))
					{
						changeParticularValue(omsResp->orderNumber, omsResp->quantity, "originalqty");
						changeParticularValue(omsResp->orderNumber, omsResp->volumeRemaining, "pendingqty");

						Order& orderDetail = getOrder(omsResp->orderNumber);
						omsResp->volumeFilledToday = orderDetail.tradeqty;
					}
				}
				catch (const std::exception& e)
				{
					std::cout << "Exception : " << e.what() << std::endl;
					LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
					return;
				}
				//std::memcpy(omsResp->errorMessage, hres.responseDetails, sizeof(hres.responseDetails));

				storeSeqNumber.erase(hres.messageEcho);

				sendToOms((char*)omsResp);
				LOG_INFO << "[TO OMS] " << omsResp->toString();
			}
			else
			{
				OmsResponseOrder* omsResp = exchResp.getorderModReject(msg);

				try
				{
					auto itr = storeSeqNumber.find(hres.messageEcho);
					if (itr != storeSeqNumber.end())
					{
						omsResp->ClOrdID = itr->second.first;			//	ClOrdId
						omsResp->price = itr->second.second * priceDevider;	//	Price
					}
					else
					{
						std::cout << "Key " << hres.messageEcho << " not found." << std::endl;
					}
				}
				catch (const std::exception& e)
				{
					std::cout << "Exception : " << __func__ << e.what() << std::endl;
					LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
				}

				storeSeqNumber.erase(hres.messageEcho);

				std::memcpy(omsResp->errorMessage, hres.responseDetails, sizeof(hres.responseDetails));
				sendToOms((char*)omsResp);
				LOG_INFO << "[TO OMS] " << omsResp->toString();

				setConsoleColor(4);
				std::cout << "Rejected -> Response Code : " << hres.responseCode << " : Response Details : " << omsResp->toResponceDetails() << std::endl;
				setConsoleColor(7);
			}
		}

		void Adapter::processDeleteOrderMsg(HeaderResponse hres, std::string msg)
		{
			if (hres.responseCode == 0)
			{
				OmsResponseOrder* omsResp = exchResp.getorderCancelConfirm(msg);

				deleteEntry(omsResp->orderNumber);
				storeSeqNumber.erase(hres.messageEcho);

				//std::memcpy(omsResp->errorMessage, hres.responseDetails, sizeof(hres.responseDetails));
				sendToOms((char*)omsResp);
				LOG_INFO << "[TO OMS] " << omsResp->toString();
			}
			else
			{
				OmsResponseOrder* omsResp = exchResp.getorderCancelConfirm(msg);

				try
				{
					auto itr = storeSeqNumber.find(hres.messageEcho);
					if (itr != storeSeqNumber.end())
					{
						omsResp->ClOrdID = itr->second.first;			//	ClOrdId
						omsResp->price = itr->second.second * priceDevider;	//	Price
					}
					else
					{
						std::cout << "Key " << hres.messageEcho << " not found." << std::endl;
					}
				}
				catch (const std::exception& e)
				{
					std::cout << "Exception : " << __func__ << e.what() << std::endl;
					LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
				}
				storeSeqNumber.erase(hres.messageEcho);
				std::memcpy(omsResp->errorMessage, hres.responseDetails, sizeof(hres.responseDetails));

				sendToOms((char*)omsResp);
				LOG_INFO << "[TO OMS] " << omsResp->toString();

				setConsoleColor(4);
				std::cout << "Rejected -> Response Code : " << hres.responseCode << " : Response Details : " << omsResp->toResponceDetails() << std::endl;
				setConsoleColor(7);
			}
		}

		void Adapter::processTradeNotificationMsg(HeaderResponse hres, std::string msg)
		{
			OmsResponseOrder* omsResp = exchResp.getTradeConfirm(msg);

			try
			{
				if (isAvailableOrderId(omsResp->orderNumber))
				{
					Order& orderDetail = getOrder(omsResp->orderNumber);
					omsResp->quantity = orderDetail.originalqty;
					int totalTradeQty = orderDetail.tradeqty;
					totalTradeQty += omsResp->lastFilledQty;			//total trade qty
					omsResp->volumeFilledToday = totalTradeQty;

					int pendingQty = orderDetail.pendingqty;
					pendingQty -= omsResp->lastFilledQty;

					if (pendingQty <= 0)
					{
						omsResp->orderStatus = 2;
						omsResp->volumeRemaining = 0;
						deleteEntry(omsResp->orderNumber);
					}
					else
					{
						omsResp->orderStatus = 1;
						omsResp->volumeRemaining = pendingQty;
						changeParticularValue(omsResp->orderNumber, totalTradeQty, "tradeqty");
						changeParticularValue(omsResp->orderNumber, pendingQty, "pendingqty");
					}
				}
			}
			catch (const std::exception& e)
			{
				std::cout << "Exception : " << __func__ << e.what() << std::endl;
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
			}

			std::memcpy(omsResp->errorMessage, hres.responseDetails, sizeof(hres.responseDetails));
			sendToOms((char*)omsResp);
			LOG_INFO << "[TO OMS] " << omsResp->toString();
		}

		void Adapter::processLimitViolationNotificationMsg(std::string msg)
		{
			OmsResponseOrder omsResp; 
			if (exchResp.getLimitViolationNotification(msg, omsResp, cMemCode))
			{
				sendToOms((char*)&omsResp);
				LOG_INFO << "[TO OMS] " << omsResp.toString();
			}
		}

		void Adapter::processOrderKillNotificationMsg(std::string msg)
		{
			OmsResponseOrder* omsResp = exchResp.getOrderkill(msg);
			if (isAvailableOrderId(omsResp->orderNumber))
			{
				Order& orderDetail = getOrder(omsResp->orderNumber);
				if (orderDetail.pendingqty == omsResp->quantity)
				{
					deleteEntry(omsResp->orderNumber);
				}
			}
			std::string str = "Order Kill";
			std::memcpy(omsResp->errorMessage, str.c_str(), sizeof(str));
			sendToOms((char*)omsResp);
			LOG_INFO << "[TO OMS] " << omsResp->toString();
		}

		void Adapter::processMemberActionNotificationMsg(std::string msg)
		{
			OmsResponseOrder* omsResp = exchResp.getMemberNotification(msg);
			std::strcpy(omsResp->tradeNumber, cMemCode.c_str());
			sendToOms((char*)omsResp);
			LOG_INFO << "[TO OMS] " << omsResp->toString();
		}

		void Adapter::processClientActionNotificationMsg(std::string msg)
		{
			OmsResponseOrder* omsResp = exchResp.getClientNotification(msg);

			sendToOms((char*)omsResp);
			LOG_INFO << "[TO OMS] " << omsResp->toString();
		}

		void Adapter::processReturnSelfMatchTradeMsg(HeaderResponse hres, std::string msg)
		{
			OmsResponseOrder* omsResp = exchResp.getSelfTrade(msg);

			if (isAvailableOrderId(omsResp->orderNumber))
			{
				Order& orderDetail = getOrder(omsResp->orderNumber);
				if (orderDetail.pendingqty == omsResp->quantity)
				{
					deleteEntry(omsResp->orderNumber);
				}
			}
			std::string str = "Self Trade Cancellation";
			std::memcpy(omsResp->errorMessage, str.c_str(), sizeof(str));
			sendToOms((char*)omsResp);
			LOG_INFO << "[TO OMS] " << omsResp->toString();
		}
		
		void Adapter::processStopLossConvertedNotificationMsg(std::string msg)
		{
			OmsResponseOrder* omsResp = exchResp.getStopLossDetail(msg);

			if (isAvailableOrderId(omsResp->orderNumber))
			{
				if (omsResp->price > 0)
				{
					changeParticularValue(omsResp->orderNumber, omsResp->price, "price");
				}
			}
		}

		void Adapter::processDownloadPendingOrderMsg(HeaderResponse hres, std::string msg)
		{
			Order ord;
			std::istringstream stream(msg);
			std::string line;
			std::vector<std::string> groups;

			bool isFirst = true;
			while (std::getline(stream, line)) 
			{
				if (isFirst)
				{
					isFirst = false;
					continue;
				}
				OmsResponseOrder omsResp;
				groups.push_back(line);
				if (!processPendingOrderHistory(line, omsResp))
				{
					//manage memory
					ord.ordertype = omsResp.orderType;
					ord.price = omsResp.price / priceDevider;
					ord.originalqty = omsResp.quantity;
					ord.pendingqty = omsResp.volumeRemaining;
					ord.tradeqty = (ord.originalqty - ord.pendingqty);
					addPendingOrderDetail(omsResp.orderNumber, ord);

					if (omsResp.quantity == omsResp.volumeRemaining)
					{
						sendToOms((char*)&omsResp);
						LOG_INFO << "[TO OMS] " << omsResp.toString();
					}
				}
				else
				{
					sendDownloadStopLossMsg();
				}
			}
		}

		void Adapter::processDownloadReturnOrderMsg(HeaderResponse hres, std::string msg)
		{
			std::istringstream stream(msg);
			std::string line;
			std::vector<std::string> groups;

			bool isFirst = true;
			while (std::getline(stream, line))
			{
				if (isFirst)
				{
					isFirst = false;
					continue;
				}
				OmsResponseOrder omsResp;
				groups.push_back(line);
				if (!processReturnOrderHistory(line, omsResp))
				{
					if (omsResp.volumeRemaining <= 0)
					{
						if (isAvailableOrderId(omsResp.orderNumber))
						{
							deleteEntry(omsResp.orderNumber);
						}
						sendToOms((char*)&omsResp);
						LOG_INFO << "[TO OMS] " << omsResp.toString();
					}
				}
				else
				{
					sendDownloadReturnedStopLossMsg();
				}
			}
		}

		void Adapter::processDownloadReturnStopLossMsg(HeaderResponse hres, std::string msg)
		{
			std::istringstream stream(msg);
			std::string line;
			std::vector<std::string> groups;

			bool isFirst = true;
			while (std::getline(stream, line))
			{
				if (isFirst)
				{
					isFirst = false;
					continue;
				}
				OmsResponseOrder omsResp;
				groups.push_back(line);
				if (!processReturnStopLossOrder(line, omsResp))
				{
					if (omsResp.volumeRemaining <= 0)
					{
						if (isAvailableOrderId(omsResp.orderNumber))
						{
							deleteEntry(omsResp.orderNumber);
						}
						sendToOms((char*)&omsResp);
						LOG_INFO << "[TO OMS] " << omsResp.toString();
					}
				}
				else
				{
					FIN::setConsoleColor(2);
					std::cout << "Recovery Completed. Now start trading..." << std::endl;
					FIN::setConsoleColor(7);
					LOG_INFO << "Recovery Completed. Now start trading...";
				}
			}
		}

		void Adapter::processDownloadTradeMsg(HeaderResponse hres, std::string msg)
		{
			std::istringstream stream(msg);
			std::string line;
			std::vector<std::string> groups;

			bool isFirst = true;
			while (std::getline(stream, line))
			{
				if (isFirst)
				{
					isFirst = false;
					continue;
				}
				OmsResponseOrder omsResp;
				groups.push_back(line);
				if (!processTradeOrderHistory(line, omsResp))
				{
					if (isAvailableOrderId(omsResp.orderNumber))
					{
						Order& ordDetail = getOrder(omsResp.orderNumber);
						ordDetail.tradeqty = omsResp.volumeFilledToday;
						
						if (ordDetail.originalqty > 0)
							omsResp.quantity = ordDetail.originalqty;
						else
							omsResp.quantity = omsResp.volumeFilledToday;
					}
					else
					{
						Order orderDetail;
						orderDetail.tradeqty = omsResp.volumeFilledToday;
						newEntry(omsResp.orderNumber, orderDetail);

						omsResp.quantity = omsResp.volumeFilledToday;
					}
					sendToOms((char*)&omsResp);
					LOG_INFO << "[TO OMS] " << omsResp.toString();
				}
				else
				{
					sendDownloadReturnOrderMsg();
				}
			}
		}

		void Adapter::processDownloadStopLossMsg(HeaderResponse hres, std::string msg)
		{
			std::istringstream stream(msg);
			std::string line;
			std::vector<std::string> groups;

			bool isFirst = true;
			while (std::getline(stream, line))
			{
				if (isFirst)
				{
					isFirst = false;
					continue;
				}
				OmsResponseOrder omsResp;
				groups.push_back(line);
				if (!processStopLossOrderHistory(line, omsResp))
				{
					//manage memory
					Order ord;
					ord.ordertype = omsResp.orderType;
					ord.price = omsResp.price / priceDevider;
					ord.originalqty = omsResp.quantity;
					ord.pendingqty = omsResp.volumeRemaining;
					ord.tradeqty = (ord.originalqty - ord.pendingqty);
					addPendingOrderDetail(omsResp.orderNumber, ord);

					sendToOms((char*)&omsResp);
					LOG_INFO << "[TO OMS] " << omsResp.toString();
				}
				else
				{
					sendDownloadTradeMsg();
				}
			}
		}

		void Adapter::processChangePasswordMsg(HeaderResponse hres)
		{
			if (hres.responseCode == 0)
			{
				std::ifstream inputFile(configFileLocation);  // Open the file for reading
				if (!inputFile.is_open()) {
					std::cerr << "Error opening file for reading!" << std::endl;
					return;
				}

				std::ofstream tempFile("temp.txt");  // Temporary file for writing
				if (!tempFile.is_open()) {
					std::cerr << "Error opening temp file for writing!" << std::endl;
					return;
				}

				// Read the entire file content into a string
				std::string line;
				while (std::getline(inputFile, line))
				{
					if (line.substr(0, line.find('=')) == "Password")
					{
						line = "Password=" + newPassword;
					}
					if (line.substr(0, line.find('=')) == "PasswordChangeRequired")
					{
						line = "PasswordChangeRequired=0";
					}
					tempFile << line << "\n";
				}

				inputFile.close();
				tempFile.close();

				std::remove(configFileLocation.c_str());				// Remove the original file
				std::rename("temp.txt", configFileLocation.c_str());	// Rename temp file to original file name

				FIN::setConsoleColor(2);
				std::cout << "Password updated successfully." << std::endl;
				FIN::setConsoleColor(7);

				LOG_INFO << "Password updated successfully.";
				sendLogOffRequest();
			}
			else
			{
				FIN::setConsoleColor(4);
				std::cout << "Password not change Try again" << std::endl;
				FIN::setConsoleColor(7);
			}
		}

		void Adapter::processDownloadMarketPictureBBOMsg(HeaderResponse hres, std::string msg)
		{
			LOG_INFO << msg;
			std::istringstream stream(msg);
			std::string line;
			std::vector<std::string> groups;

			bool isFirst = true;
			while (std::getline(stream, line))
			{
				if (isFirst)
				{
					isFirst = false;
					continue;
				}
				OmsResponseOrder omsResp;
				groups.push_back(line);
				if (!processMarketPictureBBOHistory(line, omsResp, contractClosePrice))
				{
					//manage memory
					/*Order ord;
					ord.ordertype = omsResp.orderType;
					ord.price = omsResp.price / priceDevider;
					ord.originalqty = omsResp.quantity;
					ord.pendingqty = omsResp.volumeRemaining;
					ord.tradeqty = (ord.originalqty - ord.pendingqty);
					addPendingOrderDetail(omsResp.orderNumber, ord);

					sendToOms((char*)&omsResp);
					LOG_INFO << "[TO OMS] " << omsResp.toString();*/
				}
				else
				{
					isMarketPictureDownload = true;
				}
			}
		}

		void Adapter::logonSuccess()
		{
			isLoggedOn = true;
			isFirst = true;
			_omsSender.startConnection();
			FIN::setConsoleColor(2);
			std::cout << "Exchange Connected." << std::endl;
			FIN::setConsoleColor(7);
			LOG_INFO << "Exchange Connected.";
		}
		
		void Adapter::processLogoffMsg(HeaderResponse hres)
		{
			if (hres.responseCode == 0)
			{
				isLoggedOn = false;
				FIN::setConsoleColor(2);
				std::cout << "Logout successfully" << std::endl;
				FIN::setConsoleColor(7);
				LOG_INFO << "Logout successfully";
			}
			else
			{
				FIN::setConsoleColor(4);
				std::cout << "Logout reject" << std::endl;
				FIN::setConsoleColor(7);
				LOG_INFO << "Logout reject";
			}
		}

		std::string Adapter::getCurrentDay() {
			std::time_t now = std::time(nullptr);
			std::tm* localTime = std::localtime(&now);

			const std::string days[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

			return days[localTime->tm_wday];
		}
		
		std::string Adapter::generateCustomPassword() {
			const char specialFirst = '&';
			const char specialCharLast = '$';

			// Random number generator setup
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_int_distribution<> numDist(10000, 99999);  // five-digit number

			// Generate a random number
			int randomNumber = numDist(gen);

			// Form the final string
			return getCurrentDay() + specialFirst + std::to_string(randomNumber) + specialCharLast;
		}

		void Adapter::processTechnicalFailure(HeaderResponse hres)
		{
			FIN::setConsoleColor(4);
			std::cout << "Technical Failure Please check " << hres.responseDetails << std::endl;
			FIN::setConsoleColor(7);
			LOG_INFO << "Technical Failure Please check " << hres.responseDetails;
		}

		void Adapter::processContractMsg(HeaderResponse hres, std::string msg)
		{
			std::istringstream stream(msg);
			std::string line;

			bool isFirst = true;
			while (std::getline(stream, line))
			{
				if (isFirst)
				{
					isFirst = false;
					continue;
				}
				insertValueInFile(line);
			}
		}

		void Adapter::insertValueInFile(std::string value)
		{
			//when contract download enable
			if (isContractDownload)
			{
				std::string check = value.substr(0, 1);
				if (check == "Y")
				{
					contract_send->send("DONE");
					FIN::setConsoleColor(2);
					std::cout << "Contract File download completed." << std::endl;
					FIN::setConsoleColor(7);
					LOG_INFO << "Contract File download completed. check in location : ";
					return;
				}
				LOG_INFO << value;
				const std::string prefix = "N|";
				if (value.rfind(prefix, 0) == 0)
				{
					value.erase(0, prefix.length());
				}

				value.append("|IIBX");

				contract_send->send(value);
			}

			//get dpr percentage

			OmsResponseOrder omsRes;
			std::vector<std::string> vect;
			if (!getDPRValue(vect, value))
			{
				omsRes.reset();
				omsRes.orderStatus = 100;									//status
				std::strcpy(omsRes.orderNumber, vect[2].c_str());			//symbol

				int closePrice = 0;

				auto itr = contractClosePrice.find(vect[2]);
				if (itr != contractClosePrice.end())
				{
					closePrice = itr->second;
					omsRes.price = closePrice;								//close price
				}
				omsRes.quantity = closePrice + (closePrice * std::stoi(vect[10]) * 0.01);			//dpr high
				omsRes.volumeRemaining = closePrice - (closePrice * std::stoi(vect[10]) * 0.01);	//dpr low
				std::strcpy(omsRes.tradeNumber, DPR_MSG);					//some msg

				sendToOms((char*)&omsRes);
				LOG_INFO << "[TO OMS] " << omsRes.toString();
			}
		}

		bool Adapter::getDPRValue(std::vector<std::string>& vect, std::string msg)
		{
			std::istringstream f(msg.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			if (vect[0] == "Y")
				return true;
			
			return false;
		}

		void Adapter::receiveFromOMS(const std::string& omsInput)
		{
			try
			{
				if (omsInput == "H")
					return;
				LOG_INFO << "[FROM OMS] " << omsInput;
				_fixStringParser.loadString(omsInput);
				if (isLoggedOn)
				{

					slidingThrottel->checkThrottleLimit();
					switch (_fixStringParser.getChar(Fix_MsgType))
					{
					case Fix_MsgType_NEW_ORDER: sendNewOrder();     break;
					case Fix_MsgType_CANCEL_ORDER: sendCancelOrder();  break;
					case Fix_MsgType_MODIFY_ORDER: sendModifyOrder(); break;
					default: std::cout << "Inalid Message Type";
					}

				}
				else
				{					
				}
				_fixStringParser.reset();
			}
			catch (std::exception& e)
			{
				std::cout << "Exception :" << e.what() << std::endl;
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
			}
		}
		void Adapter::startOMSChannel()
		{
			bool first = true;
			while (true) {
				if (isLoggedOn)
				{
					if (first)
					{
						doOMSConnect();
						first = false;
					}
					try
					{
						if (!rabbit_udp)
							rabbitMqConsumer();
						else
							StartReceiving();
					}
					catch (std::exception& e)
					{
						restartOmsConnections();
					}
				}
				else
				{
					Sleep(10);
				}
			}
		}
		void Adapter::doOMSConnect()
		{

			std::cout << "Initiating OMS Connection" << std::endl;

			if (!rabbit_udp)
			{
				std::string configPath = _path.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
				RabbitMqConsumer::connect(configPath); //consumer	connection
				_omsRabbitSender.connect(configPath); //producer connection		
			}
			else
			{
				init_rcv(rcv_port);
				oms_udp_sender.init(udp_sender_ip, sender_port);

			}


			_omsSender.init(confirmationPort);
			_omsSender.accept();
			_omsSender.startOmsHealthThread();

		}
		void Adapter::restartOmsConnections()
		{

			std::cout << "Restarting OMS Connection";
			_omsSender.init(confirmationPort);

			if (!rabbit_udp)
			{
				std::string configPath = _path.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE;
				RabbitMqConsumer::connect(configPath); //consumer	connection
				_omsRabbitSender.connect(configPath); //producer connection		
			}
			else
			{
				disposeSocket();
				init_rcv(rcv_port);
				oms_udp_sender.init(udp_sender_ip, sender_port);

			}

			_omsSender.accept();
			_omsSender.startOmsHealthThread();
		}
		void Adapter::sendToOms(char* resp)
		{
			if (!rabbit_udp)
			{
				_omsRabbitSender.sendToOMS(resp, sizeof(OmsResponseOrder));
			}
			else
			{
				oms_udp_sender.sendToOMS(resp, sizeof(OmsResponseOrder));
			}
			//LOG_INFO << "[TO OMS1] " << resp;
		}
	}
}