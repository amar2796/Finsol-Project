#pragma once
#ifndef FIN_STRUCT_STRING_H
#define FIN_STRUCT_STRING_H
#include "stringParser.h"
#include <iostream>
#include "ETI.h"
#include "constants.h"
#include <unordered_map>
#include "Nanolog.hpp"
using namespace Derivatives;

namespace FIN
{
	class Struct_To_String
	{
	public:
		//	std::string senderCompid = "";
		Struct_To_String();
		~Struct_To_String();
		/*std::string GetNewOrderNRResponseT(NewOrderNRResponseT* newRes);
		std::string GetDeleteOrderNRResponseT(DeleteOrderNRResponseT* remRes);
		std::string GetModifyOrderNRResponseT(ModifyOrderNRResponseT* remRes);
		std::string GetOrderExecNotificationT(OrderExecNotificationT* bookExec);
		std::string GetOrderExecResponseT(OrderExecResponseT* bookExec);
		std::string GetRejectT(RejectT* rejRes);*/
		//std::string timeNowReadable(uint64_t timeStamp);
		std::unordered_map<uint64_t, uint64_t> lastUpdatedTime;
		std::unordered_map<uint32_t, uint64_t> _orderSequence;

	private:
		FixStringCreator _fixStringCreator;
		char SOH = 1;
	};

}




#endif