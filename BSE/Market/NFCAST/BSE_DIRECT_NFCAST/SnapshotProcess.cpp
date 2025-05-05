#pragma once
#include "SnapshotProcess.h"



NFCAST_SNAPSHOT::NFCAST_SNAPSHOT(Config_Details* config, UNSIGNED_INT_16 exchID) :snap_rcver(config), _publisher(config)
{
	local_config = config;
	exchNumber = exchID;
}
NFCAST_SNAPSHOT::~NFCAST_SNAPSHOT()
{

}

// Function to convert a value from big-endian to little-endian
template <typename T>
T bigToLittleEndian(T value) {
	char* bytes = reinterpret_cast<char*>(&value);
	std::reverse(bytes, bytes + sizeof(T));
	return value;
}


//uint64_t NFCAST_SNAPSHOT::ntohll(uint64_t value) {
//	
//	return ((uint64_t)ntohl(value & 0xFFFFFFFF) << 32) | ntohl(value >> 32);
//	
//	ntohd
//	//// Split the 64-bit value into two 32-bit parts
//	//uint32_t high_part = value >> 32;
//	//uint32_t low_part = value & 0xFFFFFFFF;
//
//	//// Convert each part from network byte order to host byte order
//	//uint32_t converted_high = ntohl(high_part);
//	//uint32_t converted_low = ntohl(low_part);
//
//	//// Combine the two parts into a single 64-bit value
//	//return (static_cast<uint64_t>(converted_high) << 32) | converted_low;
//}


