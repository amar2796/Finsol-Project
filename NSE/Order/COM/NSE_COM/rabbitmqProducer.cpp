#include "rabbitmqProducer.h"

namespace FIN
{

	bool RabbitMqProducer::RabbitMqConnection(std::string fileName, std::string rabbitIp, int rabbitPort, int exchNum, int traderId)
	{
		this->_rabbitIp = rabbitIp;
		this->_rabbitPort = rabbitPort;
		this->_traderId = std::to_string(traderId);
		this->_exchNum = std::to_string(exchNum);
		_fileName = fileName;
		readConfig();
		_rp = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName,
			_exchangeType, _routingKey, _channel_id, _queueName, _username, _password);
		if (!_rp->checkConnection())
		{
			_rp->connectRM();
		}

		return _rp->checkConnection();
	}
	
	void RabbitMqProducer::readConfig()
	{
		_channel_id = 1;
		_exchangeName = "ReceiverEx" + _traderId + _exchNum;
		_exchangeType = "direct";
		_routingKey = "receiver" + _traderId + _exchNum;
		_queueName = "OMSReceiverQueue" + _traderId + _exchNum;
		_username = "finsoltech";
		_password = "pass123!";
	}

	void RabbitMqProducer::sendForOMS(std::string& _message)
	{
		props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
		props.content_type = amqp_cstring_bytes(_message.c_str());
		props.delivery_mode = 2;
		amqp_basic_publish(_rp->getConnectionState(), _channel_id, amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), 1, 0, &props, amqp_cstring_bytes(_message.c_str()));

		LOG_INFO << "[R_OUT] " << _message;
	}
	void RabbitMqProducer::isConnected(bool val)
	{
		_isConnected = val;
	}

}
