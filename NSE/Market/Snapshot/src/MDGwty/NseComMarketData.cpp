#include "NseComMarketData.h"


namespace FIN {
	namespace NSE {

		namespace COM {

			NseComMd::NseComMd(std::string fileName, int exgNum)
			{
				this->exchangeNumber = exgNum;
				FIN::ConfigReader confReader;
				FIN::MDGtwy::CurrentPath _currentPath;
				confReader.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
				this->isSendBAB = confReader.getValue <bool>(
					FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + "SINGLE_DEPTH", false);

			}

			void NseComMd::read_securityId_from_file(std::string fileName)
			{
				try
				{
					//SecurityFileHandler secHandler;					
					secHandler.init(fileName);
					std::unordered_set <std::string> symblSet = secHandler.getAllSubscribedSymbols();

					//Symbol mapping handling
					for (auto itr = symblSet.begin(); itr != symblSet.end(); itr++)
					{
						std::string instrument = *itr;

						std::string actual_id;
						std::string duplicate_id;
						size_t equal_index = instrument.find('=');

						if (equal_index != std::string::npos)
						{
							//MarketData md(9, *itr);
							actual_id = instrument.substr(0, equal_index);
							duplicate_id = instrument.substr(equal_index + 1, instrument.length());
							m_mdatamap.insert(std::make_pair(actual_id, new SymbolProcess(exchangeNumber, duplicate_id)));
							setConsoleColor(2);
							std::cout << "Successfully Subscribed Symbol : " << duplicate_id << std::endl;
							setConsoleColor(7);
							LOG_INFO << "Successfully Subscribed Symbol : " << duplicate_id;
						}
						else
						{
							MarketData md(exchangeNumber, instrument);
							m_mdatamap.insert(std::make_pair(instrument, new SymbolProcess(exchangeNumber, instrument)));
							setConsoleColor(2);
							std::cout << "Successfully Subscribed Symbol : " << instrument << std::endl;
							setConsoleColor(7);
							LOG_INFO << "Successfully Subscribed Symbol : " << instrument;
						}
					}
				}
				catch (std::exception& e)
				{
					std::cout << "Exception in Securty file : " << e.what();
				}
			}

			//process market
			void NseComMd::processMarketData(std::string ip, std::string interfaceip, int port)
			{
				//start receiving
				std::thread receive(&FIN::MulticastRcvr::init, &rcvr, ip, interfaceip, port);
				receive.detach();

				//start processing
				while (1)
				{
					std::unique_lock<std::mutex> lock(rcvr.cv_mutex);

					while (rcvr.queue.empty()) 
					{
						rcvr.consumer_waiting = true;
						rcvr.cv.wait(lock);
					}

					char*  packet = nullptr;
					rcvr.queue.pop(packet);
					
					lock.unlock();

					process(packet);
					delete[] packet;
				}
				
			}

