#include "rabbitmqConsumer.h"

namespace FIN {

	bool RabbitMqConsumer::connect(const std::string& fileName,
		const std::string& senderCompId,
		const std::string& exchangeNumber)
	{
		//initilize value
		_fileName = fileName;
		_exchangeNumber = exchangeNumber;
		_senderCompId = senderCompId;
		//call function
		readConfig();

		//create object RbInitializer class
		_rc = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		//print msg
		FIN_DEVLOG("Rabbit Consumer");

		//check connection
		if (!_rc->checkConnection())
		{
			//call connectRM function
			_rc->connectRM();
		}
		return _rc->checkConnection();
	}

	void RabbitMqConsumer::rabbitMqConsumer()
	{
		//std::cout << "rabbitMq function" << std::endl;
		//amqp_maybe_release_buffers_on_channel() will be freed, and use of that memory will caused undefined behavior.
		amqp_maybe_release_buffers_on_channel(_rc->getConnectionState(), 1);
		//This function should be used after starting a consumer with the amqp_basic_consume() function
		amqp_basic_consume(_rc->getConnectionState(), 1, amqp_cstring_bytes(_queueName.c_str()),
			amqp_empty_bytes, 0, 1, 0, amqp_empty_table);
		amqp_rpc_reply_t rec;
		amqp_envelope_t envelope;
		while (true) {
			//Wait for and consume a message.
			rec = amqp_consume_message(_rc->getConnectionState(), &(envelope), NULL, 0);
			if (rec.reply_type == AMQP_RESPONSE_NORMAL)
			{
				std::string str((char*)(envelope.message.body.bytes), envelope.message.body.len);
				receiveFromOMS(str);
				str = "";
			}
			else
			{
				FIN_ERROR("Error in consume");
			}
		}
	}

	void RabbitMqConsumer::receiveFromOMS(const std::string& str)
	{
		FIN_WARN("Please give the defination");
	}
}