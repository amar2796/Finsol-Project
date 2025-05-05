#include "TradeHandler.h"

vector<OmsTradeResponse> TradeHandler::IsEligibleToSend(OrderExecNotificationT* response)
{
	vector<OmsTradeResponse> result;
	try
	{
		for (int i = 0; i < response->NoFills; i++)
		{


		/*	OmsTradeResponse trade;
			trade.setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);
			result.emplace_back(trade);*/

			if (tradeRecord.size() <= 0 || tradeRecord.find(response->OrderID) == tradeRecord.end())
			{
				tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);

				OmsTradeResponse trade;
				//trade.SendingTime = response->RBCHeaderME.SendingTime;

				trade.setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);
				result.emplace_back(trade);
			}
			else
			{
				auto it = std::find(tradeRecord[response->OrderID].begin(), tradeRecord[response->OrderID].end(), response->FillsGrp[i].FillExecID);

				if (it == tradeRecord[response->OrderID].end())
				{
					tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);

					OmsTradeResponse trade;
					//trade.SendingTime = response->RBCHeaderME.SendingTime;

					trade.setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);

					result.emplace_back(trade);
				}
			}

		}

	}
	catch (std::exception& ex)
	{
		 LOG_INFO << ex.what();
	}
	return result;
}

vector<OmsTradeResponse> TradeHandler::IsEligibleToSend(OrderExecResponseT* response)
{
	vector<OmsTradeResponse> result;
	try
	{
		for (int i = 0; i < response->NoFills; i++)
		{
			/*OmsTradeResponse trade;
			trade.setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);

			result.emplace_back(trade);*/
			if (tradeRecord.size() <= 0 || tradeRecord.find(response->OrderID) == tradeRecord.end())
			{
				tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);
				OmsTradeResponse trade;
				//trade.SendingTime = response->ResponseHeaderME.SendingTime;
				trade.setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);

				result.emplace_back(trade);
			}
			else
			{
				auto it = std::find(tradeRecord[response->OrderID].begin(), tradeRecord[response->OrderID].end(), response->FillsGrp[i].FillExecID);

				if (it == tradeRecord[response->OrderID].end())
				{


					tradeRecord[response->OrderID].emplace_back(response->FillsGrp[i].FillExecID);
					OmsTradeResponse trade;
					//trade.SendingTime = response->ResponseHeaderME.SendingTime;

					trade.setTrade(response, response->FillsGrp[i].FillExecID, response->FillsGrp[i].FillPx, response->FillsGrp[i].FillQty);

					result.emplace_back(trade);
				}
			}

		}

	}
	catch (std::exception& ex)
	{
		LOG_INFO << ex.what();
	}
	return result;
}

bool TradeHandler::IsEligibleToSend(TradeBroadcastT* response)
{
	bool result = false;
	try
	{
		if (tradeRecord.size() <= 0 || tradeRecord.find(response->OrderID) == tradeRecord.end())
		{
			tradeRecord[response->OrderID].emplace_back(response->TradeID);
			result = true;
		}
		else
		{
			auto it = std::find(tradeRecord[response->OrderID].begin(), tradeRecord[response->OrderID].end(), response->TradeID);

			if (it == tradeRecord[response->OrderID].end())
			{
				result = true;

				tradeRecord[response->OrderID].emplace_back(response->TradeID);

			}
		}
	}
	catch (std::exception& ex)
	{
		LOG_INFO << ex.what();
	}
	return result;
}

OMSSMPFCancel TradeHandler::IsCanceled(OrderExecReportBroadcastT* response)
{
	OMSSMPFCancel result{};
	try
	{
		if (response->OrdStatus[0] == '4' || response->ExecType[0] == '4')
		{

			result.ClOrdID = response->ClOrdID;
			result.OrderID = response->OrderID;
			result.CxlQty = response->CxlQty;
			result.header = response->MessageHeaderOut;
			result.CumQty = response->CumQty;
			result.OrigClOrdID = response->OrigClOrdID;
		}
	}
	catch (std::exception& ex)
	{
		LOG_INFO << ex.what();
	}
	return result;
}
