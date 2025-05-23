#ifndef FIN_NSE_TRIMMED_FO_MESSAGES_H
#define FIN_NSE_TRIMMED_FO_MESSAGES_H

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include "defines.h"
#include <string>

#pragma pack (2)

namespace FIN {

	namespace NSE {

		namespace COM {



			struct PACKET_INFO {

				CREATE_NNF_STRING(cNetId, 2)
					CREATE_NNF_SHORT(iNoPackets)
					char cPackData[512];

			};
			struct COMPRESSED_PACKET_INFO
			{
				CREATE_NNF_SHORT(comLen)
					char broadcastData[];
			};
			struct MESSAGE_HEADER
			{
				CREATE_NNF_SHORT(TransCode)
					CREATE_NNF_LONG(LogTime)
					CREATE_NNF_STRING(AlphaChar, 2)
					CREATE_NNF_LONG(TraderId)
					CREATE_NNF_SHORT(ErrorCode)
					CREATE_NNF_LONG_LONG(Timestamp)
					CREATE_NNF_STRING(TimeStamp1, 8)
					CREATE_NNF_STRING(TimeStamp2, 8)
					CREATE_NNF_SHORT(MessageLength)

			};
			struct BROADCAST_HEADER
			{
				CREATE_NNF_STRING(Reserved, 4)
					CREATE_NNF_LONG(LogTime)
					CREATE_NNF_STRING(AlphaChar, 2)
					CREATE_NNF_SHORT(TransCode)
					CREATE_NNF_SHORT(ErrorCode)
					CREATE_NNF_LONG(BCSeqNo)
					CREATE_NNF_STRING(Reserved1, 4)
					CREATE_NNF_STRING(TimeStamp2, 8)
					CREATE_NNF_STRING(Filler2, 8)
					CREATE_NNF_SHORT(MessageLength)

			};

			struct OPEN_INTEREST
			{
				CREATE_NNF_LONG(TokeNo)
					CREATE_NNF_LONG(CurrentOI)

			};
			struct OI_UNCOM
			{
				CREATE_NNF_STRING(Reserved, 4)
					CREATE_NNF_LONG(LogTime)
					CREATE_NNF_STRING(AlphaChar, 2)
					CREATE_NNF_SHORT(TransCode)
					CREATE_NNF_SHORT(NoOfRecords)
					CREATE_NNF_STRING(Reserved1, 8)
					CREATE_NNF_LONG_LONG(Timestamp)
					CREATE_NNF_STRING(Reserved2, 8)
					CREATE_NNF_SHORT(MessageLength)
					INCLUDE_STRUCT_ARRAY(OPEN_INTEREST, OpenInterest, 58)
			};
			struct MBP_INDICATOR
			{
				CREATE_NNF_BIT(R1)
					CREATE_NNF_BIT(R2)
					CREATE_NNF_BIT(R3)
					CREATE_NNF_BIT(R4)
					CREATE_NNF_BIT(Sell)
					CREATE_NNF_BIT(Buy)
					CREATE_NNF_BIT(LastTradeLess)
					CREATE_NNF_BIT(LastTradeMore)
					CREATE_NNF_STRING(Reserved, 1)
			};
			struct INDEX
			{
				CREATE_NNF_STRING(IndexName, 21)
					CREATE_NNF_LONG(indexValue)
					CREATE_NNF_LONG(HighIndexValue)
					CREATE_NNF_LONG(LowIndexValue)
					CREATE_NNF_LONG(OpeningIndex)
					CREATE_NNF_LONG(ClosingIndex)
					CREATE_NNF_LONG(PercentChange)
					CREATE_NNF_LONG(YearlyHigh)
					CREATE_NNF_LONG(YearlyLow)
					CREATE_NNF_LONG(NoOfUpmoves)
					CREATE_NNF_LONG(NoOfDownmoves)
					CREATE_NNF_DOUBLE(MarketCapitalisation)
					CREATE_NNF_STRING(NetChangeIndicator, 1)
					CREATE_NNF_STRING(Filler, 1)
			};
			struct BROADCAST_INDICES
			{
				INCLUDE_STRUCT_ARRAY(INDEX, indices, 6)
			};


