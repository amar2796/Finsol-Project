#ifndef RABBITMQ_INTIALIZER
#define RABBITMQ_INTIALIZER

#include <iostream>
#include <amqp_tcp_socket.h>
#include <boost/log/trivial.hpp>
namespace FIN
{
	class RbInitializer
	{
	public:


		//RbInitializer( );

		RbInitializer(const int _rabbitPort, const std::string& _rabbitIp, const std::string& _exchangeName,
			const std::string& _exchangeType, const std::string& _routingKey, const int _channel_id,
			const std::string& _queueName, const std::string& _username, const std::string& _password);

		void connectRM(bool isProducer);
		amqp_connection_state_t  getConnectionState();
		bool checkConnection();

		/*
		struct timeval currentTime;
		long getMicrotime()
		{
			gettimeofday(&currentTime, NULL);
			return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
		}
		*/
		int check_queue_count();
	private:

		void channel_opening(bool isProducer);
		void queue_declaration(bool isProducer);
		void exchange_declaration(bool isProducer);
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
