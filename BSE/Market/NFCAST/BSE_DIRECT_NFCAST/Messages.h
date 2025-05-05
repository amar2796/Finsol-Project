#pragma once
#ifndef FIN_MESSAGES_H
#define FIN_MESSAGES_H

typedef struct PriceDepth {
    long bestBidRate;
    long totalBidQuantity;
    long noOfBidAtPricePoints;
    long impliedBuyQuantity;
    long bestOfferRate;
    long totalOfferQty;
    long noOfAskAtPricePoint;
    long impliedSellQuantity;
} PriceDepth;

typedef struct MarketPictureRecord {
    long instrumentContractCode;
    long noOfTrades;
    long tradedVolume;
    long tradedValue;
    char tradeValueFlag;
    char reservedField6;
    char reservedField7;
    char reservedField8;
    short marketType;
    short sessionNumber;
    char ltpHour;
    char ltpMinute;
    char ltpSecond;
    char ltpMillisecond[3];
    char reserverdField8[2];
    short reserverdField9;
    short noOfPricePoints;
    long long timestamp;
    long closeRate;
    long ltq;
    long ltp;
    long openRate;
    long previousCloseRate;
    long highRate;
    long lowRate;
    long blockDealReferencePrice;
    long indicativeEquilibriumPrice;
    long indicativeEquilibriumQuantity;
    long totalBidQuantity;
    long totalOfferQuantity;
    long lowerCircuitLimit;
    long upperCircuitLimit;
    long weightedAveragePrice;
    PriceDepth priceDepth[5];
} MarketPictureRecord;

typedef struct MarketPictureBroadcast {
    PriceDepth priceDepth[5];
} MarketPictureBroadcast;


//typedef struct MarketPictureBroadcast {
//    long messageType;
//    long reservedField1;
//    long reservedField2;
//    unsigned short reservedField3;
//    short hour;
//    short minute;
//    short second;
//    short millisecond;
//    short reservedField4;
//    short reservedField5;
//    short noOfRecords;
//    MarketPictureRecord records[6];
//} MarketPictureBroadcast;

typedef struct TimeBroadcastMessage {
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
    short reservedField6;
    char reservedField7;
    char reservedField8;
    char reservedField9[2];
} TimeBroadcastMessage;             //done

typedef struct ProductStateChangeMessage {
    long messageType;
    long reservedField1;
    long reservedField2;
    unsigned short reservedField3;
    short hour;
    short minute;
    short second;
    short millisecond;
    short productID;
    short reservedField4;
    short filler;
    short marketType;
    short sessionNumber;
    long reservedField5;
    char startEndFlag;
    char reservedField6;
    char reservedField7[2];
} ProductStateChangeMessage;

typedef struct NewsHeadlineMessage {
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
    short reservedField6;
    short newsCategory;
    short reservedField7;
    long newsId;
    char newsHeadline[40];
    char reservedField8;
    char reservedField9;
    char reservedField10[2];
} NewsHeadlineMessage;

typedef struct AuctionMarketPictureBroadcast {
    long messageType;
    long reservedField1;
    long reservedField2;
    unsigned short reservedField3;
    short hour;
    short minute;
    short second;
    short millisecond;
    short auctionNumber;
    short auctionTradingSession;
    short noOfRecords;
    char noticeNumber[11];
    char reservedField4;
    struct AuctionMarketRecord {
        long instrumentContractCode;
        long reservedField5;
        long auctionQty;
        long ceilingPrice;
        long floorPrice;
        long CutOffRate;
        long lowestOfferedRate;
        long cumulativeQty;
        long reservedField6;
        short reservedField7;
        short reservedField8;
        char reservedField9;
        char reservedField10;
        char reservedField11;
        char reservedField12;
        struct LikelyCutOff {
            long likelyCutOffRate;
            long offerQty;
        } likelyCutOff[5];
    } auctionMarketRecords[10];
} AuctionMarketPictureBroadcast;

