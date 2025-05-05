#include "rabbitmqConsumer.h"
#include <Windows.h>

namespace FIN {
	RabbitMqConsumer::RabbitMqConsumer()
	{
		_rabbitPort = 0;
		_channel_id = 0;
	}

	bool RabbitMqConsumer::connect(std::string ip, int port)//, MDFEmapiClient* empi)
	{
		//empiClient = empi;
		this->_rabbitIp = ip;
		_rabbitPort = port;
		intitializeParameter();

		_rc = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		if (!_rc->checkConnection())
		{

			_rc->connectRM(0);
			rabbitMqConsumer();

		}

		return _rc->checkConnection();

	}

	void RabbitMqConsumer::intitializeParameter()
	{
		_channel_id = 1;
		_exchangeName = "LiveSubscriptionRequests";
		_exchangeType = "topic";
		_routingKey = "NSECOM";
		_queueName = "Live_COM_Queue";
		_username = "finsoltech";
		_password = "pass123!";
		_ackRoutingKey = "livesubscriptionack";
	}

	std::vector<std::string> RabbitMqConsumer::tokenize(std::string s, std::string del)	
	{
		std::vector<std::string> result;
		int start = 0;
		int end = s.find(del);
		while (end != -1) {
			result.push_back(s.substr(start, end - start));
			start = end + del.size();
			end = s.find(del, start);
		}
		result.push_back(s.substr(start, end - start));
		return result;
	}
	
	void RabbitMqConsumer::rabbitMqConsumer()
	{
		try
		{
			//while(1) sleep(1);
			amqp_maybe_release_buffers_on_channel(_rc->getConnectionState(), 1);
			amqp_basic_consume(_rc->getConnectionState(), 1, amqp_cstring_bytes(_queueName.c_str()),
				amqp_empty_bytes, 0, 1, 0, amqp_empty_table);

			amqp_rpc_reply_t rec;
			amqp_envelope_t envelope;
			while (true)
			{


				//std::cout << "Waiting to consume\n";
				rec = amqp_consume_message(_rc->getConnectionState(), &(envelope), NULL, 0);
				
				if (rec.reply_type == AMQP_RESPONSE_NORMAL)
				{
					std::string str((char*)(envelope.message.body.bytes), envelope.message.body.len);
					LOG_INFO << "Instrument request " << str;
					//std::cout << "Instrument request " << str << std::endl;
					//ConfigParameters::getInstance()->loginfile(str);
					auto result = tokenize(str, "|");

					std::string ack_string = processSymbolUpdateFromOMS(result); //empiClient->mCallbackHandler->processInstumentRequest(str);
					LOG_INFO << "Symbol Update response to OMS : " << ack_string;
					//publishAck(ack_string);


				}
				else
				{
					std::cout << "Error in consume " << rec.reply_type << " & "<<rec.library_error << std::endl;
					//auto error = std::to_string(rec.reply_type) + " error in consumer";
					//ConfigParameters::getInstance()->loginfile(error);
					Sleep(1000);
				}
			}
		}
		catch (std::exception ex)
		{
			std::cout << ex.what();
		}
	}
	int RabbitMqProducer::publishAck(std::string ack_string)
	{
		try
		{
			LOG_INFO << "Symbol Ack Response:" << ack_string;
			//std::cout << "Symbol Ack Response:" << ack_string<<std::endl;
			//ConfigParameters::getInstance()->loginfile(ack_string);
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
			/*amqp_basic_properties_t props;
			props._flags = AMQP_BASIC_CONTENT_TYPE_FLAG | AMQP_BASIC_DELIVERY_MODE_FLAG;

			props.content_type = amqp_cstring_bytes(ack_string.c_str());

			props.delivery_mode = AMQP_DELIVERY_NONPERSISTENT;
			auto result = amqp_basic_publish(_rp->getConnectionState(), _channel_id,
				amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), 
				1, 0, &props, amqp_cstring_bytes(ack_string.c_str()));
			return result;*/
		}
		catch (std::exception ex)
		{
			std::cout << ex.what();
		}
	}

	std::string RabbitMqConsumer::processSymbolUpdateFromOMS(std::vector<std::string>& str) {
		std::cout << "RabbitMqConsumers function called\n";
		return "";
	}

	RabbitMqProducer::RabbitMqProducer()
	{
		_rabbitPort = 0;
		_channel_id = 0;
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

	void RabbitMqProducer::intitializeParameter()
	{
		_channel_id = 1;
		_exchangeName = "LiveSubscriptionRequests";
		_exchangeType = "topic";
		_routingKey = "NSECOM";
		_queueName = "";
		_username = "finsoltech";
		_password = "pass123!";
		_ackRoutingKey = "livesubscriptionack";
	}
}
