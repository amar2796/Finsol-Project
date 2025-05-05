#ifndef RABBITMQ_CONSUMER
#define RABBITMQ_CONSUMER

#include "rabbitmqInitializer.h"
#include "windows.h"

namespace FIN {


	class RabbitMqConsumer
	{

	public:

		RabbitMqConsumer()
		{
		}

		void rabbitMqConsumer();
		bool rabbit_connect(SessionSettings *rmq_details);
		virtual void receiveFromOMS(const std::string&);

	private:

		FIN::RbInitializer* _rc;

		std::string _fileName;
		std::string _senderCompId;
		std::string _exchangeNumber;

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
