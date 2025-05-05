#ifndef FIN_RABBITMQ_PRODUCER
#define FIN_RABBITMQ_PRODUCER

#include "rabbitmqInitializer.h"



namespace FIN {


	class RabbitMqProducer
	{

	public:

		RabbitMqProducer()
		{
		}
		bool  RabbitMqConnection(SessionSettings *rmq);
		int sendForOMS(const char*);

	private:


		std::string _fileName;
		std::string _senderCompId;
		std::string _exchangeNumber;


		amqp_basic_properties_t props;
		FIN::RbInitializer* _rp;

		//Reading from file instead of direct declaration.

		int _rabbitPort;
		int _channel_id;
		std::string  _rabbitIp;
		std::string  _exchangeName;
		std::string  _exchangeType;
		std::string  _routingKey;
		std::string  _queueName;
		std::string _username;
		std::string _password;



	};

}
#endif
