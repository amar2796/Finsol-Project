#ifndef FIN_CONFIG_DETAILS
#define FIN_CONFIG_DETAILS
#include <string>
#include <iostream>
#include <vector>

class Config_Details
{
public:

	std::string snapshot_multicast_addr = "";
	int snapshot_multicast_port = 0;
	std::string interfacea_Addr = "";

	std::string tempatePath = "";
	std::string template_string="";
	std::string token_path = "";

	std::string publisher_ip;
	int publisher_port;

	bool enable_log;

private:

};



#endif