#pragma once
#ifndef FIN_SNAPSHOT_EMDI_PROCESS
#define FIN_SNAPSHOT_EMDI_PROCESS
#include "ConfigDetails.h"
#include "Broadcast_Snapshot.h"
#include <unordered_map>

struct MsgType
{
	int msgType;
};

class MDI_SNAPSHOT
{
public:
	MDI_SNAPSHOT(Config_Details* config);	
	~MDI_SNAPSHOT() ;
	void init();
	void add_memory_live_request(uint64_t securityId);
	void delete_memory_live_request(uint64_t securityId);
	//std::unordered_map<int64_t, Broadcast_Snapshot*> snap_securities;	
	std::unordered_map<uint64_t, std::unique_ptr<Broadcast_Snapshot>> snap_securities;
private:
	void generateSnap_Memory();
	void processData();
	FIN::Publisher send_to_oms;
	//boost::asio::io_service ioservice;
	Config_Details* local_config;
	FIN::MulticastReceiver snap_rcver;
	std::unordered_map<int64_t, uint32_t> securities;
	std::mutex mutexlock;
};

#endif