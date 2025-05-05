#include "Struct_To_String.h"




//std::string FIN::Struct_To_String::timeNowReadable(uint64_t timeStamp)
//{
//	try
//	{
//		std::time_t time = timeStamp;
//		auto gmtime = std::localtime(&time);
//		char buffer[32];
//		strftime(buffer, 32, "%Y%m%d-%T", gmtime);
//		std::string t1(buffer);
//		//std::cout << "Time: " << t1 << std::endl;
//	}
//	catch (std::exception& ex)
//	{
//		CONSOLE_LOG << ex.what() << std::endl;
//		LOG_INFO << ex.what();
//	}
//	return t1;
//}
FIN::Struct_To_String::Struct_To_String()
{
}

FIN::Struct_To_String::~Struct_To_String()
{
}

//std::string FIN::Struct_To_String::GetNewOrderNRResponseT(NewOrderNRResponseT* newRes)
//{
//	std::string omsStr;
//	try
//	{
//		_fixStringCreator.reset();
//
//		if (_orderSequence.find(newRes->NRResponseHeaderME.MsgSeqNum) != _orderSequence.end())
//			_orderSequence.erase(newRes->NRResponseHeaderME.MsgSeqNum);
//
//		_fixStringCreator.insertField(Fix_BeginString, Fix_BeginString_Val);
//		_fixStringCreator.insertField(Fix_MsgType, Fix_MsgTyp_Val);
//		_fixStringCreator.insertField(Fix_SenderCompID, senderCompid);
//		_fixStringCreator.insertField(Fix_SendTime, std::to_string(newRes->NRResponseHeaderME.SendingTime));
//		_fixStringCreator.insertField(Fix_AvgPX, "0");
//		_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(newRes->ClOrdID));
//		_fixStringCreator.insertField(Fix_CumQty, "0");
//		_fixStringCreator.insertField(Fix_ExecID, "0");
//		_fixStringCreator.insertField(Fix_OrdStatus, newRes->OrdStatus[0]);
//		_fixStringCreator.insertField(Fix_ExecType, newRes->ExecType[0]);
//		_fixStringCreator.insertField(Fix_OrderID, std::to_string(newRes->OrderID));
//		_fixStringCreator.insertField(Fix_ExecRestatementReason, std::to_string(newRes->ExecRestatementReason));
//		_fixStringCreator.insertField(Fix_CrossIndicator, std::to_string(newRes->CrossedIndicator));
//		_fixStringCreator.insertField(Fix_TransactTime, std::to_string(newRes->ExecID));
//		_fixStringCreator.insertField(Fix_TransactioDelayIndicator, std::to_string(newRes->TransactionDelayIndicator));
//
//
//		_fixStringCreator.getOmsString(omsStr);
//		omsStr += Fix_CheckSum + "=000" + SOH;
//		LOG_INFO << omsStr;
//	}
//	catch (std::exception& ex)
//	{
//		CONSOLE_LOG << ex.what() << std::endl;
//		LOG_INFO << ex.what();
//	}
//	return omsStr;
//}
//
//std::string FIN::Struct_To_String::GetDeleteOrderNRResponseT(DeleteOrderNRResponseT* remRes)
//{
//	std::string omsStr;
//	
//	_fixStringCreator.reset();
//
//
//	_fixStringCreator.insertField("8", "FIX4.2");
//	_fixStringCreator.insertField(Fix_MsgType, "8");
//	_fixStringCreator.insertField(Fix_SenderCompID, "12345");
//	_fixStringCreator.insertField("52", std::to_string(remRes->NRResponseHeaderME.Reserve3));
//	_fixStringCreator.insertField(Fix_AvgPX, "0");
//	_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(remRes->ClOrdID));
//	_fixStringCreator.insertField(Fix_CumQty, "0");
//	_fixStringCreator.insertField(Fix_ExecID, "0");
//	_fixStringCreator.insertField(Fix_OrdStatus, remRes->OrdStatus[0]);
//	_fixStringCreator.insertField(Fix_ExecType, remRes->ExecType[0]);
//	_fixStringCreator.insertField(Fix_OrderID, std::to_string(remRes->OrderID));
//	_fixStringCreator.insertField("378", std::to_string(remRes->ExecRestatementReason));
//	_fixStringCreator.insertField(Fix_TransactTime, std::to_string(remRes->ExecID));
//	_fixStringCreator.insertField("25159", std::to_string(remRes->TransactionDelayIndicator));
//
//
//	_fixStringCreator.getOmsString(omsStr);
//	omsStr += Fix_CheckSum + "=000" + SOH;
//	LOG_INFO << omsStr;
//	return omsStr;
//}
//
//std::string FIN::Struct_To_String::GetModifyOrderNRResponseT(ModifyOrderNRResponseT* remRes)
//{
//	std::string omsStr;
//	_fixStringCreator.reset();
//
//
//	_fixStringCreator.insertField("8", "FIX4.4");
//	_fixStringCreator.insertField(Fix_MsgType, "8");
//	_fixStringCreator.insertField(Fix_SenderCompID, "12345");
//	_fixStringCreator.insertField("52", std::to_string(remRes->NRResponseHeaderME.SendingTime));
//	_fixStringCreator.insertField(Fix_AvgPX, "0");
//	_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(remRes->ClOrdID));
//	_fixStringCreator.insertField(Fix_OrigClOrdID, std::to_string(remRes->OrigClOrdID));
//
//	_fixStringCreator.insertField(Fix_CumQty, std::to_string(remRes->CumQty / FIN::qty_multipler));
//	_fixStringCreator.insertField(Fix_OrdQty, std::to_string((remRes->CumQty + remRes->LeavesQty) / FIN::qty_multipler));
//	_fixStringCreator.insertField(Fix_ExecID, "0");
//	_fixStringCreator.insertField(Fix_OrdStatus, remRes->ExecType[0]);
//	_fixStringCreator.insertField(Fix_ExecType, remRes->ExecType[0]);
//
//	_fixStringCreator.insertField(Fix_OrderID, std::to_string(remRes->OrderID));
//	_fixStringCreator.insertField("378", std::to_string(remRes->ExecRestatementReason));
//	_fixStringCreator.insertField("2523", std::to_string(remRes->CrossedIndicator));
//	_fixStringCreator.insertField(Fix_TransactTime, std::to_string(remRes->ExecID));
//	_fixStringCreator.insertField("25159", std::to_string(remRes->TransactionDelayIndicator));
//
//
//	_fixStringCreator.getOmsString(omsStr);
//	omsStr += Fix_CheckSum + "=000" + SOH;
//	LOG_INFO << omsStr;
//	return omsStr;
//}
//
//std::string FIN::Struct_To_String::GetOrderExecNotificationT(OrderExecNotificationT* bookExec)
//{
//	std::string omsStr;
//	_fixStringCreator.reset();
//
//
//
//	_fixStringCreator.insertField(Fix_BeginString, Fix_BeginString_Val);
//	_fixStringCreator.insertField(Fix_MsgType, Fix_MsgTyp_Val);
//	_fixStringCreator.insertField(Fix_SenderCompID, senderCompid);
//	_fixStringCreator.insertField(Fix_SendTime, std::to_string(bookExec->LstUpdtTime));
//	_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(bookExec->ClOrdID));
//
//	_fixStringCreator.insertField(Fix_CumQty, std::to_string(bookExec->CumQty / FIN::qty_multipler));
//	_fixStringCreator.insertField(Fix_OrdQty, std::to_string((bookExec->CumQty + bookExec->LeavesQty) / FIN::qty_multipler));
//	_fixStringCreator.insertField(Fix_OrdStatus, bookExec->OrdStatus[0]);
//	_fixStringCreator.insertField(Fix_ExecType, bookExec->ExecType[0]);
//	_fixStringCreator.insertField(Fix_OrderID, std::to_string(bookExec->OrderID));
//	_fixStringCreator.insertField(Fix_ExecRestatementReason, std::to_string(bookExec->ExecRestatementReason));
//	_fixStringCreator.insertField(Fix_TransactTime, std::to_string(bookExec->ExecID));
//
//	_fixStringCreator.getOmsString(omsStr);
//
//	omsStr += "1362=" + std::to_string(bookExec->NoFills) + SOH;
//	for (int i = 0; i < bookExec->NoFills; i++)
//	{
//		omsStr += "1364=" + std::to_string((double)(bookExec->FillsGrp[i].FillPx / FIN::price_multiplier)) +
//			SOH + "1365=" + std::to_string(bookExec->FillsGrp[i].FillQty / FIN::qty_multipler) + SOH +
//			"28708=" + std::to_string(bookExec->FillsGrp[i].FillMatchID) + SOH +
//			"1363=" + std::to_string(bookExec->FillsGrp[i].FillExecID) + SOH;
//	}
//	omsStr += Fix_CheckSum + "=000" + SOH;
//
//	LOG_INFO << omsStr;
//	return omsStr;
//}
//
//std::string FIN::Struct_To_String::GetOrderExecResponseT(OrderExecResponseT* bookExec)
//{
//	std::string omsStr;
//	_fixStringCreator.reset();
//
//
//	_fixStringCreator.insertField("8", "FIX4.4");
//	_fixStringCreator.insertField(Fix_MsgType, "8");
//	_fixStringCreator.insertField(Fix_SenderCompID, "12345");
//	_fixStringCreator.insertField("52", std::to_string(bookExec->LstUpdtTime));
//	_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(bookExec->ClOrdID));
//
//	_fixStringCreator.insertField(Fix_CumQty, std::to_string(bookExec->CumQty / FIN::qty_multipler));
//	_fixStringCreator.insertField(Fix_OrdQty, std::to_string((bookExec->CumQty + bookExec->LeavesQty) / FIN::qty_multipler));
//	_fixStringCreator.insertField(Fix_OrdStatus, bookExec->OrdStatus[0]);
//	_fixStringCreator.insertField(Fix_ExecType, bookExec->ExecType[0]);
//	_fixStringCreator.insertField(Fix_OrderID, std::to_string(bookExec->OrderID));
//	_fixStringCreator.insertField("378", std::to_string(bookExec->ExecRestatementReason));
//	_fixStringCreator.insertField(Fix_TransactTime, std::to_string(bookExec->ExecID));
//
//	_fixStringCreator.getOmsString(omsStr);
//
//	omsStr += "1362=" + std::to_string(bookExec->NoFills) + SOH;
//	for (int i = 0; i < bookExec->NoFills; i++)
//	{
//		omsStr += "1364=" + std::to_string((double)(bookExec->FillsGrp[i].FillPx / FIN::price_multiplier)) +
//			SOH + "1365=" + std::to_string(bookExec->FillsGrp[i].FillQty / FIN::qty_multipler) + SOH +
//			"28708=" + std::to_string(bookExec->FillsGrp[i].FillMatchID) + SOH +
//			"1363=" + std::to_string(bookExec->FillsGrp[i].FillExecID) + SOH;
//	}
//
//	omsStr += Fix_CheckSum + "=000" + SOH;
//	LOG_INFO << omsStr;
//	return omsStr;
//}
//
//std::string FIN::Struct_To_String::GetRejectT(RejectT* rejRes)
//{
//	std::string omsStr;
//	_fixStringCreator.reset();
//
//	_fixStringCreator.insertField("8", "FIX4.4");
//	auto reject_order = _orderSequence.find(rejRes->NRResponseHeaderME.MsgSeqNum);
//	if (reject_order != _orderSequence.end())
//	{
//
//		rejRes->NRResponseHeaderME.SendingTime = reject_order->second;
//		_fixStringCreator.insertField(Fix_MsgType, "8");
//		_fixStringCreator.insertField(Fix_ClOrdID, std::to_string(reject_order->second));
//		_orderSequence.erase(rejRes->NRResponseHeaderME.MsgSeqNum);
//	}
//	else
//		_fixStringCreator.insertField(Fix_MsgType, "9");
//
//	_fixStringCreator.insertField(Fix_SenderCompID, "12345");
//	_fixStringCreator.insertField("52", std::to_string(rejRes->NRResponseHeaderME.Reserve3));
//	_fixStringCreator.insertField(Fix_AvgPX, "0");
//
//	_fixStringCreator.insertField(Fix_CumQty, "0");
//	_fixStringCreator.insertField(Fix_ExecID, "0");
//	_fixStringCreator.insertField(Fix_OrdStatus, "8");
//	_fixStringCreator.insertField(Fix_ExecType, "8");
//	std::string error_msg(rejRes->VarText);
//	_fixStringCreator.insertField(Fix_Text, error_msg);
//
//
//	_fixStringCreator.getOmsString(omsStr);
//	omsStr += Fix_CheckSum + "=000" + SOH;
//	return omsStr;
//}
