#include "RabbitMqProducerTBT.h"
#include "Nanolog.hpp"
namespace FIN 
{
	void RabbitMqProducerTBT::setConsoleColor(int color) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
	}

	//for read token tbt
	bool RabbitMqProducerTBT::connectToSendMsgTBT(std::string ip, int port, std::string msg)
	{
		_rabbitIp = ip;
		_rabbitPort = port;
		intitializeParameterTBT();

		_rp1 = new RbInitializerTBT(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		if (!_rp1->checkConnection())
		{
			setConsoleColor(2);
			_rp1->connectRM(msg);
			setConsoleColor(7);
		}
		LOG_INFO << msg <<" : ExchangeName -> " << _exchangeName << " queueName -> " << _queueName << " routingKey -> " << _routingKey;
		return _rp1->checkConnection();

	}
	int RabbitMqProducerTBT::sendToTBT(std::string ack_string)
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

