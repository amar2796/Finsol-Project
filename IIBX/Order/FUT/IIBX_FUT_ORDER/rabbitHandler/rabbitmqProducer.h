#ifndef FIN_RABBITMQ_PRODUCER
#define FIN_RABBITMQ_PRODUCER

#include "rabbitmqInitializer.h"
#include "../configReader.h"


namespace FIN {


    class RabbitMqProducer
    {

    public:
        RabbitMqProducer(){}
        //RabbitMqProducer(const std::string& fileName)        

        bool  connect(std::string);
        bool sendToOMS(const std::string&);
        bool sendToOMS(char* msg, size_t len);

    private:


        std::string _fileName;
        //void readConfig( const std::string &fileName )
        void readConfig()
        {
            ConfigReader _configReader;
            _configReader.loadData(_fileName);

            std::string  keyRabbitPort = "RMQ_CONNECTION.rabbitMqPort";
            std::string  keyRabbitIp = "RMQ_CONNECTION.rabbitMqIP";
            std::string  keyExchnageNumber = "OMS_COMM.ExchangeNumber";
            std::string  keyMemberID = "SESSION.ClearingMemberCode";
            //std::string  keyMemberID = "SESSION.TraderId";

           /* std::string  keyRabbitPort = "RABBIT_PRODUCER.RABBIT_PORT";
            std::string  keyChannel_id = "RABBIT_PRODUCER.CHANNEL_ID";
            std::string  keyRabbitIp = "RABBIT_PRODUCER.RABBIT_IP";
            std::string  keyExchangeName = "RABBIT_PRODUCER.EXCHANGE_NAME";
            std::string  keyExchangeType = "RABBIT_PRODUCER.EXCHANGE_TYPE";
            std::string  keyRoutingKey = "RABBIT_PRODUCER.ROUTING_KEY";
            std::string  keyQueueName = "RABBIT_PRODUCER.QUEUE_NAME";
            std::string  keyUsername = "RABBIT_PRODUCER.USERNAME";
            std::string  keyPassword = "RABBIT_PRODUCER.PASSWORD";*/

            _rabbitPort = _configReader.getValue<int>(keyRabbitPort, 0);
            _rabbitIp = _configReader.getValue<std::string>(keyRabbitIp, std::string(""));
            _channel_id = 1;// _configReader.getValue<int>(keyChannel_id, 0);
            _memberId = _configReader.getValue<std::string>(keyMemberID, std::string(""));
            _exchange_number = _configReader.getValue<int>(keyExchnageNumber, 0);
            _exchangeName = "ReceiverEx" + _memberId + std::to_string(_exchange_number);
            _exchangeType = "direct";
            _routingKey = "receiver" + _memberId + std::to_string(_exchange_number);
            _queueName = "OMSReceiverQueue" + _memberId + std::to_string(_exchange_number);
            _username = "finsoltech";
            _password = "pass123!";
        }

        amqp_basic_properties_t props;
        FIN::RbInitializer* _rp;

        //Reading from file instead of direct declaration.

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
        std::string _memberId;



    };

}
#endif
