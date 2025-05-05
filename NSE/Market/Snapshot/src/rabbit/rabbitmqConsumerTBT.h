#ifndef RABBITMQ_CONSUMER_TBT
#define RABBITMQ_CONSUMER_TBT

#include "rabbitmqInitializerTBT.h"
#include <sstream>
#include "../logging/Nanolog.hpp"
namespace FIN {


	class RabbitMqConsumerTBT
	{
	public:

		RabbitMqConsumerTBT();

	public:
		void rabbitMqConsumerForToken();
		virtual void storeToken(std::string token);
		void setConsoleColor(int color);
		bool connectForTokenRead(std::string& ip, int port, std::string);
		void intitializeParameterForToken();

		FIN::RbInitializerTBT* _rc1;
		std::string token = "";
		std::string security_id = "";
		std::string symbol = "";
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
		std::string _ackRoutingKey;
	};
}

#endif