			void NseComMd::process(char* source)
			{
				auto packetInfo = (PACKET_INFO*)source;
				//std::cout << "PacketInfo : " << packetInfo->getiNoPackets() << std::endl;
				auto com_data_info = (COMPRESSED_PACKET_INFO*)packetInfo->cPackData;
				//std::cout << "getComLen : " << com_data_info->getcomLen() << std::endl;
				if (com_data_info->getcomLen() > 0)
				{
					int position = ignoreBytes;
					char destination[1500];
					int error = lzo1z_decompress((lzo_bytep)com_data_info->broadcastData, (lzo_uint)com_data_info->getcomLen(),
						(lzo_bytep)destination, (lzo_uintp)&dst_len, 0);
					if (error == 0)
					{
						while (position < dst_len)
						{
							int recordCount = 0;
							auto bc_header = (BROADCAST_HEADER*)(destination + position);
							position += sizeof(BROADCAST_HEADER);
							//std::cout << "bc_header->getMessageLength() " << bc_header->getMessageLength() <<" dst_len : "<< dst_len;
							switch (bc_header->getTransCode())
							{
							case TransCode::TransCode_MBP:
							{
								auto _mbp = (MBP*)(destination + position);
								
								for (size_t i = 0; i < _mbp->getNoOfRecords(); i++)
								{
									auto interactive = _mbp->getinteractive_buffer(i);
									process_MB_Data(interactive);
								}

								position += (bc_header->getMessageLength() - sizeof(BROADCAST_HEADER));
							}
							break;
							case TransCode::TransCode_Ticker_Trade:
							{
								//LOG_INFO << " PARSE THIS  " << bc_header->getTransCode();
								auto _ticker_data = (MS_TICKER_TRADE_DATA*)(destination + position);
								for (size_t i = 0; i < _ticker_data->getNoOfRecords(); i++)
								{
									auto ticker_index = _ticker_data->getst_ticker_index_info(i);
									process_OpenInterest(ticker_index);
								}

								position += (bc_header->getMessageLength() - sizeof(BROADCAST_HEADER));
							}
							break;
							case TransCode::TransCode_MarketWatch:
							{
								//auto _mbp = (MarketWatch*)(destination + position);
								//for (size_t i = 0; i < _mbp->getNoOfRecords(); i++)
								//{
								//	auto m = _mbp->getstk_market_watch(i);
								//	//std::cout << m.getToken() << " " << m.getOpenInterest() << std::endl;
								//}
								position += (bc_header->getMessageLength() - sizeof(BROADCAST_HEADER));

							}
							break;
							case TransCode::TransCode_BCAST_MBO_MBP_UPDATE:
							{
								//std::cout << " PARSE THIS  " << bc_header->getTransCode() << std::endl;

								position += (bc_header->getMessageLength() - sizeof(BROADCAST_HEADER));
							}
							break;
							case TransCode::TransCode_BCAST_TRADE_EXECUTION_RANGE:
							{
								//	std::cout << " PARSE THIS  " << bc_header->getTransCode() << std::endl;

								position += (bc_header->getMessageLength() - sizeof(BROADCAST_HEADER));
							}
							break;
							case TransCode::TransCode_BCAST_BUY_BACK:
							{
								//std::cout << " PARSE THIS  " << bc_header->getTransCode() << " " << bc_header->getMessageLength() << " " << sizeof(BROADCAST_HEADER) << " " << dst_len  << " "<< position << std::endl;
								position += (bc_header->getMessageLength() - sizeof(BROADCAST_HEADER));
							}
							break;
							case TransCode::TransCode_MS_ASSET_DATA:
							{
								position += (bc_header->getMessageLength() - sizeof(BROADCAST_HEADER));
							}
							break;

							default:
								LOG_INFO << " NEED TO PARSE THIS  " << bc_header->getTransCode();
								std::cout << " NEED TO PARSE THIS  " << bc_header->getTransCode() << std::endl;
								break;
							}
						}

					}
					else
					{
						std::cout << " ERROR IN DECOMPRESSION  " << error << std::endl;
						LOG_INFO << "ERROR IN DECOMRESSION" << error;
					}
				}
			}
			void NseComMd::process_MB_Data(InteractiverBuffer& data)
			{
				std::string token = std::to_string(data.getToken());
				auto itr = m_mdatamap.find(token);
				if (itr != m_mdatamap.end())
				{
					if (!isDouble)
					{
						itr->second->apply_marketDetph(&data, isSendBAB);
						itr->second->apply_ohlc(&data);
						itr->second->isProcessedFirstTime = true;
					}
					else
					{
						itr->second->apply_marketDetph_D(&data, isSendBAB);
						itr->second->apply_ohlc_D(&data);
						itr->second->isProcessedFirstTime = true;
					}
				}
			}

			void NseComMd::process_OpenInterest(ST_TICKER_INDEX_INFO& openInterest)
			{
				try
				{
					auto itr = m_mdatamap.find(std::to_string(openInterest.getToken()));

					if (itr != m_mdatamap.end())
					{
						//std::cout << openInterest.getToken() << " open interest " << openInterest.getopenInterest() << " " << openInterest.getDailyHiOI() << " " << openInterest.getDailyLoOI() << std::endl;
						itr->second->applyOpenInterest(openInterest.getopenInterest(), isSendBAB, isDouble);
						itr->second->isProcessedFirstTime = true;
					}

				}
				catch (std::exception& ex)
				{
					LOG_INFO << ex.what();
				}
			}
			void NseComMd::process_MB_Data(INDEX indexes)
			{
				std::string  value(indexes.getIndexName());
				auto pos = value.find_last_not_of(' ');
				value = (pos == std::string::npos) ? "" : value.substr(0, pos + 1);
				auto itr = m_mdatamap.find(value);
				if (itr != m_mdatamap.end())
				{
					itr->second->apply_indexValue(indexes, isDouble);
					//	itr->second->apply_ohlc(mbp);

				}
			}

