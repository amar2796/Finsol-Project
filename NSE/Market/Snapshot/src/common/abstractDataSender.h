#ifndef FIN_COMMON_DATA_SENDER_H
#define FIN_COMMON_DATA_SENDER_H

#include <cstring>

namespace FIN {

	class AbstractDataSender
	{
		public : 
		virtual int  send( const char *data, const size_t size ) = 0 ;
		protected : 
		AbstractDataSender() {};
	} ;
}

#endif
