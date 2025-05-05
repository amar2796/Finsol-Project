#include "rabbitmqConsumer.h"


namespace FIN {

	bool RabbitMqConsumer::connect(std::string fileName, std::string rabbitIp, int rabbitPort, int exchNum, int traderId)
	{
		this->_rabbitIp = rabbitIp;
		this->_rabbitPort = rabbitPort;
		this->_traderId = std::to_string(traderId);
		this->_exchNum = std::to_string(exchNum);
		_fileName = fileName;
		readConfig();
		_rc = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);
		if (!_rc->checkConnection())
		{
			_rc->connectRM();
		}

		return _rc->checkConnection();

	}

	void RabbitMqConsumer::readConfig()
	{
		_channel_id = 1;
		_exchangeName = "SenderEx" + _traderId + _exchNum;
		_exchangeType = "direct";
		_routingKey = "sender" + _traderId + _exchNum;
		_queueName = "OMSSenderQueue" + _traderId + _exchNum;
		_username = "finsoltech";
		_password = "pass123!";
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

			rec = amqp_consume_message(_rc->getConnectionState(), &(envelope), NULL, 0);
			if (rec.reply_type == AMQP_RESPONSE_NORMAL)
			{
				std::string str((char*)(envelope.message.body.bytes), envelope.message.body.len);
				//std::cout << "[IN]:  " <<  str << std::endl ;
				if (str != H)	LOG_INFO << "[R_IN] " << str;
				receiveFromOMS(str);
			}
		}
	}

	void RabbitMqConsumer::receiveFromOMS(const std::string& str)
	{
		std::cout << "Please give the defination\n";
	}

	void RabbitMqConsumer::sendRabbitHeartbeat()
	{
		const char* H = "H";
		props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
		props.content_type = amqp_cstring_bytes(H);
		props.delivery_mode = 2;
		amqp_basic_publish(_rc->getConnectionState(), _channel_id, amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), 1, 0, &props, amqp_cstring_bytes(H));

	}
}
