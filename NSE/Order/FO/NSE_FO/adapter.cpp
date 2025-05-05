#include "adapter.h"
#include "constants.h"

namespace FIN
{
	namespace OrdGtwy
	{
		namespace NNF
		{
			namespace FO
			{

				Adapter::Adapter(boost::asio::io_service& ioService)
					: ExchangeProcess(ioService),
					// OmsRcvServer ( ioService ) ,
					//RabbitMqConsumer(CONSTANT::NSE_FO_ORDER_CONFIG),
					_ioService(ioService),
					_omsSender(_ioService)
					//_omsRabbitSender(CONSTANT::NSE_FO_ORDER_CONFIG)
				{
					_omsSenderUDP.isConnected(false);
					OmsUDPRcv::isConnected(false);
					//_omsRabbitSender.isConnected(false);

					init();
					_loggedIn = false; // Use Connection Status
					_heartBeatMsg = new FIN::NSE::TRIMMED::HEARTBEAT_MESSAGE(getConnectionParam()._traderId);
				}

				bool Adapter::init()
				{
					udp_rabbit = getConnectionParam().getParam<int>("SESSION.UDP_RABBIT", 1);

					_st = new SlidingWindowThrottling(
						getConnectionParam().getParam<int>("DEFAULT.THROTTLE_LIMIT", 1),
						getConnectionParam().getParam<int>("DEFAULT.THROTTLE_TIME_WINDOW", 1));

					// std::string panKey = "SESSION.PAN" ;
					// std::string custKey = "SESSION.CUSTODIAN_ID" ;
					_defPAN = "PAN_EXEMPT";
					_custodianId = getConnectionParam()._brokerId;
					std::string algoIdKey = "SESSION.ALGO_ID";
					std::string algoCatKey = "SESSION.ALGO_CAT";

					configPath = _currPath.getCurrentPath() + FIN::CONSTANT::NSE_FO_ORDER_CONFIG;

					_nnfNewOrder = new NSE::TRIMMED::FO::ORDER_NEW_REQUEST_TR(getConnectionParam()._traderId);
					_nnfCancelOrder = new NSE::TRIMMED::FO::ORDER_CANCEL_TR(getConnectionParam()._traderId);
					_nnfReplaceOrder = new NSE::TRIMMED::FO::ORDER_MODIFY_TR(getConnectionParam()._traderId);

					_nnfstopLossOrder = new NSE::TRIMMED::FO::NEW_ORDER(getConnectionParam()._traderId);
					_nnfstopLossOrder->setBrokerId(getConnectionParam()._brokerId);
					_nnfstopLossOrder->setBranchId(getConnectionParam()._branchId);
					_nnfstopLossOrder->setAlgoID(getConnectionParam().getParam(algoIdKey, 0));
					_nnfstopLossOrder->setTraderId(getConnectionParam()._traderId);
					_nnfstopLossOrder->setDisclosedVolume(0);

					_nnfstopLossCancel = new NSE::TRIMMED::FO::CANCEL_ORDER(getConnectionParam()._traderId);
					_nnfstopLossCancel->setBrokerId(getConnectionParam()._brokerId);
					_nnfstopLossCancel->setBranchId(getConnectionParam()._branchId);
					_nnfstopLossCancel->setAlgoID(getConnectionParam().getParam(algoIdKey, 0));
					_nnfstopLossCancel->setTraderId(getConnectionParam()._traderId);

					_nnfstopLossModify = new NSE::TRIMMED::FO::MODIFY_ORDER(getConnectionParam()._traderId);
					_nnfstopLossModify->setBrokerId(getConnectionParam()._brokerId);
					_nnfstopLossModify->setBranchId(getConnectionParam()._branchId);
					_nnfstopLossModify->setAlgoID(getConnectionParam().getParam(algoIdKey, 0));
					_nnfstopLossModify->setTraderId(getConnectionParam()._traderId);

					_nnfNewOrder->setBrokerId(getConnectionParam()._brokerId);
					_nnfNewOrder->setBranchId(getConnectionParam()._branchId);
					_nnfReplaceOrder->setBrokerId(getConnectionParam()._brokerId);
					_nnfReplaceOrder->setBranchId(getConnectionParam()._branchId);
					_nnfCancelOrder->setBrokerId(getConnectionParam()._brokerId);
					_nnfCancelOrder->setBranchId(getConnectionParam()._branchId);

					_nnfNewOrder->setAlgoID(getConnectionParam().getParam(algoIdKey, 0));
					_nnfCancelOrder->setAlgoID(getConnectionParam().getParam(algoIdKey, 0));
					_nnfReplaceOrder->setAlgoID(getConnectionParam().getParam(algoIdKey, 0));

					readAccountConfigMapping();
					return false;
				}

