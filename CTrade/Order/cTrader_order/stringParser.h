#ifndef FIN_ORDER_GATEWAY_STRING_PARSER
#define FIN_ORDER_GATEWAY_STRING_PARSER

#include <iostream>
#include <sstream>
#include <unordered_map>
#include "constants.h"
#include <algorithm>


namespace FIN { 

    class FixStringParser
    {

        std::string clOrdId ;
        public :

            inline void loadString( const std::string &inputString)  
            {

                reset() ;

                std::istringstream f( inputString.c_str() );
                std::string s;
                int pos = 0 ;
                while (getline(f, s, keyValPairSep)) {
                    pos = s.find( keyValSep ) ;
                    _mapKeyValues[ s.substr( 0 , pos ) ] = s.substr ( pos+1 ) ; 
                }

                _itr = _mapKeyValues.begin() ;
                _itrEnd = _mapKeyValues.end() ;

            }

            inline void reset() 
            {
                _mapKeyValues.clear();
            }

            inline bool getIfSet( const std::string &key, std::string &output ) 
            {
                _itr = _mapKeyValues.find( key ) ;
                if( _itr == _itrEnd ) 
                {
                    //std::cout << "Unable to find field: " << key << std::endl ;
                    return false ;
                }
                output = _itr->second;
                return true ;
            }

            inline std::string get( const std::string &key ) 
            {
                //std::cout << "Key: " << key << std::endl ;
                return _mapKeyValues[ key ] ;
            }
            inline char getChar( const std::string &key )
            {
                //std::cout << "Key: " << key << std::endl ;
                return _mapKeyValues[ key ].at(0) ;
            }

            inline void print()
            {
                _itr = _mapKeyValues.begin() ;

                while ( _itr != _itrEnd ) 
                {
                    std::cout << _itr->first << " = [" << _itr->second  << "]"<< std::endl ;
                    ++_itr ;
                }
            }
        private :

            char keyValSep = '=' ;
            char keyValPairSep = 1 ; // SOH

            FixKeyValue _mapKeyValues ;
            FixKeyValue::iterator _itr ;
            FixKeyValue::iterator _itrEnd ;
    };

    class FixStringCreator
    {

        public : 

            inline void getOmsString( std::string &out ) 
            {
                _itr = _mapKeyValues.begin() ;

                updateString( out, "8"              );
                updateString( out, "9"              );
                updateString( out, Fix_MsgType      );
                updateString( out, "34"             );
                updateString( out, Fix_SenderCompID );
                updateString( out, Fix_SenderSubID  );
                updateString( out, Fix_TargetSubID  );
                updateString( out, "52"             );
                updateString( out, Fix_AvgPX        );
                updateString( out, Fix_ClOrdID      );
                updateString( out, Fix_CumQty       );
                updateString( out, Fix_OrdStatus    );
                updateString( out, Fix_ExecType     );
                updateString( out, Fix_LeavesQty    );
                updateString( out, Fix_ExecID       );
                updateString( out, Fix_OrderID      );
                updateString( out, Fix_OrdQty       );
                updateString( out, Fix_Price        );
                updateString( out, Fix_Side         );
                updateString( out, Fix_TransactTime );
                updateString( out, Fix_LastPx       );
                updateString( out, Fix_LastShares   );
                updateString( out, Fix_LeavesQty    );
                updateString( out, Fix_Text         );
                
                insertField( "10", "000" ) ;
                updateString ( out, "10" ) ;
                
                if ( getIfSet( Fix_ClOrdID, clOrdId ))
                {
                    std::cout << "[OUT]: " << clOrdId 
						//<< "|" << getMicrotime2() 
						<< std::endl ;
                }
            }

            inline void reset() 
            {
                _mapKeyValues.clear();
            }

            inline void insertField( const std::string &key, const std::string val )
            {
                _mapKeyValues[key] = val ;
            }

        private :
        
            std::string clOrdId ;

            inline bool getIfSet( const std::string &key, std::string &output ) 
            {
                _itr = _mapKeyValues.find( key ) ;
                if( _itr == _itrEnd ) 
                {
                    //std::cout << "Unable to find field: " << key << std::endl ;
                    return false ;
                }
                output = _itr->second;
                return true ;
            }

            void updateString(std::string &output, const std::string &key )
            {
                std::string val("") ;
                if( getIfSet ( key, val ) )
                    output.append( key + keyValSep + val + keyValPairSep );
            }

            char keyValSep = '=' ;
            char keyValPairSep = 1 ; // SOH

            FixKeyValue _mapKeyValues ;
            FixKeyValue::iterator _itr ;
            FixKeyValue::iterator _itrEnd ;
    };
}
#endif
