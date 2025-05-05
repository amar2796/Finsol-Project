#ifndef FIN_INITIALIZE_PROCESS_DATA
#define FIN_INITIALIZE_PROCESS_DATA


#include "Snaphot_MDI.h"
#include "ConfigReader.h"
#include "CurrentPath.h"
#include "rabbitmqConsumer.h"
#include "rabbitmqProducer.h"
#include "rabbitmqConsumerTBT.h"
#include "rabbitmqProducerTBT.h"
#include <Windows.h>

namespace FIN
{

	class Initialize_process:public RabbitMqConsumer, RabbitMqConsumerTBT
	{
	public:
		Initialize_process();
		void setConsoleColor(int color);
		void init();
		virtual std::string processSymbolUpdateFromOMS(bool flag, std::string secrity_id, std::string symbol);
		virtual void storeTokenInVector(std::string token);
		int handlingSubscription(std::string surity_id, std::string symbol);
		int handlingUnSubscription(std::string surity_id, std::string symbol);
		bool isValidSymbol(const std::string symbol);
		bool isSecurityIDSubscribed(uint64_t security_id);
		bool isSecurityIDSubscribedInTBT(uint64_t security_id);
		void add_securityid_in_vector(uint64_t security_id);
		void delete_securityid_from_vector(uint64_t security_id);
		void add_securityid_in_file(std::string security_id);
		void delete_securityid_from_file(std::string security_id);
		std::vector<std::string> allowedExchange;

	private:
		Config_Details conf_Det;
		std::string read_emdi_tempate_file();
		void readTokenFromFile();
		std::vector<std::string> readTokenFromFile_forTbt();
		void readFiltteredSymbolFile();
		void startRabbitListenToken(std::string rabbit_ip, int rabbit_port);
		void startRabbitSubscriptionProcessing(std::string rabbit_ip, int rabbit_port);
		void start_processing();
		MDI_SNAPSHOT emdi_snap;
		RabbitMqProducer sendAckToOMS;
		RabbitMqProducerTBT listenToken;

	};
}

#endif
