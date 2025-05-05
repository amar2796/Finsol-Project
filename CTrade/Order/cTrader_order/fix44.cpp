#include "fix44.h"
#include <quickfix/Message.h>

namespace FIN
{
	void FIX44Adapter::init()
	{
		ConfigReader confR;
		confR.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
		std::string config = confR.getValue < std::string >(
			FIN::FIN_CONST::TARGET_COMP_ID + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::FILE_NAME,
			FIN::FIN_CONST::EMPTY_STRING);


		sender_ip = confR.getValue<string>("UNICAST.SENDER_IP", "");
		sender_port = confR.getValue<int>("UNICAST.SENDER_PORT", 0);
		recever_port = confR.getValue<int>("UNICAST.RECEIVER_PORT", 0);



		//ReadSymbolMapping(config);
		_tcpPort = confR.getValue<int>("DEFAULT.TCP_CONNFIRMATION_PORT", 0);
		rabbit_udp = confR.getValue<bool>("COMM_CHANNEL.RABBIT_UDP", false);

		confR.loadData(config);

		//_gatewayLogEnabled = confR.getValue<bool>("DEFAULT.Enable_Gateway_Log", false);
		_boostLogEnabled = confR.getValue<bool>("DEFAULT.Enable_Nano_Log", false);



		if (_boostLogEnabled)
		{
			std::string logPath = confR.getValue<std::string>("DEFAULT.FileLogPath", "");
			initializeLogging(logPath);
		}

		_settings = new FIX::SessionSettings(config);
		_storeFactory = new FIX::FileStoreFactory(*_settings);
		_screenLogFactory = new FIX::ScreenLogFactory(*_settings);
		_logFactory = new FIX::FileLogFactory(*_settings);
		_initiator = new FIX::SocketInitiator(*this, *_storeFactory, *_settings, *_logFactory);
		//BOOST_LOG_TRIVIAL(info) << "[INTEGRAL] INIT";
		LOG_INFO<< "[INTEGRAL] INIT";
	}
	void FIX44Adapter::ReadSymbolMapping(std::string configPath)
	{
		try
		{
			std::string first_string;
			std::string second_string;
			bool nowProcess = false;
			size_t position;
			std::string line;
			std::ifstream myfile(configPath);
			if (myfile.is_open())
			{
				while (getline(myfile, line))
				{
					if (line == "") continue;
					if (line == "[SECURITY_MAPPING]")
					{
						nowProcess = true;
						continue;
					}
					else if (nowProcess)
					{
						position = line.find("=");
						if (position != std::string::npos)
						{
							first_string = line.substr(0, position);
							second_string = line.substr(position + 1, line.length() - position);
							_symbol_mapping.insert(std::make_pair(first_string, second_string));
						}
					}
				}
				myfile.close();
				nowProcess = false;
			}
		}
		catch (std::exception ex)
		{
			std::cout << ex.what() << std::endl;
		}
	}
	void FIX44Adapter::listenLogOutFromConsole()
	{
		while (1)
		{
			//print msg
			std::cout << "Enter \"logout\" without quotes to logout from exchange" << std::endl;
			std::string consoleMsg = "";
			//wait for input
			std::cin >> consoleMsg;
			//value compare
			if (consoleMsg == "logout")
			{
				//create class object
				FIX44::Logout msg;
				std::cout << "You have enered :" << consoleMsg << std::endl;
				if (_loggedIn)
				{
					//send logon msg
					if (!FIX::Session::sendToTarget(msg, _sessionID))
					{
						FIN_ERROR("Error in Sending LOGOUT Request");
					}
				}
				else
				{
					FIN_ERROR("Please Login First");
				}
			}
		}
	}
	void FIX44Adapter::startOmsRcv()
	{
		//initilize value
		bool first = true;
		//infinite while loop
		while (true) {
			//check condition (Exchange logon)
			if (_loggedIn)
			{
				//again check condition
				if (first)
				{
					//call function
					doOmsConnect();
					first = false;
				}                                                                                                                                                                             try
				{
					//check condition rabbitmq or udp
					if (!rabbit_udp)
						StartReceiving();
					else
						rabbitMqConsumer();
				}
				catch (std::exception& e)
				{
					restartOmsConnections();
				}
			}
			else
			{
				Sleep(10);
			}
		}
	}
	void FIX44Adapter::doOmsConnect()
	{
		//print msg
		FIN_DEVLOG("Initiating OMS Connection");
		//check condition rabbitMq or udp
		if (rabbit_udp)
		{
			//call function rabbitmqConsumer, It will connect rabbitmq to the socket 
			RabbitMqConsumer::connect(
				_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE,
				_senderCompId, _exchangeNumber
			);

			//call function rabbitmqProducer side
			_omsRabbitSender.RabbitMqConnection(
				_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE,
				_senderCompId, _exchangeNumber
			);
		}
		else
		{
			//call udp_recevier class init function
			UDP_RECEVIER::init(recever_port);
			//call init function oms_udp_sender
			oms_udp_sender.init(sender_ip, sender_port);
		}

		//call function init 
		_omsSender.init(_tcpPort);
		//call funciton
		_omsSender.accept();
		//call function
		_omsSender.startOmsHealthThread();
	}
	void FIX44Adapter::start()
	{
		//BOOST_LOG_TRIVIAL(info) << "[INTEGRAL] START";
		LOG_INFO<< "[INTEGRAL] START";
		_initiator->start();
	}
	void FIX44Adapter::stop() {
		_initiator->stop();
	}