typedef struct OddLotMarketPictureMessage {
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
    short noOfRecords;
    struct OddLotRecord {
        long instrumentContractCode;
        long openRate;
        long previousCloseRate;
        long highRate;
        long lowRate;
        long noOfTrades;
        long long tradedVolume;
        long tradedValue;
        long long ltq;
        long ltp;
        long closeRate;
        char tradeValueFlag;
        char reservedField7;
        char reservedField8;
        char reservedField9;
        long lowerCircuitLimit;
        long upperCircuitLimit;
        long weightedAveragePrice;
        short sessionNumber;
        char ltpHour;
        char ltpMinute;
        char ltpSecond;
        char ltpMillisecond[3];
        char reservedField10[2];
    } oddLotRecords[6];
} OddLotMarketPictureMessage;

typedef struct DebtMarketPictureMessage {
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
    short noOfRecords;
    struct DebtInstrumentRecord {
        long instrumentCode;
        long noOfTrades;
        long volume;
        long value;
        char tradeValueFlag;
        char reservedField6;
        char reservedField7;
        char reservedField8;
        short marketType;
        short sessionNumber;
        char ltpHour;
        char ltpMinute;
        char ltpSecond;
        char ltpMillisecond[3];
        char reservedField9[2];
        short reservedField10;
        short noOfPricePoints;
        long long timestamp;
        long closeRate;
        long ytm;
        long ytp;
        long ytc;
        long lastTradeQty;
        long ltp;
        long openRate;
        long previousCloseRate;
        long highRate;
        long lowRate;
        long reservedField11;
        long indicativeEquilibriumPrice;
        long indicativeEquilibriumQty;
        long totalBidQty;
        long totalOfferQty;
        long lowerCircuitLimit;
        long upperCircuitLimit;
        long weightedAverage;
        struct PricePoint {
            long bestBidRate;
            long totalBidQty;
            long buyYTM;
            long buyYTP;
            long buyYTC;
            long noOfBidAtPricePoints;
            long filler;
            long bestOfferRate;
            long totalOfferQty;
            long sellYTM;
            long sellYTP;
            long sellYTC;
            long noOfAskAtPricePoint;
            long filler1;
        } pricePoints[5];
    } debtInstrumentRecords[6];
} DebtMarketPictureMessage;

typedef struct IndexChangeMessage {
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
    short noOfRecords;
    struct IndexRecord {
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
    } indexRecords[24];
    short reservedField10;
} IndexChangeMessage;

typedef struct ClosePriceMessage {
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
    short noOfRecords;
    struct InstrumentClosePrice {
        long instrumentCode;
        long price;
        char reservedField6;
        char tradedFlag;
        char reservedField7;
        char reservedField8;
    } instrumentClosePrices[80];
} ClosePriceMessage;

typedef struct OpenInterestMessage {
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
    short noOfRecords;
    struct OIRecord {
        long instrumentCode;
        long openInterestQuantity;
        long long openInterestValue;
        long openInterestChange;
        char reservedField6[4];
        long reservedField7;
        short reservedField8;
        short reservedField9;
        char reservedField10;
        char reservedField11;
        char reservedField12[2];
    } oiRecords[26];
} OpenInterestMessage;

typedef struct VaRPercentageMessage {
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
    short noOfRecords;
    struct VaRRecord {
        long instrumentCode;
        long varImPercentage;
        long elmVarPercentage;
        long reservedField6;
        short reservedField7;
        short reservedField;
        char reservedField8;
        char identifier;
        char reservedField9[2];
    } varRecords[40];
} VaRPercentageMessage;

typedef struct RBIReferenceRateMessage {
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
    short noOfRecords;
    struct RBIReferenceRecord {
        long underlyingAssetId;
        long rbiRate;
        short reservedField6;
        short reservedField7;
        char date[11];
        char filler;
    } rbiReferenceRecords[];
} RBIReferenceRateMessage;

typedef struct ImpliedVolatilityMessage {
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

    struct Record {
        long instrumentId;
        long long impliedVolatility;
        long long reservedField6;
        long long reservedField7;
        long long reservedField8;
        long long reservedField9;
        long long reservedField10;
        long long reservedField11;
        long reservedField12;
        short reservedField13;
        short reservedField14;
        char reservedField15;
        char reservedField16;
        char reservedField17[2];
    };

    Record records[13]; // Array of records
}ImpliedVolatilityMessage;

#endif