				void Adapter::stop() {}

				void Adapter::start()
				{
					LOG_INFO << " ----------------------------------------------------------------------------";
					LOG_INFO << "  Starting NSE FO NNF TRIMMED DC ORDER GATEWAY ";
					LOG_INFO << " ----------------------------------------------------------------------------";
					std::cout << "----------------------------------------------------------------------------\n";
					std::cout << " Starting NSE FO NNF TRIMMED DC ORDER GATEWAY \n";
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
						DEVLOG("Exception :" << e.what());
					}
				}

				void Adapter::setClOrderId(uint32_t id)
				{
					orderHistory.push(id);
				}

				void Adapter::removeIdFromOrderHistory(uint32_t removeValue)
				{
					if (orderHistory.empty())
					{
						return;
					}
					std::vector<uint32_t> tempContainer;
					uint32_t value;

					// Pop all items into a temporary container
					while (orderHistory.pop(value)) 
					{
						if (value != removeValue) {
							tempContainer.push_back(value);
						}
					}

					// Push remaining items back into the queue
					for (uint32_t v : tempContainer) {
						while (!orderHistory.push(v));  // Ensure capacity
					}
				}

				bool Adapter::orderHistoryIsEmpty()
				{
					return orderHistory.empty();
				}

				uint32_t Adapter::getClOrderid()
				{
					return orderHistory.front();
				}

				void Adapter::removeClOrdId()
				{
					orderHistory.pop();
				}

				template <typename T>
				void Adapter::fillOrderCommon(T* ord)
				{
					ord->setClOrdId(std::atoi(_omsStringParser.get(Fix_ClOrdID).c_str()));

					ord->setTokenNo(std::atol(_omsStringParser.get(Fix_SecurityID).c_str()));

					ord->getContractDesc().setExpiryDate(std::atoi(_omsStringParser.get(Fix_MaturityMonthYear).c_str()));

					ord->getContractDesc().setSymbol(_omsStringParser.get(Fix_Symbol));

					if (_omsStringParser.get(Fix_SecurityType) == Fix_SecurityType_FUT)
					{
						ord->getContractDesc().setStrikePrice(-1);
						ord->getContractDesc().setOptionType(NNF_OptionType_XX);
						switch (_omsStringParser.getChar(Fix_Product))
						{
						case Fix_Product_IDX:
							ord->getContractDesc().setInstrumentName(NNF_Product_FUTIDX);
							break;
						case Fix_Product_STK:
							ord->getContractDesc().setInstrumentName(NNF_Product_FUTSTK);
							break;
						case Fix_Product_CUR:
							ord->getContractDesc().setInstrumentName(NNF_Product_FUTCUR);
							break;
						}
					}
					else
					{
						ord->getContractDesc().setStrikePrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_StrikePrice)), NSE_FO_PRICE_MULTIPLIER));

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

