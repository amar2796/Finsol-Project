#include "RabbitProducer.h"

namespace FIN {


	bool  RabbitMqProducer::RabbitMqConnection(SessionSettings* rmq_details)
	{
		try
		{
			_rabbitPort = rmq_details->rabbitMqPort;
			_rabbitIp = rmq_details->rabbitMqIP;
			_senderCompId = std::to_string(rmq_details->SessionId);
			_exchangeNumber = rmq_details->ExchangeNumber;



			_channel_id = 1;
			_exchangeName = "ReceiverEx" + _senderCompId + _exchangeNumber;
			_exchangeType = "direct";
			_routingKey = "receiverEx" + _senderCompId + _exchangeNumber;
			_queueName = "OMSReceiverQueue" + _senderCompId + _exchangeNumber;
			_username = "finsoltech";
			_password = "pass123!";


			_rp = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName,
				_exchangeType, _routingKey, _channel_id, _queueName, _username, _password);

			FIN_DEVLOG("Rabbit Producer ");
			if (!_rp->checkConnection())
			{
				_rp->connectRM();
			}

			return _rp->checkConnection();
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
		return false;
	}

	int RabbitMqProducer::sendForOMS(const char* _message)
	{
		try

		{
			amqp_bytes_t bytes = amqp_cstring_bytes(_message);
			amqp_basic_properties_t properties;
			properties._flags = 0;
			properties._flags = AMQP_BASIC_DELIVERY_MODE_FLAG;
			properties.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
			//std::cout <<"[INFO] " << "Sent: " << _message << std::endl;
			auto result = amqp_basic_publish(
				_rp->getConnectionState(),
				_channel_id,
				amqp_cstring_bytes(_exchangeName.c_str()),
				amqp_cstring_bytes(_routingKey.c_str()), 1, 0, &properties, bytes);

			LOG_INFO << "[INFO] " << "[TO OMS] " + std::string(_message);
			return result;
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
		return 0;
	}


}
