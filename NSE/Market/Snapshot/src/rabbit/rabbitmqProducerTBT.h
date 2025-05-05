#ifndef FIN_RABBITMQ_PRODUCER_TBT
#define FIN_RABBITMQ_PRODUCER_TBT

#include "rabbitmqInitializerTBT.h"


namespace FIN 
{


class RabbitMqProducerTBT
	{

	public:

		RabbitMqProducerTBT();

		void setConsoleColor(int color);
		bool connectToSendMsgTBT(std::string ip, int port, std::string);
		int sendToTBT(std::string ack_string);

	private:
		FIN::RbInitializerTBT* _rp1;
		void intitializeParameterTBT();

		int _rabbitPort;
		int _channel_id;
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
