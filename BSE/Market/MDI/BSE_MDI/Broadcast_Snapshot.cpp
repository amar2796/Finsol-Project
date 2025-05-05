#include "Broadcast_Snapshot.h"

Broadcast_Snapshot::Broadcast_Snapshot(int64_t security, uint16_t exchID, FIN::Publisher* publish) :market_depth(exchID, std::to_string(security)),
_ohlc(exchID, std::to_string(security)), _dpr(exchID, std::to_string(security))
{
	publish_oms = publish;
	/*
	* Calculated set sum globally once and stored in unorderd map as O(1) time complexity to access
	* Subsets of { 1, 2, 4, 8, 16, 128, 8192, 16384 } //read description in globalThings.h at bottom
	*/
	if (setValueOnce)
	{
		calculateAllSubsets();
		setValueOnce = false;
	}
}

Broadcast_Snapshot::~Broadcast_Snapshot()
{

}

void Broadcast_Snapshot::processDepthSnapshot(MDIFastTemplates_1_1::DepthSnapshot_cref& depth)
{
	try
	{
		market_depth.deleteCompleteDepth(); //Clear market depth as we have received updated value in snapshot
		lastProcessed_sequence_inc = lastProcessed_sequence_snap = depth.get_MsgSeqNum().value();

		marketSegmentID = depth.get_MarketSegmentID().value();

		auto group = depth.get_MDSshGrp();
		auto entries = group.length();
		bool isDpr = false;
		for (auto entry : group)
		{
			//LOG_INFO << "MDENTRYTYPE_SNAP:" << entry.get_MDEntryType().value();
			switch (entry.get_MDEntryType().value())
			{
			case 0: //Bid
				//LOG_INFO << "PROCESS BID SNAP";
				market_depth.insertBuyDepth((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value());
				break;
			case 1: //Ask
				//LOG_INFO << "PROCESS ASK SNAP";
				market_depth.insertSellDepth((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value());
				break;
			case 2://Trade
			{
				//LOG_INFO << "PROCESS TRADE SNAP";				
				FIN::SIGNED_INT_32 time = (FIN::SIGNED_INT_64)(depth.get_LastUpdateTime().value() / 1000000000);//We receive time in nanosecond and datatype int64 but in market data, datatype is of 32bits so we are converting it to milliseconds.
				market_depth.setLastTradeTime(time);
				process_Trade(entry);
			}
			break;
			case 4: //Upper circuit
				_dpr.setUpperLimit((FIN::PRICE)entry.get_MDEntryPx());
				isDpr = true;
				break;
			case 5://lower circuit
				_dpr.setLowerLimit((FIN::PRICE)entry.get_MDEntryPx());
				isDpr = true;
				break;
			case 8://open interest ( C )			
				//LOG_INFO << "Process Open Interest";
				market_depth.setOpenInterest(entry.get_MDEntrySize().value());
				break;
			case 9://Trade Volume
				market_depth.setVolume(entry.get_MDEntrySize().value());
				break;
			default:
				//LOG_INFO << "UNKNOWN ENTRY TYPE IN PROCESS_SNAPSHOT:" << depth.get_SecurityID().value() << " " << entry.get_MDEntryType().value() << " " << entry.get_MDEntryPx() << " " << entry.get_MDEntrySize();

				break;
			}


		}
		if (market_depth.checkBestBidAsk())
		{
			publish_oms->send(market_depth, sizeof(market_depth));
		}
		publish_oms->send(_ohlc, sizeof(_ohlc));
		if (isDpr)
		{
			publish_oms->send(_dpr, sizeof(_dpr));
		}
		snapshot_process = true;
		//	}

	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}

}

void Broadcast_Snapshot::process_Trade(MDIFastTemplates_1_1::DepthSnapshot_cref::MDSshGrp_element_cref& entry)
{
	try
	{
		//for (auto con : matching_result(entry.get_TradeCondition().value()))
		int tradeConditionValue = entry.get_TradeCondition().value();
		if (tradeConditonSubsets.find(tradeConditionValue) == tradeConditonSubsets.end())
		{
			LOG_INFO << "Not Calculated in SET :" << tradeConditionValue;
		}
		else
			for (auto con : tradeConditonSubsets[tradeConditionValue])
			{
				//LOG_INFO << "TRADE CONDITION " << entry.get_TradeCondition().value() << " INDIVIDUAL " << con;
				switch (con)
				{
				case FIN::FIN_CONST::EXCHANGE_LAST:
					market_depth.setLtp((FIN::PRICE)entry.get_MDEntryPx());
					//market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
					break;
				case FIN::FIN_CONST::OPEN_PRICE:
					//LOG_INFO << "OpenPrice" << (FIN::PRICE)entry.get_MDEntryPx();
					_ohlc.setOpenPrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::HIGH_PRICE:
					//LOG_INFO << "HighPrice" << (FIN::PRICE)entry.get_MDEntryPx();
					_ohlc.setHighPrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::LOW_PRICE:
					_ohlc.setLowPrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::OFFICIAL_CLOSING_PRICE:
					_ohlc.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());
					_dpr.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::LIFE_HIGH:
					break;
				case FIN::FIN_CONST::LIFE_LOW:
					break;
				case FIN::FIN_CONST::PREVIOUS_CLOSING_PRICE:
					_ohlc.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());
					_dpr.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());

					break;
					/*case FIN::FIN_CONST::VOLUMEONLY:
						market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
						break;*/
						/*case FIN::FIN_CONST::EXCHANGE_LAST + FIN::FIN_CONST::OPEN_PRICE + FIN::FIN_CONST::LOW_PRICE:
							market_depth.setLtp((FIN::PRICE)entry.get_MDEntryPx());
							market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
							_ohlc.setOpenPrice((FIN::PRICE)entry.get_MDEntryPx());
							_ohlc.setLowPrice((FIN::PRICE)entry.get_MDEntryPx());
							break;*/

				default:
					LOG_INFO << "TRADE CONDITION " << entry.get_TradeCondition().value() << " ENTRY TYPE " << entry.get_MDEntryType().value();
					break;
				}
			}



	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}

}
void Broadcast_Snapshot::process_Trade(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry)
{
	try
	{
		auto entry2 = entry.get_TradeEntryGrp();
		//for (auto con : matching_result(entry.get_TradeCondition().value()))
		int tradeConditionValue = entry2.get_TradeCondition().value();
		if (tradeConditonSubsets.find(tradeConditionValue) == tradeConditonSubsets.end())
		{
			LOG_INFO << "Not Calculated in SET INCR:" << tradeConditionValue;
		}
		else
			for (auto con : tradeConditonSubsets[tradeConditionValue])
			{
				//LOG_INFO << "TRADE CONDITION INCR " << entry2.get_TradeCondition().value() << " INDIVIDUAL " << con;
				switch (con)
				{
				case FIN::FIN_CONST::EXCHANGE_LAST:
					market_depth.setLtp((FIN::PRICE)entry.get_MDEntryPx());
					//market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
					break;
				case FIN::FIN_CONST::OPEN_PRICE:
					_ohlc.setOpenPrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::HIGH_PRICE:
					_ohlc.setHighPrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::LOW_PRICE:
					_ohlc.setLowPrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::OFFICIAL_CLOSING_PRICE:
					_ohlc.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());
					_dpr.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
				case FIN::FIN_CONST::LIFE_HIGH:
					break;
				case FIN::FIN_CONST::LIFE_LOW:
					break;
				case FIN::FIN_CONST::PREVIOUS_CLOSING_PRICE:
					_ohlc.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());
					_dpr.setClosePrice((FIN::PRICE)entry.get_MDEntryPx());
					break;
					/*case FIN::FIN_CONST::VOLUMEONLY:
						market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
						break;*/
						/*case FIN::FIN_CONST::EXCHANGE_LAST + FIN::FIN_CONST::OPEN_PRICE + FIN::FIN_CONST::LOW_PRICE:
							market_depth.setLtp((FIN::PRICE)entry.get_MDEntryPx());
							market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
							_ohlc.setOpenPrice((FIN::PRICE)entry.get_MDEntryPx());
							_ohlc.setLowPrice((FIN::PRICE)entry.get_MDEntryPx());
							break;*/

				default:
					LOG_INFO << "TRADE CONDITION " << entry2.get_TradeCondition().value() << " ENTRY TYPE " << entry.get_MDEntryType().value();
					break;
				}
			}



	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}


}

