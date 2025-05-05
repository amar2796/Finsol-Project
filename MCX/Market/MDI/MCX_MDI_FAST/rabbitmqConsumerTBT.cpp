#include "rabbitmqConsumerTBT.h"
#include <Windows.h>
#include "Nanolog.hpp"

namespace FIN {

	void RabbitMqConsumerTBT::setConsoleColor(int color) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
	}

	bool RabbitMqConsumerTBT::connectForTokenRead(std::string ip, int port, std::string msg)
	{
		_rabbitIp = ip;
		_rabbitPort = port;
		intitializeParameterForToken();

		_rc1 = new RbInitializerTBT(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		if (!_rc1->checkConnection())
		{
			setConsoleColor(2);
			_rc1->connectRM(msg);
			setConsoleColor(7);
			rabbitMqConsumerForToken();

		}

		return _rc1->checkConnection();

	}

	//this cosumer for read token
	void RabbitMqConsumerTBT::rabbitMqConsumerForToken()
	{
		try
		{
			// Set QoS to only receive 1 unacknowledged message at a time
			amqp_basic_qos(_rc1->getConnectionState(), 1, 0, 1, 0); // Set prefetch count to 1

			// Start consuming messages with manual acknowledgment (no_ack = 0)
			amqp_basic_consume(_rc1->getConnectionState(), 1,
				amqp_cstring_bytes(_queueName.c_str()),
				amqp_empty_bytes, 0, 0, 0, amqp_empty_table); // no_ack = 0 (manual ack)

			amqp_envelope_t envelope;

			

			while (!tokenCompletedReceived)
			{
				// Attempt to consume a message with a timeout of 10 seconds
				struct timeval timeout;
				timeout.tv_sec = 10; // 10 second timeout
				timeout.tv_usec = 0;

				amqp_rpc_reply_t rec = amqp_consume_message(_rc1->getConnectionState(), &envelope, &timeout, 0);

				if (rec.reply_type == AMQP_RESPONSE_NORMAL)
				{
					std::stringstream ss;
					ss.write((char*)(envelope.message.body.bytes), envelope.message.body.len);
					std::string message = ss.str();

					if (message == "TokenCompleted")
					{
						setConsoleColor(6);
						std::cout << "Token Completed." << std::endl;
						setConsoleColor(7);
						tokenCompletedReceived = true; 

						// Acknowledge the message after processing
						amqp_basic_ack(_rc1->getConnectionState(), 1, envelope.delivery_tag, 0);

						// Cleanup the envelope to avoid memory leaks
						amqp_destroy_envelope(&envelope);
					
						break;
					}
					setConsoleColor(2);
					std::cout << "Token received from TBT : " << message << std::endl;
					LOG_INFO << "Token received from TBT : " << message;

					storeTokenInVector(message);

					// Acknowledge the message after processing
					amqp_basic_ack(_rc1->getConnectionState(), 1, envelope.delivery_tag, 0);

					// Cleanup the envelope to avoid memory leaks
					amqp_destroy_envelope(&envelope);
				}
				else if (rec.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION && rec.library_error == AMQP_STATUS_TIMEOUT)
				{
					std::cout << "Token list not found from TBT. Please wait..." << std::endl;
				}
				else
				{
					std::cout << "Error consuming message: " << rec.reply_type << " & " << rec.library_error << std::endl;
					LOG_INFO << "Error consuming message: " << rec.reply_type << " & " << rec.library_error;

					std::this_thread::sleep_for(std::chrono::milliseconds(100));

					if (rec.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
					{
						std::cout << "Severe error encountered. Stopping consumer." << std::endl;
						break; // Exit the loop on severe error
					}
				}
			}
		}
		catch (const std::exception& ex)
		{
			setConsoleColor(4);
			std::cout << "Exception caught: " << ex.what() << std::endl;
			setConsoleColor(7);
			LOG_INFO << "Exception caught: " << ex.what();
		}
		catch (...)
		{
			LOG_INFO << "Unhandled exception in RabbitMQ consumer.";
			setConsoleColor(4);
			std::cout << "Unhandled exception in RabbitMQ consumer." << std::endl;
			setConsoleColor(7);
		}

	}

	void RabbitMqConsumerTBT::storeTokenInVector(std::string token)
	{

	}
}
