#include "adapter.h"
#include "constants.h"

namespace FIN
{
	namespace OrdGtwy
	{
		namespace NNF
		{
			namespace COM
			{

				Adapter::Adapter(ConfigParameter* configDetail)	:ExchangeProcess(configDetail),	_omsSender()
				{
					configDetails = configDetail;
					_omsSenderUDP.isConnected(false);
					OmsUDPRcv::isConnected(false);
					init(configDetail);
					_loggedIn = false; // Use Connection Status

					_orderHistory.reserve(3000);
					_heartBeatMsg = new FIN::NSE::TRIMMED::HEARTBEAT_MESSAGE(configDetails->trader_id);
				}

				bool Adapter::init(ConfigParameter* configDetails)
				{
					_st = new SlidingWindowThrottling(configDetails->throttle_limit, configDetails->throttle_time_windows);

					udp_rabbit = configDetails->udp_rabbit;

					_defPAN = "PAN_EXEMPT";

					_custodianId = configDetails->brokerId;
					std::string algoIdKey = "SESSION.ALGO_ID";
					std::string algoCatKey = "SESSION.ALGO_CAT";

					configPath = _currPath.getCurrentPath() + FIN::CONSTANT::NSECOM_ORDER_CONFIG;

					_nnfNewOrder = new NSE::TRIMMED::COM::ORDER_NEW_REQUEST_TR(configDetails->trader_id);
					_nnfNewOrder->setBrokerId(configDetails->brokerId);
					_nnfNewOrder->setBranchId(configDetails->branchId);
					_nnfNewOrder->setAlgoID(std::stol(configDetails->algo_id));
					
					_nnfCancelOrder = new NSE::TRIMMED::COM::ORDER_CANCEL_TR(configDetails->trader_id);
					_nnfCancelOrder->setBrokerId(configDetails->brokerId);
					_nnfCancelOrder->setBranchId(configDetails->branchId);
					_nnfCancelOrder->setAlgoID(std::stol(configDetails->algo_id));
					
					_nnfReplaceOrder = new NSE::TRIMMED::COM::ORDER_MODIFY_TR(configDetails->trader_id);
					_nnfReplaceOrder->setBrokerId(configDetails->brokerId);
					_nnfReplaceOrder->setBranchId(configDetails->branchId);
					_nnfReplaceOrder->setAlgoID(std::stol(configDetails->algo_id));

					_nnfstopLossOrder = new NSE::TRIMMED::COM::NEW_ORDER(configDetails->trader_id);
					_nnfstopLossOrder->setBrokerId(configDetails->brokerId);
					_nnfstopLossOrder->setBranchId(configDetails->branchId);
					_nnfstopLossOrder->setAlgoID(std::stol(configDetails->algo_id));
					_nnfstopLossOrder->setTraderId(configDetails->trader_id);
					_nnfstopLossOrder->setDisclosedVolume(0);

					_nnfstopLossCancel = new NSE::TRIMMED::COM::CANCEL_ORDER(configDetails->trader_id);
					_nnfstopLossCancel->setBrokerId(configDetails->brokerId);
					_nnfstopLossCancel->setBranchId(configDetails->branchId);
					_nnfstopLossCancel->setAlgoID(std::stol(configDetails->algo_id));
					_nnfstopLossCancel->setTraderId(configDetails->trader_id);

					_nnfstopLossModify = new NSE::TRIMMED::COM::MODIFY_ORDER(configDetails->trader_id);
					_nnfstopLossModify->setBrokerId(configDetails->brokerId);
					_nnfstopLossModify->setBranchId(configDetails->branchId);
					_nnfstopLossModify->setAlgoID(std::stol(configDetails->algo_id));
					_nnfstopLossModify->setTraderId(configDetails->trader_id);

					readAccountConfigMapping();
					return false;
				}

				void Adapter::stop() 
				{
				}

				void Adapter::start()
				{
					LOG_INFO << " ----------------------------------------------------------------------------";
					LOG_INFO << "  Starting NSE COM NNF TRIMMED DC ORDER GATEWAY ";
					LOG_INFO << " ----------------------------------------------------------------------------";
					std::cout << "----------------------------------------------------------------------------\n";
					std::cout << " Starting NSE COM NNF TRIMMED DC ORDER GATEWAY \n";
					std::cout << "----------------------------------------------------------------------------\n";
					Connection::start();
				}

