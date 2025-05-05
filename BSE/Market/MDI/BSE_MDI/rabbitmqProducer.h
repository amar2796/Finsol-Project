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
		int publishAck(std::string);				
		bool connect(std::string ip,int port);

	private:
		FIN::RbInitializer* _rp;
		void intitializeParameter()
		{


			_channel_id = 1;
			_exchangeName = "LiveSubscriptionRequests";
			_exchangeType = "topic";
			_routingKey = "BSEED";
			_queueName = "";
			_username = "finsoltech";
			_password = "pass123!";
			_ackRoutingKey = "livesubscriptionack";



			/*	_channel_id = 1;
				_exchangeName = "BroadcastSenderEx1";
				_exchangeType = "topic";
				_routingKey = "broadcast.master.12585857856278169924";
				_queueName = "";
				_username = "finsoltech";
				_password = "pass123!";
				_ackRoutingKey = "broadcast.master.12585857856278169924";*/

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