	void FIX44Adapter::receiveFromOMS(const std::string& omsInput)
	{
		try
		{
			//if msg is not empty
			if (omsInput != "")
			{
				//parse string from msg
				_fixStringParser.loadString(omsInput);

				//check condition and call function
				switch (_fixStringParser.getChar(Fix_MsgType))
				{
				case Fix_MsgType_NEW_ORDER: sendNewOrderOMS(); break;
				case Fix_MsgType_CANCEL_ORDER:sendCancelOrderOMS(); break;
				case Fix_MsgType_MODIFY_ORDER:sendCancel_ReplaceOrderOMS(); break;
				default:FIN_DEVLOG("Invalid Message Type");
				}
				_fixStringParser.reset();
			}
		}
		catch (std::exception& e)
		{
			FIN_DEVLOG("Exception : " << e.what());
		}
	}

	void FIX44Adapter::sendNewOrderOMS()
	{
		FIX::ClOrdID f_clOrderId(_fixStringParser.get(Fix_ClOrdID));
		FIX::Side f_side(_fixStringParser.getChar(Fix_Side));
		FIX::OrdType f_ordType(_fixStringParser.getChar(Fix_OrdType));
		FIX::TransactTime f_transactTime;
		FIX44::NewOrderSingle f_order(f_clOrderId, f_side, f_transactTime, f_ordType);

		auto time_inforce = _fixStringParser.getChar(Fix_TimeInForce);


		//f_order.set(FIX::SenderSubID(_fixStringParser.get(Fix_SenderSubID).c_str()));
		//f_order.set(FIX::TargetSubID(_fixStringParser.get(Fix_TargetSubID).c_str()));

		f_order.set(FIX::TimeInForce(time_inforce));
		switch (f_ordType)
		{
		case FIX::OrdType_MARKET:
		{
		}
		break;
		case FIX::OrdType_LIMIT:
		{
			f_order.set(FIX::Price(std::stod(_fixStringParser.get(Fix_Price).c_str())));
		}
		break;
		case FIX::OrdType_STOP:
		{
			f_order.set(FIX::StopPx(std::stod(_fixStringParser.get(Fix_StopPX).c_str())));
		}
		default:
		{}
		break;
		}


		f_order.set(FIX::Symbol(_fixStringParser.get(Fix_Symbol)));

		f_order.set(FIX::OrderQty(std::atol(_fixStringParser.get(Fix_OrdQty).c_str())));
		if (!FIX::Session::sendToTarget(f_order, _sessionID))
		{
			FIN_ERROR("Error in Sending Order"
				<< _fixStringParser.get(Fix_OrigClOrdID));
		}
	}

