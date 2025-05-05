#ifndef FIN_ORDER_GW_NNF_TRIMMED_DC_FO_H
#define FIN_ORDER_GW_NNF_TRIMMED_DC_FO_H

#include <unordered_map>
#include <boost/asio.hpp>
#include "stringParser.h"
#include "omsSocketHandler.h"
#include "omsUdpSocketHandler.h"
#include "ExchangeProcess.h"
#include "orderBook.h"
#include "ThreadSafeMap.h"
#include "throttling.h"
#include "CurrentPath.h"
#include "rabbitmqProducer.h"
#include "rabbitmqConsumer.h"
#include "constants.h"
#include "fixConstants.h"
#include <inttypes.h>
#include <stdio.h>
#include "errorCodes.h"
#include "messages.h"
#include <boost/lockfree/spsc_queue.hpp>
namespace FIN
{
	namespace OrdGtwy
	{
		namespace NNF
		{
			namespace FO
			{

				class Adapter : public NSE::TRIMMED::FO::ExchangeProcess, OmsUDPRcv, RabbitMqConsumer
				{
				public:
					Adapter(boost::asio::io_service& ioService);
					bool init();
					void stop();
					void start();

					void startOmsRcv();

					void processHeartbeat();
					std::string heartbeat = "H";
					//FIN::NSE::TRIMMED::HEARTBEAT_MESSAGE* _heartBeatMsg;
					

				private:
					bool isOmsConnected = false;
					SlidingWindowThrottling* _st;

					template <typename T>
					void fillOrderCommon(T* ord);

					bool sendOrder(const std::string& omsInput);
					void sendNewOrder();
					void sendReplaceOrder();
					void sendCancelOrder();
					void sendNew_StopLossOrder();
					void SendCancel_StopLossOrder();
					void SendReplace_StopLossOrder();
					void fillMultiLegOrder();
					void sendMulti();
					template <typename T>
					void fillCommonOrder_Stoploss_Order(T* ord);

					void onLogon();

					int udp_rabbit = 0;
					CurrentPath _currPath;
					std::string configPath;

					NSE::TRIMMED::FO::ORDER_NEW_REQUEST_TR* _nnfNewOrder;
					NSE::TRIMMED::FO::ORDER_CANCEL_TR* _nnfCancelOrder;
					NSE::TRIMMED::FO::ORDER_MODIFY_TR* _nnfReplaceOrder;

					NSE::TRIMMED::FO::NEW_ORDER* _nnfstopLossOrder;
					NSE::TRIMMED::FO::CANCEL_ORDER* _nnfstopLossCancel;
					NSE::TRIMMED::FO::MODIFY_ORDER* _nnfstopLossModify;

					//  OMS -> GW
					FixStringParser _omsStringParser;
					void handleOmsWrite(const std::string& str);

					void receiveFromOMS(const std::string&);

					void setClOrderId(uint32_t id);
					void removeIdFromOrderHistory(uint32_t id);
					bool orderHistoryIsEmpty();
					uint32_t getClOrderid();
					void removeClOrdId();

					boost::asio::io_service& _ioService;

					//  GW -> OMS
					FixStringCreator _fixStringCreator;
					OmsSenderServer _omsSender;

					RabbitMqProducer _omsRabbitSender;
					OmsUDPSend _omsSenderUDP;
					void restartOmsConnections();
					volatile bool _loggedIn;

					bool _nowExchangeLive = false;
					void setExchangeIsUPStatus(bool status);
					bool getExchangeUpStatus();

					bool getOmsConnectionStatus();
					void doOMSConnect();
					void onMessage(NSE::TRIMMED::FO::ORDER_NEW_CONFIRMATION_TR* msg);
					void onMessage(NSE::TRIMMED::FO::ORDER_CXL_CONFIRMATION_TR* msg);
					void onMessage(NSE::TRIMMED::FO::ORDER_MOD_CONFIRMATION_TR* msg);
					void onMessage(NSE::TRIMMED::FO::MS_TRADE_CONFIRM* msg);
					void onMessage(NSE::TRIMMED::FO::MS_TRADE_CONFIRM_TR* msg);
					void onMessage(NSE::TRIMMED::FO::ORDER_ERROR_RESPONSE* msg);
					void onMessage(NSE::TRIMMED::FO::ORDER_MODIFICATION_ERROR* msg);
					void onMessage(NSE::TRIMMED::FO::ORDER_CANCELATION_ERROR* msg);
					void onMessage(NSE::TRIMMED::FO::MS_OE_REQUEST* msg);
					void onMessage(NSE::TRIMMED::FO::ORDER_CONFIRM_RESPONSE* msg);
					//            void onMessage ( NSE::TRIMMED::FO::ORDER_ERROR_RESPONSE * msg ) ;
					void onMessage(NSE::TRIMMED::FO::ORDER_CANCEL_CONFIRMATION* msg);
					void onMessage(NSE::TRIMMED::FO::ORDER_MODIFICATION_CONFIRMATION* msg);
					void rejectMessage(long clOrderId, const std::string errorMsg = "Exchange not ready for transation");
					void sendToOMS(std::string);
					void sendRequestDisconnectOmsConn();
					// std::unordered_map<int64_t, std::pair<long, long>> _orderIdExchangeTimeMap;
					FIN::ThreadSafeMap<int64_t, std::pair<long, long>> _orderIdExchangeTimeMap;

