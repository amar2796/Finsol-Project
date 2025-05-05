#include "rabbitmqConsumer.h"


namespace FIN {

	bool RabbitMqConsumer::connect(std::string fileName)
	{
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
		ConfigReader _configReader;
		_configReader.loadData(_fileName);

		std::string keyRabbitIp = "RABBIT.RABBIT_IP";
		std::string keyRabbitPort = "RABBIT.RABBIT_PORT";
		std::string traderId = "SESSION.TRADERID";

		// std::string keyChannel_id = "RABBIT_CONSUMER.CHANNEL_ID";
		// std::string keyExchangeName = "RABBIT_CONSUMER.EXCHANGE_NAME";
		// std::string keyExchangeType = "RABBIT_CONSUMER.EXCHANGE_TYPE";
		// std::string keyRoutingKey = "RABBIT_CONSUMER.ROUTING_KEY";
		// std::string keyQueueName = "RABBIT_CONSUMER.QUEUE_NAME";
		// std::string keyUsername = "RABBIT_CONSUMER.USERNAME";
		// std::string keyPassword = "RABBIT_CONSUMER.PASSWORD";

		_rabbitPort = _configReader.getValue<int>(keyRabbitPort, 0);
		_rabbitIp = _configReader.getValue<std::string>(keyRabbitIp, std::string(""));
		_channel_id = 1; // _configReader.getValue<int>        (keyChannel_id   , 0)   ;
		_traderId = _configReader.getValue<std::string>(traderId, std::string(""));

		_exchangeName = "SenderEx" + _traderId + "8";    // _configReader.getValue<std::string>(keyExchangeName, std::string(""));
		_exchangeType = "direct";                          //_configReader.getValue<std::string>(keyExchangeType, std::string(""));
		_routingKey = "sender" + _traderId + "8";        // _configReader.getValue<std::string>(keyRoutingKey, std::string(""));
		_queueName = "OMSSenderQueue" + _traderId + "8"; //_configReader.getValue<std::string>(keyQueueName, std::string(""));
		_username = "finsoltech";                          // _configReader.getValue<std::string>(keyUsername, std::string(""));
		_password = "pass123!";                            //_configReader.getValue<std::string>(keyPassword, std::string(""));

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
