#include "rabbitmqProducer.h"

namespace FIN {
	bool RabbitMqProducer::RabbitMqConnection(const std::string& fileName,
		const std::string& senderCompId,
		const std::string& exchangeNumber)
	{
		//initilize variable
		_fileName = fileName;
		_exchangeNumber = exchangeNumber;
		_senderCompId = senderCompId;
		readConfig();
		_rp = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName,
			_exchangeType, _routingKey, _channel_id, _queueName, _username, _password);

		FIN_DEVLOG("Rabbit Prodecer");
		if (!_rp->checkConnection())
		{
			_rp->connectRM();
		}
		return _rp->checkConnection();
	}

	int RabbitMqProducer::sendForOMS(std::string& _message)
	{
		//std::cout << "rabbit" << std::endl;
		//string (null-terminated string)
		amqp_bytes_t bytes = amqp_cstring_bytes(_message.c_str());
		amqp_basic_properties_t properties;
		properties._flags = 0;
		properties._flags = AMQP_BASIC_DELIVERY_MODE_FLAG;
		properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
		return amqp_basic_publish(
			_rp->getConnectionState(),
			_channel_id,
			amqp_cstring_bytes(_exchangeName.c_str()),
			amqp_cstring_bytes(_routingKey.c_str()), 1, 0, &properties, bytes);
	}
}