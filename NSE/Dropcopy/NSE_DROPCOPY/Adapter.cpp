#include "adapter.h"
namespace FIN
{
	Adapter::Adapter(boost::asio::io_service& ioService)
		:ConnectionBuilder(ioService),
		_ioService(ioService)
	{
		_loggedIn = false; // Use Connection Status
		storeCurrentDate = GetCurrentDate();
	}
	bool Adapter::init()
	{
		ConfigReader _conf;
		confFileName = _currentPath.getCurrentPath() + FIN::CONSTANT::NSE_DROPCOPY_CONFIG;
		nnfSubscriptionFileName = _currentPath.getCurrentPath() + FIN::CONSTANT::NSE_DROPCOPY_SUBSCRIPTION;
		_conf.loadData(confFileName);

		/*startRabbitMqConnection();*/
		traderId = _conf.getValue<INT64>("SESSION.TRADERID", 0);

		//check if need to store in CSV or not
		//isStoreCSV = _conf.getValue<bool>(FIN::CONSTANT::LOGGING + FIN::CONSTANT::SEP_DOT + "IS_STORE_CSV", false);
		//drop_file.filePath = CSVPath = _conf.getValue<std::string>(FIN::CONSTANT::LOGGING + FIN::CONSTANT::SEP_DOT + "FILE_STORE_PATH", FIN::CONSTANT::EMPTY_STRING);
		//std::string fileExtension = _conf.getValue<std::string>(FIN::CONSTANT::LOGGING + FIN::CONSTANT::SEP_DOT + "FILE_EXTENSION", FIN::CONSTANT::EMPTY_STRING);
		//isRMQEnable = _conf.getValue<bool>("RABBIT.isEnable", false);
		isLoggingEnable = _conf.getValue<bool>(FIN::CONSTANT::LOGGING + FIN::CONSTANT::SEP_DOT + "IS_LOGGING_ENABLE", false);


		/*if (isStoreCSV) {
			initCSVStoring(CSVPath, fileExtension);
		}*/
		//if (isRMQEnable)
		//	_nnfFieldSubscriptionHandler.init(nnfSubscriptionFileName);
		return false;
	}
	void Adapter::stop() {}

	void Adapter::start()
	{
		LOG_INFO << " ----------------------------------------------------------------------------";
		LOG_INFO << "  Starting NSE FO DROPCOPY ";
		LOG_INFO << " ----------------------------------------------------------------------------";
		CONSOLE_INFO_LOG("----------------------------------------------------------------------------");
		CONSOLE_INFO_LOG(" Starting NSE FO DROPCOPY ");
		CONSOLE_INFO_LOG("----------------------------------------------------------------------------");
		Connection::start();

	}