						switch (_omsStringParser.getChar(Fix_Product))
						{
						case Fix_Product_IDX:
							ord->getContractDesc().setInstrumentName(NNF_Product_OPTIDX);
							break;
						case Fix_Product_STK:
							ord->getContractDesc().setInstrumentName(NNF_Product_OPTSTK);
							break;
						case Fix_Product_CUR:
							ord->getContractDesc().setInstrumentName(NNF_Product_OPTCUR);
							break;
						}
					}

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
					auto termInfo = _omsStringParser.get(Fix_TerminalInfo);
					if (termInfo == "0")
						ord->setNnfField(std::stod(getConnectionParam()._terminalInfo));
					else
						ord->setNnfField(std::stod(termInfo));


				}
				template <typename T>
				void Adapter::fillCommonOrder_Stoploss_Order(T* ord)
				{

					ord->setTokenNo(std::atol(_omsStringParser.get(Fix_SecurityID).c_str()));
					ord->getContractDesc().setExpiryDate(std::atoi(_omsStringParser.get(Fix_MaturityMonthYear).c_str()));
					ord->getContractDesc().setSymbol(_omsStringParser.get(Fix_Symbol));
					ord->setBookType(3);

					ord->setClOrdId(std::atoi(_omsStringParser.get(Fix_ClOrdID).c_str()));

					if (_omsStringParser.get(Fix_SecurityType) == Fix_SecurityType_FUT)
					{
						ord->getContractDesc().setStrikePrice(-1);
						ord->getContractDesc().setOptionType(NNF_OptionType_XX);
						switch (_omsStringParser.getChar(Fix_Product))
						{
						case Fix_Product_IDX:
							ord->getContractDesc().setInstrumentName(NNF_Product_FUTIDX);
							break;
						case Fix_Product_STK:
							ord->getContractDesc().setInstrumentName(NNF_Product_FUTSTK);
							break;
						case Fix_Product_CUR:
							ord->getContractDesc().setInstrumentName(NNF_Product_FUTCUR);
							break;
						}
					}
					else
					{
						ord->getContractDesc().setStrikePrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_StrikePrice)), NSE_FO_PRICE_MULTIPLIER));

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

						switch (_omsStringParser.getChar(Fix_Product))
						{
						case Fix_Product_IDX:
							ord->getContractDesc().setInstrumentName(NNF_Product_OPTIDX);
							break;
						case Fix_Product_STK:
							ord->getContractDesc().setInstrumentName(NNF_Product_OPTSTK);
							break;
						case Fix_Product_CUR:
							ord->getContractDesc().setInstrumentName(NNF_Product_OPTCUR);
							break;
						}
					}

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
					// ord->setNnfField(std::stol(getConnectionParam()._terminalInfo));

					auto termInfo = _omsStringParser.get(Fix_TerminalInfo);
					if (termInfo == "0")
						ord->setNnfField(std::stol(getConnectionParam()._terminalInfo));
					else
						ord->setNnfField(std::stol(termInfo));
				}
				void Adapter::sendNew_StopLossOrder()
				{
					// fillCommonOrder_Stoploss_Order(_nnfstopLossOrder);
					fillCommonOrder_Stoploss_Order<NSE::TRIMMED::FO::NEW_ORDER>(_nnfstopLossOrder);
					_nnfstopLossOrder->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()));
					_nnfstopLossOrder->setTotalVolumeRemaining(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()));
					_nnfstopLossOrder->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_FO_PRICE_MULTIPLIER));
					_nnfstopLossOrder->setTriggerPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_TriggerPrice)), NSE_FO_PRICE_MULTIPLIER));

					Connection::sendToTarget_Encrypt(*_nnfstopLossOrder);
					_nnfstopLossOrder->reset();
				}
				void Adapter::SendCancel_StopLossOrder()
				{
					fillCommonOrder_Stoploss_Order<NSE::TRIMMED::FO::CANCEL_ORDER>(_nnfstopLossCancel);

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfstopLossCancel->setOrderNumber(orderNo);
						// std::unordered_map<int64_t, std::pair<long, long>>::iterator itr;
						auto is_contained = _orderIdExchangeTimeMap.find(orderNo);

						if (is_contained)
						{
							auto item = _orderIdExchangeTimeMap.get(orderNo);
							_nnfstopLossCancel->setEntryDateTime(item.first);
							_nnfstopLossCancel->setLastModified(item.second);
						}
						//	std::unordered_map<int64_t, int64_t>::iterator itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);
						auto is_key = _orderIdExLastActivityRefTimeMap.find(orderNo);
						if (is_key)
						{
							auto item_ = _orderIdExLastActivityRefTimeMap.get(orderNo);

							_nnfstopLossCancel->setLastActivityReference(item_);
						}
					}

					//_nnfCancelOrder.setDisclosedVolumeRemaining( singleOrder.getDisclosedQuantity() );
					_nnfstopLossCancel->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfstopLossCancel);
					_nnfstopLossCancel->reset();
				}
				void Adapter::SendReplace_StopLossOrder()
				{
					fillCommonOrder_Stoploss_Order<NSE::TRIMMED::FO::MODIFY_ORDER>(_nnfstopLossModify);

					_nnfstopLossModify->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_FO_PRICE_MULTIPLIER));
					_nnfstopLossModify->setTriggerPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_TriggerPrice)), NSE_FO_PRICE_MULTIPLIER));
					_nnfstopLossModify->setTotalVolumeRemaining(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfstopLossModify->setVolumeFilledToday(std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfstopLossModify->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfstopLossModify->setOrderNumber(orderNo);

						// std::unordered_map<int64_t, std::pair<long, long>>::iterator itr;
						auto is_contain = _orderIdExchangeTimeMap.find(orderNo);

						if (is_contain)
						{
							auto item = _orderIdExchangeTimeMap.get(orderNo);
							_nnfstopLossModify->setEntryDateTime(item.first);
							_nnfstopLossModify->setLastModified(item.second);
						}
						// std::unordered_map<int64_t, int64_t>::iterator itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);

						auto is_key = _orderIdExLastActivityRefTimeMap.find(orderNo);
						if (is_key)
						{
							auto item_ = _orderIdExLastActivityRefTimeMap.get(orderNo);

							_nnfstopLossModify->setLastActivityReference(item_);
						}
					}

					_nnfstopLossModify->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfstopLossModify);
					_nnfstopLossModify->reset();
				}
				void Adapter::sendNewOrder()
				{

					fillOrderCommon<NSE::TRIMMED::FO::ORDER_NEW_REQUEST_TR>(_nnfNewOrder);
					_nnfNewOrder->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()));


					if (_omsStringParser.get(Fix_OrdType) == Fix_OrdType_Limit)
					{
						_nnfNewOrder->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_FO_PRICE_MULTIPLIER));
					}

					Connection::sendToTarget_Encrypt(*_nnfNewOrder);
					_nnfNewOrder->reset();

				}

				void Adapter::sendReplaceOrder()
				{

					_nnfReplaceOrder->setPrice(floatToInt32Multiplied(std::stod(_omsStringParser.get(Fix_Price)), NSE_FO_PRICE_MULTIPLIER));
					_nnfReplaceOrder->setTotalVolumeRemaining(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfReplaceOrder->setVolumeFilledToday(std::atol(_omsStringParser.get(Fix_CumQty).c_str()));
					_nnfReplaceOrder->setVolume(std::atol(_omsStringParser.get(Fix_OrdQty).c_str()) - std::atol(_omsStringParser.get(Fix_CumQty).c_str()));

					fillOrderCommon<NSE::TRIMMED::FO::ORDER_MODIFY_TR>(_nnfReplaceOrder);

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfReplaceOrder->setOrderNumber(orderNo);
						// std::unordered_map<int64_t, std::pair<long, long>>::iterator itr;
						auto is_contain = _orderIdExchangeTimeMap.find(orderNo);

						if (is_contain)
						{
							auto item = _orderIdExchangeTimeMap.get(orderNo);
							_nnfReplaceOrder->setEntryDateTime(item.first);
							_nnfReplaceOrder->setLastModified(item.second);
						}
						// std::unordered_map<int64_t, int64_t>::iterator itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);
						auto is_key = _orderIdExLastActivityRefTimeMap.find(orderNo);
						if (is_key)
						{
							auto item_ = _orderIdExLastActivityRefTimeMap.get(orderNo);
							_nnfReplaceOrder->setLastActivityReference(item_);
						}
					}

					_nnfReplaceOrder->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfReplaceOrder);
					_nnfReplaceOrder->reset();
				}

				void Adapter::sendCancelOrder()
				{
					fillOrderCommon<NSE::TRIMMED::FO::ORDER_CANCEL_TR>(_nnfCancelOrder);

					std::string val;
					if (_omsStringParser.getIfSet(Fix_OrderID, val))
					{
						int64_t orderNo;
						orderNo = std::stoll(val.c_str());
						_nnfCancelOrder->setOrderNumber(orderNo);
						// std::unordered_map<int64_t, std::pair<long, long>>::iterator itr;
						auto is_contained = _orderIdExchangeTimeMap.find(orderNo);

						if (is_contained)
						{
							auto item = _orderIdExchangeTimeMap.get(orderNo);
							_nnfCancelOrder->setEntryDateTime(item.first);
							_nnfCancelOrder->setLastModified(item.second);
						}
						//	std::unordered_map<int64_t, int64_t>::iterator itrActivityTime = _orderIdExLastActivityRefTimeMap.find(orderNo);
						auto is_key = _orderIdExLastActivityRefTimeMap.find(orderNo);

						if (is_key)
						{
							auto item_ = _orderIdExLastActivityRefTimeMap.get(orderNo);
							_nnfCancelOrder->setLastActivityReference(item_);
						}
					}

					//_nnfCancelOrder.setDisclosedVolumeRemaining( singleOrder.getDisclosedQuantity() );
					_nnfCancelOrder->setModifiedCancelledBy('T');

					Connection::sendToTarget_Encrypt(*_nnfCancelOrder);
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
								restartOmsConnections();
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

				/*bool Adapter::getOmsConnectionStatus()
				{
					return isOmsConnected;
				}*/

				void Adapter::restartOmsConnections()
				{

					_omsSender.init(getConnectionParam().getParam("DEFAULT.TCP_CONNFIRMATION_PORT", 0));
					if (udp_rabbit)
					{
						_omsRabbitSender.isConnected(true);
						RabbitMqConsumer::connect(configPath);
						_omsRabbitSender.RabbitMqConnection(configPath);
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
					_omsSender.accept();
					isOmsConnected = true;
				}

				void Adapter::onLogon()
				{
					LOG_INFO << " ----------------------------------------------------------------------------";
					LOG_INFO << "  NSE FO DC | BrokerId: " << getConnectionParam()._brokerId << " | TraderId: " << getConnectionParam()._traderId << " LOGGED IN";
					LOG_INFO << " ----------------------------------------------------------------------------";
					std::cout << "----------------------------------------------------------------------------\n";
					std::cout << " NSE FO DC | BrokerId: " << getConnectionParam()._brokerId << " | TraderId: " << getConnectionParam()._traderId << " LOGGED IN\n";
					std::cout << "----------------------------------------------------------------------------\n";
					_loggedIn = true;
					_omsSender.startConnection();
				}

				void Adapter::doOMSConnect()
				{
					if (udp_rabbit != 1)
					{
						OmsUDPRcv::connect(getConnectionParam().getParam("OMS.REQUEST_PORT", 0));

						_omsSenderUDP.connect(
							_ioService,
							getConnectionParam().getParam("OMS.UNICAST_IP", std::string("")),
							getConnectionParam().getParam("OMS.RESPONSE_PORT", 0));
						_omsSenderUDP.isConnected(true);
						OmsUDPRcv::isConnected(true);
					}
					else
					{
						_omsRabbitSender.isConnected(true);
						RabbitMqConsumer::connect(configPath);
						_omsRabbitSender.RabbitMqConnection(configPath);
					}
					_omsSender.init(getConnectionParam().getParam("DEFAULT.TCP_CONNFIRMATION_PORT", 0));
					_omsSender.accept();
					isOmsConnected = true;
					//_omsSender.startOmsHealthThread();
				}

				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_CANCELATION_ERROR* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "9");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "8");
					_fixStringCreator.insertField(Fix_ExecType, "8");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getTotalVolumeRemaining() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_CxlRejResponseTo, "1");
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					// _orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					// _orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();

					_orderIdExchangeTimeMap.insert_pair((int64_t)msg->getOrderNumber(), std::make_pair(msg->getEntryDateTime(), msg->getLastModified()));

					//_orderIdExLastActivityRefTimeMap[ (int64_t)msg->getOrderNumber() ] = msg->getLastActivityReference() ;
					_orderIdClOrderId.insert((int64_t)msg->getOrderNumber(), msg->getClOrdId());

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//        _omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_MODIFICATION_ERROR* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "9");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "8");
					_fixStringCreator.insertField(Fix_ExecType, "8");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_CxlRejResponseTo, "2");
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					//	_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					//	_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();

					_orderIdExchangeTimeMap.insert_pair((int64_t)msg->getOrderNumber(), std::make_pair(msg->getEntryDateTime(), msg->getLastModified()));

					//_orderIdExLastActivityRefTimeMap[ (int64_t)msg->getOrderNumber() ] = msg->getLastActivityReference() ;
					_orderIdClOrderId.insert((int64_t)msg->getOrderNumber(), msg->getClOrdId());

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_MOD_CONFIRMATION_TR* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						DEVLOG("Handle Mod Trim OrderRejection");
						return;
					}
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());

					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "5");
					_fixStringCreator.insertField(Fix_ExecType, "5");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));
					//_fixStringCreator.insertField( Fix_LastPx      , " " ) ;
					//_fixStringCreator.insertField( Fix_LastShares  , " " ) ;
					//_fixStringCreator.insertField( Fix_OrigClOrdID , " " ) ;
					//_fixStringCreator.insertField( Fix_Text        , " " ) ;

					//_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					//_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();

					_orderIdExchangeTimeMap.insert_pair((int64_t)msg->getOrderNumber(), std::make_pair(msg->getEntryDateTime(), msg->getLastModified()));

					//	_orderIdExLastActivityRefTimeMap[(int64_t)msg->getOrderNumber()] = msg->getLastActivityReference();
					_orderIdExLastActivityRefTimeMap.insert((int64_t)msg->getOrderNumber(), msg->getLastActivityReference());
					_orderIdClOrderId.insert((int64_t)msg->getOrderNumber(), msg->getClOrdId());

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_MODIFICATION_CONFIRMATION* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						DEVLOG("Handle Mod Trim OrderRejection");
						return;
					}
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());

					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "5");
					_fixStringCreator.insertField(Fix_ExecType, "5");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_TriggerPrice, std::to_string((double)(msg->getTriggerPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));
					//_fixStringCreator.insertField( Fix_LastPx      , " " ) ;
					//_fixStringCreator.insertField( Fix_LastShares  , " " ) ;
					//_fixStringCreator.insertField( Fix_OrigClOrdID , " " ) ;
					//_fixStringCreator.insertField( Fix_Text        , " " ) ;

					//	_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].first = msg->getEntryDateTime();
					//	_orderIdExchangeTimeMap[(int64_t)msg->getOrderNumber()].second = msg->getLastModified();

					_orderIdExchangeTimeMap.insert_pair((int64_t)msg->getOrderNumber(), std::make_pair(msg->getEntryDateTime(), msg->getLastModified()));

					//	_orderIdExLastActivityRefTimeMap[(int64_t)msg->getOrderNumber()] = msg->getLastActivityReference();
					_orderIdExLastActivityRefTimeMap.insert((int64_t)msg->getOrderNumber(), msg->getLastActivityReference());
					_orderIdClOrderId.insert((int64_t)msg->getOrderNumber(), msg->getClOrdId());

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_CANCEL_CONFIRMATION* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						// Handle Cancel REJECT
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					}
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "4");
					_fixStringCreator.insertField(Fix_ExecType, "4");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getTotalVolumeRemaining() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					_orderIdExchangeTimeMap.erase((int64_t)msg->getOrderNumber());

					_orderIdExLastActivityRefTimeMap.erase((int64_t)msg->getOrderNumber());

					_orderIdClOrderId.erase((int64_t)msg->getOrderNumber());
					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_CXL_CONFIRMATION_TR* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());
					if (msg->getErrorCode() != 0)
					{
						// Handle Cancel REJECT
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					}

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "4");
					_fixStringCreator.insertField(Fix_ExecType, "4");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getTotalVolumeRemaining() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					_orderIdExchangeTimeMap.erase((int64_t)msg->getOrderNumber());
					_orderIdExLastActivityRefTimeMap.erase((int64_t)msg->getOrderNumber());
					_orderIdClOrderId.erase((int64_t)msg->getOrderNumber());
					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_NEW_CONFIRMATION_TR* msg)
				{
					if (msg->getErrorCode() != 0)
					{
						// Handle NEW REJECT
						std::string errTxt(NSE::TRIMMED::FO::ExchErrorCodes::getErrorText(msg->getErrorCode()));
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
						return;
					}

					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					// _orderIdExchangeTimeMap.insert(std::make_pair(
					// 	(int64_t)msg->getOrderNumber(),
					// 	std::make_pair(msg->getEntryDateTime(), msg->getLastModified())));
					// _orderIdExLastActivityRefTimeMap.insert(std::make_pair(
					// 	(int64_t)msg->getOrderNumber(), msg->getLastActivityReference()));

					_orderIdExchangeTimeMap.insert_pair(
						(int64_t)msg->getOrderNumber(),
						std::make_pair(msg->getEntryDateTime(), msg->getLastModified()));

					_orderIdExLastActivityRefTimeMap.insert(
						(int64_t)msg->getOrderNumber(), msg->getLastActivityReference());

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "0");
					_fixStringCreator.insertField(Fix_ExecType, "0");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					_orderIdClOrderId.insert((int64_t)msg->getOrderNumber(), msg->getClOrdId());
					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_CONFIRM_RESPONSE* msg)
				{
					LOG_INFO << "INSIDE ORDER_CONFIRMATION_RESPONSE onMessage jsjhsgdhdhsgs";
					if (msg->getErrorCode() != 0)
					{
						// Handle NEW REJECT
						std::string errTxt(NSE::TRIMMED::FO::ExchErrorCodes::getErrorText(msg->getErrorCode()));
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
						// LOG_INFO<<"INSIDE ORDNEW CONFRTR Error Code : " <<std::to_string( msg->getErrorCode() );
						return;
					}

					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					// _orderIdExchangeTimeMap.insert(std::make_pair(
					// 	(int64_t)msg->getOrderNumber(),
					// 	std::make_pair(msg->getEntryDateTime(), msg->getLastModified())));

					_orderIdExchangeTimeMap.insert_pair(
						(int64_t)msg->getOrderNumber(),
						std::make_pair(msg->getEntryDateTime(), msg->getLastModified()));

					_orderIdExLastActivityRefTimeMap.insert(
						(int64_t)msg->getOrderNumber(), msg->getLastActivityReference());

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "0");
					_fixStringCreator.insertField(Fix_ExecType, "0");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_TriggerPrice, std::to_string((double)(msg->getTriggerPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField("81", std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					_orderIdClOrderId.insert((int64_t)msg->getOrderNumber(), msg->getClOrdId());
					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::FO::MS_OE_REQUEST* msg)
				{
					LOG_INFO << "INSIDE ORDER_NEW_CONFIRMA onMessage";
					if (msg->getErrorCode() != 0)
					{
						// Handle NEW REJECT
						std::string errTxt(NSE::TRIMMED::FO::ExchErrorCodes::getErrorText(msg->getErrorCode()));
						_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
						LOG_INFO << "INSIDE Order new Error Code : " << std::to_string(msg->getErrorCode());
						return;
					}

					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					// _orderIdExchangeTimeMap.insert(std::make_pair(
					// 	(int64_t)msg->getOrderNumber(),
					// 	std::make_pair(msg->getEntryDateTime(), msg->getLastModified())));

					_orderIdExchangeTimeMap.insert_pair(
						(int64_t)msg->getOrderNumber(),
						std::make_pair(msg->getEntryDateTime(), msg->getLastModified()));

					_orderIdExLastActivityRefTimeMap.insert(
						(int64_t)msg->getOrderNumber(), msg->getLastActivityReference());

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "0");
					_fixStringCreator.insertField(Fix_ExecType, "0");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					_orderIdClOrderId.insert((int64_t)msg->getOrderNumber(), msg->getClOrdId());
					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;

					sendToOMS(omsStr);
				}
				void Adapter::onMessage(NSE::TRIMMED::FO::MS_TRADE_CONFIRM* msg)
				{
					_fixStringCreator.reset();

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(_orderIdClOrderId.get((int64_t)msg->getResponseOrderNumber())));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_LeavesQty, std::to_string(msg->getRemainingVolume()));
					_fixStringCreator.insertField(Fix_ExecID, std::to_string(msg->getFillNumber()));
					if (msg->getRemainingVolume() == 0)
					{
						_fixStringCreator.insertField(Fix_OrdStatus, "2");
						_fixStringCreator.insertField(Fix_ExecType, "2");
					}
					else
					{
						_fixStringCreator.insertField(Fix_OrdStatus, "1");
						_fixStringCreator.insertField(Fix_ExecType, "1");
					}
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getResponseOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getRemainingVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getLogTime() + 315532800));
					_fixStringCreator.insertField(Fix_LastPx, std::to_string((double)(msg->getFillPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_LastShares, std::to_string(msg->getFillQuantity()));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;

					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::FO::MS_TRADE_CONFIRM_TR* msg)
				{
					_fixStringCreator.reset();

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(_orderIdClOrderId.get((int64_t)msg->getResponseOrderNumber())));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_LeavesQty, std::to_string(msg->getRemainingVolume()));
					_fixStringCreator.insertField(Fix_ExecID, std::to_string(msg->getFillNumber()));
					if (msg->getRemainingVolume() == 0)
					{
						_fixStringCreator.insertField(Fix_OrdStatus, "2");
						_fixStringCreator.insertField(Fix_ExecType, "2");
					}
					else
					{
						_fixStringCreator.insertField(Fix_OrdStatus, "1");
						_fixStringCreator.insertField(Fix_ExecType, "1");
					}
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getResponseOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getRemainingVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getLogTimeAsLocal()));
					_fixStringCreator.insertField(Fix_LastPx, std::to_string((double)(msg->getFillPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_LastShares, std::to_string(msg->getFillQty()));

					// std::unordered_map<int64_t, std::pair<long, long>>::iterator itr = _orderIdExchangeTimeMap.find((int64_t)msg->getResponseOrderNumber());
					auto is_contained = _orderIdExchangeTimeMap.find((int64_t)msg->getResponseOrderNumber());

					if (is_contained)
					{
						auto item = _orderIdExchangeTimeMap.get((int64_t)msg->getResponseOrderNumber());
						_orderIdExchangeTimeMap.insert((int64_t)msg->getResponseOrderNumber(), std::make_pair(item.first, msg->getActivityTime()));
					}
					_orderIdExLastActivityRefTimeMap.insert((int64_t)msg->getResponseOrderNumber(), msg->getLastActivityReference());

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;
					sendToOMS(omsStr);
				}

				void Adapter::onMessage(NSE::TRIMMED::FO::ORDER_ERROR_RESPONSE* msg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(msg->getClOrdId());

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField(Fix_Text, "Error Code : " + std::to_string(msg->getErrorCode()));
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(msg->getClOrdId()));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_OrdStatus, "8");
					_fixStringCreator.insertField(Fix_ExecType, "8");
					_fixStringCreator.insertField(Fix_OrderID, std::to_string((int64_t)msg->getOrderNumber()));
					_fixStringCreator.insertField(Fix_OrdQty, std::to_string(msg->getVolume() + msg->getVolumeFilledToday()));
					_fixStringCreator.insertField(Fix_Price, std::to_string((double)(msg->getPrice() / NSE_FO_PRICE_MULTIPLIER)));
					_fixStringCreator.insertField(Fix_Side, std::to_string(msg->getBuySellIndicator()));
					_fixStringCreator.insertField(Fix_TransactTime, timeNowReadable(msg->getEntryDateTime() + 315532800));

					std::string omsStr;
					_fixStringCreator.getOmsString(omsStr);
					//_omsRabbitSender.sendForOMS( omsStr ) ;
					//_omsSenderUDP.send(omsStr);
					// LOG_INFO << "[OMS_SND] " << omsStr ;

					sendToOMS(omsStr);
				}

				//only for reject when exchange not up
				void Adapter::rejectMessage(long clOrderId, std::string errorMsg)
				{
					_fixStringCreator.reset();
					removeIdFromOrderHistory(clOrderId);

					_fixStringCreator.insertField("8", "FIX4.2");
					_fixStringCreator.insertField("9", "300");
					_fixStringCreator.insertField(Fix_MsgType, "8");
					_fixStringCreator.insertField("34", "0");
					_fixStringCreator.insertField(Fix_SenderCompID, "12345");
					_fixStringCreator.insertField("52", currentTimeReadable());
					_fixStringCreator.insertField(Fix_AvgPX, "0");
					_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(clOrderId));
					_fixStringCreator.insertField(Fix_CumQty, std::to_string(0));
					_fixStringCreator.insertField(Fix_ExecID, "0");
					_fixStringCreator.insertField(Fix_OrdStatus, "4");
					_fixStringCreator.insertField(Fix_ExecType, "4");
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

					//send logout msg
					std::cout << "Closing oms connection..." << std::endl;
					isOmsConnected = false;
					_omsSender.stopConnection();
					_omsSender.sendLogoutMsg(_omsSender.logoutMessage());
					_omsSender.stopProcessing();
					//restartOmsConnections();
					_omsSender.closeSocketConnection();
				}

				void Adapter::setExchangeIsUPStatus(bool status)
				{
					_nowExchangeLive = status;
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
					NSE::TRIMMED::FO::MULTI_LEG_TWO_L multiReq(getConnectionParam()._traderId, NSE::TRIMMED::TransCode_TWOL_BOARD_LOT_IN);

					multiReq.setFiller(std::to_string(time(NULL)));
					multiReq.setSettlementPeriod1(10);
					multiReq.setNnfField(380015001011100);
					multiReq.setTraderId1(getConnectionParam()._traderId);
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
