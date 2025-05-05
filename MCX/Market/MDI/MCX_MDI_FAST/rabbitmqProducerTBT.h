#ifndef FIN_RABBITMQ_PRODUCER_TBT
#define FIN_RABBITMQ_PRODUCER_TBT

#include "rabbitmqInitializerTBT.h"


namespace FIN {


class RabbitMqProducerTBT
	{

	public:

		RabbitMqProducerTBT()
		{
		}
		void setConsoleColor(int color);
		bool connectToSendMsgTBT(std::string ip, int port, std::string);
		int sendToTBT(std::string ack_string);

	private:
		FIN::RbInitializerTBT* _rp1;
		void intitializeParameterTBT()
		{


			_channel_id = 1;
			_exchangeName = "TBT_TokenList";
			_exchangeType = "direct";
			_routingKey = "MCX";
			_queueName = "MCX_Token_Req";
			_username = "finsoltech";
			_password = "pass123!";
			_ackRoutingKey = "token.request";

		}

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