	void Adapter::initCSVStoring(std::string filePath, std::string fileExtension)
	{
		std::time_t t = time(NULL) + 19800;
		auto gmtime = std::gmtime(&t);
		char buffer[32];
		//strftime(buffer, 32, "%Y%m%d-%T", gmtime);
		strftime(buffer, 32, "%Y%m%d%T", gmtime);
		std::string t1(buffer);
		t1.erase(remove(t1.begin(), t1.end(), ':'), t1.end());
		if (fileExtension == "") {
			fileExtension = ".txt";
		}
		else {
			fileExtension = "." + fileExtension;
		}
		csvfileName = filePath + "\\NSE_DROPCOPY_" + std::to_string(traderId) + "_" + t1 + fileExtension;
		//csv_fout.open(csvfileName, std::ios::out | std::ios::app);		
	}
	void Adapter::onLogon()
	{

		LOG_INFO << " ----------------------------------------------------------------------------";
		LOG_INFO << "  NSE FO DC | BrokerId: " << getConnectionParam()._brokerId << " | TraderId: " << getConnectionParam()._traderId << " LOGGED IN";
		LOG_INFO << " ----------------------------------------------------------------------------";
		CONSOLE_INFO_LOG("----------------------------------------------------------------------------");
		CONSOLE_INFO_LOG(" NSE FO DC | BrokerId: " << getConnectionParam()._brokerId << " | TraderId: " << getConnectionParam()._traderId << " LOGGED IN");
		CONSOLE_INFO_LOG("----------------------------------------------------------------------------");
		_loggedIn = true;

	}
	void Adapter::onMessage(std::string msg)
	{

	}
	std::string Adapter::paddingSpace(const std::string& input, size_t totalWidth, bool isRight)
	{
		if (input.length() >= totalWidth) {
			return input;
		}
		if (isRight)
			return input + std::string(totalWidth - input.length(), ' ');
		else
			return  std::string(totalWidth - input.length(), ' ') + input;

	}
	std::string Adapter::GetTransactTime(long long timestamp_ns)
	{
		string transacttime = "";
		try
		{
			// Convert nanoseconds to seconds and remaining nanoseconds
			int64_t timestamp_sec = timestamp_ns / 1'000'000'000;
			int64_t remaining_ns = timestamp_ns % 1'000'000'000;

			// Convert the timestamp to a time_point
			std::chrono::system_clock::time_point tp = std::chrono::system_clock::from_time_t(timestamp_sec);

			// Add nanoseconds using duration_cast
			tp += std::chrono::duration_cast<std::chrono::system_clock::duration>(std::chrono::nanoseconds(remaining_ns));

			// Convert the time_point to a tm structure
			std::time_t tt = std::chrono::system_clock::to_time_t(tp);
			std::tm time_info = *gmtime(&tt); // Use gmtime here

			time_info.tm_year += 10;
			// Format the time using put_time
			std::stringstream ss;
			ss << std::put_time(&time_info, "%d %b %Y %H:%M:%S");
			transacttime = ss.str();

			for (char& d : transacttime)
			{
				d = std::toupper(d);
			}

			if (transacttime.substr(0, storeCurrentDate.length()) != storeCurrentDate)
			{
				transacttime.replace(0, storeCurrentDate.length(), storeCurrentDate);
			}
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
		return transacttime;
	}

	std::string Adapter::GetCurrentDate()
	{
		auto now = std::chrono::system_clock::now();
		std::time_t now_time1 = std::chrono::system_clock::to_time_t(now);
		std::tm* time_info1 = std::localtime(&now_time1);
		std::stringstream ss;
		ss << std::put_time(time_info1, "%d %b %Y");
		std::string formatted_date = ss.str();
		for (char& d : formatted_date)
		{
			d = std::toupper(d);
		}

		// Output the formatted date
		//std::cout << "Formatted Date: " << formatted_date << std::endl;

		return formatted_date;
	}

	std::string Adapter::ExpiryConverter(long seconds) {
		string MaturityMonthYear;
		try {


			if (cache.find(seconds) != cache.end()) {
				// If the result is already cached, return it directly.
				MaturityMonthYear = cache[seconds].first;
				NSE_SymbolName = cache[seconds].second;
				//std::cout << "Same Entry " << seconds << " " << cache[seconds].first << " " << cache[seconds].second << std::endl;


			}
			else
			{
				// Define the base date as January 1, 1980, 00:00:00
				std::tm fromDate = {};
				fromDate.tm_year = 80; // Year since 1900 (1980)
				fromDate.tm_mon = 0;  // Month (January)
				fromDate.tm_mday = 1; // Day
				fromDate.tm_hour = 0; // Hour
				fromDate.tm_min = 0;  // Minute
				fromDate.tm_sec = 0;  // Second

				std::tm dataw_expiry_month = fromDate;
				dataw_expiry_month.tm_sec += seconds;

				std::mktime(&dataw_expiry_month); // Normalize the time structure

				// Format the date as "dd-MMM-yy"
				char buffer[10]; // Room for "dd-MMM-yy" + '\0'
				std::strftime(buffer, sizeof(buffer), "%d%b%Y", &dataw_expiry_month);
				MaturityMonthYear = buffer;
				for (char& c : MaturityMonthYear) {
					c = std::toupper(c);
				}

				cache[seconds].first = MaturityMonthYear;

				char sym_date_buffer[6]; // Room for "yyMMM" + '\0'
				std::strftime(sym_date_buffer, sizeof(sym_date_buffer), "%y%b", &dataw_expiry_month);
				std::string	local_NSE_SymbolName = sym_date_buffer;

				for (char& d : local_NSE_SymbolName) {
					d = std::toupper(d);
				}
				NSE_SymbolName = local_NSE_SymbolName;
				cache[seconds].second = local_NSE_SymbolName;

				//	std::cout << "New Entry " << seconds << " " << cache[seconds].first << " " << cache[seconds].second << std::endl;


			}
		}
		catch (const std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
		return MaturityMonthYear;
	}

	std::string Adapter::GetNseSymbol(std::string strike_price, std::string& symbol, std::string& optionType)
	{
		string localSymbol = "";
		try
		{
			symbol.erase(std::remove_if(symbol.begin(), symbol.end(), ::isspace), symbol.end());

			if (optionType != "XX")
			{
				strike_price.erase(strike_price.length() - 3);
				localSymbol = symbol + NSE_SymbolName + strike_price + optionType;
			}
			else
				localSymbol = symbol + NSE_SymbolName + "FUT";


		}
		catch (const std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
		return localSymbol;
	}

	std::string Adapter::CheckStrike(long strikePrice, string opt_strike)
	{
		std::string result = " ";
		try
		{
			if (strikePrice <= 0)
			{
				return result;
			}
			else
			{

				return opt_strike /*+ FIN::ZERO_2*/;
			}
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
		return opt_strike /*+ FIN::ZERO_2*/;
	}

	std::string Adapter::CheckOptionType(string optString)
	{
		std::string result = "  ";
		try
		{
			if (optString == "XX")
			{
				return result;
			}
			else
			{
				return optString;
			}
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
		return optString;
	}

	void Adapter::onMessageToTxt(FIN::TRADE_CONFIRMATION* msg)
	{
		try
		{

			/*if (drop_file.IsDuplicateTrade(msg->getFillNumber()))
				return;
			*/

			dropCopyString = "";
			std::string str;
			str += paddingSpace((std::to_string(msg->getFillNumber())), 8, 1);
			str += (",");   // trade number
			str += "11,";
			char opt_fut[6];
			memset(opt_fut, 0, 6);
			memcpy(opt_fut, msg->_contractDesc.getInstrumentName(), 6);
			std::string local_opt_fut(opt_fut, 6);
			str.append(local_opt_fut);
			str.append(",");
			char symbol[10];
			memset(symbol, 0, 10);
			memcpy(symbol, msg->_contractDesc.getSymbol(), 10);
			std::string local_Symbol(symbol, 10);
			str += paddingSpace(local_Symbol, 10, 1) + ",";

			str += ExpiryConverter(msg->_contractDesc.getExpiryDate()) + ",";

			std::string strike = std::to_string(msg->_contractDesc.getStrikePrice() / 100);

			std::string optionType(msg->_contractDesc.getOptionType(), 2);

			str += paddingSpace(strike, 10, 0) + ",";
			str += optionType + ",";
			str += paddingSpace(GetNseSymbol(strike, local_Symbol, optionType), 25, 1) + ",";
			str += "1 ,RL ,1 ,";
			str += std::to_string(getConnectionParam()._traderId) + ",";
			str += "1  ,";
			str += std::to_string(msg->getBuySell()) + ",";
			str += paddingSpace(std::to_string(msg->getFillQty()), 9, 1) + ",";
			auto fillP = msg->getFillPrice() / 100.00;
			str += paddingSpace(std::to_string(fillP), 10, 0) + ",";
			str += msg->getReservedFiller2() + ",";
			str += "          ,";
			str += " ,";
			str += "OPEN,";
			str += "UNCOVER,";
			str += GetTransactTime(msg->getLastActivityReference()) + ",";
			str += GetTransactTime(msg->getLastActivityReference()) + ",";
			auto val = byte_swap<network_endian, host_endian>(msg->Response_order_number);
			str.append(std::to_string(static_cast<long long>(val)));
			str += ",NIL,";
			str += GetTransactTime(msg->getLastActivityReference()) + ",";

			char acc_num[10];
			memset(acc_num, 0, 10);
			memcpy(acc_num, msg->getAccountNum(), 10);
			std::string local_acc_num(acc_num, 10);

			str += local_acc_num;

			drop_file.pushToQueue(str);

		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}

	}

	void Adapter::onMessageToNewTxt(FIN::TRADE_CONFIRMATION* msg, std::string status)
	{
		try
		{
			/*trade_count += 1;
			LOG_INFO << " RCV COUNT " << trade_count;*/
			/*	if (drop_file.IsDuplicateTrade(msg->getFillNumber()))
					return;*/
					/*	Unique_trade_count += 1;
						LOG_INFO << " FILE COUNT " << Unique_trade_count;*/



						//	auto start = std::chrono::high_resolution_clock::now();
			dropCopyString = "";
			dropCopyString += paddingSpace((std::to_string(msg->getFillNumber())), 0, 1) + FIN::COMMA;
			dropCopyString += status;						//trade status
			dropCopyString.append(msg->_contractDesc.getInstrumentName(), 0, 6);
			dropCopyString.append(FIN::COMMA);

			std::string symbol(msg->_contractDesc.getSymbol(), 10);

			symbol = symbol.substr(0, symbol.find_last_not_of(" ") + 1);

			dropCopyString += paddingSpace(symbol, 0, 1) + FIN::COMMA;


			dropCopyString += ExpiryConverter(msg->_contractDesc.getExpiryDate()) + FIN::COMMA;

			//std::string strike = std::to_string(msg->_contractDesc.getStrikePrice() / 100.00);
			double strike_double = msg->_contractDesc.getStrikePrice() / 100.00;
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(2) << strike_double;
			std::string strike = oss.str();


			std::string optionType(msg->_contractDesc.getOptionType(), 2);
			dropCopyString += paddingSpace(CheckStrike(msg->_contractDesc.getStrikePrice(), strike), 0, 0) + FIN::COMMA;

			dropCopyString += CheckOptionType(optionType) + FIN::COMMA;

			
			/*if (contract_Read.contractDetails.find(std::to_string(msg->getToken())) != contract_Read.contractDetails.end())
				dropCopyString += contract_Read.contractDetails[std::to_string(msg->getToken())][53] + FIN::COMMA;
			else
				*/
			dropCopyString += paddingSpace(GetNseSymbol(strike, symbol, optionType), 0, 1) + FIN::COMMA;

			dropCopyString += FIN::RL;
			dropCopyString += std::to_string(msg->getTraderNum()) + FIN::COMMA;
			dropCopyString += std::to_string(getConnectionParam()._branchId) + FIN::COMMA;//FIN::SPACE_1;
			dropCopyString += std::to_string(msg->getBuySell()) + FIN::COMMA;
			dropCopyString += paddingSpace(std::to_string(msg->getFillQty()), 0, 1) + FIN::COMMA;
			dropCopyString += paddingSpace(std::to_string(msg->getFillPrice() / 100.00), 0, 0) + FIN::COMMA;
			dropCopyString += FIN::PRO_CLIENT;


			std::string account(msg->getAccountNum(), 10);
			account = account.substr(0, account.find_last_not_of(" ") + 1);


			dropCopyString += account + FIN::COMMA;
			
			std::string cpCode(msg->getParticipant(), 12);
			cpCode = cpCode.substr(0, cpCode.find_last_not_of(" ") + 1);

			
			dropCopyString += cpCode + FIN::COMMA;
			//dropCopyString += getConnectionParam()._brokerId + FIN::COMMA;

			dropCopyString += FIN::OPEN;
			dropCopyString += FIN::UNCOVER;
			dropCopyString += GetTransactTime(msg->getLastActivityReference()) + FIN::COMMA;
			dropCopyString += GetTransactTime(msg->getLastActivityReference()) + FIN::COMMA;
			dropCopyString.append(std::to_string(static_cast<long long>(byte_swap<network_endian, host_endian>(msg->Response_order_number))));
			dropCopyString += FIN::NIL;
			dropCopyString += GetTransactTime(msg->getLastActivityReference()) + FIN::COMMA;
			dropCopyString.append(std::to_string(static_cast<long long>(byte_swap<network_endian, host_endian>(msg->nnfField))));


			drop_file.pushToQueue(dropCopyString);
			/*auto stop = std::chrono::high_resolution_clock::now();


			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();

			std::cout << elapsed << " in micro " << counter++ << std::endl;*/

		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}

	}


	void Adapter::onMessageStringSend(FIN::TRADE_CONFIRMATION* msg)
	{
		//LOG_INFO << "[TRADE_CONFIRM]";
		std::string str;
		//LOG_INFO<<"[ResOrdNoDoub]"<<std::to_string((double)_byteswap_uint64(msg->Response_order_number))<<" and"<< std::to_string((msg->Response_order_number))<<"and"<< std::to_string(_byteswap_uint64(msg->Response_order_number));
		//int token = htonl(msg->Token);
		//LOG_INFO<<"[Token]"<<std::to_string((int)(htonl(msg->Token)))<<"and"<<msg->Token<< "[and]" <<token;
		/*
		str.append(std::to_string(msg->getTransCode()));							    str += (",");
		str.append(std::to_string(msg->getLogTime()));							        str += (",");
		str.append(std::to_string(msg->getTraderId()));										str += (",");
		str.append(std::to_string(msg->Response_order_number));          				str += (",");
		str.append(               msg->getBrokerID());									str += (",");
		str.append(std::to_string(msg->getReserved9()));								str += (",");
		str.append(std::to_string(msg->getTraderNum()));								str += (",");
		str.append(               msg->getAccountNum());								str += (",");
		str.append(std::to_string(msg->getBuySell()));									str += (",");
		str.append(std::to_string(msg->getOriginalVol()));								str += (",");
		str.append(std::to_string(msg->getDisclosedVol()));								str += (",");
		str.append(std::to_string(msg->getRemainingVol()));								str += (",");
		str.append(std::to_string(msg->getDisclosedVolRemaining()));					str += (",");
		str.append(std::to_string(msg->getPrice()));									str += (",");
		//_orderFlags.getString(str);													str += (",");
			 if(msg->_orderFlags.getAON()) { str.append("AON"); }
		else if(msg->_orderFlags.getIOC())			{str.append("IOC");}
		else if(msg->_orderFlags.getGTC())			{str.append("GTC");}
		else if(msg->_orderFlags.getDay())			{str.append("Day");}
		else if(msg->_orderFlags.getMIT())			{str.append("MIT");}
		else if(msg->_orderFlags.getSL())			{str.append("SL");}
		else if(msg->_orderFlags.getMarket())		{str.append("Market");}
		else if(msg->_orderFlags.getATO())			{str.append("ATO");}
		//else if(msg->_orderFlags.getReserved1())	{str.append("Reserved1");}
		//else if(msg->_orderFlags.getReserved2())	{str.append("Reserved2");}
		//else if(msg->_orderFlags.getReserved3())	{str.append("Reserved3");}
		else if(msg->_orderFlags.getFrozen())		{str.append("Frozen");}
		else if(msg->_orderFlags.getModified())		{str.append("Modified");}
		else if(msg->_orderFlags.getTraded())		{str.append("Traded");}
		else if(msg->_orderFlags.getMatchedInd())	{str.append("MatchedInd");}
		else if(msg->_orderFlags.getMF())			{str.append("MF");}
		else { str.append("No_Data"); }
		str += (",");

		//
		str.append(std::to_string(msg->getGtd()));										str += (",");
		str.append(std::to_string(msg->getFillNumber()));								str += (",");
		str.append(std::to_string(msg->getFillQty()));									str += (",");
		str.append(std::to_string(msg->getFillPrice()));								str += (",");
		str.append(std::to_string(msg->getVolFilledToday()));							str += (",");
		str.append(				 (msg->getActivityType()));								str += (",");
		str.append(std::to_string(msg->getActivityTime()));								str += (",");
		str.append(std::to_string(msg->OpOrderNumber));									str += (",");
		str.append(				  msg->getOpBrokerId());								str += (",");
		str.append(std::to_string(msg->getToken()));									str += (",");
		//
		msg->_contractDesc.getString(str);												    str += (",");
		//str.append(				  msg->getInstrumentName());							str += (",");
		//str.append(				  msg->getSymbol());									str += (",");
		//str.append(std::to_string(msg->getExpiryDate()));								str += (",");
		//str.append(std::to_string(msg->getStrikePrice()));								str += (",");
		//str.append(				  msg->getOptionType());								str += (",");
		//str.append(std::to_string(msg->getCALevel()));
		//
		str.append(std::to_string(msg->getOpenClose()));								str += (",");
		str.append(std::to_string(msg->getOldOpenCLose()));								str += (",");
		str.append(std::to_string(msg->getBookType()));									str += (",");
		str.append(std::to_string(msg->getNewVolume()));								str += (",");
		str.append				 (msg->getOldAccountNumber());							str += (",");
		str.append				 (msg->getParticipant());								str += (",");
		str.append				 (msg->getOldParticipant());							str += (",");
		//_addOrderFlags.getString(str);												    str += (",");

		str.append(std::to_string(msg->getReservedFiller()));							str += (",");
		str.append(std::to_string(msg->getGiveUpTrade()));								str += (",");
		str.append(std::to_string(msg->getReservedFiller2()));							str += (",");
		str.append(				 (msg->getPAN()));										str += (",");
		str.append(				 (msg->getOldPAN()));									str += (",");
		str.append(std::to_string(msg->getAlgoID()));									str += (",");
		str.append(std::to_string(msg->getReserved_Filler()));							str += (",");
		str.append(				 (msg->getReserved()));									str += (",");
		*/

		str += (std::to_string(msg->getTransCode()));							    str += (",");
		str += (std::to_string(msg->getLogTime() + 315532800));							        str += (",");
		str += (std::to_string(msg->getTraderId()));										str += (",");
		auto val = byte_swap<network_endian, host_endian>(msg->Response_order_number);
		//str.append(std::to_string((double)_byteswap_uint64((uint64_t) Response_order_number)));          				str += (",");
		str.append(std::to_string(val));          				str += (",");
		str += (msg->getBrokerID());									str += (",");
		str += (std::to_string(msg->getReserved9()));								str += (",");
		//str += ("");								str += (",");
		str += (std::to_string(msg->getTraderNum()));								str += (",");
		str += (msg->getAccountNum());								str += (",");
		str += (std::to_string(msg->getBuySell()));									str += (",");
		str += (std::to_string(msg->getOriginalVol()));								str += (",");
		str += (std::to_string(msg->getDisclosedVol()));								str += (",");
		str += (std::to_string(msg->getRemainingVol()));								str += (",");
		str += (std::to_string(msg->getDisclosedVolRemaining()));					str += (",");
		str += (std::to_string(msg->getPrice()));									str += (",");
		//_orderFlags.getString(str);													str += (",");
		//Time_In_Force
		if (msg->_orderFlags.getAON()) { str += ("AON"); }
		else if (msg->_orderFlags.getIOC()) { str += ("IOC"); }
		else if (msg->_orderFlags.getGTC()) { str += ("GTC"); }
		else if (msg->_orderFlags.getDay()) { str += ("Day"); }
		else if (msg->_orderFlags.getMIT()) { str += ("MIT"); }
		else if (msg->_orderFlags.getSL()) { str += ("SL"); }
		else if (msg->_orderFlags.getMarket()) { str += ("Market"); }
		else if (msg->_orderFlags.getATO()) { str += ("ATO"); }
		//else if(msg->_orderFlags.getReserved1())	{str+=("Reserved1");}
		//else if(msg->_orderFlags.getReserved2())	{str+=("Reserved2");}
		//else if(msg->_orderFlags.getReserved3())	{str+=("Reserved3");}
		else if (msg->_orderFlags.getFrozen()) { str += ("Frozen"); }
		else if (msg->_orderFlags.getModified()) { str += ("Modified"); }
		else if (msg->_orderFlags.getTraded()) { str += ("Traded"); }
		else if (msg->_orderFlags.getMatchedInd()) { str += ("MatchedInd"); }
		else if (msg->_orderFlags.getMF()) { str += ("MF"); }
		else { str += ("No_Data"); }
		str += (",");

		//	 
		str += (std::to_string(msg->getGtd()));										str += (",");
		str += (std::to_string(msg->getFillNumber()));								str += (",");
		str += (std::to_string(msg->getFillQty()));									str += (",");
		str += (std::to_string(msg->getFillPrice()));								str += (",");
		str += (std::to_string(msg->getVolFilledToday()));							str += (",");
		std::string activeType(msg->getActivityType(), 2);
		//std::cout <<"activity type="<< activeType << std::endl;
		for (auto chat : activeType)
		{
			if (chat == ' ' || chat == '0' || chat == '\0')
				break;
			str += chat;

		}
		//str += ((msg->getActivityType()));								
		str += (",");
		str += (std::to_string(msg->getActivityTime() + 315532800));								str += (",");
		auto val1 = byte_swap<network_endian, host_endian>(msg->OpOrderNumber);
		//str.append(std::to_string((double)((int64_t)_byteswap_uint64((uint64_t) OpOrderNumber))));									str += (",");
		str.append(std::to_string(val1));									str += (",");
		str += msg->getOpBrokerId();								str += (",");
		str += (std::to_string(msg->getToken()));									str += (",");
		//str +=  std::to_string((int)(htonl(msg->Token)));									str += (",");
		//
		//msg->_contractDesc.getString(str);												    str += (",");
		std::string InstrumentName = msg->_contractDesc.getInstrumentName();
		for (auto chat : InstrumentName)
		{
			if (chat == ' ' || chat == '0' || chat == '\0')
				break;
			str += chat;

		}
		//str+=(				  msg->_contractDesc.getInstrumentName());				
		str += (",");
		std::string symbol = msg->_contractDesc.getSymbol();
		for (auto chat : symbol)
		{
			if (chat == ' ' || chat == '0' || chat == '\0')
				break;
			str += chat;

		}
		//str+=(				  msg->_contractDesc.getSymbol());			
		str += (",");
		str += (std::to_string(msg->_contractDesc.getExpiryDate() + 315532800));								str += (",");
		str += (std::to_string(msg->_contractDesc.getStrikePrice()));								str += (",");
		str += (msg->_contractDesc.getOptionType());								str += (",");
		str += (std::to_string(msg->_contractDesc.getCALevel()));   str += (",");
		//
		str += (std::to_string((int)msg->getOpenClose()));								str += (",");
		str += (std::to_string((int)msg->getOldOpenCLose()));								str += (",");
		std::string booktype = std::to_string((int)msg->getBookType());
		str += (booktype);									str += (",");
		std::string newVol = std::to_string(msg->getNewVolume());
		str += (newVol);								str += (",");
		//str += (std::to_string(msg->getNewVolume()));								str += (",");
		std::string oldAccNum = trimAfterSpace(msg->getOldAccountNumber());
		str += ((oldAccNum));						str += (",");
		//str += (msg->getOldAccountNumber());							str += (",");
		std::string Participant(msg->getParticipant(), 12);
		str += (Participant); str += (",");
		//str += ((msg->getParticipant()));								str += (",");
		std::string oldParticipant(msg->getOldParticipant(), 12);
		str += (oldParticipant);							str += (",");
		//_addOrderFlags.getString(str);												    str += (",");

		str += (std::to_string(msg->getReservedFiller()));							str += (",");
		//str += ("");							str += (",");
		str += (std::to_string((int)msg->getGiveUpTrade()));								str += (",");
		str += (std::to_string(msg->getReservedFiller2()));							str += (",");
		//str += ("");							str += (",");
		std::string PAN(msg->getPAN(), 10);
		str += (PAN);										str += (",");
		str += ((msg->getOldPAN()));									str += (",");
		str += (std::to_string(msg->getAlgoID()));									str += (",");
		str += (std::to_string(msg->getAlgoCategory()));str += (",");
		str += (std::to_string(msg->getLastActivityReference()));							str += (",");
		double nnfField_ = byte_swap<network_endian, host_endian>(msg->nnfField);;
		std::string nnfFieldTrimmed = trimNNFFieldTo12Digit(nnfField_);
		if (nnfFieldTrimmed != "")
		{
			//msgToOms.nnfField = std::stoll(nnfFieldTrimmed);
			str += (nnfFieldTrimmed);		str += (",");
		}
		else {
			str += "            "; str += (",");
		}
		std::string reserved(msg->getReserved(), 44);
		str += (reserved);									str += (",");
		//str += ("");			str += (",");



		csv_fout.open(csvfileName, std::ios::out | std::ios::app);
		csv_fout << str << "\n";
		if (csv_fout.is_open()) {
			csv_fout.close();
		}
	}
	void Adapter::onMessage(FIN::TRADE_CONFIRMATION* msg)
	{
		/*double d1, d2,d3 = 0.0;
		memcpy(&d1, msg->getTimestamp1(), 4);
		memcpy(&d2, msg->getTimestamp1()+4, 4);
		memcpy(&d3, &d1, 4);
		memcpy(&d3+4, &d2, 4);
		LOG_INFO << " TimeStampD1:" << d1 << "and:" << d1 / 65536
			<< " TimeStampD2:" << d2 << "and:" << d2 / 65536
			<< " TimeStampD3:" << d3 << "and:" << d3 / 65536;*/
			//check if rabbitMq enable and nnfFiled present in subscription file
		double nnfField_ = byte_swap<network_endian, host_endian>(msg->nnfField);;
		std::string nnfFieldTrimmed = trimNNFFieldTo12Digit(nnfField_);
		bool isSubscibed = _nnfFieldSubscriptionHandler.isSymbolSubscribed(nnfFieldTrimmed);
		if (isRMQEnable && isSubscibed) {
			FIN::tradeResponseToOMS msgToOms;
			msgToOms.TransCode = (msg->getTransCode());
			msgToOms.LogTime = (msg->getLogTime());	//+ 315532800));
			msgToOms.TraderId = (msg->getTraderId());
			auto val = byte_swap<network_endian, host_endian>(msg->Response_order_number);
			//str.append(std::to_string((double)_byteswap_uint64((uint64_t) Response_order_number)));          				str += (",");
			msgToOms.Response_order_number = val;
			memcpy(msgToOms.BrokerID, msg->getBrokerID(), 5);
			msgToOms.Reserved9 = (msg->getReserved9());
			msgToOms.TraderNum = (msg->getTraderNum());
			memcpy(msgToOms.AccountNum, msg->getAccountNum(), 10);
			msgToOms.BuySell = (msg->getBuySell());
			msgToOms.OriginalVol = (msg->getOriginalVol());
			msgToOms.DisclosedVol = (msg->getDisclosedVol());
			msgToOms.RemainingVol = (msg->getRemainingVol());
			msgToOms.DisclosedVolRemaining = (msg->getDisclosedVolRemaining());
			msgToOms.Price = (msg->getPrice());
			//_orderFlags.getString(str);													str += (",");
			//Time_In_Force
			std::string str;
			if (msg->_orderFlags.getAON()) { str += ("AON"); }
			else if (msg->_orderFlags.getIOC()) { str += ("IOC"); }
			else if (msg->_orderFlags.getGTC()) { str += ("GTC"); }
			else if (msg->_orderFlags.getDay()) { str += ("Day"); }
			else if (msg->_orderFlags.getMIT()) { str += ("MIT"); }
			else if (msg->_orderFlags.getSL()) { str += ("SL"); }
			else if (msg->_orderFlags.getMarket()) { str += ("Market"); }
			else if (msg->_orderFlags.getATO()) { str += ("ATO"); }
			//else if(msg->_orderFlags.getReserved1())	{str+=("Reserved1");}
			//else if(msg->_orderFlags.getReserved2())	{str+=("Reserved2");}
			//else if(msg->_orderFlags.getReserved3())	{str+=("Reserved3");}
			else if (msg->_orderFlags.getFrozen()) { str += ("Frozen"); }
			else if (msg->_orderFlags.getModified()) { str += ("Modified"); }
			else if (msg->_orderFlags.getTraded()) { str += ("Traded"); }
			else if (msg->_orderFlags.getMatchedInd()) { str += ("MatchedInd"); }
			else if (msg->_orderFlags.getMF()) { str += ("MF"); }
			else { str += ("No_Data"); }

			/*We have created array of 10 size so adding trailing space if string is less then 10 size*/
			int noOfSpacesAdd = 10 - str.length();
			while (noOfSpacesAdd--) {
				str += ' ';
			}

			memcpy(msgToOms.OrderFlags, str.c_str(), 10);

			//	 
			msgToOms.Gtd = (msg->getGtd());
			msgToOms.FillNumber = (msg->getFillNumber());
			msgToOms.FillQty = (msg->getFillQty());
			msgToOms.FillPrice = (msg->getFillPrice());
			msgToOms.VolFilledToday = (msg->getVolFilledToday());
			//std::cout <<"activity type="<< activeType << std::endl;
			/*for (auto chat : activeType)
			{
				if (chat == ' ' || chat == '0' || chat == '\0')
					break;
				str += chat;

			}*/
			memcpy(msgToOms.ActivityType, msg->getActivityType(), 2);
			msgToOms.ActivityTime = (msg->getActivityTime());//+ 315532800));
			auto val1 = byte_swap<network_endian, host_endian>(msg->OpOrderNumber);
			//str.append(std::to_string((double)((int64_t)_byteswap_uint64((uint64_t) OpOrderNumber))));									str += (",");
			msgToOms.OpOrderNumber = val1;
			memcpy(msgToOms.OpBrokerId, msg->getOpBrokerId(), 5);
			msgToOms.Token = (msg->getToken());
			//str +=  std::to_string((int)(htonl(msg->Token)));									str += (",");
			//
			//msg->_contractDesc.getString(str);												    str += (",");
			/*std::string InstrumentName = msg->_contractDesc.getInstrumentName();
			for (auto chat : InstrumentName)
			{
				if (chat == ' ' || chat == '0' || chat == '\0')
					break;
				str += chat;

			}*/
			memcpy(msgToOms.InstrumentName, msg->_contractDesc.getInstrumentName(), 6);

			/*std::string symbol = msg->_contractDesc.getSymbol();
			for (auto chat : symbol)
			{
				if (chat == ' ' || chat == '0' || chat == '\0')
					break;
				str += chat;

			}*/
			memcpy(msgToOms.Symbol, msg->_contractDesc.getSymbol(), 10);
			msgToOms.ExpiryDate = (msg->_contractDesc.getExpiryDate());// +315532800));
			msgToOms.StrikePrice = (msg->_contractDesc.getStrikePrice());
			memcpy(msgToOms.OptionType, msg->_contractDesc.getOptionType(), 2);
			msgToOms.CALevel = (msg->_contractDesc.getCALevel());
			//
			msgToOms.OpenClose = (msg->getOpenClose());
			msgToOms.OldOpenCLose = (msg->getOldOpenCLose());
			msgToOms.BookType = (msg->getBookType());
			/*str += (booktype);									str += (",");*/
			//std::string newVol = std::to_string(msg->getNewVolume());
			//str += (newVol);								str += (",");
			msgToOms.NewVolume = (msg->getNewVolume());
			/*std::string oldAccNum = trimAfterSpace(msg->getOldAccountNumber());
			memcpy(msgToOms.OldAccountNumber,oldAccNum.c_str(),10);*/
			memcpy(msgToOms.OldAccountNumber, msg->getOldAccountNumber(), 10);
			/*std::string Participant(msg->getParticipant(), 12);
			msgToOms.Participant=(Participant);*/
			memcpy(msgToOms.Participant, msg->getParticipant(), 12);
			/*std::string oldParticipant(msg->getOldParticipant(), 12);
			msgToOms.OldParticipant=(oldParticipant);*/
			memcpy(msgToOms.OldParticipant, msg->getOldParticipant(), 12);
			//_addOrderFlags.getString(str);

			msgToOms.ReservedFiller = (msg->getReservedFiller());
			msgToOms.GiveUpTrade = (msg->getGiveUpTrade());
			msgToOms.ReservedFiller2 = (msg->getReservedFiller2());

			/*std::string PAN(msg->getPAN(), 10);
			str += (PAN);*/
			memcpy(msgToOms.PAN, msg->getPAN(), 10);
			memcpy(msgToOms.OldPAN, msg->getOldPAN(), 10);
			msgToOms.AlgoID = (msg->getAlgoID());
			msgToOms.AlgoCategory = (msg->getAlgoCategory());
			msgToOms.LastActivityReference = (msg->getLastActivityReference());
			//std::string reserved(msg->getReserved(), 52);
			//msgToOms.nnfField = byte_swap<network_endian, host_endian>(msg->nnfField);
			if (nnfFieldTrimmed != "")
			{
				msgToOms.nnfField = std::stoll(nnfFieldTrimmed);
			}
			memcpy(msgToOms.Reserved, msg->getReserved(), 44);

			//	_sendDataRabbitmq.sendMsg(msgToOms);


			std::string omsString;
			msgToOms.getString(omsString);
			if (isLoggingEnable)
			{
				LOG_INFO << "[OMS_SND]" << omsString;
			}
		}
		if (isStoreCSV) {
			/*std::string csvString; msgToOms.getString(csvString, ",");
			csv_fout.open(csvfileName, std::ios::out | std::ios::app);
			csv_fout << csvString<<"\n";
			if (csv_fout.is_open()) {
				csv_fout.close();
			}*/
			onMessageStringSend(msg);
		}
	}

}