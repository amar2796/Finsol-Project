#include "InitializeProcess.h"

FIN::Initialize_process::Initialize_process() :emdi_snap(&conf_Det)
{
	LOG_INFO << "Initialize process ";
}

void FIN::Initialize_process::setConsoleColor(int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
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

		//for udp unicast
		conf_Det.enable_Unicast = confReader.getValue < bool >(
			FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Use_Unicast,
			false);

		conf_Det.publisher_uni_ip = confReader.getValue < std::string >(
			FIN::FIN_CONST::UDP_UNICAST_PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Unicast_Addr,
			FIN::FIN_CONST::EMPTY_STRING);

		conf_Det.publisher_uni_port = confReader.getValue < FIN::INT_32 >(
			FIN::FIN_CONST::UDP_UNICAST_PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Unicast_Port,
			FIN::FIN_CONST::EMPTY_INTEGER);

		//for udp multicast
		conf_Det.enable_Multicast = confReader.getValue < bool >(
			FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Use_Multicast,
			false);

		conf_Det.publisher_multi_ip = confReader.getValue < std::string >(
			FIN::FIN_CONST::UDP_MULTICAST_PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Multicast_addr,
			FIN::FIN_CONST::EMPTY_STRING);

		conf_Det.publisher_multi_port = confReader.getValue < FIN::INT_32 >(
			FIN::FIN_CONST::UDP_MULTICAST_PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Multicast_port,
			FIN::FIN_CONST::EMPTY_INTEGER);

		conf_Det.publisher_interface = confReader.getValue < std::string >(
			FIN::FIN_CONST::UDP_MULTICAST_PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Listen_interface,
			FIN::FIN_CONST::EMPTY_STRING);

		conf_Det.enable_log = confReader.getValue < bool >(
			FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::Enable_Log,
			false);

		conf_Det.tempatePath = _currentPath.getCurrentPath() + FIN::FIN_CONST::FAST_TEMPLATE;
		conf_Det.token_path = _currentPath.getCurrentPath() + FIN::FIN_CONST::TOKEN;
		conf_Det.filtered_path = _currentPath.getCurrentPath() + FIN::FIN_CONST::FILLTEREDSYMBOL;
		conf_Det.template_string = read_emdi_tempate_file();

		conf_Det.listenLiveRequestFlag = confReader.getValue < bool >(
			FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::ISLiveRequest,
			false);

		//for listen token from tbt
		conf_Det.listenTokenFlag = confReader.getValue < bool >(
			FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::GetTbt_Token,
			false);
		if (conf_Det.listenLiveRequestFlag && conf_Det.listenTokenFlag)
		{
			conf_Det.rabbit_ip_for_listen = confReader.getValue < std::string >(
				FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RabbitIP,
				FIN::FIN_CONST::EMPTY_STRING);

			conf_Det.rabbit_port_for_listen = confReader.getValue < FIN::INT_32 >(
				FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RabbitPort,
				FIN::FIN_CONST::EMPTY_INTEGER);
			std::thread rabbitSubProcess(&FIN::Initialize_process::startRabbitListenToken, this, conf_Det.rabbit_ip_for_listen, conf_Det.rabbit_port_for_listen);
			rabbitSubProcess.detach();



		}

		readTokenFromFile();
		readFiltteredSymbolFile();

		//for livesubscrion process
		if (conf_Det.listenLiveRequestFlag)
		{
			//read config rabbit ip, port
			conf_Det.rabbit_ip = confReader.getValue < std::string >(
				FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RabbitIP,
				FIN::FIN_CONST::EMPTY_STRING);

			conf_Det.rabbit_port = confReader.getValue < FIN::INT_32 >(
				FIN::FIN_CONST::RABBITLIVESUBSCRIPTION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::RabbitPort,
				FIN::FIN_CONST::EMPTY_INTEGER);


			std::thread rabbitSubProcess(&FIN::Initialize_process::startRabbitSubscriptionProcessing, this, conf_Det.rabbit_ip, conf_Det.rabbit_port);
			rabbitSubProcess.detach();
		}

		start_processing();
	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex);
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

void FIN::Initialize_process::storeTokenInVector(std::string token)
{
	conf_Det.tokens_vector_check_tbt.push_back(std::stoull(token));
}


