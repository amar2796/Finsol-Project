#include "rabbitmqProducer.h"

namespace FIN {


	bool  RabbitMqProducer::connect(std::string fileName)
	{
		_fileName = fileName;
		readConfig();
		/*std::cout << "Producer Rabbit Details\n";
		std::cout << "_rabbitPort:" << _rabbitPort << " rabbitIP:" << _rabbitIp << " _exchangeName:" << _exchangeName << " _exchangeType:" << _exchangeType << " _routingKey:" << _routingKey <<
			" channel_id:" << _channel_id << " queueName:" << _queueName << " username:" << _username << " password:" << _password << std::endl;*/
		_rp = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName,
			_exchangeType, _routingKey, _channel_id, _queueName, _username, _password);

		if (!_rp->checkConnection())
		{
			_rp->connectRM();
		}

		return _rp->checkConnection();
	}

	//int RabbitMqProducer::sendForOMS(std::string& _message)
	bool RabbitMqProducer::sendToOMS(char* _message, size_t len, short orderStaus, uint64_t orderId)
	{
		int result = 0;
		try
		{
			amqp_bytes_t msg;
			msg.bytes = _message;
			msg.len = len;
			//LOG_INFO << "[OUT]: " << _message;
			props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;

			props.content_type = amqp_cstring_bytes(_message);

			props.delivery_mode = 2;
			 result = amqp_basic_publish(_rp->getConnectionState(), _channel_id, amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), 1, 0, &props, msg);

			LOG_INFO << "[TO_OMS] :" << orderStaus << " " << orderId;
		}
		catch(std::exception &ex)
		{
			LOG_INFO << ex.what();
		}
		return result;
	}


}