	void FIX44Adapter::sendCancelOrderOMS()
	{
		FIX44::OrderCancelRequest f_order;
		f_order.set(FIX::OrigClOrdID(_fixStringParser.get(Fix_OrigClOrdID)));
		f_order.set(FIX::ClOrdID(_fixStringParser.get(Fix_ClOrdID)));

		//(Side) indicate whether an order is a buy or sell order
		//f_order.set(FIX::Side(_fixStringParser.getChar(Fix_Side)));

		//f_order.set(FIX::TransactTime());



		//f_order.set(FIX::Account(_fixStringParser.get(Fix_Account)));
		//f_order.set(FIX::OrderID(_fixStringParser.get(Fix_OrderID)));


		if (!FIX::Session::sendToTarget(f_order, _sessionID))
		{
			FIN_ERROR("Error in sending order : " << _fixStringParser.get(Fix_OrigClOrdID));
		}
	}

	void FIX44Adapter::sendCancel_ReplaceOrderOMS() {
		FIX44::OrderCancelReplaceRequest r_order;
		//r_order.set(FIX::Account(_fixStringParser.get(Fix_Account)));
		r_order.set(FIX::ClOrdID(_fixStringParser.get(Fix_ClOrdID)));

		//r_order.set(FIX::OrderID(_fixStringParser.get(Fix_OrderID)));

		r_order.set(FIX::OrigClOrdID(_fixStringParser.get(Fix_OrigClOrdID)));
		r_order.set(FIX::Price(std::stod(_fixStringParser.get(Fix_Price).c_str())));
		//r_order.set(FIX::Symbol(_fixStringParser.get(Fix_Symbol)));
		r_order.set(FIX::OrderQty(std::atol(_fixStringParser.get(Fix_OrdQty).c_str())));


		//r_order.set(FIX::TransactTime());

		if (!FIX::Session::sendToTarget(r_order, _sessionID))
		{
			FIN_ERROR("Error in sending order : " << _fixStringParser.get(Fix_OrigClOrdID));
		}

	}

	void FIX44Adapter::sendSelfLogin()
	{
		//FIX::Session::lookupSession(_sessionID)->logon();
		init();
		start();

	}

	void FIX44Adapter::restartOmsConnections()
	{
		FIN_DEVLOG("Restarting OMS Connection");
		_omsSender.init(_tcpPort);
		if (rabbit_udp)
		{
			RabbitMqConsumer::connect(
				_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE,
				_senderCompId, _exchangeNumber
			);
			_omsRabbitSender.RabbitMqConnection(
				_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE,
				_senderCompId, _exchangeNumber
			);
		}
		else
		{
			UDP_RECEVIER::init(recever_port);
			oms_udp_sender.init(sender_ip, sender_port);
		}

		_omsSender.accept();
		_omsSender.startOmsHealthThread();
	}

	void FIX44Adapter::onCreate(const FIX::SessionID& sessionId)
	{
		if (_settings->get(sessionId).has("Password"))
		{
			_password = _settings->get(sessionId).getString("Password");
		}
		if (_settings->get(sessionId).has("Key"))
		{
			_key = _settings->get(sessionId).getString("Key");
		}
		if (_settings->get(sessionId).has("Username"))
		{
			_username = _settings->get(sessionId).getString("Username");
		}
		if (_settings->get(sessionId).has("SenderSubId"))
		{
			_senderSubID = _settings->get(sessionId).getString("SenderSubId");
		}
		if (_settings->get(sessionId).has("SendSecList"))
		{
			_sendSecurityListReq = _settings->get(sessionId).getBool("SendSecList");
		}
		/*if (_settings->get(sessionId).has("ScreenLogShowIncoming"))
		{
			_isLogEnabledIncoming = _settings->get(sessionId).getBool("ScreenLogShowIncoming");
		}
		if (_settings->get(sessionId).has("ScreenLogShowOutgoing"))
		{
			_isLogEnabledOutgoing = _settings->get(sessionId).getBool("ScreenLogShowOutgoing");
		}*/
		if (_settings->get(sessionId).has("SenderCompID"))
		{
			_senderCompId = _settings->get(sessionId).getString("SenderCompID");
		}
		/*if (_settings->get(sessionId).has("TradeFileCounter"))
		{
			_max = _settings->get(sessionId).getInt("TradeFileCounter");
		}
		else
		{
			_max = 2000;
		}*/
		_exchangeNumber = "39";
		_firstSecList = true;
	}

