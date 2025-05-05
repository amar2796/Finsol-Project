#include "InitializeProcess.h"

FIN::Initialize_process::Initialize_process() :emdi_snap(&conf_Det)
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

		conf_Det.incremental_multicast_addr = confReader.getValue < std::string >(
			FIN::FIN_CONST::INCREMENTAL + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::multicast_addr,
			FIN::FIN_CONST::EMPTY_STRING);


		conf_Det.incremental_multicast_port = confReader.getValue < FIN::INT_32 >(
			FIN::FIN_CONST::INCREMENTAL + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::multicast_port,
			FIN::FIN_CONST::EMPTY_INTEGER);

		conf_Det.interfacea_Addr = confReader.getValue < std::string >(
			FIN::FIN_CONST::INCREMENTAL + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Interface_Addr,
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

		conf_Det.tempatePath = _currentPath.getCurrentPath() + FIN::FIN_CONST::FAST_TEMPLATE;
		conf_Det.token_path = _currentPath.getCurrentPath() + FIN::FIN_CONST::TOKEN;
		conf_Det.template_string = read_emdi_tempate_file();
		conf_Det.contract_path = confReader.getValue < std::string >(
			FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + "ContractPath",
			FIN::FIN_CONST::EMPTY_STRING);

		readTokenFromFile();

		//process live subscription
		conf_Det.isEnableLiveSubscription = confReader.getValue<bool>(
			FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::ISLiveRequest,
			false);
		if (conf_Det.isEnableLiveSubscription)
		{
			conf_Det.rabbitIp = confReader.getValue < std::string >(
				FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RabbitIP,
				FIN::FIN_CONST::EMPTY_STRING);

			conf_Det.rabbitPort = confReader.getValue < FIN::INT_32 >(
				FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RabbitPort,
				FIN::FIN_CONST::EMPTY_INTEGER);

			std::thread rabbitSubProcess(&FIN::Initialize_process::startRabbitSubscriptionProcessing, this, conf_Det.rabbitIp, conf_Det.rabbitPort);
			rabbitSubProcess.detach();
		}

		start_processing();
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex);
	}
}

void FIN::Initialize_process::startRabbitSubscriptionProcessing(std::string rabbit_ip, int rabbit_port)
{
	sendAckToOMS.connect(rabbit_ip, rabbit_port);
	connect(rabbit_ip, rabbit_port);
}

std::string FIN::Initialize_process::processSymbolUpdateFromOMS(bool flag, std::string security_id, std::string symbol)
{
	//std::cout << "[INFO ]Instrument request " << security_id << std::endl;
	//std::cout << "Receive From OMS";
	std::string result;
	int returnValue;
	if (flag)
	{
		returnValue = handlingSubscription(security_id, symbol);
	}
	else
	{
		returnValue = handlingUnSubscription(security_id, symbol);
	}

	result = std::to_string(returnValue) + "|" + security_id;
	//result = std::to_string(returnValue) + "|" + std::string("10001094574538769537");
	sendAckToOMS.publishAck(result);					//send acknoleage to oms
	return result;
}



int  FIN::Initialize_process::handlingSubscription(std::string security_id, std::string symbol)
{
	//if (isValidSymbol(symbol))										//check symbol
	//{
		uint64_t security = stoull(security_id);
		if (!isSecurityIDSubscribed(security))						//check securityID
		{
			add_securityid_in_vector(security);						//add in vector
			add_securityid_in_file(security_id);					//update in file
			emdi_snap.add_memory_live_request(security);			//add in snapshot map
			LOG_INFO << "SecurityID Subscribed : " << security_id;
			return 1;
		}
		else
		{
			LOG_INFO << "SecurityID Already Subscribed : " << security_id;
			std::cout << "SecurityID Already Subscribed : " << security_id << std::endl;
			return -5;
		}
	/*}
	else
	{
		std::cout << "Symbol is not allowed for this gateway : " << symbol << std::endl;
		LOG_INFO << "Symbol is not allowed for this gateway : " << symbol;
		return -10;
	}*/
}

