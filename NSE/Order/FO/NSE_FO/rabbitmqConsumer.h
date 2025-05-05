#ifndef RABBITMQ_CONSUMER
#define RABBITMQ_CONSUMER

#include "rabbitmqInitializer.h"
#include "configReader.h"

namespace FIN
{

	class RabbitMqConsumer
	{

	public:
		std::string _fileName;
		RabbitMqConsumer()
		{
		}
		RabbitMqConsumer(const std::string& fileName)
		{
			_fileName = fileName;
		}

		void rabbitMqConsumer();
		bool connect(std::string);
		virtual void receiveFromOMS(const std::string&);

		void sendRabbitHeartbeat();
		std::string H = "H";
		bool isConsumerDone = false;

	private:
		FIN::RbInitializer* _rc;
		amqp_basic_properties_t props;


		void readConfig();


		int _rabbitPort;
		int _channel_id;
		std::string _rabbitIp;
		std::string _exchangeName;
		std::string _exchangeType;
		std::string _routingKey;
		std::string _queueName;
		std::string _username;
		std::string _password;
		std::string _traderId;
	};

}
#endif
