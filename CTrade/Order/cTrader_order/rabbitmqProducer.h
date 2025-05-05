#ifndef FIN_RABBITMQ_PRODUCER
#define FIN_RABBITMQ_PRODUCER

#include "rabbitmqInitializer.h"
#include "configReader.h"


namespace FIN {


	class RabbitMqProducer
	{

		public:

			RabbitMqProducer ()
			{				
			}
			bool  RabbitMqConnection (const std::string &fileName,
				const std::string &senderCompId,
				const std::string &exchangeNumber
			);
			int sendForOMS( std::string& ) ;
		
		private:


            std::string _fileName ;
			std::string _senderCompId;
			std::string _exchangeNumber;

            //void readConfig( const std::string &fileName )
            void readConfig()
            {
                ConfigReader _configReader ;
                _configReader.loadData( _fileName ) ;

                std::string  keyRabbitPort  = "RABBIT.RABBIT_PORT"   ;
                std::string  keyRabbitIp    = "RABBIT.RABBIT_IP"     ;
                
				_channel_id = 1;
                _rabbitPort     = _configReader.getValue<int>        (keyRabbitPort   , 0)   ;
                _rabbitIp       = _configReader.getValue<std::string>(keyRabbitIp     , std::string(""))   ;

                _exchangeName   = "ReceiverEx" + _senderCompId + _exchangeNumber   ;
                _exchangeType   = "direct"   ;
                _routingKey     = "receiverEx" + _senderCompId + _exchangeNumber;
                _queueName      = "OMSReceiverQueue" + _senderCompId + _exchangeNumber;
                _username       = "finsoltech"   ;
                _password       = "pass123!" ;
				
            }

			amqp_basic_properties_t props;
			FIN::RbInitializer *_rp ;
			
			//Reading from file instead of direct declaration.
			
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
