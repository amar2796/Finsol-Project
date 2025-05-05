#pragma once
#pragma once
#ifndef FIN_SNAPSHOT_EMDI_PROCESS
#define FIN_SNAPSHOT_EMDI_PROCESS
#include "Messages.h"
#include <vector>
#include "ConfigReader.h"

#include "MulticastReceiver.h"
#include "constants.h"
#include "Publisher.h"

#include <unordered_map>
#include "marketDataMessage.h"
#include "handleData.h"
using namespace FIN;

typedef struct {
	long messageType;
	long reservedField1;
	long reservedField2;
	unsigned short reservedField3;
	short hour;
	short minute;
	short second;
	short millisecond;
	short reservedField4;
	short reservedField5;
	short numberOfRecords;
} indexChangeMsgH;
typedef struct {
	long indexCode;
	long indexHigh;
	long indexLow;
	long indexOpen;
	long previousIndexClose;
	long indexValue;
	char indexID[7];
	char reservedField6;
	char reservedField7;
	char reservedField8;
	char reservedField9[2];
	short indexCloseValueIndicator;
	short reservedField10;

} indexChangeMsgBody;

class NFCAST_SNAPSHOT
{
public:
	NFCAST_SNAPSHOT(Config_Details* config, UNSIGNED_INT_16 exchID);
	~NFCAST_SNAPSHOT();
	//uint64_t ntohll(uint64_t value);
	void init();
	void processData();
	void decompressMarketPicture(char* buffer, int32_t length);
	void decompressMarketPictureComplexInstruments(char* buffer, size_t length);
	void decompressIndexChnageMessage1(char* buffer, size_t length);
	void processClosePrice(char* buffer, size_t length);
	void processOpenInterest(char* buffer, size_t length);
	void processLimitPrice(char* buffer, size_t length);
	void storeTokenInMap(std::unordered_set < int32_t > _symbolsInFile);
	
	int32_t checkReadByte(int32_t baseValue, char* buffer, int& pos);
	int16_t readTwoByte(char* buffer, int& pos);
	int32_t readFourByte(char* buffer, int& pos);
	int64_t readEightByte(char* buffer, int& pos);
	void readBuyDepth(MarketPictureBroadcast& marketMsg, char* buffer, int& pos, int16_t noOfPricePoints, int32_t ltp, int32_t ltq, int32_t instrumentContractCode);
	void readSellDepth(MarketPictureBroadcast& marketMsg, char* buffer, int& pos, int16_t noOfPricePoints, int32_t ltp, int32_t ltq, int32_t instrumentContractCode);

private:
	Publisher _publisher;
	Config_Details* local_config;
	FIN::MulticastReceiver snap_rcver;
	SecurityFileHandler secFileReader;

	std::unordered_map < int, HandleData* > _map;
	UNSIGNED_INT_16 exchNumber;
};

#endif