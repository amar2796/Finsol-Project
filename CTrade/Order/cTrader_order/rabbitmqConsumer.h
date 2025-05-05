#ifndef RABBITMQ_CONSUMER
#define RABBITMQ_CONSUMER

#include "rabbitmqInitializer.h"
#include "configReader.h"


namespace FIN{


	class RabbitMqConsumer
	{

		public:

            RabbitMqConsumer()
            {
            }

			void rabbitMqConsumer() ;
            bool connect(const std::string &fileName, 
				const std::string &senderCompId, 
				const std::string &exchangeNumber
			);
			virtual void receiveFromOMS( const std::string& ) ;

		private:

			FIN::RbInitializer *_rc ; 

            std::string _fileName ;
			std::string _senderCompId;
			std::string _exchangeNumber;

            void readConfig()
            {
				//create object
                ConfigReader _configReader ;
				//?
                _configReader.loadData( _fileName ) ;

                std::string  keyRabbitPort  = "RABBIT.RABBIT_PORT"   ;
                std::string  keyRabbitIp    = "RABBIT.RABBIT_IP"     ;

                _rabbitPort     = _configReader.getValue<int>        (keyRabbitPort   , 0)   ;
				_rabbitIp       = _configReader.getValue<std::string>(keyRabbitIp     , std::string(""))   ;
				_channel_id = 1;
                _exchangeName   = "SenderEx" + _senderCompId + _exchangeNumber;
                _exchangeType   = "direct"   ;
				//queue address (_routingkey)
                _routingKey     = "senderEx"	   + _senderCompId + _exchangeNumber;
                _queueName      = "OMSSenderQueue" + _senderCompId + _exchangeNumber;
                _username       = "finsoltech" ;
                _password       = "pass123!";
            }

			int _rabbitPort             ;
            int _channel_id             ;
			std::string  _rabbitIp      ;
			std::string  _exchangeName  ;
			std::string  _exchangeType  ;
			std::string  _routingKey    ;
			std::string  _queueName     ;
			std::string _username       ;
			std::string _password       ;
	};


}
#endif
