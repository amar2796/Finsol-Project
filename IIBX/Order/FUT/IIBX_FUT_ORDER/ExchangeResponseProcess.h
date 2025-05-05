#ifndef FIN_EXCHANGE_RESPONSE_PROCESS
#define FIN_EXCHANGE_RESPONSE_PROCESS
#include <iostream>
#include "messages.h"
#include <vector>
#include <sstream>
#include "constants.h"
#include <algorithm>
#include"Nanolog.hpp"
#include <unordered_map>
namespace FIN {
	namespace IIBX {
		class ExchangeResponseProcess
		{
		public:
			ExchangeResponseProcess();
			~ExchangeResponseProcess()
			{
				delete ordResp;				
			}
			SIGNED_LONG strinToInt(std::string str);
			OmsResponseOrder* getOrderEntryConfirm(const std::string& resp);
			OmsResponseOrder* getorderEntryReject(std::string&);
			OmsResponseOrder* getorderModConfirm(std::string& resp);
			OmsResponseOrder* getorderModReject(std::string& resp);
			OmsResponseOrder* getorderCancelConfirm(std::string& resp);
			OmsResponseOrder* getorderCancelReject(std::string& resp);
			OmsResponseOrder* getTradeConfirm(std::string& resp);
			bool getLimitViolationNotification(std::string& resp, OmsResponseOrder& omsResp, std::string cmemcode);
			OmsResponseOrder* getMemberNotification(std::string& resp);
			OmsResponseOrder* getClientNotification(std::string& resp);
			OmsResponseOrder* getOrderkill(std::string& resp);
			OmsResponseOrder* getSelfTrade(std::string& resp);
			OmsResponseOrder* getStopLossDetail(std::string& resp);
			
		private:
			OmsResponseOrder *ordResp;	
			int priceDevider = 10000;
		};

	}
}
#endif // !FIN_EXCHANGE_RESPONSE_PROCESS

