#include "rabbitmqConsumer.h"


namespace FIN {

	bool RabbitMqConsumer::connect(std::string fileName)
	{
		try
		{
			_fileName = fileName;
			readConfig();
			/*std::cout << "Consumer Rabbit Details\n";
			std::cout <<"_rabbitPort:"<< _rabbitPort << " rabbitIP:" << _rabbitIp <<" _exchangeName:" << _exchangeName << " _exchangeType:" << _exchangeType << " _routingKey:" << _routingKey <<
				" channel_id:"<<_channel_id << " queueName:" << _queueName <<" username:"<< _username <<" password:" <<_password << std::endl;*/
			_rc = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
				_channel_id, _queueName, _username, _password);
			if (!_rc->checkConnection())
			{
				_rc->connectRM();
			}
		}
		catch (std::exception& ex)
		{
			CONSOLE_LOG << ex.what() << std::endl;
			LOG_INFO << ex.what();
		}

		return _rc->checkConnection();

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

				rec = amqp_consume_message(_rc->getConnectionState(), &(envelope), NULL, 0);
				if (rec.reply_type == AMQP_RESPONSE_NORMAL)
				{
					std::string str((char*)(envelope.message.body.bytes), envelope.message.body.len);
					/*std::cout << "[IN]:  " <<  str << std::endl ;
					LOG_INFO << "[IN] " << str;*/
					receiveFromOMS(str);
				}
			}
		}

		catch (std::exception& ex)
		{
			CONSOLE_LOG << ex.what() << std::endl;
			LOG_INFO << ex.what();
		}
	}

	void RabbitMqConsumer::receiveFromOMS(const std::string& str)
	{
		std::cout << "Please give the defination\n";
	}
}