int FIN::Initialize_process::handlingUnSubscription(std::string security_id, std::string symbol)
{
	int64_t security = stoull(security_id);
	if (isSecurityIDSubscribed(security))							//check securityID
	{
		delete_securityid_from_vector(security);					//delete in vector
		delete_securityid_from_file(security_id);					//update in file
		emdi_snap.delete_memory_live_request(security);				//delete in snapshot map
		std::cout << "SecurityID Unsubscribed : " << security_id << std::endl;
		LOG_INFO << "SecurityID Unsubscribed : " << security_id;
		return 2;
	}
	else
	{
		std::cout << "SecurityID Already UnSubscribed : " << security_id << std::endl;
		LOG_INFO << "SecurityID Already UnSubscribed : " << security_id;
		return -6;
	}
}

//bool FIN::Initialize_process::isValidSymbol(const std::string symbol)
//{
//	if (conf_Det.symbol_set.empty())
//		return true;
//	else
//		return conf_Det.symbol_set.find(symbol) != conf_Det.symbol_set.end();
//
//	return false;
//}

bool FIN::Initialize_process::isSecurityIDSubscribed(uint64_t security_id)
{
	if (std::find(conf_Det.tokens_vector.begin(), conf_Det.tokens_vector.end(), security_id) != conf_Det.tokens_vector.end())
		return true;
	else
		return false;
}

void FIN::Initialize_process::add_securityid_in_vector(uint64_t security_id)
{
	conf_Det.tokens_vector.push_back(security_id);
}

void FIN::Initialize_process::delete_securityid_from_vector(uint64_t security_id)
{
	try
	{
		conf_Det.tokens_vector.erase(std::remove(conf_Det.tokens_vector.begin(), conf_Det.tokens_vector.end(),
			security_id), conf_Det.tokens_vector.end());
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << ex.what();
	}
}

void FIN::Initialize_process::add_securityid_in_file(std::string security_id)
{
	try
	{
		std::fstream myfile(conf_Det.token_path, std::ios::app);
		if (myfile.is_open())
		{
			if (conf_Det.tokens_vector.size() > 1)
				myfile << "\n";
			myfile << security_id;

			myfile.close();
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Unable to read Token file : " << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}
}

void FIN::Initialize_process::delete_securityid_from_file(std::string security_id)
{
	try
	{
		std::vector<std::string> symVec;
		std::vector< std::string >::iterator itr;

		std::fstream myfile(conf_Det.token_path, std::ios::in);
		std::string line;

		int countLine = 0;

		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				if (line != security_id && !line.empty())
				{
					symVec.push_back(line);
					countLine++;
				}
			}
			myfile.close();
		}

		std::ofstream myFile(conf_Det.token_path, std::ios::trunc);
		if (symVec.size() > 0)
		{
			if (myFile.is_open())
			{

				for (itr = symVec.begin(); itr != symVec.end(); itr++)
				{
					--countLine;
					myFile << (*itr);
					if (symVec.size() > 0 && countLine != 0)
						myFile << "\n";
				}
				myFile.close();

			}
		}
	}
	catch (const std::exception& ex)
	{
		std::cout << "Unable to read Token file : " << ex.what() << std::endl;
		LOG_INFO << ex.what();
	}
}

void FIN::Initialize_process::start_processing()
{
	try
	{

		emdi_snap.init();

	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex);
	}

}

std::string FIN::Initialize_process::read_emdi_tempate_file()
{
	std::string result_string = "";
	try
	{
		std::fstream newfile;
		newfile.open(conf_Det.tempatePath, std::ios::in);
		if (newfile.is_open()) {
			std::string tp;
			while (getline(newfile, tp))
			{
				result_string += tp;
			}
			newfile.close();
		}

	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex) << " \n " << boost::diagnostic_information_what(ex);
	}	
	return result_string;
}

void FIN::Initialize_process::readTokenFromFile()
{
	try
	{
		std::fstream newfile;
		newfile.open(conf_Det.token_path, std::ios::in);
		if (newfile.is_open()) {
			std::string tp;
			while (getline(newfile, tp))
			{
				conf_Det.tokens_vector.push_back(std::stoll(tp));
			}
			newfile.close();
		}


	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex);
	}
}
