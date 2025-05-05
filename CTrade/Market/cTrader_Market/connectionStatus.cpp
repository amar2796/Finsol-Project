#include "connectionStatus.h"

namespace FIN {

	std::string getName ( ConnectionStatus cs ) 
	{
		switch ( cs ) 
		{
			case ConnectionStatus::Con_INVALID      : return strConnectionStatus_INVALID       ; 
			case ConnectionStatus::Con_CONNECTING   : return strConnectionStatus_CONNECTING    ; 
			case ConnectionStatus::Con_DOWNLOADING  : return strConnectionStatus_DOWNLOADING   ; 
			case ConnectionStatus::Con_CONNECTED    : return strConnectionStatus_CONNECTED     ; 
			case ConnectionStatus::Con_DISCONNECTED : return strConnectionStatus_DISCONNECTED  ; 
			case ConnectionStatus::Con_DISABLED     : return strConnectionStatus_DISABLED      ;
            case ConnectionStatus::Con_LOGGED_IN    : return strConnectionStatus_LOGGED_OUT  ;  
            case ConnectionStatus::Con_LOGGED_OUT   : return strConnectionStatus_CONNECTED   ;  
			default : return ""; 
		}
		return "" ;
	}

	bool isConnected( ConnectionStatus cs ) 
	{
		return cs == ConnectionStatus::Con_CONNECTED ;
	}

}
