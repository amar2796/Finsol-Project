#ifndef FIN_MD_NSETBT_ABSTRACT_DATA
#define FIN_MD_NSETBT_ABSTRACT_DATA

#include "defines.h"
#include "marketDataMessage.h"

#include <cstring>

namespace FIN {

	class AbstractDataSender
	{
	public:
		virtual void sendData(const char* data, const size_t size) = 0;
	protected:
		AbstractDataSender() {};
	};
}


#endif
