#pragma once
#include "Snaphot_MDI.h"

MDI_SNAPSHOT::MDI_SNAPSHOT(Config_Details* config) :snap_rcver(config), send_to_oms(config)
{
	local_config = config;
}
MDI_SNAPSHOT::~MDI_SNAPSHOT()
{

}

void MDI_SNAPSHOT::init()
{
	snap_rcver.init(); //Initialize multicast receiver
	send_to_oms.init(); //Initialize publiser

	std::thread thread_multicast(&FIN::MulticastReceiver::start_Reciver, &snap_rcver);
	thread_multicast.detach();
	generateSnap_Memory();
	processData();
}

void MDI_SNAPSHOT::add_memory_live_request(uint64_t securityId)
{
	try
	{
		std::lock_guard<std::mutex> lock(mutexlock);
		snap_securities.insert(std::make_pair(securityId, std::make_unique<Broadcast_Snapshot>(securityId, 3, &send_to_oms)));

		std::cout << "Successfully Subscribed SecurityID: " << securityId << std::endl;
		std::cout << "Security File Size : " << snap_securities.size() << std::endl;
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << "\n";
	}
}

void MDI_SNAPSHOT::delete_memory_live_request(uint64_t securityId)
{
	try
	{
		std::lock_guard<std::mutex> lock(mutexlock);
		snap_securities.erase(securityId);

		std::cout << "Successfully UnSubscribed SecurityID: " << securityId << std::endl;
		std::cout << "Security File Size : " << snap_securities.size() << std::endl;
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << "\n";
	}
}

void MDI_SNAPSHOT::generateSnap_Memory()
{
	try
	{
		if (local_config->tokens_vector.size() > 0)
		{
			for (auto token : local_config->tokens_vector)
			{
				snap_securities.insert(std::make_pair(token, new Broadcast_Snapshot(token, 3, &send_to_oms)));

				std::cout << "[INFO ]Successfully Subscribed Symbol: " << token << std::endl;
			}
			std::cout << "[INFO ]Security File Size : " << snap_securities.size() << std::endl;
		}

	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << "\n";

	}
}


void MDI_SNAPSHOT::processData()
{
	try
	{
		mfast::dynamic_templates_description description(local_config->template_string);
		const mfast::templates_description* descriptions[] = { &description };
		mfast::fast_decoder decoder;
		decoder.include(descriptions);
		while (1)
		{
			auto packet = snap_rcver.multicast_q.front();

			try
			{

				auto intType = (MsgType*)packet.msg;

				//std::cout << intType->msgType << std::endl;

				/*mfast::dynamic_templates_description description(local_config->template_string);
				const mfast::templates_description* descriptions[] = { &description };

				mfast::fast_decoder decoder;
				decoder.include(descriptions);*/



				auto start_stop = std::chrono::high_resolution_clock::now();
				const char* start = packet.msg;
				const char* end = packet.msg + packet.len;
				while (start != end)
				{
					mfast::message_cref decode_msg = decoder.decode(start, end, true);

					switch (decode_msg.id())
					{

					case MDIFastTemplates_1_1::DepthSnapshot::the_id:
					{
						auto snap_msg = static_cast<MDIFastTemplates_1_1::DepthSnapshot_cref>(decode_msg);
						/*	if (snap_msg.get_SecurityID().value() == 856057 || snap_msg.get_SecurityID().value() == 1159894)
								LOG_INFO << "Security ID:" << snap_msg.get_SecurityID().value();*/
								//std::cout << "Security ID:" << snap_msg.get_SecurityID().value() << std::endl;
						if (snap_securities.find(snap_msg.get_SecurityID().value()) != snap_securities.end())
						{
							//LOG_INFO << " Process Depth Snapshot";
							snap_securities[snap_msg.get_SecurityID().value()]->processDepthSnapshot(snap_msg);

						}
						/*else
						{
							snap_securities.insert(std::make_pair(snap_msg.get_SecurityID().value(), new Broadcast_Snapshot(snap_msg.get_SecurityID().value(), 11,&send_to_oms)));
							snap_securities[snap_msg.get_SecurityID().value()]->processDepthSnapshot(snap_msg);
						}*/
					}
					break;
					case MDIFastTemplates_1_1::MDPacketHeader::the_id:
						break;
					/*case MDIFastTemplates_1_1::Reset::the_id:
						break;*/
					case MDIFastTemplates_1_1::DepthIncremental::the_id:
					{
						auto inc_msg = static_cast<MDIFastTemplates_1_1::DepthIncremental_cref>(decode_msg);

						for (auto entry : inc_msg.get_MDIncGrp())
						{
							/*if (entry.get_SecurityID().value() == 856057 || entry.get_SecurityID().value() == 1159894)
								LOG_INFO << "Security ID:" << entry.get_SecurityID().value();*/
								//std::cout << entry.get_SecurityID().value() << std::endl;
							if (snap_securities.find(entry.get_SecurityID().value()) != snap_securities.end())
							{
								//LOG_INFO << " Process Depth Incremental";
								snap_securities[entry.get_SecurityID().value()]->process_Increment_Depth(entry, inc_msg.get_MsgSeqNum().value(), inc_msg.get_MarketSegmentID().value());
								securities[entry.get_SecurityID().value()] = inc_msg.get_MsgSeqNum().value();
							}
						}
						if (!securities.empty())
						{
							//LOG_INFO << securities.size();
							for (auto sequenceUpdate : securities)
								if (inc_msg.get_MsgSeqNum().value() != 0)
									snap_securities[sequenceUpdate.first]->lastProcessed_sequence_inc = inc_msg.get_MsgSeqNum().value();
								else
									LOG_INFO << "Security  " << sequenceUpdate.first << " seq " << sequenceUpdate.second;
							securities.clear();
							//LOG_INFO <<"After clear "<< securities.size();
						}

					}
					break;
					default:
						LOG_INFO << "TEMPLATE ID NOT FOUND " << decode_msg.id();
						//std::cout << "TEMPLATE ID NOT FOUND " << decode_msg.id() << " " << packet.len << std::endl;;
						break;
					}

				}
				snap_rcver.multicast_q.pop();
				delete[] packet.msg;
				//free(packet.msg);
				/*auto  stop = std::chrono::high_resolution_clock::now();

				auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop - start_stop).count();
				std::cout << " Elapsed " << elapsed << std::endl;*/
			}
			catch (boost::exception& ex)
			{
				//	LOG_INFO << "[IN WHILE PROCESS ]" << boost::diagnostic_information(ex);
				snap_rcver.multicast_q.pop();
				delete[] packet.msg;
			}


		}
	}
	catch (boost::exception& ex)
	{
		//LOG_INFO << "[OUT WHILE PROCESS ]" << boost::diagnostic_information(ex);
	}

}