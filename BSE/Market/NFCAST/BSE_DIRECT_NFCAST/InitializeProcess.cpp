#include "InitializeProcess.h"



FIN::Initialize_process::Initialize_process(UNSIGNED_INT_16 exchID) : nfcast_snap(&conf_Det, exchID)
{
	LOG_INFO << "Initialize process ";
}

void FIN::Initialize_process::init()
{

	try
	{
		ConfigReader confReader;
		CurrentPath _currentPath;
		confReader.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);


		conf_Det.snapshot_multicast_addr = confReader.getValue < std::string >(
			FIN::FIN_CONST::SNAPSHOT + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::multicast_addr_A,
			FIN::FIN_CONST::EMPTY_STRING);

		conf_Det.snapshot_multicast_port = confReader.getValue < FIN::INT_32 >(
			FIN::FIN_CONST::SNAPSHOT + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::multicast_port_A,
			FIN::FIN_CONST::EMPTY_INTEGER);

		conf_Det.interfacea_Addr = confReader.getValue < std::string >(
			FIN::FIN_CONST::SNAPSHOT + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Interface_Addr,
			FIN::FIN_CONST::EMPTY_STRING);

		conf_Det.publisher_ip = confReader.getValue < std::string >(
			FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Unicast_Addr,
			FIN::FIN_CONST::EMPTY_STRING);

		conf_Det.publisher_port = confReader.getValue < FIN::INT_32 >(
			FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Unicast_Port,
			FIN::FIN_CONST::EMPTY_INTEGER);

		conf_Det.enable_log = confReader.getValue < bool >(
			FIN::FIN_CONST::LOGGING + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Enable_Log,
			false);

		conf_Det.token_path = _currentPath.getCurrentPath() + FIN::FIN_CONST::TOKEN;

		start_processing();
	}
	catch (exception& ex)
	{
		LOG_INFO << ex.what();
	}


}

void FIN::Initialize_process::start_processing()
{
	try
	{

		nfcast_snap.init();

	}
	catch (exception& ex)
	{
		LOG_INFO << ex.what();
	}

}
