#include "rabbitmqInitializer.h"

namespace FIN {
	RbInitializer::RbInitializer(const int _rabbitPort, const std::string& _rabbitIp, const std::string& _exchangeName,
		const std::string& _exchangeType, const std::string& _routingKey, const int _channel_id,
		const std::string& _queueName, const std::string& _username, const std::string& _password)
	{
		//initilize variable
		this->_rabbitIp = _rabbitIp.c_str();
		this->_rabbitPort = _rabbitPort;
		this->_exchangeName = _exchangeName.c_str();
		this->_exchangeType = _exchangeType.c_str();
		this->_routingKey = _routingKey.c_str();
		this->_channel_id = _channel_id;
		this->_queueName = _queueName.c_str();
		this->_username = _username;
		this->_password = _password;
	}

	bool RbInitializer::checkConnection()
	{
		return this->_isConnected;
	}

	void RbInitializer::connectRM()
	{
		// Establish a connection to RabbitMQ server.
		_connection_state = amqp_new_connection();
		amqp_socket_t* socket = amqp_tcp_socket_new(_connection_state);

		if (!socket) {
			std::cerr << "Failed to create a new socket" << std::endl;
			return;
		}

		int status = amqp_socket_open(socket, _rabbitIp.c_str(), _rabbitPort);
		if (status < 0) {
			std::cerr << "Failed to open a socket to RabbitMQ" << std::endl;
			return;
		}

		// Perform authentication.
		amqp_rpc_reply_t login_check = amqp_login(
			_connection_state, "/", 0, AMQP_DEFAULT_FRAME_SIZE, AMQP_DEFAULT_HEARTBEAT,
			AMQP_SASL_METHOD_PLAIN, _username.c_str(), _password.c_str()
		);

		if (login_check.reply_type != AMQP_RESPONSE_NORMAL) {
			std::cerr << "Failed to log in to RabbitMQ" << std::endl;
			return;
		}

		std::cout << "Connected to RabbitMQ!" << std::endl;

		if (login_check.reply_type == AMQP_RESPONSE_NORMAL)
		{
			FIN_INFO("\tLogin Done");
			channel_opening();
		}
		else if (login_check.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
		{
			FIN_ERROR("\tLogin failed\n" << "\tLibrary Error: "
				<< amqp_error_string2(login_check.library_error));
		}
		else if (login_check.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
		{
			FIN_ERROR("\tLogin failed server error ");
		}
	}
	void RbInitializer::channel_opening()
	{
		//check condition
		if (amqp_channel_open(_connection_state, _channel_id))
		{
			//declare the queue
			queue_declaration();
		}
		else
		{
			FIN_ERROR("\tChannel creation is failed");
		}
	}
	amqp_connection_state_t RbInitializer::getConnectionState()
	{
		return this->_connection_state;
	}


	void RbInitializer::queue_binding()
	{
		if (amqp_queue_bind(_connection_state, _channel_id, amqp_cstring_bytes(_queueName.c_str()), amqp_cstring_bytes(_exchangeName.c_str()), amqp_cstring_bytes(_routingKey.c_str()), amqp_empty_table))
		{
			_isConnected = true;
			FIN_INFO("\tQueue binding done ");
		}
		else
		{
			FIN_ERROR("\tError in Queue Binding ");;
		}


	}
	void RbInitializer::exchange_declaration()
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
			queue_binding();

		}

		catch (...)
		{
			FIN_ERROR("\tError in Exchange declaration ");
		}

	}
	void  RbInitializer::queue_declaration()
	{



		if (amqp_queue_declare(_connection_state, _channel_id, amqp_cstring_bytes(_queueName.c_str()), 0, 0, 0, 0, amqp_empty_table))
		{

			exchange_declaration();

		}
		else
		{
			FIN_ERROR("\tQueue has been not declared in rabbitmq ");;
		}

	}
	
}