void Broadcast_Snapshot::process_Increment_Depth(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry, uint32_t seq, uint32_t product)
{
	try
	{
		//LOG_INFO << "Do incremental = " << snapshot_process;
		//LOG_INFO << "SEQ NICREMENTAL " << seq << " last " << lastProcessed_sequence_inc;
	//	if (snapshot_process)//First we need to process one snaphot then only we can use depth increment packet
		{
			//std::cout << "SEQ NICREMENTAL " << seq << " last " << lastProcessed_sequence_inc << " " << entry.get_MDEntryType().value() << " action " << entry.get_MDUpdateAction().value() << std::endl;
			if (seq > lastProcessed_sequence_inc)
			{

				switch (entry.get_MDEntryType().value())
				{
				case 2:
				{
					//market_depth.setLtp((FIN::PRICE)entry.get_MDEntryPx());
					//FIN::SIGNED_INT_32 time = (FIN::SIGNED_INT_64)(entry.get_MDEntryTime().value() / 1000000000);//We receive time in nanosecond and datatype int64 but in market data, datatype is of 32bits so we are converting it to milliseconds.
					//market_depth.setLastTradeTime(time);
					process_Trade(entry);
				}
				break;
				case 8:
					market_depth.setOpenInterest(entry.get_MDEntrySize().value());
					break;
				case 9://Trade Volume
					market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize().value());
					break;
					//default:
						//std::cout << "Not handled MDEntryType in Incremental:" << entry.get_MDEntryType().value() << std::endl;
				}
				//if (entry.get_MDEntryType().value() == 2)
				//{
				//	//market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
				//	

				//}
				//else if (entry.get_MDEntryType().value() == 9)
				//{
				//	market_depth.setVolume((FIN::VOLUME)entry.get_MDEntrySize());
				//}

				switch (entry.get_MDUpdateAction().value())
				{
				case 0:
					process_Increment_New(entry);
					break;
				case 1:
					process_Increment_Update(entry);
					break;
				case 2:
					process_Increment_Delete(entry);
					break;

				case 3:
					process_Increment_Delete_thru(entry);
					break;

				case 4:
					process_Increment_Delete_from(entry);
					break;
				case 5:
					process_Increment_Update_Overlay(entry);
					break;

				default:
					LOG_INFO << "UNKNKOW ACTION VALUE " << entry.get_MDUpdateAction().value() << " level " << entry.get_MDPriceLevel().value();
					break;
				}

			}
			else if (lastProcessed_sequence_snap >= seq)
			{
				LOG_INFO << "LAST IS GREATER THAN  : LAST INC SEQ " << lastProcessed_sequence_inc << " CURRENT " << seq;
				return;
			}
			else if (lastProcessed_sequence_inc == seq)
			{

			}
			else
			{
				LOG_INFO << "MISSING SEQUEUNCE : LAST INC SEQ " << lastProcessed_sequence_inc << " CURRENT " << seq;
				snapshot_process = false;
			}
			//	lastProcessed_sequence_inc = seq;
			if (market_depth.checkBestBidAsk())
			{
				//std::cout << "SEC ID " << market_depth.getSecId() << " incremental " << std::endl;
				publish_oms->send(market_depth, sizeof(market_depth));
			}
			publish_oms->send(_ohlc, sizeof(_ohlc));
		}

	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}



}

