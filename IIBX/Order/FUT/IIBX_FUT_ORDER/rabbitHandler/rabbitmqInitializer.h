#ifndef RABBITMQ_INTIALIZER
#define RABBITMQ_INTIALIZER

#include <iostream>
#include <amqp_tcp_socket.h>
#include <amqp.h>
#include <amqp_framing.h>
//#include "glog/logging.h"
//#include "Nanolog.hpp"

namespace FIN
{


	class RbInitializer
	{
	public:


		//RbInitializer( );

		RbInitializer(const int _rabbitPort, const std::string& _rabbitIp, const std::string& _exchangeName,
			const std::string& _exchangeType, const std::string& _routingKey, const int _channel_id,
			const std::string& _queueName, const std::string& _username, const std::string& _password);

		void connectRM();
		amqp_connection_state_t  getConnectionState();
		bool checkConnection();

	private:

		void channel_opening();
		void queue_declaration();
		void exchange_declaration();
		void queue_binding();


	private:

		amqp_socket_t* socket = NULL;
		amqp_connection_state_t   _connection_state;
		bool _isConnected = false;
		int _rabbitPort;
		int _channel_id;
		std::string  _rabbitIp;
		std::string  _exchangeName;
		std::string  _exchangeType;
		std::string  _routingKey;
		std::string  _queueName;
		std::string _password;
		std::string _username;


	};
}
#endif