			struct MBP_INFORMATION
			{
				CREATE_NNF_LONG(Quantity)
					CREATE_NNF_LONG(Price)
					CREATE_NNF_SHORT(NumberOfOrders)
					CREATE_NNF_SHORT(BbBuySellFlag)

				std::string toLogString() const 
				{
					std::ostringstream oss;
					oss << "Quantity=" << std::to_string(getQuantity()) << ", "
						<< "Price=" << std::to_string(getPrice());

					return oss.str();
				}

					MBP_INFORMATION()
				{
				}

				void print() const
				{
					PRINT_NNF_FIELD(Quantity)
						PRINT_NNF_FIELD(Price)
						PRINT_NNF_FIELD(NumberOfOrders)
				}

			};
			struct InteractiverBuffer
			{
					CREATE_NNF_LONG(Token)
					CREATE_NNF_SHORT(BookType)
					CREATE_NNF_SHORT(TradingStatus)
					CREATE_NNF_LONG(VolumeTradedToday)
					CREATE_NNF_LONG(LastTradingPrice)
					CREATE_NNF_STRING(NetChangeIndicator, 1)
					CREATE_NNF_LONG(NetPriceChangeFromClosingPrice)
					CREATE_NNF_LONG(LastTradeQuantity)
					CREATE_NNF_LONG(LastTradeTime)
					CREATE_NNF_LONG(AverageTradePrice)
					CREATE_NNF_SHORT(AuctionNumber)
					CREATE_NNF_SHORT(AuctionStatus)
					CREATE_NNF_SHORT(InitiatorType)
					CREATE_NNF_LONG(InitiatorPrice)
					CREATE_NNF_LONG(InitiatorQuantity)
					CREATE_NNF_LONG(AuctionPrice)
					CREATE_NNF_LONG(AuctionQuantity)
					INCLUDE_STRUCT_ARRAY(MBP_INFORMATION, BuyRecordBuffer, 5)
					INCLUDE_STRUCT_ARRAY(MBP_INFORMATION, SellRecordBuffer, 5)
					CREATE_NNF_SHORT(BbTotalBuyFlag)
					CREATE_NNF_SHORT(BbTotalSellFlag)
					CREATE_NNF_DOUBLE(TotalBuyQuantity)
					CREATE_NNF_DOUBLE(TotalSellQuantity)
					INCLUDE_STRUCT(MBP_INDICATOR, BuySellIndicator)
					CREATE_NNF_LONG(ClosingPrice)
					CREATE_NNF_LONG(OpeningPrice)
					CREATE_NNF_LONG(HighingPrice)
					CREATE_NNF_LONG(LowingPrice)

					std::string toLogString() const {
						std::ostringstream oss;
						oss << "Token=" << getToken() << ", "
							<< "BookType=" << getBookType() << ", "
							<< "TradingStatus=" << getTradingStatus() << ", "
							<< "VolumeTradedToday=" << getVolumeTradedToday() << ", "
							<< "LastTradingPrice=" << getLastTradingPrice() << ", "
							<< "NetChangeIndicator=" << getNetChangeIndicator() << ", "
							<< "NetPriceChangeFromClosingPrice=" << getNetPriceChangeFromClosingPrice() << ", "
							<< "LastTradeQuantity=" << getLastTradeQuantity() << ", "
							<< "LastTradeTime=" << getLastTradeTime() << ", "
							<< "AverageTradePrice=" << getAverageTradePrice() << ", "
							<< "AuctionNumber=" << getAuctionNumber() << ", "
							<< "AuctionStatus=" << getAuctionStatus() << ", "
							<< "InitiatorType=" << getInitiatorType() << ", "
							<< "InitiatorPrice=" << getInitiatorPrice() << ", "
							<< "InitiatorQuantity=" << getInitiatorQuantity() << ", "
							<< "AuctionPrice=" << getAuctionPrice() << ", "
							<< "AuctionQuantity=" << getAuctionQuantity() << ", "
							<< "BbTotalBuyFlag=" << getBbTotalBuyFlag() << ", "
							<< "BbTotalSellFlag=" << getBbTotalSellFlag() << ", "
							<< "TotalBuyQuantity=" << std::to_string(getTotalBuyQuantity()) << ", "
							<< "TotalSellQuantity=" << std::to_string(getTotalSellQuantity()) << ", "
							<< "ClosingPrice=" << getClosingPrice() << ", "
							<< "OpeningPrice=" << getOpeningPrice() << ", "
							<< "HighingPrice=" << getHighingPrice() << ", "
							<< "LowingPrice=" << getLowingPrice();
						return oss.str();
					}

			};
			struct MBP
			{

