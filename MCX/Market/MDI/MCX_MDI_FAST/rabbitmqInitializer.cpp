#include "rabbitmqInitializer.h"

namespace FIN
{
	void RbInitializer::setConsoleColor(int color) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
	}

	void RbInitializer::connectRM(bool isProducer)
	{
		_connection_state = amqp_new_connection();
		socket = amqp_tcp_socket_new(_connection_state);

		amqp_socket_open(socket, _rabbitIp.c_str(), _rabbitPort);

		amqp_rpc_reply_t login_check = amqp_login(_connection_state, "/", 0, AMQP_DEFAULT_FRAME_SIZE,
			AMQP_DEFAULT_HEARTBEAT, AMQP_SASL_METHOD_PLAIN, _username.c_str(), _password.c_str());

		if (login_check.reply_type == AMQP_RESPONSE_NORMAL)
		{
			setConsoleColor(2);
			std::cout << (isProducer ? "Producer " : "Consumer ");
			std::cout << " for SNAPSHOT : Login Done";
			setConsoleColor(7);
			channel_opening(isProducer);
		}
		else if (login_check.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
		{
			setConsoleColor(4);
			std::cout << "\tLogin failed\n" << "\tLibrary Error : "
				<< amqp_error_string2(login_check.library_error);
			setConsoleColor(7);
		}
		else if (login_check.reply_type == AMQP_RESPONSE_SERVER_EXCEPTION)
		{
			setConsoleColor(4);
			std::cout << "\tLogin failed server error ";
			setConsoleColor(7);
		}
	}

	void RbInitializer::channel_opening(bool isProducer)
	{
		if (amqp_channel_open(_connection_state, _channel_id))
		{
			setConsoleColor(2);
			std::cout << "\tchannel open";
			setConsoleColor(7);
			
			exchange_declaration(isProducer);
		}
		else
		{
			setConsoleColor(4);
			std::cout << "\tChannel creation is failed";
			setConsoleColor(7);
		}
	}

	void RbInitializer::queue_declaration()
	{
		if (amqp_queue_declare(_connection_state, _channel_id, amqp_cstring_bytes(_queueName.c_str()), 0, 0, 0, 1, amqp_empty_table))
		{
			setConsoleColor(2);
			std::cout << "\tqueue declare";
			setConsoleColor(7);
			queue_binding();
		}
		else
		{
			setConsoleColor(4);
			std::cout << "\tQueue has been not declared in rabbitmq ";
			setConsoleColor(7);
		}
	}

	void RbInitializer::exchange_declaration(bool isProducer)
	{
		try
		{
			if (amqp_exchange_declare(_connection_state, _channel_id, amqp_cstring_bytes(_exchangeName.c_str()),
				amqp_cstring_bytes(_exchangeType.c_str()),
				0, 0,
				0, 0,
				amqp_empty_table)
				)
			{
				setConsoleColor(2);
				std::cout << "\texchange declare";
				setConsoleColor(7);
				if (!isProducer)
					queue_declaration();
				else
					std::cout << std::endl;
			}
			else
			{
				setConsoleColor(4);
				std::cout << "\tError in Exchange declaration ";
				setConsoleColor(7);
			}
			
		}
		catch (...)
		{
			setConsoleColor(4);
			std::cout << "\tError in Exchange declaration ";
			setConsoleColor(7);
		}
	}

	void RbInitializer::queue_binding()
	{
		if (amqp_queue_bind(_connection_state, _channel_id, amqp_cstring_bytes(_queueName.c_str()), 
							amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), amqp_empty_table))
		{
			_isConnected = true;
			setConsoleColor(2);
			std::cout << "\tQueue binding done " << std::endl;
			setConsoleColor(7);
		}
		else
		{
			setConsoleColor(4);
			std::cout << "\tError in Queue Binding ";
			setConsoleColor(7);
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
