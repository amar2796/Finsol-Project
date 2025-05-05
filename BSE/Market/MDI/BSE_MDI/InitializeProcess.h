#ifndef FIN_INITIALIZE_PROCESS_DATA
#define FIN_INITIALIZE_PROCESS_DATA


#include "Snaphot_MDI.h"
#include "ConfigReader.h"
#include "CurrentPath.h"
#include "rabbitmqConsumer.h"
#include "rabbitmqProducer.h"
namespace FIN
{

	class Initialize_process : public RabbitMqConsumer
	{
	public:
		Initialize_process();
		void init();

		void startRabbitSubscriptionProcessing(std::string rabbit_ip, int rabbit_port);
		virtual std::string processSymbolUpdateFromOMS(bool flag, std::string security_id, std::string symbol);
		int handlingSubscription(std::string security_id, std::string symbol);
		int handlingUnSubscription(std::string security_id, std::string symbol);
		//bool isValidSymbol(const std::string symbol);
		bool isSecurityIDSubscribed(uint64_t security_id);
		void add_securityid_in_vector(uint64_t security_id);
		void delete_securityid_from_vector(uint64_t security_id);
		void add_securityid_in_file(std::string security_id);
		void delete_securityid_from_file(std::string security_id);

	private:
		Config_Details conf_Det;
		std::string read_emdi_tempate_file();
		void readTokenFromFile();
		void start_processing();
		MDI_SNAPSHOT emdi_snap;
		RabbitMqProducer sendAckToOMS;

	};
}

#endif