				CREATE_NNF_SHORT(NoOfRecords)
					INCLUDE_STRUCT_ARRAY(InteractiverBuffer, interactive_buffer, 2)

					MBP()
				{
				}


			};


			struct ST_TICKER_INDEX_INFO
			{
				CREATE_NNF_LONG(Token)
					CREATE_NNF_SHORT(MarketType)
					CREATE_NNF_LONG(FillPrice)
					CREATE_NNF_LONG(FillVolume)
					CREATE_NNF_LONG(openInterest)
					CREATE_NNF_LONG(DailyHiOI)
					CREATE_NNF_LONG(DailyLoOI)
			};


			// 7202
			struct MS_TICKER_TRADE_DATA
			{
				CREATE_NNF_SHORT(NoOfRecords)
					INCLUDE_STRUCT_ARRAY(ST_TICKER_INDEX_INFO, st_ticker_index_info, 17)
			};



			struct ST_MKT_WISE_INFO
			{
				CREATE_NNF_SHORT(ST_INDICATOR)
					CREATE_NNF_LONG(BuyVolume)
					CREATE_NNF_LONG(BuyPrice)
					CREATE_NNF_LONG(SellVolume)
					CREATE_NNF_LONG(SellPrice)
					CREATE_NNF_LONG(LastTradePrice)
					CREATE_NNF_LONG(LastTradeTime)

					ST_MKT_WISE_INFO()
				{
				}


			};

			struct ST_MARKET_WATCH_BCAST
			{

				CREATE_NNF_LONG(Token)
					INCLUDE_STRUCT_ARRAY(ST_MKT_WISE_INFO, stk_wise, 3)
					CREATE_NNF_LONG(OpenInterest)

			};

			struct MarketWatch
			{
				CREATE_NNF_SHORT(NoOfRecords)
					INCLUDE_STRUCT_ARRAY(ST_MARKET_WATCH_BCAST, stk_market_watch, 5)
			};


			struct OI                 //Open High Low
			{
				CREATE_NNF_LONG(Token)
					CREATE_NNF_LONG(OpenPrice)
					CREATE_NNF_LONG(HighPrice)
					CREATE_NNF_LONG(LowPrice)
					CREATE_NNF_LONG(ClosePrice)
					CREATE_NNF_LONG(52WHighPrice)
					CREATE_NNF_LONG(52WLowPrice)

					OI()
				{
				}

				void print() const
				{
					PRINT_NNF_FIELD(Token)
						PRINT_NNF_FIELD(OpenPrice)
						PRINT_NNF_FIELD(HighPrice)
						PRINT_NNF_FIELD(LowPrice)
						PRINT_NNF_FIELD(ClosePrice)
						PRINT_NNF_FIELD(52WHighPrice)
						PRINT_NNF_FIELD(52WLowPrice)
				}
			};

			struct MASTER_SECURITY                //DPR
			{
				CREATE_NNF_LONG(Token)
					CREATE_NNF_CHAR(SecurityEligibility)
					CREATE_NNF_LONG(LowPriceRange)
					CREATE_NNF_LONG(HighPriceRange)
					CREATE_NNF_LONG(LowExecutionRange)
					CREATE_NNF_LONG(HighExecutionRange)


					MASTER_SECURITY()
				{
				}

				void print() const
				{
					PRINT_NNF_FIELD(Token)
						PRINT_NNF_FIELD(SecurityEligibility)
						PRINT_NNF_FIELD(HighPriceRange)
						PRINT_NNF_FIELD(LowExecutionRange)
						PRINT_NNF_FIELD(HighExecutionRange)

				}
			};

			struct Ticker_Trade
			{
				CREATE_NNF_LONG(Token)
					CREATE_NNF_LONG(Value)
					CREATE_NNF_LONG(TradeQty)
					CREATE_NNF_LONG(LastTradeTime)

