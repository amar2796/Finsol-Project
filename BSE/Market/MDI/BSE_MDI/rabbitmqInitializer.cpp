#include "rabbitmqInitializer.h"

namespace FIN
{
	void RbInitializer::connectRM(bool isProducer)
	{
		_connection_state = amqp_new_connection();
		socket = amqp_tcp_socket_new(_connection_state);

		amqp_socket_open(socket, _rabbitIp.c_str(), _rabbitPort);

		amqp_rpc_reply_t login_check = amqp_login(_connection_state, "/", 0, AMQP_DEFAULT_FRAME_SIZE,
			AMQP_DEFAULT_HEARTBEAT, AMQP_SASL_METHOD_PLAIN, _username.c_str(), _password.c_str());

		if (login_check.reply_type == AMQP_RESPONSE_NORMAL)
		{
			std::cout << "Login Done";
			std::cout << (isProducer ? " Producer " : " Consumer ");
			channel_opening(isProducer);
		}
		else if (login_check.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
		{
			std::cout << "\tLogin failed\n" << "\tLibrary Error : "
				<< amqp_error_string2(login_check.library_error);
		}
		else if (login_check.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION)
		{
			std::cout << "\tLogin failed server error ";
		}
	}

	void RbInitializer::channel_opening(bool isProducer)
	{
		if (amqp_channel_open(_connection_state, _channel_id))
		{
			if (isProducer)
				exchange_declaration(1);
			else
			{
				queue_declaration(0);
			}
		}
		else
		{
			std::cout << "\tChannel creation is failed";
		}
	}

	void RbInitializer::queue_declaration(bool isProducer)
	{
		if (amqp_queue_declare(_connection_state, _channel_id, amqp_cstring_bytes(_queueName.c_str()), 0, 0, 0, 1, amqp_empty_table))
		{
			exchange_declaration(isProducer);
		}
		else
		{
			std::cout << "\tQueue has been not declared in rabbitmq ";
		}
	}

	void RbInitializer::exchange_declaration(bool isProducer)
	{
		try
		{
			amqp_exchange_declare
			(_connection_state, _channel_id,
				amqp_cstring_bytes(_exchangeName.c_str()),
				amqp_cstring_bytes(_exchangeType.c_str()),
				0, 0,
				0, 0,
				amqp_empty_table
			);
			if (!isProducer)
				queue_binding();
		}
		catch (...)
		{
			std::cout << "\tError in Exchange declaration ";
		}
	}

	void RbInitializer::queue_binding()
	{
		if (amqp_queue_bind(_connection_state, _channel_id, amqp_cstring_bytes(_queueName.c_str()), amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), amqp_empty_table))
		{
			_isConnected = true;
			std::cout << "\tQueue binding done " << std::endl;
		}
		else
		{
			std::cout << "\tError in Queue Binding ";
		}
	}

	int RbInitializer::check_queue_count()
	{
		// Declare the queue and get its properties
		amqp_queue_declare_ok_t* result = amqp_queue_declare(_connection_state, _channel_id,
			amqp_cstring_bytes(_queueName.c_str()), 0, 0, 0, 0, amqp_empty_table);

		if (result)
		{
			return result->message_count; // Assuming message_count is a valid field
		}
		else
		{
			std::cout << "Failed to declare queue or retrieve message count." << std::endl;
			return 0;
		}
	}

	bool RbInitializer::checkConnection()
	{
		return this->_isConnected;
	}

	amqp_connection_state_t RbInitializer::getConnectionState()
	{
		return this->_connection_state;
	}

	RbInitializer::RbInitializer(const int _rabbitPort, const std::string& _rabbitIp, const std::string& _exchangeName,
		const std::string& _exchangeType, const std::string& _routingKey, const int _channel_id,
		const std::string& _queueName, const std::string& _username, const std::string& _password)
	{
		this->_rabbitPort = _rabbitPort;
		this->_rabbitIp = _rabbitIp.c_str();
		this->_exchangeName = _exchangeName.c_str();
		this->_exchangeType = _exchangeType.c_str();
		this->_routingKey = _routingKey.c_str();
		this->_channel_id = _channel_id;
		this->_queueName = _queueName.c_str();
		this->_password = _password;
		this->_username = _username;
	}
}
