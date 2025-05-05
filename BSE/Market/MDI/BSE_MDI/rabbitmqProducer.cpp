#include "rabbitmqProducer.h"
#include "Nanolog.hpp"
namespace FIN {
	int RabbitMqProducer::publishAck(std::string ack_string)
	{
		try
		{

			LOG_INFO << "Instrument Ack to Oms:" << ack_string;
			amqp_bytes_t bytes = amqp_cstring_bytes(ack_string.c_str());
			amqp_basic_properties_t properties;
			properties._flags = 0;
			properties._flags = AMQP_BASIC_DELIVERY_MODE_FLAG;
			properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;

			auto result = amqp_basic_publish(
				_rp->getConnectionState(),
				_channel_id,
				amqp_cstring_bytes(_exchangeName.c_str()),
				amqp_cstring_bytes(_ackRoutingKey.c_str()), 1, 0, &properties, bytes);
			return result;
		}
		catch (std::exception ex)
		{
			std::cout << ex.what();
		}
	}
	bool RabbitMqProducer::connect(std::string ip, int port)//, MDFEmapiClient* empi)
	{
		//empiClient = empi;
		_rabbitIp = ip;
		_rabbitPort = port;
		intitializeParameter();

		_rp = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		if (!_rp->checkConnection())
		{
			_rp->connectRM(1);
		}

		return _rp->checkConnection();

	}
}

