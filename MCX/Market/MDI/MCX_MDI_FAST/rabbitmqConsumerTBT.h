#ifndef RABBITMQ_CONSUMER_TBT
#define RABBITMQ_CONSUMER_TBT

#include "rabbitmqInitializerTBT.h"
#include <string>
#include <sstream>
#include <vector>
#include <thread>
namespace FIN {


	class RabbitMqConsumerTBT
	{
	public:

		RabbitMqConsumerTBT() = default;

	public:
		void rabbitMqConsumerForToken();
		virtual void storeTokenInVector(std::string token);
		void setConsoleColor(int color);
		bool connectForTokenRead(std::string ip, int port, std::string);
		bool tokenCompletedReceived = false;
	private:

		FIN::RbInitializerTBT* _rc1;
		std::string token;
		bool flag;
		std::string security_id;
		std::string symbol;

		void intitializeParameterForToken()
		{
			_channel_id = 1;
			_exchangeName = "TBT_TokenList";
			_exchangeType = "direct";
			_routingKey = "MCX";
			_queueName = "MCX_Token";
			_username = "finsoltech";
			_password = "pass123!";
			_ackRoutingKey = "livesubscriptionack";

			/*_channel_id = 1;
			_exchangeName = "tbt_update";
			_exchangeType = "topic";
			_routingKey = "security.mcx";
			_queueName = "tbt_securitylist";
			_username = "finsoltech";
			_password = "pass123!";
			_ackRoutingKey = "livesubscriptionack";*/
			
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