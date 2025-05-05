#ifndef FIN_CONFIG_DETAILS
#define FIN_CONFIG_DETAILS
#include <string>
#include <iostream>
#include <vector>

class Config_Details
{
public:

	std::string incremental_multicast_addr = "";
	int incremental_multicast_port = 0;
	std::string interfacea_Addr = "";



	std::string tempatePath = "";
	std::string template_string="";
	std::string token_path = "";
	std::string contract_path = "";
	std::vector<int64_t> tokens_vector;

	std::string publisher_ip;
	int publisher_port;

	bool enable_log;

	bool isEnableLiveSubscription = false;
	std::string rabbitIp;
	int rabbitPort;

private:

};



#endif