void NFCAST_SNAPSHOT::init()
{
	try
	{
		_publisher.init();														//publisher initilize
		secFileReader.readTokenFile(local_config->token_path);					//read token file and store

		storeTokenInMap(secFileReader._symbolsInFile);							//store token number in map and initilize marketdepth, ohlc

		std::thread thread_multicast(&MulticastReceiver::init, &snap_rcver);	//initilize multicast and receive data
		thread_multicast.detach();
		processData();															//received data process
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
}

void NFCAST_SNAPSHOT::processData()
{
	while (1)
	{
		try
		{

			auto packet = snap_rcver.multicast_q.front();

			//int templateID = ntohl(packet.msgType);
			//if (packet.msgType == 2020 || packet.msgType == 2021 || packet.msgType == 2015)
			//{
			//	//std::cout << packet.msgType << std::endl;
			//}
			//else {
			//	std::cout << packet.msgType << std::endl;
			//}
			//std::cout << packet.msgType << std::endl;
			switch (packet.msgType)
			{

			case FIN::FIN_CONST::TIME_BROADCAST_MESSAGE:	{}	break;
			case FIN::FIN_CONST::AUCTION_KEEP_ALIVE_MESSAGE: {}	break;
			case FIN::FIN_CONST::PRODUCT_STATE_CHANGE_MESSAGE: {}	break;
			case FIN::FIN_CONST::SHORTAGE_AUCTION_SESSION_CHANGE_MESSAGE: {}	break;
			case FIN::FIN_CONST::NEWS_HEADLINE: {}	break;
			case FIN::FIN_CONST::MARKET_PICHTURE_BROADCAST:
			{
				decompressMarketPicture(packet.msg, packet.length);
			}
			break;
			case FIN::FIN_CONST::MARKET_PICHTURE_BROADCAST_COMPLEX:
			{
				decompressMarketPictureComplexInstruments(packet.msg, packet.length);
			}
			break;
			case FIN::FIN_CONST::AUCTION_MARKET_PICTURE_BROADCAST: {}	break;
			case FIN::FIN_CONST::ODD_LOT_MARKET_PICTURE_MESSAGE: {}	break;
			case FIN::FIN_CONST::DEBT_MARKET_PICTURE: {}	break;
			case FIN::FIN_CONST::INDEX_CHANGE_MESSAGE1: 
			{
				decompressIndexChnageMessage1(packet.msg, packet.length);
			}	break;
			case FIN::FIN_CONST::INDEX_CHANGE_MESSAGE2: 
			{
				decompressIndexChnageMessage1(packet.msg, packet.length);
			}	break;
			case FIN::FIN_CONST::LIMIT_PRICE_PROTECTION_RANGE: 
			{
				//processLimitPrice(packet.msg, packet.length);
			}	
			break;
			case FIN::FIN_CONST::CLOSE_PRICE: 
			{
				processClosePrice(packet.msg, packet.length);
			}
			break;
			case FIN::FIN_CONST::OPEN_INTEREST_MESSAGE: 
			{
				processOpenInterest(packet.msg, packet.length);
			}
			break;
			case FIN::FIN_CONST::VAR_PERCENTAGE: {}	break;
			case FIN::FIN_CONST::RBI_REFERENCE_RATE: {}	break;
			case FIN::FIN_CONST::IMPLIED_VOLATILITY:	{}	break;
			case 2019: {}	break;

			default:
				std::cout << "Invalid msgType : "<<packet.msgType << std::endl;
				LOG_INFO << "Invalid msgType : " << packet.msgType;
				break;
			}

			snap_rcver.multicast_q.pop();
			delete[] packet.msg;
		}
		catch (exception& ex)
		{
			LOG_INFO << ex.what();
		}
	}

}

void NFCAST_SNAPSHOT::decompressMarketPicture(char* buffer, int32_t length) {

	try
	{
		int pos = 0;
		pos += 26;

		int16_t noOfRecords = readTwoByte(buffer, pos);

		for (int i = 0; i < noOfRecords; i++) {

			if (pos < length) {

				int32_t instrumentContractCode = readFourByte(buffer, pos);			//Token number
				if(instrumentContractCode<100)
					std::cout << "Token number : " << instrumentContractCode << std::endl;

				auto it = secFileReader._symbolsInFile.find(instrumentContractCode);
				if (it != secFileReader._symbolsInFile.end())
				{
					auto& md = *_map[instrumentContractCode];						// Reference to MarketData object
					pos += sizeof(long);

					int32_t tradedVolume = readFourByte(buffer, pos);
					md.setVolume(tradedVolume);										//set Volume
					pos += 22;

					int16_t noOfPricePoints = readTwoByte(buffer, pos);
					pos += sizeof(long long);

					int32_t closeRate = readFourByte(buffer, pos);

					int32_t ltq = readFourByte(buffer, pos);					//last trade quantity

					int32_t ltp = readFourByte(buffer, pos);
					md.setLTP(double(ltp) / FIN::FIN_CONST::DEVIDEBY100);		//set last trade price

					int32_t openRate = checkReadByte(ltp, buffer, pos);
					md.setOpenPrice(double(openRate) / FIN::FIN_CONST::DEVIDEBY100);						//set OpenPrice

					int32_t previousCloseRate = checkReadByte(ltp, buffer, pos);
					md.setClosePrice(double(previousCloseRate) / FIN::FIN_CONST::DEVIDEBY100);			//set ClosePrice

					int32_t highRate = checkReadByte(ltp, buffer, pos);
					md.setHighPrice(double(highRate) / FIN::FIN_CONST::DEVIDEBY100);						//set HighPrice

					int32_t lowRate = checkReadByte(ltp, buffer, pos);
					md.setLowPrice(double(lowRate) / FIN::FIN_CONST::DEVIDEBY100);						//set LowPrice

					int32_t blockDealReferencePrice = checkReadByte(ltp, buffer, pos);

					int32_t indicativeEquilibriumPrice = checkReadByte(ltp, buffer, pos);

					int32_t indicativeEquilibriumQuantity = checkReadByte(ltq, buffer, pos);

					int32_t totalBidQuantity = checkReadByte(ltq, buffer, pos);

					int32_t totalOfferQuantity = checkReadByte(ltq, buffer, pos);

					int32_t lowerCircuitLimit = checkReadByte(ltp, buffer, pos);
					//md.setLowerLimit(double(lowerCircuitLimit));

					int32_t upperCircuitLimit = checkReadByte(ltp, buffer, pos);
					//md.setUpperLimit(double(upperCircuitLimit));

					int32_t weightedAveragePrice = checkReadByte(ltp, buffer, pos);

					MarketPictureBroadcast marketMsg;
					readBuyDepth(marketMsg, buffer, pos, noOfPricePoints, ltp, ltq, instrumentContractCode);		//read full depth both side 
					readSellDepth(marketMsg, buffer, pos, noOfPricePoints, ltp, ltq, instrumentContractCode);		
					
					md.sendToBroadcast(true);
				}
				else {
					return;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
}

void NFCAST_SNAPSHOT::decompressMarketPictureComplexInstruments(char* buffer, size_t length) {
	try
	{

		int pos = 0;

		pos += 26;
		int16_t noOfRecords = readTwoByte(buffer, pos);

		for (int i = 0; i < noOfRecords; i++) {

			if (pos < length) {

				long long instrumentContractCode = readEightByte(buffer, pos);		//Token number

				auto it = secFileReader._symbolsInFile.find(instrumentContractCode);
				if (it != secFileReader._symbolsInFile.end())
				{
					auto& md = *_map[instrumentContractCode];						// Reference to MarketData object
					pos += sizeof(long);

					int32_t tradedVolume = readFourByte(buffer, pos);
					md.setVolume(tradedVolume);										//set Volume
					pos += 22;

					int16_t noOfPricePoints = readTwoByte(buffer, pos);
					pos += sizeof(long long);

					int32_t closeRate = readFourByte(buffer, pos);

					int32_t ltq = readFourByte(buffer, pos);

					int32_t ltp = readFourByte(buffer, pos);
					md.setLTP(double(ltp) / FIN::FIN_CONST::DEVIDEBY100);		//set LTP

					int32_t openRate = checkReadByte(ltp, buffer, pos);
					md.setOpenPrice(double(openRate) / FIN::FIN_CONST::DEVIDEBY100);						//set OpenPrice

					int32_t previousCloseRate = checkReadByte(ltp, buffer, pos);
					md.setClosePrice(double(previousCloseRate) / FIN::FIN_CONST::DEVIDEBY100);			//set ClosePrice

					int32_t highRate = checkReadByte(ltp, buffer, pos);
					md.setHighPrice(double(highRate) / FIN::FIN_CONST::DEVIDEBY100);						//set HighPrice

					int32_t lowRate = checkReadByte(ltp, buffer, pos);
					md.setLowPrice(double(lowRate) / FIN::FIN_CONST::DEVIDEBY100);						//set LowPrice

					int32_t blockDealReferencePrice = checkReadByte(ltp, buffer, pos);

					int32_t indicativeEquilibriumPrice = checkReadByte(ltp, buffer, pos);

					int32_t indicativeEquilibriumQuantity = checkReadByte(ltq, buffer, pos);

					int32_t totalBidQuantity = checkReadByte(ltq, buffer, pos);

					int32_t totalOfferQuantity = checkReadByte(ltq, buffer, pos);

					int32_t lowerCircuitLimit = checkReadByte(ltp, buffer, pos);

					int32_t upperCircuitLimit = checkReadByte(ltp, buffer, pos);

					int32_t weightedAveragePrice = checkReadByte(ltp, buffer, pos);

					MarketPictureBroadcast marketMsg;
					readBuyDepth(marketMsg, buffer, pos, noOfPricePoints, ltp, ltq, instrumentContractCode);		//read full depth both side 
					readSellDepth(marketMsg, buffer, pos, noOfPricePoints, ltp, ltq, instrumentContractCode);		
					
					md.sendToBroadcast(true);
				}
				else {
					return;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
}

void NFCAST_SNAPSHOT::decompressIndexChnageMessage1(char* buffer, size_t length)
{
	try
	{
		int pos = 0;
		pos += 26;

		int16_t noOfRecords = readTwoByte(buffer, pos);

		for (int i = 0; i < noOfRecords; i++) {

			if (pos < length) {

				int32_t instrumentContractCode = readFourByte(buffer, pos);			//Token number
				//if (instrumentContractCode < 100)
				//std::cout << "Token number : " << instrumentContractCode << std::endl;

				auto it = secFileReader._symbolsInFile.find(instrumentContractCode);
				if (it != secFileReader._symbolsInFile.end())
				{
					auto& md = *_map[instrumentContractCode];						// Reference to MarketData object
					
					int32_t indexHigh = readFourByte(buffer, pos);
					md.setHighPrice(double(indexHigh) / FIN::FIN_CONST::DEVIDEBY100);

					int32_t indexLow = readFourByte(buffer, pos);
					md.setLowPrice (double(indexLow) / FIN::FIN_CONST::DEVIDEBY100);
					
					int32_t indexOpen = readFourByte(buffer, pos);
					md.setOpenPrice(double(indexOpen) / FIN::FIN_CONST::DEVIDEBY100);

					int32_t preIndexClose = readFourByte(buffer, pos);
					//md.setClosePrice(double(preIndexClose) / FIN::FIN_CONST::DEVIDEBY100);

					int32_t indexValue = readFourByte(buffer, pos);
					_map[instrumentContractCode]->insertBuyDepth((double(indexValue) / FIN::FIN_CONST::DEVIDEBY100), 1, 0);
					_map[instrumentContractCode]->insertSellDepth((double(indexValue) / FIN::FIN_CONST::DEVIDEBY100), 1, 0);

					pos += 12;
					int16_t indexCloseValueIndicator = readTwoByte(buffer, pos);
					if (indexCloseValueIndicator == 0) {
						md.setClosePrice(double(preIndexClose) / FIN::FIN_CONST::DEVIDEBY100);
					}
					pos += 2;

					md.sendToBroadcast(true);
				}
				else {
					pos+=36;
				}
			}
		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
}

void NFCAST_SNAPSHOT::processClosePrice(char* buffer, size_t length)
{
	int pos = 0;
	pos += 26;
	int16_t noOfRecords = readTwoByte(buffer, pos);

	for (int i = 0; i < noOfRecords; i++)
	{
		if (pos < length)
		{
			int32_t instrumentContractCode = readFourByte(buffer, pos);

			auto it = secFileReader._symbolsInFile.find(instrumentContractCode);
			if (it != secFileReader._symbolsInFile.end())
			{
				auto& md = *_map[instrumentContractCode];						// Reference to MarketData object

				int32_t closePrice = readFourByte(buffer, pos);
				md.setClosePrice(double(closePrice) / FIN::FIN_CONST::DEVIDEBY100);

				pos += 4;
			}
		}
		else {
			return;
		}
	}
}

void NFCAST_SNAPSHOT::processOpenInterest(char* buffer, size_t length)
{
	int pos = 0;
	pos += 26;
	int16_t noOfRecords = readTwoByte(buffer, pos);

	for (int i = 0; i < noOfRecords; i++)
	{
		if (pos < length)
		{
			int32_t instrumentContractCode = readFourByte(buffer, pos);

			auto it = secFileReader._symbolsInFile.find(instrumentContractCode);
			if (it != secFileReader._symbolsInFile.end())
			{
				auto& md = *_map[instrumentContractCode];						// Reference to MarketData object

				int32_t openInterestQty = readFourByte(buffer, pos);
				md.setOpenInterest(openInterestQty);

				pos += 28;
			}
		}
		else {
			return;
		}
	}
}

void NFCAST_SNAPSHOT::processLimitPrice(char* buffer, size_t length)
{
	int pos = 0;
	pos += 26;
	int16_t noOfRecords = readTwoByte(buffer, pos);

	for (int i = 0; i < noOfRecords; i++)
	{
		if (pos < length)
		{
			int32_t instrumentContractCode = readFourByte(buffer, pos);

			auto it = secFileReader._symbolsInFile.find(instrumentContractCode);
			if (it != secFileReader._symbolsInFile.end())
			{
				auto& md = *_map[instrumentContractCode];						// Reference to MarketData object

				int32_t upperLimit = readFourByte(buffer, pos);

				int32_t lowerLimit = readFourByte(buffer, pos);

				pos += 8;
			}
		}
		else {
			return;
		}
	}
}

int32_t NFCAST_SNAPSHOT::checkReadByte(int32_t baseValue, char* buffer, int& pos) {

	int32_t returnAns;
	try
	{
		int16_t result;
		std::memcpy(&result, buffer + pos, sizeof(short));
		result = ntohs(result);
		if (result == 32766 || result == -32766) {
			pos += sizeof(short);
			return result;
		}
		if (result == 32767)
		{
			pos += sizeof(short);
			std::memcpy(&returnAns, buffer + pos, sizeof(long));
			returnAns = ntohl(returnAns);
			pos += sizeof(long);
		}
		else {

			pos += sizeof(short);
			returnAns = baseValue + result;
		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO<< e.what();
	}

	return returnAns;
}

void NFCAST_SNAPSHOT::storeTokenInMap(std::unordered_set<int32_t> _symbolsInFile)
{
	try
	{
		for (auto itr = _symbolsInFile.begin(); itr != _symbolsInFile.end(); itr++)
		{
			_map.insert(std::make_pair(*itr, new HandleData(exchNumber, std::to_string(*itr), _publisher)));
		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO << "Exception_storeTokenInMap : " << e.what();
	}
}

int16_t NFCAST_SNAPSHOT::readTwoByte(char* buffer, int& pos) {

	int16_t returnValue;
	try
	{
		std::memcpy(&returnValue, buffer + pos, sizeof(short));
		returnValue = ntohs(returnValue);
		pos += sizeof(short);
		return returnValue;
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
	return 0;
}

int32_t NFCAST_SNAPSHOT::readFourByte(char* buffer, int& pos) {

	int32_t returnValue;
	try
	{
		std::memcpy(&returnValue, buffer + pos, sizeof(long));
		returnValue = ntohl(returnValue);
		pos += sizeof(long);
		return returnValue;
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
	return 0;
}

int64_t NFCAST_SNAPSHOT::readEightByte(char* buffer, int& pos) {

	int64_t returnValue;
	try
	{
		std::memcpy(&returnValue, buffer + pos, sizeof(long long));
		returnValue = ntohll(returnValue);
		pos += sizeof(int64_t);
		return returnValue;
	}
	catch (const std::exception& e)
	{
		LOG_INFO << e.what();
	}
	return 0;
}

void NFCAST_SNAPSHOT::readBuyDepth(MarketPictureBroadcast& marketMsg, char* buffer, int& pos, int16_t noOfPricePoints, int32_t ltp, int32_t ltq, int32_t instrumentContractCode)
{
	try
	{
		for (int j = 0; j < noOfPricePoints; j++)
		{

			if (j == 0) {
				marketMsg.priceDepth[j].bestBidRate = checkReadByte(ltp, buffer, pos);
				if (marketMsg.priceDepth[j].bestBidRate == 32766)
					break;

				marketMsg.priceDepth[j].totalBidQuantity = checkReadByte(ltq, buffer, pos);

				marketMsg.priceDepth[j].noOfBidAtPricePoints = checkReadByte(ltq, buffer, pos);

				marketMsg.priceDepth[j].impliedBuyQuantity = checkReadByte(ltq, buffer, pos);
				//std::cout <<"b " << marketMsg.priceDepth[j].bestBidRate << " : " << marketMsg.priceDepth[j].totalBidQuantity << " : " << j << std::endl;
			}
			else {
				marketMsg.priceDepth[j].bestBidRate = checkReadByte(marketMsg.priceDepth[j - 1].bestBidRate, buffer, pos);
				if (marketMsg.priceDepth[j].bestBidRate == 32766)
					break;

				marketMsg.priceDepth[j].totalBidQuantity = checkReadByte(marketMsg.priceDepth[j - 1].totalBidQuantity, buffer, pos);

				marketMsg.priceDepth[j].noOfBidAtPricePoints = checkReadByte(marketMsg.priceDepth[j - 1].noOfBidAtPricePoints, buffer, pos);

				marketMsg.priceDepth[j].impliedBuyQuantity = checkReadByte(marketMsg.priceDepth[j - 1].impliedBuyQuantity, buffer, pos);
				//std::cout <<"b " << marketMsg.priceDepth[j].bestBidRate << " : " << marketMsg.priceDepth[j].totalBidQuantity << " : " << j << std::endl;
			}

			//insert in market depth
			_map[instrumentContractCode]->insertBuyDepth((double(marketMsg.priceDepth[j].bestBidRate) / FIN::FIN_CONST::DEVIDEBY100), marketMsg.priceDepth[j].totalBidQuantity, j);

		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO <<"buyDepth " << e.what();
	}
}

void NFCAST_SNAPSHOT::readSellDepth(MarketPictureBroadcast& marketMsg, char* buffer, int& pos, int16_t noOfPricePoints, int32_t ltp, int32_t ltq, int32_t instrumentContractCode)
{
	try
	{
		for (int j = 0; j < noOfPricePoints; j++)
		{

			if (j == 0) {
				marketMsg.priceDepth[j].bestOfferRate = checkReadByte(ltp, buffer, pos);
				if (marketMsg.priceDepth[j].bestOfferRate == -32766)
					break;

				marketMsg.priceDepth[j].totalOfferQty = checkReadByte(ltq, buffer, pos);

				marketMsg.priceDepth[j].noOfAskAtPricePoint = checkReadByte(ltq, buffer, pos);

				marketMsg.priceDepth[j].impliedSellQuantity = checkReadByte(ltq, buffer, pos);

			}
			else {
				marketMsg.priceDepth[j].bestOfferRate = checkReadByte(marketMsg.priceDepth[j - 1].bestOfferRate, buffer, pos);
				if (marketMsg.priceDepth[j].bestOfferRate == -32766)
					break;

				marketMsg.priceDepth[j].totalOfferQty = checkReadByte(marketMsg.priceDepth[j - 1].totalOfferQty, buffer, pos);

				marketMsg.priceDepth[j].noOfAskAtPricePoint = checkReadByte(marketMsg.priceDepth[j - 1].noOfAskAtPricePoint, buffer, pos);

				marketMsg.priceDepth[j].impliedSellQuantity = checkReadByte(marketMsg.priceDepth[j - 1].impliedSellQuantity, buffer, pos);
			}
			//std::cout << "s " << marketMsg.priceDepth[j].bestOfferRate << " : " << marketMsg.priceDepth[j].bestOfferRate << " : " << j << std::endl;
			_map[instrumentContractCode]->insertSellDepth((double(marketMsg.priceDepth[j].bestOfferRate) / FIN::FIN_CONST::DEVIDEBY100), marketMsg.priceDepth[j].totalOfferQty, j);
		}
	}
	catch (const std::exception& e)
	{
		LOG_INFO << "sell depth " << e.what();
	}
}