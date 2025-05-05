#pragma once

#include "../common/threadSafeQueue.h"

namespace FIN {

	class OmsReceiver
	{
		public:
			void printMenu() {}
			int  getNextMessageOption() {}
	};

	class GatewayObject {};

	class OmsHandler
	{
		private:
			OmsReceiver _omsReceiver;


			GatewayObject _gatewayObject;

			ThreadSafeQueue<std::string> _confirmationQueue;
		public:

			void init();
			void start()
			{
				while (true)
				{
					//if (_gatewayObject.isConnected())
					{
					}
				}
			}

			void startConfirmationThread()
			{
			}

	};


}