					Ticker_Trade()
				{
				}

				void print() const
				{
					PRINT_NNF_FIELD(Token)
						PRINT_NNF_FIELD(Value)
						PRINT_NNF_FIELD(TradeQty)
						PRINT_NNF_FIELD(LastTradeTime)
				}
			};



			struct MBP_INFORMATION_IFSC
			{
				CREATE_NNF_LONG(Quantity)
					CREATE_NNF_LONG(Price)
					CREATE_NNF_SHORT(NumberOfOrders)
					MBP_INFORMATION_IFSC()
				{
				}

				void print() const
				{
					PRINT_NNF_FIELD(Quantity)
						PRINT_NNF_FIELD(Price)
						PRINT_NNF_FIELD(NumberOfOrders)
				}

			};
			struct MBP_IFSC
			{

				CREATE_NNF_LONG(Token)
					CREATE_NNF_LONG(VolumeTradedToday)
					CREATE_NNF_LONG(LastTradingPrice)
					CREATE_NNF_LONG(LastTradeTime)
					CREATE_NNF_LONG(VWAP)
					CREATE_NNF_DOUBLE(TotalBuyQuantity)
					CREATE_NNF_DOUBLE(TotalSellQuantity)
					INCLUDE_STRUCT(MBP_INDICATOR, BuySellIndicator)
					INCLUDE_STRUCT_ARRAY(MBP_INFORMATION_IFSC, BuyRecordBuffer, 5)
					INCLUDE_STRUCT_ARRAY(MBP_INFORMATION_IFSC, SellRecordBuffer, 5)


					MBP_IFSC()
				{
				}


			};
			struct BCAST_HEADER_IFSC
			{
				CREATE_NNF_SHORT(TransCode)
					CREATE_NNF_SHORT(CompressedSize)
					CREATE_NNF_LONG(NumberOfRecords)
			};
			struct Ticker_Trade_IFSC
			{
				CREATE_NNF_LONG(Token)
					CREATE_NNF_LONG(Value)
					CREATE_NNF_LONG(TradeQty)
					CREATE_NNF_LONG(LastTradeTime)

					Ticker_Trade_IFSC()
				{
				}

				void print() const
				{
					PRINT_NNF_FIELD(Token)
						PRINT_NNF_FIELD(Value)
						PRINT_NNF_FIELD(TradeQty)
						PRINT_NNF_FIELD(LastTradeTime)
				}
			};
			struct OI_OHLC_IFSC                 //Open High Low
			{
				CREATE_NNF_LONG(Token)
					CREATE_NNF_LONG(OpenPrice)
					CREATE_NNF_LONG(HighPrice)
					CREATE_NNF_LONG(LowPrice)
					CREATE_NNF_LONG(ClosePrice)
					CREATE_NNF_LONG(52WHighPrice)
					CREATE_NNF_LONG(52WLowPrice)

					OI_OHLC_IFSC()
				{
				}

				void print() const
				{
					PRINT_NNF_FIELD(Token)
						PRINT_NNF_FIELD(OpenPrice)
						PRINT_NNF_FIELD(HighPrice)
						PRINT_NNF_FIELD(LowPrice)
						PRINT_NNF_FIELD(ClosePrice)
						PRINT_NNF_FIELD(52WHighPrice)
						PRINT_NNF_FIELD(52WLowPrice)
				}
			};
			struct EOD_BHAV_IFSC                 //Open High Low
			{
				CREATE_NNF_LONG(Token)
					CREATE_NNF_SHORT(MarketType)
					CREATE_NNF_LONG(OpenPrice)
					CREATE_NNF_LONG(HighPrice)
					CREATE_NNF_LONG(LowPrice)
					CREATE_NNF_LONG(ClosePrice)
					CREATE_NNF_LONG(TotalQuantityTraded)
					CREATE_NNF_LONG_LONG(Total_Value_Traded)
					CREATE_NNF_LONG(Prev_Close_Price)
					CREATE_NNF_LONG(OpenInterest)
					CREATE_NNF_LONG(CHANGE_OI)
					CREATE_NNF_LONG(CorporateIndicator)

					EOD_BHAV_IFSC()
				{
				}

				void print() const
				{

				}
			};




		}
	}
}
#endif