					// std::unordered_map < int64_t, int32_t > _orderIdClOrderId;
					FIN::ThreadSafeMap<int64_t, int32_t> _orderIdClOrderId;

					//	std::unordered_map < int64_t, int64_t > _orderIdExLastActivityRefTimeMap;
					FIN::ThreadSafeMap<int64_t, int64_t> _orderIdExLastActivityRefTimeMap;

					//make order history
					boost::lockfree::spsc_queue<uint32_t, boost::lockfree::capacity<2000>> orderHistory;

					//int64_t S64(const char* s)
					//{
					//	int64_t i;
					//	char c;
					//	int scanned = sscanf(s, "%" SCNd64 "%c", &i, &c);
					//	if (scanned == 1)
					//		return i;
					//	if (scanned > 1)
					//	{
					//		return i;
					//	}
					//	return 0;
					//}

					//int nseTime(int year, int mon, int day)
					//{
					//	struct tm t = { 0 };
					//	t.tm_year = 100 + year;
					//	t.tm_mon = mon - 1;
					//	t.tm_mday = day;
					//	return mktime(&t) - 315513000 + 19800 + 32400;
					//}

					//int nseTime(const std::string& yearMon, const std::string& day)
					//{
					//	int y = atoi(yearMon.c_str());
					//	int d = atoi(day.c_str());
					//	int m = y % 100;

					//	// FYI, this Logic will fail after 82 years.
					//	// Those who will be around, Do change :D :D
					//	y /= 100;
					//	y %= 100;

					//	return nseTime(y, m, d);
					//}

					std::string currentTimeReadable()
					{
						return timeNowReadable(time(NULL) + 19800);
					}

					std::string timeNowReadable(uint64_t timeStamp)
					{
						std::time_t time = timeStamp;
						auto gmtime = std::gmtime(&time);
						char buffer[32];
						strftime(buffer, 32, "%Y%m%d-%T", gmtime);
						std::string t1(buffer);
						// std::cout << "Time: " << t1 << std::endl;
						return t1;
					}

					int32_t floatToInt32Multiplied(double dVal, double multiplier)
					{
						return dVal >= 0 ? (dVal + 0.0000000001f) * multiplier
							: (dVal - 0.0000000001f) * multiplier;
					}
					int i;
					std::string panKey = "SESSION.PAN";
					std::string defPAN = "PAN_EXEMPT";

					std::string _panValue;
					std::string _custodianId;

					std::string _defPAN;

					// This object can hold mappings for the variuos accounts.
					struct Config_Mapping
					{
						Config_Mapping(const std::string& cust, const std::string& pan)
						{
							custodianId = cust;
							panNumber = pan;
						}
						std::string custodianId;
						std::string panNumber;
					};

					std::unordered_map<std::string, Config_Mapping> _accountConfigMapping;
					std::unordered_map<std::string, Config_Mapping>::iterator _accountConfigMappingItr;

					void readAccountConfigMapping()
					{
						std::string line;
						std::ifstream myfile((_currPath.getCurrentPath() + "NSEFO_ORDER_CLIENT_MAPPING.txt"));
						if (myfile.is_open())
						{
							while (getline(myfile, line))
							{
								std::string account = line.substr(0, line.find(":"));
								line = line.substr(line.find(":") + 1);
								std::string pan = line.substr(0, line.find(":"));
								line = line.substr(line.find(":") + 1);
								_accountConfigMapping.insert(std::make_pair(account, Config_Mapping(line, pan)));
							}
							myfile.close();
							printClinetMapping();
						}
					}

					void printClinetMapping()
					{
						_accountConfigMappingItr = _accountConfigMapping.begin();
						while (_accountConfigMappingItr != _accountConfigMapping.end())
						{
							LOG_INFO
								<< " PAN No(" << _accountConfigMappingItr->first << ") - "
								<< _accountConfigMappingItr->second.panNumber << " || "
								<< " CustId(" << _accountConfigMappingItr->first << ") - "
								<< _accountConfigMappingItr->second.custodianId;

							_accountConfigMappingItr++;
						}

						LOG_INFO << " PAN No(<Unknown>) - " << _defPAN << " || "
							<< "CustId(<Unknown>) - " << _custodianId;
						LOG_INFO << " PAN No(OWN) - " << _defPAN << " || "
							<< "CustId(OWN) - No custodian/settlor is set for OWN order";
					}

					std::string getPanNumber(const std::string& account)
					{
						_accountConfigMappingItr = _accountConfigMapping.find(account);
						if (_accountConfigMappingItr != _accountConfigMapping.end())
						{
							return _accountConfigMappingItr->second.panNumber;
						}
						else
						{
							return _defPAN;
						}
					}

					std::string getCustodianId(const std::string& account)
					{
						_accountConfigMappingItr = _accountConfigMapping.find(account);
						if (_accountConfigMappingItr != _accountConfigMapping.end())
						{
							return _accountConfigMappingItr->second.custodianId;
						}
						else
						{
							return _custodianId;
						}
					}
				};
			}
		}
	}
}

#endif
