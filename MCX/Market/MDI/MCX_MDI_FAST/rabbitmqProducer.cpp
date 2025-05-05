#include "rabbitmqProducer.h"
#include "Nanolog.hpp"
#include <Windows.h>
namespace FIN 
{
	void RabbitMqProducer::setConsoleColor(int color) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
	}

	//for live subscription token process
	bool RabbitMqProducer::connect(std::string ip, int port)
	{
		_rabbitIp = ip;
		_rabbitPort = port;
		intitializeParameter();

		_rp = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		if (!_rp->checkConnection())
		{
			_rp->connectRM(1);
		}
		LOG_INFO << "Producer for SNAPSHOT : ExchangeName -> " << _exchangeName << " queueName -> " << _queueName << " routingKey -> " << _routingKey;

		return _rp->checkConnection();

	}
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

	//for read token tbt
	bool RabbitMqProducer::connectToSendMsgTBT(std::string ip, int port)
	{
		_rabbitIp = ip;
		_rabbitPort = port;
		intitializeParameterTBT();

		_rp1 = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		if (!_rp1->checkConnection())
		{
			_rp1->connectRM(0);
		}
		LOG_INFO << "Producer : ExchangeName -> " << _exchangeName << " queueName -> " << _queueName << " routingKey -> " << _routingKey;
		return _rp1->checkConnection();

	}
	int RabbitMqProducer::sendToTBT(std::string ack_string)
	{
		try
		{
			amqp_bytes_t bytes = amqp_cstring_bytes(ack_string.c_str());
			amqp_basic_properties_t properties;
			properties._flags = 0;
			properties._flags = AMQP_BASIC_DELIVERY_MODE_FLAG;
			properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;

			auto result = amqp_basic_publish(
				_rp1->getConnectionState(),
				_channel_id,
				amqp_cstring_bytes(_exchangeName.c_str()),
				amqp_cstring_bytes(_ackRoutingKey.c_str()), 1, 0, &properties, bytes);

			LOG_INFO << "Instrument Ack to TBT:" << ack_string;
			return result;
		}
		catch (std::exception ex)
		{
			std::cout << ex.what();
		}
	}
}

