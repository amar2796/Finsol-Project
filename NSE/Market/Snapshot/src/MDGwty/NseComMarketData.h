#ifndef FIN_NSE_MARKETDATA
#define FIN_NSE_MARKETDATA
#include <iostream>
#include <algorithm>  // For std::min and std::max
#include "message.h"
#include "../common/multicastRcvr.h"
#include "../common/marketDataMessage.h"
#include "../common/common.h"
#include "../common/configReader.h"
#include "publisher.h"
#include <unordered_set>
#include <unordered_map>
#include "../common/processMonitor.h"
#include <thread>
#include<string>
#include  <lzo/lzo1z.h>
#include  <lzo/lzoconf.h>
#include <iostream>
#include "transactCodes.h"
#include "SymbolProcess.h"
#include "../rabbit/rabbitmqConsumer.h"
#include "../rabbit/rabbitmqConsumerTBT.h"
#include "../rabbit/rabbitmqProducerTBT.h"
namespace FIN {
	namespace NSE {

		namespace COM {

			class NseComMd : public RabbitMqConsumer, RabbitMqConsumerTBT
			{
			public:
				NseComMd(std::string, int exchangeNum);

				void read_securityId_from_file(std::string fileName);

				void processMarketData(std::string ip, std::string interfaceip, int port);

				void process(char* data);
				void process_MB_Data(INDEX indexes);
				void process_MB_Data(InteractiverBuffer& indexes);
				void process_OpenInterest(ST_TICKER_INDEX_INFO& openInterest);
				void storeToken(std::string);
				std::string processSymbolUpdateFromOMS(std::vector<std::string>& str);
				std::unordered_map<std::string, bool> securityIDFromOMS; //security id, subscribed/unsubscribed
				bool handleSubscription(std::string& symbol);
				bool handleUnSubscription(std::string& symbol);
				void startRabbitListenToken(std::string rabbitIp, int rabbitPort);
				void startRabbitSubscriptionProcess(std::string rabbitIp, int rabbitPort);
				void IsDoublePriceFormat(std::string&);

			private:
				bool print = false;
				bool isSendBAB = false;
				lzo_uint dst_len = 3000;
				short ignoreBytes = 8;
				std::unordered_map<std::string, SymbolProcess*>m_mdatamap;
				SecurityFileHandler secHandler;
				std::mutex _mutex;
				RabbitMqProducer sendAckToOMS;
				RabbitMqProducerTBT listenToken;
				int exchangeNumber=0;
				bool isDouble = true;

				MulticastRcvr rcvr;

			};
		}
	}

}

#endif

