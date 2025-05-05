#ifndef FIN_CONNECTION_STATUS
#define FIN_CONNECTION_STATUS

#include <iostream>
#include "configReader.h"

namespace FIN {


	enum class ConnectionStatus
	{
		Con_INVALID     ,

		Con_CONNECTING  ,
		Con_CONNECTED   ,
		Con_DOWNLOADING ,
        Con_LOGGED_IN   ,
        Con_LOGGED_OUT  ,
		Con_DISCONNECTED,
		Con_DISABLED
	};

	static const std::string strConnectionStatus_INVALID     = "ConnectionStatus_INVALID     " ;  
	static const std::string strConnectionStatus_CONNECTING  = "ConnectionStatus_CONNECTING  " ;  
	static const std::string strConnectionStatus_DOWNLOADING = "ConnectionStatus_DOWNLOADING " ;  
	static const std::string strConnectionStatus_LOGGED_IN   = "ConnectionStatus_LOGGED_IN   " ;  
	static const std::string strConnectionStatus_LOGGED_OUT  = "ConnectionStatus_LOGGED_OUT  " ;  
	static const std::string strConnectionStatus_CONNECTED   = "ConnectionStatus_CONNECTED   " ;  
	static const std::string strConnectionStatus_DISCONNECTED= "ConnectionStatus_DISCONNECTED" ; 
	static const std::string strConnectionStatus_DISABLED    = "ConnectionStatus_DISABLED    " ; 

	std::string getName ( ConnectionStatus cs ) ; 
	bool isConnected( ConnectionStatus cs )  ;
	
    class ConnectionParam
	{
		public:

			template < typename T >
				T getParam( const std::string &key, const T& defaultValue  )
				{
					return getValue ( key, defaultValue ) ;
				}		

			std::string getParamsString()
			{
				std::string str = _market + "| " + _connectHost + ":" + std::to_string( _connectPort ) ;
				return str ;
			}
	
			std::string getConnectHost() { return _connectHost ; }
			int getConnectPort() { return _connectPort ; }

            ConnectionStatus getConnectionStatus() const
            {
                return _connectionStatus ;
            }

            void setConnectionStatus( const ConnectionStatus conStat )
            {
                _connectionStatus = conStat ;
            }

		private:

			std::string _market     ;
			std::string _password   ;
			std::string _connectHost;
			int _traderid  ;
			int _connectPort; 

			ConfigReader _confReader ;
			ConnectionStatus _connectionStatus ;	
	};
}

#endif