std::vector<std::string> FIN::Initialize_process::readTokenFromFile_forTbt()
{
	try
	{
		std::vector<std::string> snap_token;
		std::ifstream newfile(conf_Det.token_path);
		if (newfile.is_open()) {
			std::string tp;
			while (getline(newfile, tp))
			{
				if (!tp.empty())
				{
					snap_token.push_back(tp);
				}
			}
			newfile.close();
		}
		return snap_token;


	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex);
	}
}
void FIN::Initialize_process::readTokenFromFile()
{
	try
	{
		std::ifstream newfile(conf_Det.token_path);
		if (newfile.is_open()) {
			std::string tp;
			while (getline(newfile, tp))
			{
				if (!tp.empty())
				{
					if (!isSecurityIDSubscribedInTBT(std::stoull(tp)))
						conf_Det.tokens_vector.push_back(std::stoull(tp));
					else
					{
						setConsoleColor(4);
						std::cout << "Token is already subscribed in TBT : " << tp << std::endl;
						setConsoleColor(7);
					}
				}
			}
			newfile.close();
		}


	}
	catch (boost::exception& ex)
	{
		LOG_INFO << boost::diagnostic_information(ex);
	}
}

void FIN::Initialize_process::readFiltteredSymbolFile()
{
	std::string line;
	std::ifstream myfile(conf_Det.filtered_path);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			if (!line.empty())
				conf_Det.symbol_set.insert(line);
		}
		myfile.close();
	}
}

void FIN::Initialize_process::startRabbitListenToken(std::string rabbit_ip, int rabbit_port)
{
	listenToken.connectToSendMsgTBT(rabbit_ip, rabbit_port, "Producer");
	Sleep(2000);
	
	listenToken.sendToTBT("Token Request");
	for (auto token : readTokenFromFile_forTbt())
		listenToken.sendToTBT(token);
	listenToken.sendToTBT("Done");

	std::cout << "Request send sucessfully to listen Token from TBT" << std::endl;
	connectForTokenRead(rabbit_ip, rabbit_port, "Consumer");

	
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
	if (isValidSymbol(symbol))										//check symbol
	{
		uint64_t security = stoull(security_id);
		if (!isSecurityIDSubscribedInTBT(security))
		{
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
				setConsoleColor(12);
				std::cout << "SecurityID Already Subscribed		 : " << security_id << std::endl;
				setConsoleColor(7);
				return -5;
			}
		}
		else
		{
			LOG_INFO << "SecurityID Already Subscribed in TBT : " << security_id;
			setConsoleColor(12);
			std::cout << "SecurityID Already Subscribed in TBT : " << security_id << std::endl;
			setConsoleColor(7);
			return -5;
		}
	}
	else
	{
		std::cout << "Symbol is not allowed for this gateway : " << symbol << std::endl;
		LOG_INFO << "Symbol is not allowed for this gateway : " << symbol;
		return -10;
	}
}

int FIN::Initialize_process::handlingUnSubscription(std::string security_id, std::string symbol)
{
	int64_t security = stoull(security_id);
	if (isSecurityIDSubscribed(security))							//check securityID
	{
		delete_securityid_from_vector(security);					//delete in vector
		delete_securityid_from_file(security_id);					//update in file
		emdi_snap.delete_memory_live_request(security);				//delete in snapshot map
		LOG_INFO << "SecurityID Unsubscribed : " << security_id;
		return 2;
	}
	else
	{
		setConsoleColor(12);
		std::cout << "SecurityID Already UnSubscribed		 : " << security_id << std::endl;
		setConsoleColor(7);
		LOG_INFO << "SecurityID Already UnSubscribed : " << security_id;
		return -6;
	}
}

bool FIN::Initialize_process::isValidSymbol(const std::string symbol)
{
	if (conf_Det.symbol_set.empty())
		return true;
	else
		return conf_Det.symbol_set.find(symbol) != conf_Det.symbol_set.end();

	return false;
}

bool FIN::Initialize_process::isSecurityIDSubscribed(uint64_t security_id)
{
	if (std::find(conf_Det.tokens_vector.begin(), conf_Det.tokens_vector.end(), security_id) != conf_Det.tokens_vector.end())
		return true;
	else
		return false;
}

bool FIN::Initialize_process::isSecurityIDSubscribedInTBT(uint64_t security_id)
{
	if (std::find(conf_Det.tokens_vector_check_tbt.begin(), conf_Det.tokens_vector_check_tbt.end(), security_id) != conf_Det.tokens_vector_check_tbt.end())
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