				// void Adapter::handleOmsWrite( const std::string &omsInput )
				void Adapter::receiveFromOMS(const std::string& omsInput)
				{
					// LOG_INFO << "[OMS_RCV] " << omsInput ;

					if (omsInput == heartbeat)
					{
						sendToTarget_Encrypt(*_heartBeatMsg);
						return;
					}

					try
					{
						_omsStringParser.loadString(omsInput);

						//checking exchange up if not then reject and return
						if (!getExchangeUpStatus())
						{
							rejectMessage(std::atoi(_omsStringParser.get(Fix_ClOrdID).c_str()));
							return;
						}

						_st->checkThrottleLimit();

						//collect clorderid for tracking
						setClOrderId(std::atol(_omsStringParser.get(Fix_ClOrdID).c_str()));

						auto orderType = _omsStringParser.getChar(Fix_OrdType);
						switch (_omsStringParser.getChar(Fix_MsgType))
						{
						case Fix_MsgType_NEW_ORDER:
							if (orderType == FIX_OrderType_stop_loss)
								sendNew_StopLossOrder();
							else
								sendNewOrder();
							break;
						case Fix_MsgType_CANCEL_ORDER:
							if (orderType == FIX_OrderType_stop_loss)
								SendCancel_StopLossOrder();
							else
								sendCancelOrder();
							break;
						case Fix_MsgType_MODIFY_ORDER:
							if (orderType == FIX_OrderType_stop_loss)
								SendReplace_StopLossOrder();
							else
								sendReplaceOrder();
							break;
						default:
							DEVLOG("Inalid Message Type");
						}

						_omsStringParser.reset();
					}
					catch (std::exception& e)
					{
						DEVLOG("Exception : " << e.what());
					}
				}

				void Adapter::setClOrderId(uint32_t id)
				{
					_orderHistory.insert(id);
				}

				void Adapter::removeIdFromOrderHistory(uint32_t removeValue)
				{
					_orderHistory.erase(removeValue);
				}

				std::unordered_set<int> Adapter::getClOrdIdInstance()
				{
					return _orderHistory;
				}

				bool Adapter::orderHistoryIsEmpty()
				{
					return _orderHistory.empty();
				}

				template <typename T>
				void Adapter::fillOrderCommon(T* ord)
				{
					ord->setClOrdId(std::stol(_omsStringParser.get(Fix_ClOrdID)));

					ord->setTokenNo(std::stol(_omsStringParser.get(Fix_SecurityID)));

					ord->getContractDesc().setExpiryDate(std::stoi(_omsStringParser.get(Fix_MaturityMonthYear)));

					ord->getContractDesc().setSymbol(_omsStringParser.get(Fix_Symbol));

					std::string securityType = _omsStringParser.get(Fix_SecurityType);
					if (securityType.length() > 3) {
						securityType = securityType.substr(0, 3); // Removes the first 3 characters
					}
					if (securityType == Fix_SecurityType_FUT)
					{
						ord->getContractDesc().setStrikePrice(-1);
						ord->getContractDesc().setOptionType(NNF_OptionType_XX);
					}
					else
					{
						ord->getContractDesc().setStrikePrice(std::stoi(_omsStringParser.get(Fix_StrikePrice)) * NSE_COM_STRICK_PRICE_MULTIPLIER);
						
						switch (_omsStringParser.getChar(Fix_OptionType))
						{
						case Fix_OptionType_PutEuropean:
							ord->getContractDesc().setOptionType(NNF_OptionType_PE);
							break;
						case Fix_OptionType_CallEuropean:
							ord->getContractDesc().setOptionType(NNF_OptionType_CE);
							break;
						case Fix_OptionType_CallAmerican:
							ord->getContractDesc().setOptionType(NNF_OptionType_CA);
							break;
						case Fix_OptionType_PutAmerican:
							ord->getContractDesc().setOptionType(NNF_OptionType_PA);
							break;
						}
						
					}
					ord->getContractDesc().setInstrumentName(_omsStringParser.get(Fix_SecurityType));
					ord->getAdditionalOrderFlags().setCOL(1);
					ord->getAdditionalOrderFlags().setSTPC(1);
					switch (_omsStringParser.getChar(Fix_TimeInForce))
					{
					case Fix_TimeInForce_DAY:
						ord->getStOrderFlags().setDay(1);
						break;
					case Fix_TimeInForce_IOC:
						ord->getStOrderFlags().setIOC(1);
						break;
					}
					if (_omsStringParser.get(Fix_Account) == Fix_Account_OWN ||	_omsStringParser.get(Fix_Account) == Fix_Account_PRO)
					{
						ord->setProClientIndicator(ProClientIndicator::PRO);
						ord->setPAN(getPanNumber(_omsStringParser.get(Fix_Account)));
					}
					else
					{
						ord->setProClientIndicator(ProClientIndicator::CLIENT);
						ord->setAccountNumber(_omsStringParser.get(Fix_Account));
						ord->setSettlor(getCustodianId(_omsStringParser.get(Fix_Account)));
						ord->setPAN(getPanNumber(_omsStringParser.get(Fix_Account)));
						// std::cout << ord->getAccountNumber() << " : " << ord->getPAN() << " : " << ord->getSettlor() << std::endl ;
					}
					switch (_omsStringParser.getChar(Fix_Side))
					{
					case Fix_Side_Buy:
						ord->setBuySellIndicator(SIDE::SIDE_BUY);
						break;
					case Fix_Side_Sell:
						ord->setBuySellIndicator(SIDE::SIDE_SELL);
						break;
					}
					auto termInfo = _omsStringParser.get(Fix_TerminalInfo);
					if (termInfo == "0")
						ord->setNnfField(std::stod(configDetails->terminalInfo));
					else
						ord->setNnfField(std::stod(termInfo));
				}

