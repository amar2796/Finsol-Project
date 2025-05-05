#ifndef FIN_RABBITMQ_PRODUCER
#define FIN_RABBITMQ_PRODUCER

#include "rabbitmqInitializer.h"
#include "configReader.h"
#include "threadSafeQueue.h"
namespace FIN
{

    class RabbitMqProducer
    {

    public:
        RabbitMqProducer()
        {
        }
        RabbitMqProducer(const std::string& fileName)
        {
            _fileName = fileName;
            readConfig();
            _rp = new RbInitializer(_rabbitPort, _rabbitIp, _exchangeName,
                _exchangeType, _routingKey, _channel_id, _queueName, _username, _password);
        }
        bool RabbitMqConnection(std::string fileName, std::string rabbitIp, int rabbitPort, int exchNum, int traderId);
        void isConnected(bool val);
        void sendForOMS(std::string&);
        ThreadSafeQueue<std::string> _exchResponseQ;
        bool _isConnected = false;
        bool first = false;
    private:
        int processQueue();
        std::string _fileName;
        // void readConfig( const std::string &fileName )
        void readConfig();

        amqp_basic_properties_t props;
        FIN::RbInitializer* _rp;

        // Reading from file instead of direct declaration.

        int _rabbitPort;
        int _channel_id;
        std::string _rabbitIp;
        std::string _exchangeName;
        std::string _exchangeType;
        std::string _routingKey;
        std::string _queueName;
        std::string _username;
        std::string _password;
        std::string _traderId;
        std::string _exchNum;
    };

}
#endif