	void FIX44Adapter::onLogon(const FIX::SessionID& sessionID)
	{
		FIN_INFO("Logged ON [" << sessionID << "]");
		LOG_INFO << "[INTEGRAL] Logged ON  [" << sessionID.toString() << "]";
		_sessionID = sessionID;
		_counterMReq = 0;
		_loggedIn = true;
	}

	void FIX44Adapter::onLogout(const FIX::SessionID& sessionID)
	{
		FIN_ERROR("Logged OUT [" << sessionID << "]");
		LOG_INFO << "[INTEGRAL] Logged OUT  [" << sessionID.toString() << "]";
	}

	// Client >> to >> Server
	void FIX44Adapter::toAdmin(FIX::Message& msg, const FIX::SessionID& sessionId)
	{
		LOG_INFO << msg.toString();
		const std::string& msgTypeValue = msg.getHeader().getField(FIX::FIELD::MsgType);
		if (msgTypeValue == FIX::MsgType_Logon)
		{
			sendLogonReq((FIX44::Logon&)msg, sessionId);
		}
	}

	void FIX44Adapter::sendLogonReq(FIX44::Logon& msgLogon, const FIX::SessionID&)
	{
		msgLogon.setField(FIX::TargetSubID(_senderSubID));
		msgLogon.setField(FIX::SenderSubID(_senderSubID));
		msgLogon.setField(FIX::Username(_username));
		msgLogon.setField(FIX::Password(_password));
	}

	void FIX44Adapter::toApp(FIX::Message& msg, const FIX::SessionID&)
		throw(FIX::DoNotSend)
	{
		LOG_INFO << msg.toString();
	}

	//Server << to << Client		//Exchange To Gateway
	void FIX44Adapter::fromAdmin(const FIX::Message& msg, const FIX::SessionID& sessionId)
		throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::RejectLogon)
	{
		
		LOG_INFO << msg.toString();
		crack(msg, sessionId);
	}

	void FIX44Adapter::fromApp(const FIX::Message& msg, const FIX::SessionID& sessioonId)
		throw(FIX::FieldNotFound, FIX::IncorrectDataFormat, FIX::IncorrectTagValue, FIX::UnsupportedMessageType)
	{
		LOG_INFO << msg.toString();
		
		const std::string& msgTypeValue = msg.getHeader().getField(FIX::FIELD::MsgType);
		if (msgTypeValue == FIX::MsgType_ExecutionReport)
		{
			FIX44::ExecutionReport report(msg);
			onMessage(report, sessioonId);
		}
		else
		{
			crack(msg, _sessionID);
		}
	}

	void FIX44Adapter::onMessage(const FIX44::BusinessMessageReject& msg, const FIX::SessionID&)
	{
		try
		{
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}


	}


	void FIX44Adapter::onMessage(const FIX44::OrderCancelReject& report, const FIX::SessionID& sess)
	{

		std::string str = report.toString();
		if (rabbit_udp)
			_omsRabbitSender.sendForOMS(str);
		else
			oms_udp_sender.sendToOMS(&str[0], str.length());

	}

	void FIX44Adapter::onMessage(const FIX44::ExecutionReport& report, const FIX::SessionID& sess)
	{

		FIX::ExecType execType;
		report.get(execType);

		bool flag = false;
		switch (execType)
		{
		case FIX::ExecType_NEW: {flag = true; }break;
		case FIX::ExecType_CANCELLED: {flag = true; } break;
		case FIX::ExecType_REPLACE: {flag = true; } break;
		case FIX::ExecType_REJECTED: {flag = true; } break;
		case FIX::ExecType_EXPIRED: {flag = true; } break;
		case FIX::ExecType_TRADE: {flag = true; } break;
		case FIX::ExecType_ORDER_STATUS: {flag = true; } break;

		}
		if (flag)
		{
			std::string str = report.toString();
			if (rabbit_udp)
				_omsRabbitSender.sendForOMS(str);
			else
				oms_udp_sender.sendToOMS(&str[0], str.length());
		}
		else
		{
			LOG_WARN << "execType not match";
			std::cout << "execType not match" << std::endl;
		}
	}
}