				template <typename T>
				void Adapter::fillCommonOrder_Stoploss_Order(T* ord)
				{

					ord->setTokenNo(std::stol(_omsStringParser.get(Fix_SecurityID)));
					ord->getContractDesc().setExpiryDate(std::atoi(_omsStringParser.get(Fix_MaturityMonthYear).c_str()));
					ord->getContractDesc().setSymbol(_omsStringParser.get(Fix_Symbol));
					ord->setBookType(3);

					ord->setClOrdId(std::stoi(_omsStringParser.get(Fix_ClOrdID)));

					std::string securityType = _omsStringParser.get(Fix_SecurityType);
					if (securityType.length() > 3) {
						securityType = securityType.substr(0, 3); // Removes the first 3 characters
					}
					if (securityType == Fix_SecurityType_FUT)
					{
						ord->getContractDesc().setStrikePrice(-1);
						ord->getContractDesc().setOptionType(NNF_OptionType_XX);

					}
					else
					{
						ord->getContractDesc().setStrikePrice(std::stoi(_omsStringParser.get(Fix_StrikePrice)) * NSE_COM_STRICK_PRICE_MULTIPLIER);

						switch (_omsStringParser.getChar(Fix_OptionType))
						{
						case Fix_OptionType_PutEuropean:
							ord->getContractDesc().setOptionType(NNF_OptionType_PE);
							break;
						case Fix_OptionType_CallEuropean:
							ord->getContractDesc().setOptionType(NNF_OptionType_CE);
							break;
						case Fix_OptionType_CallAmerican:
							ord->getContractDesc().setOptionType(NNF_OptionType_CA);
							break;
						case Fix_OptionType_PutAmerican:
							ord->getContractDesc().setOptionType(NNF_OptionType_PA);
							break;
						}


					}
					ord->getContractDesc().setInstrumentName(_omsStringParser.get(Fix_SecurityType));
					ord->getAdditionalOrderFlags().setCOL(1);
					ord->getAdditionalOrderFlags().setSTPC(1);
					ord->getStOrderFlags().setSL(1);
					ord->getStOrderFlags().setDay(1);
					if (_omsStringParser.getChar(Fix_MsgType) == 'G')
						ord->getStOrderFlags().setModified(1);
					/*switch (_omsStringParser.getChar(Fix_TimeInForce))
					{
					case Fix_TimeInForce_DAY: ord->getStOrderFlags().setDay(1); break;
					case Fix_TimeInForce_IOC: ord->getStOrderFlags().setIOC(1); break;
					default: ord->getgetStOrderFlags().setSL(1); break;
					}*/

					if (_omsStringParser.get(Fix_Account) == Fix_Account_OWN ||
						_omsStringParser.get(Fix_Account) == Fix_Account_PRO)
					{
						ord->setProClientIndicator(ProClientIndicator::PRO);
						ord->setPAN(getPanNumber(_omsStringParser.get(Fix_Account)));
					}
					else
					{
						ord->setProClientIndicator(ProClientIndicator::CLIENT);
						ord->setAccountNumber(_omsStringParser.get(Fix_Account));
						ord->setSettlor(getCustodianId(_omsStringParser.get(Fix_Account)));
						ord->setPAN(getPanNumber(_omsStringParser.get(Fix_Account)));
						// std::cout << ord->getAccountNumber() << " : " << ord->getPAN() << " : " << ord->getSettlor() << std::endl ;
					}

					switch (_omsStringParser.getChar(Fix_Side))
					{
					case Fix_Side_Buy:
						ord->setBuySellIndicator(SIDE::SIDE_BUY);
						break;
					case Fix_Side_Sell:
						ord->setBuySellIndicator(SIDE::SIDE_SELL);
						break;
					}
					ord->setOpenClose('O');
					// ord->setNnfField(std::stol(configDetails->_terminalInfo));

					auto termInfo = _omsStringParser.get(Fix_TerminalInfo);
					if (termInfo == "0")
						ord->setNnfField(std::stoll(configDetails->terminalInfo));
					else
						ord->setNnfField(std::stoll(termInfo));
				}
				void Adapter::sendNew_StopLossOrder()
				{
					// fillCommonOrder_Stoploss_Order(_nnfstopLossOrder);
					fillCommonOrder_Stoploss_Order<NSE::TRIMMED::COM::NEW_ORDER>(_nnfstopLossOrder);
					_nnfstopLossOrder->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()));
					_nnfstopLossOrder->setTotalVolumeRemaining(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()));
					_nnfstopLossOrder->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_COM_STOP_LOSS_PRICE_MULTIPLIER));
					_nnfstopLossOrder->setTriggerPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_TriggerPrice)), NSE_COM_STOP_LOSS_PRICE_MULTIPLIER));

					Connection::sendToTarget_Encrypt(*_nnfstopLossOrder);
					_nnfstopLossOrder->reset();
				}
				void Adapter::SendCancel_StopLossOrder()
				{
					fillCommonOrder_Stoploss_Order<NSE::TRIMMED::COM::CANCEL_ORDER>(_nnfstopLossCancel);

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfstopLossCancel->setOrderNumber(orderNo);
						
						itrExchangeTimeMap = _orderIdExchangeTimeMap.find(orderNo);
						if (itrExchangeTimeMap!=_orderIdExchangeTimeMap.end())
						{
							_nnfstopLossCancel->setEntryDateTime(itrExchangeTimeMap->second.first);
							_nnfstopLossCancel->setLastModified(itrExchangeTimeMap->second.second);
						}
						
						itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);
						if (itrActivityTime!=_orderIdExLastActivityRefTimeMap.end())
						{
							_nnfstopLossCancel->setLastActivityReference(itrActivityTime->second);
						}
					}

					//_nnfCancelOrder.setDisclosedVolumeRemaining( singleOrder.getDisclosedQuantity() );
					_nnfstopLossCancel->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfstopLossCancel);
					_nnfstopLossCancel->reset();
				}
				void Adapter::SendReplace_StopLossOrder()
				{
					fillCommonOrder_Stoploss_Order<NSE::TRIMMED::COM::MODIFY_ORDER>(_nnfstopLossModify);

					_nnfstopLossModify->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_COM_STOP_LOSS_PRICE_MULTIPLIER));
					_nnfstopLossModify->setTriggerPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_TriggerPrice)), NSE_COM_STOP_LOSS_PRICE_MULTIPLIER));
					_nnfstopLossModify->setTotalVolumeRemaining(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfstopLossModify->setVolumeFilledToday(std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfstopLossModify->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfstopLossModify->setOrderNumber(orderNo);

						itrExchangeTimeMap = _orderIdExchangeTimeMap.find(orderNo);
						if (itrExchangeTimeMap != _orderIdExchangeTimeMap.end())
						{
							_nnfstopLossModify->setEntryDateTime(itrExchangeTimeMap->second.first);
							_nnfstopLossModify->setLastModified(itrExchangeTimeMap->second.second);
						}

						itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);
						if (itrActivityTime != _orderIdExLastActivityRefTimeMap.end())
						{
							_nnfstopLossModify->setLastActivityReference(itrActivityTime->second);
						}
					}

					_nnfstopLossModify->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfstopLossModify);
					_nnfstopLossModify->reset();
				}
				void Adapter::sendNewOrder()
				{
					fillOrderCommon<NSE::TRIMMED::COM::ORDER_NEW_REQUEST_TR>(_nnfNewOrder);
					
					_nnfNewOrder->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()));

					if (_omsStringParser.get(Fix_OrdType) == Fix_OrdType_Limit)
					{
						_nnfNewOrder->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_COM_PRICE_MULTIPLIER));
					}
					Connection::sendToTarget_Encrypt(*_nnfNewOrder);

					/*std::string str;
					_nnfNewOrder->getStringWithValue(str);
					LOG_INFO << "new order : " << str;*/
					
					_nnfNewOrder->reset();
				}

				void Adapter::sendReplaceOrder()
				{

					_nnfReplaceOrder->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_COM_PRICE_MULTIPLIER));
					_nnfReplaceOrder->setTotalVolumeRemaining(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfReplaceOrder->setVolumeFilledToday(std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfReplaceOrder->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));

					fillOrderCommon<NSE::TRIMMED::COM::ORDER_MODIFY_TR>(_nnfReplaceOrder);

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfReplaceOrder->setOrderNumber(orderNo);
						
						itrExchangeTimeMap = _orderIdExchangeTimeMap.find(orderNo);
						if (itrExchangeTimeMap != _orderIdExchangeTimeMap.end())
						{
							_nnfReplaceOrder->setEntryDateTime(itrExchangeTimeMap->second.first);
							_nnfReplaceOrder->setLastModified(itrExchangeTimeMap->second.second);
						}

						itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);
						if (itrActivityTime != _orderIdExLastActivityRefTimeMap.end())
						{
							LOG_INFO << "activity time get : " << itrActivityTime->second;
							_nnfReplaceOrder->setLastActivityReference(itrActivityTime->second);
						}
					}

					_nnfReplaceOrder->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfReplaceOrder);

					/*std::string str;
					_nnfReplaceOrder->getStringWithValue(str);
					LOG_INFO << "modify order : " << str;*/

					_nnfReplaceOrder->reset();
				}

				void Adapter::sendCancelOrder()
				{
					fillOrderCommon<NSE::TRIMMED::COM::ORDER_CANCEL_TR>(_nnfCancelOrder);

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfCancelOrder->setOrderNumber(orderNo);
						
						itrExchangeTimeMap = _orderIdExchangeTimeMap.find(orderNo);
						if (itrExchangeTimeMap != _orderIdExchangeTimeMap.end())
						{
							_nnfCancelOrder->setEntryDateTime(itrExchangeTimeMap->second.first);
							_nnfCancelOrder->setLastModified(itrExchangeTimeMap->second.second);
						}

						itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);
						if (itrActivityTime != _orderIdExLastActivityRefTimeMap.end())
						{
							_nnfCancelOrder->setLastActivityReference(itrActivityTime->second);
						}
					}

					//_nnfCancelOrder.setDisclosedVolumeRemaining( singleOrder.getDisclosedQuantity() );
					_nnfCancelOrder->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfCancelOrder);

					/*std::string str;
					_nnfCancelOrder->getStringWithValue(str);
					LOG_INFO << "cancel order : " << str;*/

					_nnfCancelOrder->reset();
				}

				void Adapter::startOmsRcv()
				{
					bool first = true;
					while (true)
					{
						if (_loggedIn)
						{
							if (first)
							{
								std::thread heartbeat(&Adapter::processHeartbeat, this);
								heartbeat.detach();
								doOMSConnect();
								first = false;
								//process heartbeat
							}
							try
							{

								isConsumerDone = true;
								if (udp_rabbit)
									rabbitMqConsumer();
								else
									OmsUDPRcv::getOMSReq();
							}
							catch (std::exception& e)
							{
								isOmsConnected = false;
								//_omsSenderUDP.isConnected(true);
								// OmsUDPRcv::isConnected(true) ;
								std::thread([this]() {restartOmsConnections();}).detach();
							}
						}
						else
						{
							// DEVLOG ( "Waiting for exch logon to accept from  OMS" ) ;
							//_omsSenderUDP.isConnected(false);
							// OmsUDPRcv::isConnected(false) ;
							Sleep(100);
						}
					}
				}

				void Adapter::processHeartbeat()
				{
					while (1)
					{
						if (isSecureBoxDone)
						{
							Sleep(20000);
							if (isConsumerDone && isSecureBoxDone)
							{
								if (udp_rabbit)
									sendRabbitHeartbeat();
								else
									OmsUDPRcv::sendHeartBeat();
							}
							else
								sendToTarget_Encrypt(*_heartBeatMsg_);
						}
						else
						{
							Sleep(1000);
						}
					}
				}

				void Adapter::restartOmsConnections()
				{
					std::cout << "Restarting OMS Connection\n";
					if (udp_rabbit)
					{
						_omsRabbitSender.isConnected(true);
						RabbitMqConsumer::connect(configPath, configDetails->rabbit_ip, configDetails->rabbit_port, configDetails->exch_number, configDetails->trader_id);
						_omsRabbitSender.RabbitMqConnection(configPath, configDetails->rabbit_ip, configDetails->rabbit_port, configDetails->exch_number, configDetails->trader_id);
					}
					else
					{
						_omsSenderUDP.isConnected(true);
						OmsUDPRcv::isConnected(true);
					}
					while (!_loggedIn)
					{
						std::this_thread::sleep_for(std::chrono::seconds(1));
					}
					_omsSender.init(configDetails->tcp_connfirmation_port);
					_omsSender.accept();
					isOmsConnected = true;
				}

				void Adapter::onLogon()
				{
					LOG_INFO << " ----------------------------------------------------------------------------";
					LOG_INFO << "  NSE COM DC | BrokerId: " << configDetails->brokerId << " | TraderId: " << configDetails->trader_id << " LOGGED IN";
					LOG_INFO << " ----------------------------------------------------------------------------";
					FIN::setConsoleColor(2);
					std::cout << "----------------------------------------------------------------------------\n";
					std::cout << " NSE COM DC | BrokerId: " << configDetails->brokerId << " | TraderId: " << configDetails->trader_id << " LOGGED IN\n";
					std::cout << "----------------------------------------------------------------------------\n";
					FIN::setConsoleColor(7);
					_loggedIn = true;
					_omsSender.startConnection();
				}

				void Adapter::doOMSConnect()
				{
					if (udp_rabbit != 1)
					{
						OmsUDPRcv::connect(configDetails->request_port);
						_omsSenderUDP.connect(configDetails->unicast_ip, configDetails->response_port);
						_omsSenderUDP.isConnected(true);
						OmsUDPRcv::isConnected(true);
					}
					else
					{
						_omsRabbitSender.isConnected(true);
						RabbitMqConsumer::connect(configPath, configDetails->rabbit_ip, configDetails->rabbit_port, configDetails->exch_number, configDetails->trader_id);				//consumer	connection
						_omsRabbitSender.RabbitMqConnection(configPath, configDetails->rabbit_ip, configDetails->rabbit_port, configDetails->exch_number, configDetails->trader_id);		//producer connection

					}
					_omsSender.init(configDetails->tcp_connfirmation_port);
					_omsSender.accept();
					isOmsConnected = true;
					//_omsSender.startOmsHealthThread();
				}

				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_CANCELATION_ERROR* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					_fixStringCreator.insertField(Fix_MsgType, Order_cancel_reject);
					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, Rejected);
					_fixStringCreator.insertField(Fix_ExecType, Rejected);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getTotalVolumeRemaining() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_CxlRejResponseTo, "1");
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();
					_orderIdClOrderId[(int64_t)msg->getOrderNumber()] = msg->getClOrdId();

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_MODIFICATION_ERROR* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					_fixStringCreator.insertField(Fix_MsgType, Order_cancel_reject);
					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, Rejected);
					_fixStringCreator.insertField(Fix_ExecType, Rejected);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_CxlRejResponseTo, "2");
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();
					_orderIdClOrderId[(int64_t)msg->getOrderNumber()] = msg->getClOrdId();

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_MOD_CONFIRMATION_TR* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						DEVLOG("Handle Mod Trim OrderRejection");
						return;
					}
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, Replaced);
					_fixStringCreator.insertField(Fix_ExecType, Replaced);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));
					
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();
					LOG_INFO << "activity time set : " << msg->getLastActivityReference();
					_orderIdExLastActivityRefTimeMap[(int64_t)msg->getOrderNumber()] = msg->getLastActivityReference();
					_orderIdClOrderId[(int64_t)msg->getOrderNumber()] = msg->getClOrdId();

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_MODIFICATION_CONFIRMATION* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						DEVLOG("Handle Mod Trim OrderRejection");
						return;
					}
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, Replaced);
					_fixStringCreator.insertField(Fix_ExecType, Replaced);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_TriggerPrice, std::to_string((double)(msg->getTriggerPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));
					
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();
					_orderIdClOrderId[(int64_t)msg->getOrderNumber()] = msg->getClOrdId();
					_orderIdExLastActivityRefTimeMap.insert(std::make_pair((int64_t)msg->getOrderNumber(), msg->getLastActivityReference()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_CANCEL_CONFIRMATION* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						// Handle Cancel REJECT
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					}
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, Canceled);
					_fixStringCreator.insertField(Fix_ExecType, Canceled);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getTotalVolumeRemaining() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					_orderIdExchangeTimeMap.erase((int64_t)msg->getOrderNumber());

					_orderIdExLastActivityRefTimeMap.erase((int64_t)msg->getOrderNumber());

					_orderIdClOrderId.erase((int64_t)msg->getOrderNumber());
					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_CXL_CONFIRMATION_TR* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());
					if (msg->getErrorCode() != 0)
					{
						// Handle Cancel REJECT
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					}

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, Canceled);
					_fixStringCreator.insertField(Fix_ExecType, Canceled);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getTotalVolumeRemaining() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					_orderIdExchangeTimeMap.erase((int64_t)msg->getOrderNumber());
					_orderIdExLastActivityRefTimeMap.erase((int64_t)msg->getOrderNumber());
					_orderIdClOrderId.erase((int64_t)msg->getOrderNumber());
					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_NEW_CONFIRMATION_TR* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						// Handle NEW REJECT
						std::string errTxt(NSE::TRIMMED::COM::ExchErrorCodes::getErrorText(msg->getErrorCode()));
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
						return;
					}

					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();
					_orderIdClOrderId[(int64_t)msg->getOrderNumber()] = msg->getClOrdId();

					_orderIdExLastActivityRefTimeMap.insert(std::make_pair((int64_t)msg->getOrderNumber(), msg->getLastActivityReference()));

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, New);
					_fixStringCreator.insertField(Fix_ExecType, New);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_CONFIRM_RESPONSE* msg)
				{
					LOG_INFO << "INSIDE ORDER_CONFIRMATION_RESPONSE onMessage jsjhsgdhdhsgs";
					if (msg->getErrorCode() != 0)
					{
						// Handle NEW REJECT
						std::string errTxt(NSE::TRIMMED::COM::ExchErrorCodes::getErrorText(msg->getErrorCode()));
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
						// LOG_INFO<<"INSIDE ORDNEW CONFRTR Error Code : " <<std::to_string( msg->getErrorCode() );
						return;
					}

					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();
					_orderIdClOrderId[(int64_t)msg->getOrderNumber()] = msg->getClOrdId();
					_orderIdExLastActivityRefTimeMap.insert(std::make_pair((int64_t)msg->getOrderNumber(), msg->getLastActivityReference()));

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, New);
					_fixStringCreator.insertField(Fix_ExecType, New);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_TriggerPrice, std::to_string((double)(msg->getTriggerPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField("81", std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::COM::MS_OE_REQUEST* msg)
				{
					LOG_INFO << "INSIDE ORDER_NEW_CONFIRMA onMessage";
					if (msg->getErrorCode() != 0)
					{
						// Handle NEW REJECT
						std::string errTxt(NSE::TRIMMED::COM::ExchErrorCodes::getErrorText(msg->getErrorCode()));
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
						LOG_INFO << "INSIDE Order new Error Code : " << std::to_string(msg->getErrorCode());
						return;
					}

					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();
					_orderIdClOrderId[(int64_t)msg->getOrderNumber()] = msg->getClOrdId();
					_orderIdExLastActivityRefTimeMap.insert(std::make_pair((int64_t)msg->getOrderNumber(), msg->getLastActivityReference()));

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, New);
					_fixStringCreator.insertField(Fix_ExecType, New);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::COM::MS_TRADE_CONFIRM* msg)
				{
					_fixStringCreator.reset();

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					itrClOrderId = _orderIdClOrderId.find((int64_t)msg->getResponseOrderNumber());
					if (itrClOrderId != _orderIdClOrderId.end())
					{
						_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(itrClOrderId->second));
					}
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_LeavesQty, std::to_string(msg->getRemainingVolume()));
					_fixStringCreator.insertField(Fix_ExecID, std::to_string(msg->getFillNumber()));
					if (msg->getRemainingVolume() == 0)
					{
						_fixStringCreator.insertField(Fix_OrdStatus, Filled);
						_fixStringCreator.insertField(Fix_ExecType, Filled);
					}
					else
					{
						_fixStringCreator.insertField(Fix_OrdStatus, Partial_filled);
						_fixStringCreator.insertField(Fix_ExecType, Partial_filled);
					}
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getResponseOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getRemainingVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getLogTime()));
					_fixStringCreator.insertField(Fix_LastPx, std::to_string((double)(msg->getFillPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_LastShares, std::to_string(msg->getFillQuantity()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::COM::MS_TRADE_CONFIRM_TR* msg)
				{
					_fixStringCreator.reset();

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					itrClOrderId = _orderIdClOrderId.find((int64_t)msg->getResponseOrderNumber());
					if (itrClOrderId != _orderIdClOrderId.end())
					{
						_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(itrClOrderId->second));
					}
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_LeavesQty, std::to_string(msg->getRemainingVolume()));
					_fixStringCreator.insertField(Fix_ExecID, std::to_string(msg->getFillNumber()));
					if (msg->getRemainingVolume() == 0)
					{
						_fixStringCreator.insertField(Fix_OrdStatus, Filled);
						_fixStringCreator.insertField(Fix_ExecType, Filled);
					}
					else
					{
						_fixStringCreator.insertField(Fix_OrdStatus, Partial_filled);
						_fixStringCreator.insertField(Fix_ExecType, Partial_filled);
					}
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getResponseOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getRemainingVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getLogTimeAsLocal(), 0));
					_fixStringCreator.insertField(Fix_LastPx, std::to_string((double)(msg->getFillPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_LastShares, std::to_string(msg->getFillQty()));

					itrExchangeTimeMap = _orderIdExchangeTimeMap.find((int64_t)msg->getResponseOrderNumber());
					if (itrExchangeTimeMap!=_orderIdExchangeTimeMap.end())
					{
						_orderIdExchangeTimeMap[(int64_t)msg->getResponseOrderNumber()].second = msg->getActivityTime();
					}
					_orderIdExLastActivityRefTimeMap.insert(std::make_pair((int64_t)msg->getResponseOrderNumber(), msg->getLastActivityReference()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::COM::ORDER_ERROR_RESPONSE* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()) + "Reason Code : " + std::to_string(msg->getReasonCode()));
					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, Rejected);
					_fixStringCreator.insertField(Fix_ExecType, Rejected);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_COM_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}

				//only for reject when exchange not up
				void Adapter::rejectMessage(long clOrderId, std::string errorMsg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(clOrderId);

					_fixStringCreator.insertField(Fix_SendingTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(clOrderId));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(0));
					_fixStringCreator.insertField(Fix_OrdStatus, Canceled);
					_fixStringCreator.insertField(Fix_ExecType, Canceled);
					_fixStringCreator.insertField(Fix_OrderID, std::to_string(0));
					_fixStringCreator.insertField(Fix_TransactTime, currentTimeReadable());
					_fixStringCreator.insertField(Fix_Text, errorMsg);

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					sendToOMS(omsStr);
				}

				void Adapter::sendToOMS(std::string omsStr)
				{
					if (udp_rabbit)
						_omsRabbitSender.sendForOMS(omsStr);
					else
						_omsSenderUDP.send(omsStr);
				}

				void Adapter::sendRequestDisconnectOmsConn()
				{
					//exchange disconnected
					setExchangeIsUPStatus(false);
					_loggedIn = false;

					//send logout msg
					std::cout << "Closing oms connection..." << std::endl;
					isOmsConnected = false;
					_omsSender.stopConnection();
					_omsSender.sendLogoutMsg(_omsSender.logoutMessage());
					_omsSender.stopProcessing();
					_omsSender.closeSocketConnection();
					std::thread([this]() {restartOmsConnections(); }).detach();
				}

				void Adapter::setExchangeIsUPStatus(bool status)
				{
					_nowExchangeLive = status;
				}

				void Adapter::stopConnection()
				{
					stopSocketConnection();
				}

				bool Adapter::getExchangeUpStatus()
				{
					return _nowExchangeLive;
				}

				bool Adapter::getOmsConnectionStatus()
				{
					return _omsSender.isOmsConnect();
				}

				void Adapter::fillMultiLegOrder()
				{
				}

				void Adapter::sendMulti()
				{
					NSE::TRIMMED::COM::MULTI_LEG_TWO_L multiReq(configDetails->trader_id, NSE::TRIMMED::TransCode_TWOL_BOARD_LOT_IN);

					multiReq.setFiller(std::to_string(time(NULL)));
					multiReq.setSettlementPeriod1(10);
					multiReq.setNnfField(380015001011100);
					multiReq.setTraderId1(configDetails->trader_id);
					multiReq.setBrokerId1("90012");
					multiReq.setBranchId1(1);

					multiReq.setProClient1(ProClientIndicator::PRO);

					{
						multiReq.setBuySell1(SIDE::SIDE_BUY);
						multiReq.setDisclosedVol1(0);
						multiReq.setDisclosedVol1(0);
						multiReq.getContractDesc1().setInstrumentName("FUTIDX");
						multiReq.getContractDesc1().setOptionType("XX");
						multiReq.setOpenClose1('O');

						int q;
						std::cout << "Enter Leg1 Quantity: ";
						std::cin >> q;
						multiReq.setTotalVolRemaining1(q);
						multiReq.setVolume1(q);

						int p;
						std::cout << "Enter Leg1 Price: ";
						std::cin >> p;
						p *= 100;
						multiReq.setPrice1(p);

						multiReq.getStOrderFlags1().setDay(1);

						multiReq.getContractDesc1().setStrikePrice(-1);
						// multiReq.setToken1( 42937 ) ;
						// multiReq.getContractDesc1().setSymbol("NIFTY") ;
						// multiReq.getContractDesc1().setExpiryDate( 1222525800 ) ;
						multiReq.setToken1(48572);
						multiReq.getContractDesc1().setSymbol("BANKNIFTY");
						multiReq.getContractDesc1().setExpiryDate(1224945000); // Handle the NSE's time format in Defines itself
					}

					{
						multiReq.getLeg2().setBuySell(SIDE::SIDE_BUY);
						multiReq.getLeg2().setDisclosedVol(0);
						multiReq.getLeg2().setDisclosedVol(0);
						multiReq.getLeg2().getContractDesc().setInstrumentName("FUTIDX");
						multiReq.getLeg2().getContractDesc().setOptionType("XX");

						multiReq.getLeg2().setOpenClose('O');
						multiReq.getLeg2().getStOrderFlags().setDay(1);

						int q;
						std::cout << "Enter Leg2 Quantity: ";
						std::cin >> q;
						multiReq.getLeg2().setTotalVolRemaining(q);
						multiReq.getLeg2().setVolume(q);

						int p;
						std::cout << "Enter Leg2 Price: ";
						std::cin >> p;
						p *= 100;
						;
						multiReq.getLeg2().setPrice(p);

						multiReq.getLeg2().setToken(42936);
						multiReq.getLeg2().getContractDesc().setSymbol("BANKNIFTY");
						multiReq.getLeg2().getContractDesc().setStrikePrice(-1);
						multiReq.getLeg2().getContractDesc().setExpiryDate(1222525800);
					}

					Connection::sendToTarget_Encrypt(multiReq);
				}
			}
		}
	}
}
