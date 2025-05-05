#include "rabbitmqConsumer.h"
#include <Windows.h>
#include "Nanolog.hpp"

namespace FIN {

	void RabbitMqConsumer::setConsoleColor(int color) {
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, color);
	}

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
			LOG_INFO << "Consumer for SNAPSHOT : ExchangeName -> " << _exchangeName << " queueName -> " << _queueName << " routingKey -> " << _routingKey;
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

					//std::stringstream ss1;
					//ss1.write((char*)(envelope.message.body.bytes), envelope.message.body.len);
					//std::cout << ss1.str() << std::endl;
					//if (!std::getline(ss1, token, '|') || token.empty())
					//	throw std::runtime_error("Invalid format: Missing or empty boolean part.");
					//flag = std::stoi(token) != 0;  // Convert to bool (true for 1, false for 0)

					//std::string str;
					//if (temp)
					//{
					//	 str = std::to_string(flag) + "|123456|GOLD";
					//	temp = false;
					//}
					//else
					//{
					//	str = std::to_string(flag) + "|123456|GOLD";
					//	temp = true;
					//}
					//ss.write(str.c_str(), str.length());

					/*setConsoleColor(2);
					std::cout << "Token received form OMS : " << ss.str() << std::endl;
					setConsoleColor(7);*/
					LOG_INFO << "Token received form OMS " << ss.str();
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

						//if (!std::getline(ss, token, '|') || token.empty())
						//	throw std::runtime_error("Invalid format: Missing or empty symbol.");

						//symbol = token;

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
	std::string RabbitMqConsumer::processSymbolUpdateFromOMS(bool, std::string, std::string)
	{

		std::cout << "RabbitMqConsumers function called\n";
		return "";
	}
}
