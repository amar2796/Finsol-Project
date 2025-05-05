#ifndef FIN_CONFIG_DETAILS
#define FIN_CONFIG_DETAILS
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <set>

class Config_Details
{
public:

	std::string incremental_multicast_addr = "";
	int incremental_multicast_port = 0;
	std::string interfacea_Addr = "";



	std::string tempatePath = "";
	std::string template_string="";
	std::string token_path = "";
	std::string filtered_path = "";
	std::vector<uint64_t> tokens_vector;
	std::vector<uint64_t> tokens_vector_check_tbt;
	std::set<std::string> symbol_set;

	bool enable_Unicast;
	std::string publisher_uni_ip;
	int publisher_uni_port;

	bool enable_Multicast;
	std::string publisher_multi_ip;
	int publisher_multi_port;
	std::string publisher_interface;


	bool enable_log;

	bool listenLiveRequestFlag;
	bool listenTokenFlag;
	std::string exchangeName;
	std::string rabbit_ip;
	int rabbit_port;

	std::string rabbit_ip_for_listen;
	int rabbit_port_for_listen;

private:

};



#endif