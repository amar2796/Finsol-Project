#ifndef RABBITMQ_CONSUMER
#define RABBITMQ_CONSUMER

#include "rabbitmqInitializer.h"
#include "configReader.h"
#include "constants.h"

namespace FIN {


    class RabbitMqConsumer
    {

    public:

        RabbitMqConsumer()//(const std::string& fileName)        
        {
            //_fileName = fileName;
        }

        void rabbitMqConsumer();
        bool connect(std::string);
        virtual void receiveFromOMS(const std::string&);

    private:

        FIN::RbInitializer* _rc;

        std::string _fileName;

        void readConfig()
        {
            ConfigReader _configReader;
            _configReader.loadData(_fileName);

            std::string  keyRabbitPort = "RMQ_CONNECTION.rabbitMqPort";
            std::string  keyRabbitIp = "RMQ_CONNECTION.rabbitMqIP";
            std::string  keyExchnageNumber = "RMQ_CONNECTION.ExchangeNumber";
            std::string  keyUserId = "SESSION.UserId";
            //std::string  keyChannel_id = "RABBIT_CONSUMER.CHANNEL_ID";
            //std::string  keyExchangeName = "RABBIT_CONSUMER.EXCHANGE_NAME";
            //std::string  keyExchangeType = "RABBIT_CONSUMER.EXCHANGE_TYPE";
            //std::string  keyRoutingKey = "RABBIT_CONSUMER.ROUTING_KEY";
            //std::string  keyQueueName = "RABBIT_CONSUMER.QUEUE_NAME";
            //std::string  keyUsername = "RABBIT_CONSUMER.USERNAME";
            //std::string  keyPassword = "RABBIT_CONSUMER.PASSWORD";

            _rabbitPort = _configReader.getValue<int>(keyRabbitPort, 0);
            _rabbitIp = _configReader.getValue<std::string>(keyRabbitIp, std::string(""));
            _channel_id = 1;// _configReader.getValue<int>(keyChannel_id, 0);
            _userid = _configReader.getValue<std::string>(keyUserId, std::string(""));
            _exchange_number = _configReader.getValue<int>(keyExchnageNumber, 0);
            _exchangeName = "SenderEx" + _userid + std::to_string(_exchange_number);
            _exchangeType = "direct";
            _routingKey   = "sender"+ _userid + std::to_string(_exchange_number);
            _queueName    = "OMSSenderQueue"+ _userid + std::to_string(_exchange_number);
            _username     = "finsoltech";
            _password     = "pass123!";
        }

        int _rabbitPort;
        int _channel_id;
        int _exchange_number;
        std::string  _rabbitIp;
        std::string  _exchangeName;
        std::string  _exchangeType;
        std::string  _routingKey;
        std::string  _queueName;
        std::string _username;
        std::string _password;
        std::string _userid;
    };


}
#endif
