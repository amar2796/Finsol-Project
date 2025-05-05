#include "RabbitConsumer.h"


namespace FIN {

	bool RabbitMqConsumer::rabbit_connect(SessionSettings* rmq_details)
	{
		
		_rabbitPort = rmq_details->rabbitMqPort;
		_rabbitIp = rmq_details->rabbitMqIP;
		_senderCompId = std::to_string(rmq_details->SessionId);
		_exchangeNumber = rmq_details->ExchangeNumber;
		
		_channel_id = 1;
		_exchangeName = "SenderEx" + _senderCompId + _exchangeNumber;
		_exchangeType = "direct";
		_routingKey = "senderEx" + _senderCompId + _exchangeNumber;
		_queueName = "OMSSenderQueue" + _senderCompId + _exchangeNumber;
		_username = "finsoltech";
		_password = "pass123!";


		_rc = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		FIN_DEVLOG("Rabbit Consumer ");
		if (!_rc->checkConnection())
		{
			_rc->connectRM();
		}

		return _rc->checkConnection();

	}

	void RabbitMqConsumer::rabbitMqConsumer()
	{
		//while(1) sleep(1);
		amqp_maybe_release_buffers_on_channel(_rc->getConnectionState(), 1);
		amqp_basic_consume(_rc->getConnectionState(), 1, amqp_cstring_bytes(_queueName.c_str()),
			amqp_empty_bytes, 0, 1, 0, amqp_empty_table);

		amqp_rpc_reply_t rec;
		amqp_envelope_t envelope;
		
		while (true)
		{
			//std::cout <<"[INFO] " << "Waiting to consume\n";
			rec = amqp_consume_message(_rc->getConnectionState(), &(envelope), NULL, 0);
			//std::cout <<"[INFO] " << "Message consumed\n";
			if (rec.reply_type == AMQP_RESPONSE_NORMAL)
			{
				std::string str((char*)(envelope.message.body.bytes), envelope.message.body.len);
				//LOG(INFO) << "[Rabbit_RCVD]" << str;
				receiveFromOMS(str);
			}
			else
			{
				FIN_ERROR("Error in consume");
				Sleep(1000);
			}
		}
	}

	void RabbitMqConsumer::receiveFromOMS(const std::string& str)
	{
		FIN_WARN("Please give the defination");
	}
}
