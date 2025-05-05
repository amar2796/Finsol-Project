#pragma once
#ifndef FIN_ETI_ADAPTER_H
#define FIN_ETI_ADAPTER_H

#include "Exchange_Processor.h"
#include "IOServiceHanlder.h"
#include "Nanolog.hpp"
namespace FIN
{
	namespace ETI
	{

		class ETI_Adapter : public Exchange_Processor
		{
		public:



			ETI_Adapter(boost::asio::io_service& ioService);
			bool init();
			void stop();
			void start_exch_con();

			


		private:
			boost::asio::io_service& _ioService;

		};





	}
}



#endif