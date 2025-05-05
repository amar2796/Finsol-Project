#ifndef RABBITMQ_CONSUMER
#define RABBITMQ_CONSUMER

#include "rabbitmqInitializer.h"
#include <string>
#include <sstream>
#include <vector>
#include <thread>
namespace FIN {


	class RabbitMqConsumer
	{

	public:

		RabbitMqConsumer()
		{
		}

		void rabbitMqConsumer();
		void rabbitMqConsumerForToken();
		bool publishAck(std::string);
		std::vector<std::string> tokenize(std::string s, std::string del);
		virtual std::string processSymbolUpdateFromOMS(bool, std::string, std::string);
		virtual void storeTokenInVector(std::string token);
		bool connect(std::string ip, int port);
		bool connectForTokenRead(std::string ip, int port);
		
	private:

		FIN::RbInitializer* _rc;
		FIN::RbInitializer* _rc1;
		std::string token;
		bool flag;
		std::string security_id;
		std::string symbol;

		void intitializeParameter()
		{
			_channel_id = 1;
			_exchangeName = "LiveSubscriptionRequests";
			_exchangeType = "topic";
			_routingKey = "BSEED";
			_queueName = "BSE_Snapshot_queue";
			_username = "finsoltech";
			_password = "pass123!";
			_ackRoutingKey = "livesubscriptionack";

		}
		void intitializeParameterForToken()
		{
			_channel_id = 1;
			_exchangeName = "tbt_update";
			_exchangeType = "topic";
			_routingKey = "security.mcx";
			_queueName = "tbt_securitylist";
			_username = "finsoltech";
			_password = "pass123!";
			_ackRoutingKey = "livesubscriptionack";

		}

		int _rabbitPort;
		int _channel_id;
		std::string security_exchangeName;
		std::string  _rabbitIp;
		std::string  _exchangeName;
		std::string  _exchangeType;
		std::string  _routingKey;
		std::string  _queueName;
		std::string _username;
		std::string _password;
		//		MDFEmapiClient* empiClient;
		std::string _ackRoutingKey;
	};
}

#endif