void Broadcast_Snapshot::process_Increment_New(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry)
{
	try
	{
		//std::cout << "New " << entry.get_MDEntryType().value() << " " << (FIN::PRICE)entry.get_MDEntryPx() << " " << (FIN::QUANTITY)entry.get_MDEntrySize() << std::endl;
		switch (entry.get_MDEntryType().value())
		{
		case 0:
			market_depth.insertBuyDepth((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value());
			break;
		case 1:
			market_depth.insertSellDepth((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value());
			break;

		default:
			break;
		}


	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}

}

void Broadcast_Snapshot::process_Increment_Update(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry)
{
	try
	{
		//std::cout << "Update " << entry.get_MDEntryType().value() << " " << (FIN::PRICE)entry.get_MDEntryPx() << " " << (FIN::QUANTITY)entry.get_MDEntrySize() << std::endl;

		switch (entry.get_MDEntryType().value())
		{
		case 0:
			market_depth.updateBuyDepth((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value());
			break;
		case 1:
			market_depth.updateSellDepth((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value());
			break;

		default:
			break;
		}
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}

}

void Broadcast_Snapshot::process_Increment_Delete(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry)
{

	try
	{
		//std::cout << "Delete " << entry.get_MDEntryType().value() << " " << (FIN::PRICE)entry.get_MDEntryPx() << " " << (FIN::QUANTITY)entry.get_MDEntrySize() << std::endl;


		switch (entry.get_MDEntryType().value())
		{
		case 0:
			market_depth.deleteBuyDepth((FIN::PRICE)entry.get_MDEntryPx());
			break;
		case 1:
			market_depth.deleteSellDepth((FIN::PRICE)entry.get_MDEntryPx());
			break;

		default:
			break;
		}
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}


}

void Broadcast_Snapshot::process_Increment_Delete_thru(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry)
{
	try
	{
		//std::cout << "Delete thru " << entry.get_MDEntryType().value() << " " << (FIN::PRICE)entry.get_MDEntryPx() << " " << (FIN::QUANTITY)entry.get_MDEntrySize() << std::endl;

		switch (entry.get_MDEntryType().value())
		{
		case 0:
			market_depth.deleteBuyDepthThru((FIN::PRICE)entry.get_MDEntryPx(), entry.get_MDPriceLevel().value());
			break;
		case 1:
			market_depth.deleteSellDepthThru((FIN::PRICE)entry.get_MDEntryPx(), entry.get_MDPriceLevel().value());
			break;

		default:
			break;
		}
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}
}

void Broadcast_Snapshot::process_Increment_Delete_from(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry)
{
	try
	{
		//std::cout << "Delete from " << entry.get_MDEntryType().value() << " " << (FIN::PRICE)entry.get_MDEntryPx() << " " << (FIN::QUANTITY)entry.get_MDEntrySize() << std::endl;

		switch (entry.get_MDEntryType().value())
		{
		case 0:
			market_depth.deleteBuyDepthFrom((FIN::PRICE)entry.get_MDEntryPx(), entry.get_MDPriceLevel().value());
			break;
		case 1:
			market_depth.deleteBuyDepthFrom((FIN::PRICE)entry.get_MDEntryPx(), entry.get_MDPriceLevel().value());
			break;

		default:
			break;
		}
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}
}

void Broadcast_Snapshot::process_Increment_Update_Overlay(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref& entry)
{
	try
	{
		//std::cout << "Overlay " << entry.get_MDEntryType().value() << " " << (FIN::PRICE)entry.get_MDEntryPx() << " " << (FIN::QUANTITY)entry.get_MDEntrySize() << std::endl;

		switch (entry.get_MDEntryType().value())
		{
		case 0:
			market_depth.updateBuyDepth_Overlay((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value(), entry.get_MDPriceLevel().value());
			break;
		case 1:
			market_depth.updateSellDepth_Overlay((FIN::PRICE)entry.get_MDEntryPx(), (FIN::QUANTITY)entry.get_MDEntrySize().value(), entry.get_MDPriceLevel().value());
			break;

		default:
			break;
		}
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}
}

std::vector<uint64_t> Broadcast_Snapshot::matching_result(uint64_t given_val)
{
	std::vector<uint64_t> result;
	try
	{

		for (auto val : FIN::FIN_CONST::trade_con)
			if (val == given_val)
			{
				result.push_back(val);
				return result;
			}


		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (given_val == FIN::FIN_CONST::trade_con[i] + FIN::FIN_CONST::trade_con[j])
				{
					result.push_back(FIN::FIN_CONST::trade_con[i]);
					result.push_back(FIN::FIN_CONST::trade_con[j]);
					return result;
				}

			}
		}
		result.push_back(given_val);
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " " << boost::diagnostic_information_what(ex);
	}
	return result;
}




