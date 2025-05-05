#include"Application.h"
namespace FIN::IIBX {//C++17
	Application::Application(int exchId)//: publisher(exchId, io_service)
	{
		this->exchId = exchId;
		init();
	}

	Application::~Application()
	{
	}
	void Application::init()
	{
		//read config
		config_param.loadData(currPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
		tcpRcvIp = config_param.getValue<std::string>(FIN::FIN_CONST::IIBXEXCHANGEDETAILS + FIN::FIN_CONST::DOT + FIN::FIN_CONST::tcp_ip, FIN::FIN_CONST::EMPTY_STRING);
		tcpRcvPort = config_param.getValue<int>(FIN::FIN_CONST::IIBXEXCHANGEDETAILS + FIN::FIN_CONST::DOT + FIN::FIN_CONST::tcp_port, FIN::FIN_CONST::EMPTY_INTEGER);
		host_interface_ip= config_param.getValue<std::string>(FIN::FIN_CONST::IIBXEXCHANGEDETAILS + FIN::FIN_CONST::DOT + FIN::FIN_CONST::listen_interface, FIN::FIN_CONST::EMPTY_STRING);
		std::string isLogEnabled = FIN::FIN_CONST::PUBLISHER + FIN::FIN_CONST::DOT + FIN::FIN_CONST::Log_Enabled;

		isLogEnable= config_param.getValue<bool>(isLogEnabled, false);

		//read token file and store in memory
		_secHandler.registerFile(currPath.getCurrentPath() + FIN::FIN_CONST::FIN_TOKEN_FILE);

		FIN::IIBX::Publisher::getInstance()->init(io_service, currPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);

		try {
			//SecurityFileHandler secHandler;					
			symblSet = _secHandler._symbolsInFile;

			//Symbol mapping handling
			for (auto itr = symblSet.begin(); itr != symblSet.end(); itr++)
			{
				std::string instrument = *itr;				
				m_mdatamap.insert(std::make_pair(instrument, std::make_unique<SymbolProcess>(exchId, instrument)));
			}

		}
		catch (std::exception& e)
		{
			std::cout << "Exception in Securty file : " << e.what();
		}
		start();

	}
	void Application::start()
	{		
		//make connection
		if (createTCPConnection())
		{
			recvr.init(host_ip_res, host_port_res, host_interface_ip);


			std::thread heartbeat(&FIN::IIBX::Application::checkConnection, this);
			heartbeat.detach();
			
			while (true)
			{
				//process msg
				std::string packet = recvr.multicast_q.front();
				handleMessage(packet);

				//delete
				recvr.multicast_q.pop();
			}
		}
		else
		{
			std::cout << "Retry after 10 second..." << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
			start();
		}
	}

	void Application::checkConnection()
	{
		while (1)
		{
			std::this_thread::sleep_for(std::chrono::seconds(30));
			if (!isConnection)
			{
				FIN::setConsoleColor(4);
				std::cout << "Check VPN Connection..." << std::endl;
				FIN::setConsoleColor(7);
			}
			isConnection = false;
		}
	}

	bool Application::createTCPConnection()
	{
		try
		{
			std::cout << "Connecting to IEC Lookup IP : " << tcpRcvIp
				<< " : " << tcpRcvPort << std::endl;
			LOG_INFO << "Connecting to IEC Lookup IP : " << tcpRcvIp << " : " << tcpRcvPort;
			// create socket
			boost::asio::io_context io_context;

			// Define server and port
			std::string host = tcpRcvIp;
			std::string port = std::to_string(tcpRcvPort);

			// Resolve the host and port to endpoints
			tcp::resolver resolver(io_context);
			auto endpoints = resolver.resolve(host, port);

			// Create a socket
			tcp::socket socket(io_context);

			// Connect to the server
			boost::asio::connect(socket, endpoints);

			// Check if the socket is connected
			if (socket.is_open()) 
			{
				FIN::setConsoleColor(2);
				std::cout << "Connected to IEC Lookup Service!" << std::endl;
				FIN::setConsoleColor(7);

				const size_t bufferSize = 159; //
				char data[bufferSize];

				//read data from the socket
				while(1)
				{
					if (!socket.is_open()) {
						std::cout << "Socket is closed. Exiting read loop." << std::endl;
						break;
					}

					boost::system::error_code error;
					size_t length = socket.read_some(boost::asio::buffer(data, bufferSize), error);

					// Check for end of file or any error
					if (error == boost::asio::error::eof) {
						std::cout << "Connection closed by the server." << std::endl;
						break; // Connection closed cleanly by peer
					}
					else if (error) {
						std::cerr << "Error: " << error.message() << std::endl;
						break;
					}
					
					//process response
					int resCode;
					std::memcpy(&resCode, data + 8, sizeof(int));
					if (resCode == 0)
					{
						std::string ip(data + 131, 15);
						host_ip_res = ip;

						int port;
						std::memcpy(&port, data + 146, sizeof(int));
						host_port_res = port;

						LOG_INFO << "IEC response : IP : " << host_ip_res;
						LOG_INFO << "Port : " << std::to_string(host_port_res);

						socket.close();
						std::cout << "Close IEC Lookup Service connection." << std::endl;

						return true;
					}
					else
					{
						FIN::setConsoleColor(4);
						std::cout << "IEC Lookup Response code: " << resCode << std::endl;
						FIN::setConsoleColor(7);
						return false;
					}
				}
			}
			else 
			{
				FIN::setConsoleColor(4);
				std::cerr << "Failed to connect to the server." << std::endl;
				FIN::setConsoleColor(7);
			}
			return false;
		}
		catch (std::exception& e) 
		{
			FIN::setConsoleColor(4);
			std::cerr << "Exception: " << e.what() << std::endl;
			FIN::setConsoleColor(7);
		}
		return false;
	}

	void Application::handleMessage(std::string& msg)
	{
		std::stringstream ss(msg);
		std::string token;
		std::vector<std::string> vect;

		while (std::getline(ss, token, '|')) {
			vect.push_back(token); // Store each separated value in the vector
		}

		if (vect[2] == "BC_MARKET_PICTURE")
		{
			std::string securityID = vect[3];
			if (symblSet.find(securityID) != symblSet.end())
			{
				m_mdatamap[securityID]->processMarketPicture(vect);
				if (isLogEnable)
					LOG_INFO << "Packet: " << msg;
			}
		}
		else if(vect[2] == "BC_TIME")
		{
			LOG_INFO << "Heartbeat receive msgType : BC_TIME";
			isConnection = true;
		}
		else if(vect[2] == "BC_SESSION_CHANGE")
		{
			LOG_INFO << "BC_SESSION_CHANGE : " << msg;
		}
		else if (vect[2] == "BC_CONTRACT_DPR_HIT")
		{
			LOG_INFO << "BC_CONTRACT_DPR_HIT : " << msg;
		}
		else if(vect[2] == "BC_CONTRACT_DPR_RELAXED")
		{
			std::string securityID = vect[3];
			if (symblSet.find(securityID) != symblSet.end())
			{
				m_mdatamap[securityID]->processDPRPrice(vect);
				if (isLogEnable)
					LOG_INFO << "Packet: " << msg;
			}
			LOG_INFO << "BC_CONTRACT_DPR_RELAXED : " << msg;
		}
		else if(vect[2] == "BC_TRADE")
		{
			//LOG_INFO << "BC_TRADE : " << msg;
		}
		else if(vect[2] == "BC_CONTRACT_ACTION")
		{
			std::string securityID = vect[3];
			if (symblSet.find(securityID) != symblSet.end())
			{
				m_mdatamap[securityID]->processContractAction(vect);
				if (isLogEnable)
					LOG_INFO << "Packet: " << msg;
			}
			LOG_INFO << "BC_CONTRACT_ACTION : " << msg;
		}
		else if(vect[2] == "BC_MKTRESM_ORDRET")
		{
			auto itr = symblSet.begin();
			if (itr != symblSet.end())
			{
				m_mdatamap[*itr]->processMarketAction(vect);
				if (isLogEnable)
					LOG_INFO << "Packet: " << msg;
			}
			LOG_INFO << "BC_MKTRESM_ORDRET : " << msg;
		}
		else
		{
			std::cout << "Not handle msgType : " << vect[2] << std::endl;
			LOG_INFO << "Not handle msgType : " << vect[2];
		}
	}
}