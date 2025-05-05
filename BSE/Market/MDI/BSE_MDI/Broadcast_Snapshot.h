#pragma once
#ifndef BROADCAST_SNAPSHOT_H
#define BROADCAST_SNAPSHOT_H


//#include <fstream>
//#include <mfast.h>
#include <mfast/coder/fast_decoder.h>
//#include <mfast/json/json.h>
#include <mfast/xml_parser/dynamic_templates_description.h>
#include "MDIFastTemplates_1_1.h"
#include "MulticastReceiver.h"
#include "constants.h"
#include "Publisher.h"
#include "globalThings.h"
#include "marketDataMessage.h"
#include <unordered_map>
//#include <unordered_map>
//using namespace FIN;
//using mfast::templates_description;
//using mfast::dynamic_templates_description;
//using mfast::fast_decoder;
//using mfast::message_cref;
//using mfast::ascii_string_cref;
//using mfast::json::encode;




class Broadcast_Snapshot
{
public:

	Broadcast_Snapshot(int64_t, uint16_t, FIN::Publisher*);
	~Broadcast_Snapshot();
	void sendDummyDpr();
	void processDepthSnapshot(MDIFastTemplates_1_1::DepthSnapshot_cref&);
	void process_Trade(MDIFastTemplates_1_1::DepthSnapshot_cref::MDSshGrp_element_cref&);
	void process_Trade(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&);
	void process_Increment_Depth(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&, uint32_t, uint32_t);
	bool snapshot_process = false;
	uint32_t lastProcessed_sequence_inc;

private:
	void process_Increment_New(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&);
	void process_Increment_Update(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&);
	void process_Increment_Delete(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&);

	void process_Increment_Delete_thru(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&);
	void process_Increment_Delete_from(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&);
	void process_Increment_Update_Overlay(MDIFastTemplates_1_1::DepthIncremental_cref::MDIncGrp_element_cref&);

	uint32_t lastProcessed_sequence_snap;
	uint32_t marketSegmentID;

	FIN::Publisher* publish_oms;
	FIN::MarketData market_depth;
	FIN::OHLC _ohlc;
	FIN::DPR _dpr;
	std::vector<uint64_t> matching_result(uint64_t);




};

#endif // !BROADCAST_SNAPSHOT_H