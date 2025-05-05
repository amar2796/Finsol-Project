#include "rabbitmqProducer.h"

namespace FIN
{

	bool RabbitMqProducer::RabbitMqConnection(std::string fileName)
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
			// if (!first)
			// {
			// 	first = true;
			// 	static std::thread tOmsWrite(&RabbitMqProducer::processQueue, this);
			// }
		}

		return _rp->checkConnection();
	}
	
	void RabbitMqProducer::readConfig()
	{
			ConfigReader _configReader;
			_configReader.loadData(_fileName);

			std::string keyRabbitIp = "RABBIT.RABBIT_IP";
			std::string keyRabbitPort = "RABBIT.RABBIT_PORT";
			std::string traderId = "SESSION.TRADERID";

			// std::string keyChannel_id = "RABBIT_PRODUCER.CHANNEL_ID";
			// std::string keyExchangeName = "RABBIT_PRODUCER.EXCHANGE_NAME";
			// std::string keyExchangeType = "RABBIT_PRODUCER.EXCHANGE_TYPE";
			// std::string keyRoutingKey = "RABBIT_PRODUCER.ROUTING_KEY";
			// std::string keyQueueName = "RABBIT_PRODUCER.QUEUE_NAME";
			// std::string keyUsername = "RABBIT_PRODUCER.USERNAME";
			// std::string keyPassword = "RABBIT_PRODUCER.PASSWORD";

			_rabbitPort = _configReader.getValue<int>(keyRabbitPort, 0);
			_rabbitIp = _configReader.getValue<std::string>(keyRabbitIp, std::string(""));
			_channel_id = 1; // _configReader.getValue<int>        (keyChannel_id   , 0)   ;
			_traderId = _configReader.getValue<std::string>(traderId, std::string(""));

			_exchangeName = "ReceiverEx" + _traderId + "8";    // _configReader.getValue<std::string>(keyExchangeName, std::string(""));
			_exchangeType = "direct";                          //_configReader.getValue<std::string>(keyExchangeType, std::string(""));
			_routingKey = "receiver" + _traderId + "8";        // _configReader.getValue<std::string>(keyRoutingKey, std::string(""));
			_queueName = "OMSReceiverQueue" + _traderId + "8"; //_configReader.getValue<std::string>(keyQueueName, std::string(""));
			_username = "finsoltech";                          // _configReader.getValue<std::string>(keyUsername, std::string(""));
			_password = "pass123!";                            //_configReader.getValue<std::string>(keyPassword, std::string(""));
	
	}

	// int RabbitMqProducer::processQueue()
	// {

	// 	std::string _message;
	// 	while (true)
	// 	{
	// 		if (_isConnected)
	// 		{
	// 			_message = (_exchResponseQ.top());
	// 			props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;
	// 			props.content_type = amqp_cstring_bytes(_message.c_str());
	// 			props.delivery_mode = 2;
	// 			amqp_basic_publish(_rp->getConnectionState(), _channel_id, amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), 1, 0, &props, amqp_cstring_bytes(_message.c_str()));

	// 			LOG_INFO << "[R_OUT] " << _message;
	// 			_exchResponseQ.pop();
	// 		}
	// 		else
	// 		{
	// 			sleep(1);
	// 		}
	// 	}
	// 	// LOG_INFO << "[OUT]: " << _message;
	// 	// props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;

	// 	// props.content_type = amqp_cstring_bytes(_message.c_str());

	// 	// props.delivery_mode = 2;
	// 	// return amqp_basic_publish(_rp->getConnectionState(), _channel_id, amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), 1, 0, &props, amqp_cstring_bytes(_message.c_str()));
	// 	return 0;
	// }
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
