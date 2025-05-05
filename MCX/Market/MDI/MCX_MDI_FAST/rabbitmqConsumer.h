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
		std::vector<std::string> tokenize(std::string s, std::string del);
		virtual std::string processSymbolUpdateFromOMS(bool, std::string, std::string);
		void setConsoleColor(int color);
		bool connect(std::string ip, int port);
		
	private:

		FIN::RbInitializer* _rc;
		std::string token;
		bool flag;
		std::string security_id;
		std::string symbol;

		void intitializeParameter()
		{
			_channel_id = 1;
			_exchangeName = "LiveSubscriptionRequests";
			_exchangeType = "topic";
			_routingKey = "MCX";
			_queueName = "MCX_Snapshot_queue";
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
		std::string _ackRoutingKey;
	};
}

#endif