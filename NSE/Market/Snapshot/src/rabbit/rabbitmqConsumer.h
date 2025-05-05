#ifndef RABBITMQ_CONSUMER
#define RABBITMQ_CONSUMER

#include "rabbitmqInitializer.h"
#include <vector>
//#include "../../../../../../DGCX/Market/EmapiDGCX/MDFEmapiClient.h"
#include "../logging/Nanolog.hpp"


namespace FIN {


	class RabbitMqConsumer
	{

	public:

		RabbitMqConsumer();

		void rabbitMqConsumer();
		std::vector<std::string> tokenize(std::string s, std::string del);
		virtual std::string processSymbolUpdateFromOMS(std::vector<std::string>&);
		bool connect(std::string ip, int port);
		void intitializeParameter();

	private:

		FIN::RbInitializer* _rc;
		int _rabbitPort;
		int _channel_id;
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

	class RabbitMqProducer
	{

	public:

		RabbitMqProducer();

		int publishAck(std::string);				
		bool connect(std::string ip,int port);
		void intitializeParameter();

	private:
		FIN::RbInitializer* _rp;

		int _rabbitPort;
		int _channel_id;
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
