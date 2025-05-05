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
		void setConsoleColor(int color);
		bool connect(std::string ip,int port);
		int publishAck(std::string);				

		bool connectToSendMsgTBT(std::string ip, int port);
		int sendToTBT(std::string ack_string);

	private:
		FIN::RbInitializer* _rp;
		void intitializeParameter()
		{


			_channel_id = 1;
			_exchangeName = "LiveSubscriptionRequests";
			_exchangeType = "topic";
			_routingKey = "MCX";
			_queueName = "";
			_username = "finsoltech";
			_password = "pass123!";
			_ackRoutingKey = "livesubscriptionack";
		}
		FIN::RbInitializer* _rp1;
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
		//		MDFEmapiClient* empiClient;
		std::string _ackRoutingKey;
	};

}
#endif
