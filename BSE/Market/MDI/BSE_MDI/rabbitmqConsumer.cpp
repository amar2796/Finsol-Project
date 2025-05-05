#include "rabbitmqConsumer.h"
#include <Windows.h>
#include "Nanolog.hpp"

namespace FIN {

	bool RabbitMqConsumer::connect(std::string ip, int port)
	{
		_rabbitIp = ip;
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
			amqp_maybe_release_buffers_on_channel(_rc->getConnectionState(), 1);
			amqp_basic_consume(_rc->getConnectionState(), 1, amqp_cstring_bytes(_queueName.c_str()),
				amqp_empty_bytes, 0, 1, 0, amqp_empty_table);

			amqp_rpc_reply_t rec;
			amqp_envelope_t envelope;

			bool temp = true;

			while (true)
			{
				rec = amqp_consume_message(_rc->getConnectionState(), &(envelope), NULL, 0);
				if (rec.reply_type == AMQP_RESPONSE_NORMAL)
				{
					std::stringstream ss;
					ss.write((char*)(envelope.message.body.bytes), envelope.message.body.len);
					
					//std::cout << "consume msg to rabbitmq : " << ss.str() << std::endl;
					LOG_INFO << "[INFO ]Request from Oms " << ss.str();
					try 
					{
						if (!std::getline(ss, token, '|') || token.empty()) 
							throw std::runtime_error("Invalid format: Missing or empty boolean part.");
						
						if (token != "0" && token != "1") 
							throw std::runtime_error("Invalid format: First part should be '0' or '1'.");
						
						flag = std::stoi(token) != 0;  // Convert to bool (true for 1, false for 0)

						if (!std::getline(ss, token, '|') || token.empty()) 
							throw std::runtime_error("Invalid format: Missing or empty security_id.");
						
						//security_id = token;
						security_id = token;

						if (!std::getline(ss, token, '|') || token.empty())
							throw std::runtime_error("Invalid format: Missing or empty symbol.");

						symbol = token;

						std::string ack_string = processSymbolUpdateFromOMS(flag, security_id, symbol);
					}
					catch (const std::exception& e) 
					{
						LOG_INFO << e.what();
						std::cerr << "Error: " << e.what() << std::endl;
					}
				}
				else
				{
					std::cout << "Error in consume " << rec.reply_type << " & " << rec.library_error << std::endl;
					LOG_INFO << "Error in consume " << rec.reply_type << " & " << rec.library_error;
					Sleep(100);
					break;
				}
			}
			std::cout << "Error in consumer. Reconnect the RabbitMQ consumer " << std::endl;
			LOG_INFO << "Error in consumer. Reconnect the RabbitMQ consumer ";

			connect(_rabbitIp, _rabbitPort);
		}
		catch (std::exception& ex)
		{
			std::cout << ex.what();
			LOG_INFO << ex.what();
		}
		catch (...)
		{
			LOG_INFO << "RABBIT CONSUMER UNHANDLED EXCEPTION";

		}
	}

	bool RabbitMqConsumer::connectForTokenRead(std::string ip, int port)
	{
		_rabbitIp = ip;
		_rabbitPort = port;
		intitializeParameterForToken();

		_rc1 = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName, _exchangeType, _routingKey,
			_channel_id, _queueName, _username, _password);

		if (!_rc1->checkConnection())
		{

			_rc1->connectRM(0);
			rabbitMqConsumerForToken();

		}

		return _rc1->checkConnection();

	}

	//this cosumer for read token
	void RabbitMqConsumer::rabbitMqConsumerForToken()
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

			bool tokenCompletedReceived = false;

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
					std::cout << "Consumed message from RabbitMQ: " << message << std::endl;

					if (message == "TokenCompleted")
					{
						std::cout << "Received TokenCompleted message. Stopping consumer." << std::endl;
						tokenCompletedReceived = true; 

						// Acknowledge the message after processing
						amqp_basic_ack(_rc1->getConnectionState(), 1, envelope.delivery_tag, 0);

						// Cleanup the envelope to avoid memory leaks
						amqp_destroy_envelope(&envelope);
						break;
					}

					storeTokenInVector(message);

					// Acknowledge the message after processing
					amqp_basic_ack(_rc1->getConnectionState(), 1, envelope.delivery_tag, 0);

					// Cleanup the envelope to avoid memory leaks
					amqp_destroy_envelope(&envelope);
				}
				else if (rec.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION && rec.library_error == AMQP_STATUS_TIMEOUT)
				{
					std::cout << "No messages in the queue. Retrying in 10 seconds..." << std::endl;
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
			std::cout << "Exception caught: " << ex.what() << std::endl;
			LOG_INFO << "Exception caught: " << ex.what();
		}
		catch (...)
		{
			LOG_INFO << "Unhandled exception in RabbitMQ consumer.";
			std::cout << "Unhandled exception in RabbitMQ consumer." << std::endl;
		}

	}

	std::string RabbitMqConsumer::processSymbolUpdateFromOMS(bool , std::string, std::string)
	{

		std::cout << "RabbitMqConsumers function called\n";
		return "";
	}

	void RabbitMqConsumer::storeTokenInVector(std::string token)
	{

	}
}