			void NseComMd::startRabbitListenToken(std::string rabbit_ip, int rabbit_port)
			{
				listenToken.connectToSendMsgTBT(rabbit_ip, rabbit_port, "Producer");
				listenToken.sendToTBT("Token Request");
				std::cout << "Request send sucessfully to listen Symbol from TBT" << std::endl;
				connectForTokenRead(rabbit_ip, rabbit_port, "Consumer");
			}

			void NseComMd::storeToken(std::string token)
			{
				secHandler.tokenFromTBT(token);
			}

			void NseComMd::startRabbitSubscriptionProcess(std::string rabbitIp, int rabbitPort)
			{
				sendAckToOMS.connect(rabbitIp, rabbitPort);
				connect(rabbitIp, rabbitPort);
			}

			std::string NseComMd::processSymbolUpdateFromOMS(std::vector<std::string>& splited) {
				std::string result;
				result = "0|" + splited[1];
				if (splited[0] == "1")
				{//subscribe					
					//securityIDFromOMS[splited[1]] = true;
					bool isSubscribe = handleSubscription(splited[1]);
					if (isSubscribe)
						result = "1|" + splited[1];
				}
				else if (splited[0] == "0") {
					//unsubscribe					
					//securityIDFromOMS[splited[1]] = false;
					bool isUnSubscribe = handleUnSubscription(splited[1]);
					if (isUnSubscribe)
						result = "0|" + splited[1];
				}
				sendAckToOMS.publishAck(result);
				return result;
			}
			bool NseComMd::handleSubscription(std::string& symbol)
			{
				std::lock_guard < std::mutex > lock(_mutex);
				//add symbol in map
				if (!secHandler.findTokenInTBT(symbol))
				{
					if (m_mdatamap.find(symbol) == m_mdatamap.end())
					{
						m_mdatamap.insert(std::make_pair(symbol, new SymbolProcess(exchangeNumber, symbol)));
						setConsoleColor(2);
						std::cout << "Successfully Subscribed Symbol : " << symbol << std::endl;
						setConsoleColor(7);
						LOG_INFO << "Successfully Subscribed Symbol : " << symbol;
						//add symbol in file
						return secHandler.addSymbolForSubscription(symbol);
					}
					else {
						setConsoleColor(12);
						std::cout << symbol << " : is Already subscribed\n";
						setConsoleColor(7);
						LOG_INFO << symbol << " : is Already subscribed";
					}
				}
				else
				{
					setConsoleColor(12);
					std::cout << symbol << " : is Already subscribed in TBT" << std::endl;
					setConsoleColor(7);
					LOG_INFO << symbol << " : is Already subscribed in TBT";
				}
				return false;
			}
			bool NseComMd::handleUnSubscription(std::string& symbol)
			{
				std::lock_guard < std::mutex > lock(_mutex);
				if (m_mdatamap.find(symbol) == m_mdatamap.end())
				{
					std::cout << symbol << " : is not present to unscubscribe" << std::endl;
					LOG_INFO << symbol << " : is not present to unscubscribe";
				}
				else 
				{
					m_mdatamap.erase(symbol);
					setConsoleColor(12);
					std::cout << "Successfully Unsubscribed Symbol : " << symbol << std::endl;
					setConsoleColor(7);
					LOG_INFO << "Successfully Unsubscribed Symbol : " << symbol;
					return secHandler.symbolForUnSubscription(symbol);
				}
				return false;
			}

			void NseComMd::IsDoublePriceFormat(std::string& format)
			{
				if (format.find(".") != std::string::npos)
				{
					isDouble = true;
					std::cout << "Will send decimal price \n";
					LOG_INFO << "Will send decmal price";
				}
				else
				{
					std::cout << "Will send integer price \n";
					LOG_INFO << "Will send integer price";
				}
			}

		}

	}
}