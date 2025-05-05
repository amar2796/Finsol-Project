#ifndef FIN_MESSAGES_H
#define FIN_MESSAGES_H
#pragma pack ( push, 2)

#include <stdlib.h>
#include <iostream>
#include "transcodes.h"
#include "globalDefines.h"
#include <string>
#include <vector>
#include <sstream>
#include "boost/lexical_cast.hpp"
#include "BYTECONVERSION.h"
#pragma pack(2) 
namespace FIN {
	//Client
	struct DC_PACKET_WRAPPER
	{
		CREATE_NNF_SHORT(Length);
		CREATE_NNF_LONG(SequenceNo);
		CREATE_NNF_STRING(MD5Checksum, 16);

		DC_PACKET_WRAPPER()
		{
		}


		DC_PACKET_WRAPPER(size_t length)
		{
			reset();
			memset(this, 0, length);
			setLength(length);
			setMD5Checksum(std::string(16, ' '));
		}

		void print() const
		{
			PRINT_NNF_FIELD(Length);
			PRINT_NNF_FIELD(SequenceNo);
			//PRINT_NNF_FIELD ( MD5Checksum ) ;
		}

		void reset()
		{
			setSequenceNo(0);
			setMD5Checksum(std::string(16, ' '));
		}
	};
	struct MESSAGE_HEADER
	{
		CREATE_NNF_SHORT(TransCode)
			CREATE_NNF_LONG(LogTime)
			CREATE_NNF_STRING(AlphaChar, 2)
			CREATE_NNF_LONG(TraderId)
			CREATE_NNF_SHORT(ErrorCode)
			//CREATE_NNF_STRING(Timestamp, 8)
			CREATE_NNF_LONG_LONG(Timestamp)
			CREATE_NNF_STRING(Timestamp1, 8)
			CREATE_NNF_STRING(Timestamp2, 8)
			CREATE_NNF_SHORT(MessageLength)

			MESSAGE_HEADER()
		{
			reset();

		}

		MESSAGE_HEADER(TransCode tc, SIGNED_LONG traderID, size_t msgLength)
		{
			reset();
			setTransCode(tc);
			setTraderId(traderID);
			setMessageLength(msgLength);
		}
		char* htonf8(const char* value)
		{
			double result;
			char* pSource = reinterpret_cast<char*>(&value);
			char pDest[8];//= reinterpret_cast<char*>(&result);
			pDest[0] = pSource[7];
			pDest[1] = pSource[6];
			pDest[2] = pSource[5];
			pDest[3] = pSource[4];
			pDest[4] = pSource[3];
			pDest[5] = pSource[2];
			pDest[6] = pSource[1];
			pDest[7] = pSource[0];
			return pDest;
		}
		void getString(std::string& str)
		{

			str.append(std::to_string(getTransCode()));
			str.append("|");
			str.append(std::to_string(getLogTime()));
			str.append("|");
			str.append(getAlphaChar());
			str.append("|");
			str.append(std::to_string(getTraderId()));
			str.append("|");
			str.append(std::to_string(getErrorCode()));
			str.append("|");
			//str.append(getTimestamp());
			str.append(std::to_string(getTimestamp()));
			str.append("|");
			str.append(getTimestamp1());
			str.append("|");
			str.append(getTimestamp2());
			str.append("|");
			str.append(std::to_string(getMessageLength()));
			//char s1[4];
			//char s2[4];
			//memcpy(s1, getTimestamp1(), 4);
			//memcpy(s2, &_Timestamp1[4], 4);
			/*char s3[8];
			memcpy(s3, getTimestamp1(), 8);*/
			////DEVLOG("Seq: " << atof(s1) << "-" << atof(s2));						

			//if (getTransCode() == 2222) {
			//	double out,out1;
			//	char* s4 = htonf8(getTimestamp1());
			//	memcpy(&out, s4, 8);
			//	memcpy(&out1, s3, 8);
			//	printf(" %lld %lld\n", (long long)out,(long long)out1);
			//	/*printf("%f %f %f %f %f %f %f %f\n", s3[0], s3[1], s3[2], s3[3], s3[4], s3[5], s3[6], s3[7]);
			//	printf("%d %d %d %d %d %d %d %d\n", s3[0], s3[1], s3[2], s3[3], s3[4], s3[5], s3[6], s3[7]);*/
			//}
		}

		// till here..................
		void reset()
		{
			//memset(this, 0, sizeof(MESSAGE_HEADER));

			//std::cout << "MSG_HDR_RESETCALLED" << std::endl;
			//memset(this, 0, sizeof(MESSAGE_HEADER));
			setTransCode(0);
			setLogTime(0);
			setAlphaChar(std::string(2, ' '));
			setTraderId(0);
			setErrorCode(0);
			//setTimestamp  ( std::string(8,' ')); 
			setTimestamp(0);
			//setTimestamp1 ( std::string(8,' '));
			setTimestamp1(std::string("0        ", 8));
			setTimestamp2(std::string("0        ", 8));
			setMessageLength(0);
		}

		short getTCode() { return _TransCode; }

		char getStreamNo() { return _AlphaChar[0]; }
		void setStreamNo(char val) { _AlphaChar[0] = val; }
		void print() const
		{
			PRINT_BODY_START(MESSAGE_HEADER)
				PRINT_NNF_FIELD(TransCode)
				PRINT_NNF_FIELD(LogTime)
				PRINT_NNF_FIELD(AlphaChar)
				PRINT_NNF_FIELD(TraderId)
				PRINT_NNF_FIELD(ErrorCode)
				PRINT_NNF_FIELD(Timestamp)
				PRINT_NNF_FIELD(Timestamp1)
				PRINT_NNF_FIELD(Timestamp2)
				PRINT_NNF_FIELD(MessageLength)

				char s1[4];
			char s2[4];

			memcpy(s1, _Timestamp1, 4);
			memcpy(s2, &_Timestamp1[4], 4);
			//DEVLOG("Seq: " << atof(s1) << "-" << atof(s2));

			PRINT_BODY_END(MESSAGE_HEADER)
		}
	};

	//23506
	struct HEARTBEAT_MESSAGE : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
	{

		HEARTBEAT_MESSAGE(int32_t traderID)
			:DC_PACKET_WRAPPER(sizeof(HEARTBEAT_MESSAGE)),
			MESSAGE_HEADER(TransCode_HEARTBEAT, traderID, sizeof(HEARTBEAT_MESSAGE) - sizeof(DC_PACKET_WRAPPER))
		{
		}
		void getString(std::string& str)
		{
			//str.append(MESSAGE_HEADER::getString(str));
			MESSAGE_HEADER::getString(str);

		}

		void print() const
		{
			PRINT_BODY_START(HEARTBEAT_MESSAGE)
				MESSAGE_HEADER::print();
			PRINT_BODY_END(HEARTBEAT_MESSAGE)
		}
	};

	struct TEST_TIME_STAMP : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
	{
		TEST_TIME_STAMP(SIGNED_LONG traderID)
			:DC_PACKET_WRAPPER(sizeof(TEST_TIME_STAMP)),
			MESSAGE_HEADER(TrancCode_TIMESTAMP_LOCAL, traderID, sizeof(TEST_TIME_STAMP) - sizeof(DC_PACKET_WRAPPER))
		{
		}

		void print() const
		{
			//PRINT_BODY_START( TEST_TIME_STAMP )
			PRINT_NNF_FIELD(TimeStamp)
				//PRINT_BODY_END( TEST_TIME_STAMP )
		}

		CREATE_NNF_LONG_LONG(TimeStamp)
	};
	struct MS_SIGNON : public MESSAGE_HEADER
	{
		CREATE_NNF_LONG(UserID)
			CREATE_NNF_STRING(Reserved1, 8)
			CREATE_NNF_STRING(Password, 8)
			CREATE_NNF_STRING(Reserved2, 8)
			CREATE_NNF_STRING(Reserved3, 38)
			CREATE_NNF_STRING(BrokerID, 5)
			CREATE_NNF_STRING(Reserved4, 119)
			CREATE_NNF_STRING(Reserved5, 16)
			CREATE_NNF_STRING(Reserved6, 16)
			CREATE_NNF_STRING(Reserved7, 16)

			MS_SIGNON()
		{
			reset();
		}

		MS_SIGNON(SIGNED_LONG traderID)
			:MESSAGE_HEADER(TransCode_SIGN_ON_REQUEST_IN,
				traderID, sizeof(MS_SIGNON))
		{
			//std::cout << "Msg signon:" << sizeof(MS_SIGNON) << std::endl;
			//reset();
		}

		void getString(std::string& str)
		{
			MESSAGE_HEADER::getString(str);
			str.append("|");
			str.append(std::to_string(getUserID()));
			str.append("|");
			str.append(getReserved1());
			str.append("|");
			str.append(getPassword());
			str.append("|");
			str.append(getReserved2());
			str.append("|");
			str.append(getReserved3());
			str.append("|");
			str.append(getBrokerID());
			str.append("|");
			str.append(getReserved4());
			str.append("|");
			str.append(getReserved5());
			str.append("|");
			str.append(getReserved6());
			str.append("|");
			str.append(getReserved7());


		}

		void reset()
		{
			std::cout << "MSG_SIGNON_RESET_CALLED" << std::endl;
			memset(this, ' ', sizeof(MS_SIGNON));
			setUserID(0);
			//setReserved1(std::string("          ", 8));
			//setPassword(std::string("         ", 8));
			//setReserved2(std::string("         ", 8));
			//setReserved3(std::string("                                    ", 38));
			//setBrokerID(std::string("         ", 8));
			//setReserved4(std::string(119, ' '));
			//setReserved5(std::string(16, ' '));
			//setReserved6(std::string(16, ' '));
			//setReserved7(std::string(16, ' '));


		}

		void print() const
		{
			PRINT_BODY_START(MS_SIGNON)
				MESSAGE_HEADER::print();
			PRINT_NNF_FIELD(UserID)
				PRINT_NNF_FIELD(Reserved1)
				PRINT_NNF_FIELD(Password)
				PRINT_NNF_FIELD(Reserved2)
				PRINT_NNF_FIELD(Reserved3)
				PRINT_NNF_FIELD(BrokerID)
				PRINT_NNF_FIELD(Reserved4)
				PRINT_NNF_FIELD(Reserved5)
				PRINT_NNF_FIELD(Reserved6)
				PRINT_NNF_FIELD(Reserved7)
				PRINT_BODY_END(MS_SIGNON)
		}
	};
	struct SIGNON_REQUETS : public DC_PACKET_WRAPPER, public MS_SIGNON
	{
		SIGNON_REQUETS(SIGNED_LONG traderID)
			: DC_PACKET_WRAPPER(sizeof(*this)),
			MS_SIGNON(traderID)
		{

		}

		void getString(std::string& str)
		{
			//str.append (MS_SIGNON :: getString(str));
			MS_SIGNON::getString(str);

		}
		void print() const
		{
			PRINT_BODY_START(SIGNON_REQUETS)
				MS_SIGNON::print();
			PRINT_BODY_END(SIGNON_REQUETS)
		}
	};
	struct SIGNON_CONFIRMATION : public MS_SIGNON
	{

		void getString(std::string& str)
		{
			//str.append (MS_SIGNON :: getString(str));
			MS_SIGNON::getString(str);

		}

		void print() const
		{
			PRINT_BODY_START(SIGNON_CONFIRMATION)
				MS_SIGNON::print();
			PRINT_BODY_END(SIGNON_CONFIRMATION)
		}
	};
	struct MS_ERROR_RESPONSE : public MESSAGE_HEADER
	{
		CREATE_NNF_STRING(Reserved, 14);
		CREATE_NNF_STRING(ErrorMsg, 128);

		MS_ERROR_RESPONSE() : MESSAGE_HEADER()
		{
			reset();
		}

		void getString(std::string& str)
		{
			MESSAGE_HEADER::getString(str);
			str.append("|");
			str.append(getReserved());
			str.append("|");
			str.append(getErrorMsg());
			str.append("|");

		}

		void reset()
		{
			setReserved(std::string(14, ' '));
			setErrorMsg(std::string(128, ' '));
		}
		void print() const
		{
			PRINT_BODY_START(MS_ERROR_RESPONSE)
				//MESSAGE_HEADER::print();
				PRINT_NNF_FIELD(Reserved)
				PRINT_NNF_FIELD(ErrorMsg)
				PRINT_BODY_END(MS_ERROR_RESPONSE)
		}
	};
	// ------------------------------- ORDER ENTRY ------------------------//

	struct CONTRACT_DESC
	{
		CREATE_NNF_STRING(InstrumentName, 6)
			CREATE_NNF_STRING(Symbol, 10)
			CREATE_NNF_LONG(ExpiryDate)
			CREATE_NNF_LONG(StrikePrice)
			CREATE_NNF_STRING(OptionType, 2)
			CREATE_NNF_SHORT(CALevel)

			CONTRACT_DESC()
		{
			reset();
		}
		void getString(std::string& str)
		{

			str.append(getInstrumentName());
			str.append("|");
			str.append(getSymbol());
			str.append("|");
			str.append(std::to_string(getExpiryDate()));
			str.append("|");
			str.append(std::to_string(getStrikePrice()));
			str.append("|");
			str.append(getOptionType());
			str.append("|");
			str.append(std::to_string(getCALevel()));

		}
		void reset()
		{
			setInstrumentName(std::string(6, ' '));
			setSymbol(std::string(10, ' '));
			setExpiryDate(0);
			setStrikePrice(0);
			setOptionType(std::string(2, ' '));
			setCALevel(0);
		}

		void print() const
		{
			PRINT_BODY_START(CONTRACT_DESC)
				PRINT_NNF_FIELD(InstrumentName)
				PRINT_NNF_FIELD(Symbol)
				PRINT_NNF_FIELD(ExpiryDate)
				PRINT_NNF_FIELD(StrikePrice)
				PRINT_NNF_FIELD(OptionType)
				PRINT_NNF_FIELD(CALevel)
				PRINT_BODY_END(CONTRACT_DESC)
		}
	};

	struct ST_ORDER_FLAGS
	{
		CREATE_NNF_BIT(AON)
			CREATE_NNF_BIT(IOC)
			CREATE_NNF_BIT(GTC)
			CREATE_NNF_BIT(Day)
			CREATE_NNF_BIT(MIT)
			CREATE_NNF_BIT(SL)
			CREATE_NNF_BIT(Market)
			CREATE_NNF_BIT(ATO)
			CREATE_NNF_BIT(Reserved1)
			CREATE_NNF_BIT(Reserved2)
			CREATE_NNF_BIT(Reserved3)
			CREATE_NNF_BIT(Frozen)
			CREATE_NNF_BIT(Modified)
			CREATE_NNF_BIT(Traded)
			CREATE_NNF_BIT(MatchedInd)
			CREATE_NNF_BIT(MF)

			ST_ORDER_FLAGS()
		{
			reset();
		}
		void reset()
		{
			memset(this, 0, sizeof(ST_ORDER_FLAGS));
		}
		void getString(std::string& str)
		{

			str.append(std::to_string(getAON())); str.append("|");
			str.append(std::to_string(getIOC())); str.append("|");
			str.append(std::to_string(getGTC())); str.append("|");
			str.append(std::to_string(getDay())); str.append("|");
			str.append(std::to_string(getMIT())); str.append("|");
			str.append(std::to_string(getSL())); str.append("|");
			str.append(std::to_string(getMarket())); str.append("|");
			str.append(std::to_string(getATO())); str.append("|");
			str.append(std::to_string(getReserved1())); str.append("|");
			str.append(std::to_string(getReserved2())); str.append("|");
			str.append(std::to_string(getReserved3())); str.append("|");
			str.append(std::to_string(getFrozen())); str.append("|");
			str.append(std::to_string(getModified())); str.append("|");
			str.append(std::to_string(getTraded())); str.append("|");
			str.append(std::to_string(getMatchedInd())); str.append("|");
			str.append(std::to_string(getMF())); str.append("|");

		}

		void print() const
		{
			PRINT_BODY_START(ST_ORDER_FLAGS)
				PRINT_NNF_FIELD(AON)
				PRINT_NNF_FIELD(IOC)
				PRINT_NNF_FIELD(GTC)
				PRINT_NNF_FIELD(Day)
				PRINT_NNF_FIELD(MIT)
				PRINT_NNF_FIELD(SL)
				PRINT_NNF_FIELD(Market)
				PRINT_NNF_FIELD(ATO)
				//PRINT_NNF_FIELD ( Reserved1)
				//PRINT_NNF_FIELD ( Reserved2)
				//PRINT_NNF_FIELD ( Reserved3)
				PRINT_NNF_FIELD(Frozen)
				PRINT_NNF_FIELD(Modified)
				PRINT_NNF_FIELD(Traded)
				PRINT_NNF_FIELD(MatchedInd)
				PRINT_NNF_FIELD(MF)
				PRINT_BODY_END(ST_ORDER_FLAGS)
		}
	};

	struct ADDITIONAL_ORDER_FLAGS
	{
		CREATE_NNF_BIT(Reserved1)
			CREATE_NNF_BIT(COL)
			CREATE_NNF_BIT(Reserved2)
			CREATE_NNF_BIT(Reserved3)
			CREATE_NNF_BIT(Reserved4)
			CREATE_NNF_BIT(Reserved5)
			CREATE_NNF_BIT(Reserved6)
			CREATE_NNF_BIT(Reserved7)

			ADDITIONAL_ORDER_FLAGS()
		{
			reset();
		}
		void getString(std::string& str)
		{
			str.append(std::to_string(getReserved1())); str.append("|");
			str.append(std::to_string(getCOL())); str.append("|");
			str.append(std::to_string(getReserved2())); str.append("|");
			str.append(std::to_string(getReserved3())); str.append("|");
			str.append(std::to_string(getReserved4())); str.append("|");
			str.append(std::to_string(getReserved5())); str.append("|");
			str.append(std::to_string(getReserved6())); str.append("|");
			str.append(std::to_string(getReserved7())); str.append("|");
		}

		void reset()
		{
			memset(this, 0, sizeof(ADDITIONAL_ORDER_FLAGS));
		}

		void print() const
		{
			PRINT_BODY_START(ADDITIONAL_ORDER_FLAGS)
				//PRINT_NNF_FIELD ( Reserved1)
				PRINT_NNF_FIELD(COL)
				//PRINT_NNF_FIELD ( Reserved2)
				//PRINT_NNF_FIELD ( Reserved3)

				//PRINT_NNF_FIELD ( Reserved4)
				//PRINT_NNF_FIELD ( Reserved5)
				//PRINT_NNF_FIELD ( Reserved6)
				PRINT_BODY_END(ADDITIONAL_ORDER_FLAGS)
		}
	};

	struct DROPCOPY_MSG_DOWNLOAD_REQ :public DC_PACKET_WRAPPER, public MESSAGE_HEADER
	{
		double Sequence_number;
		DROPCOPY_MSG_DOWNLOAD_REQ() : MESSAGE_HEADER()
		{
			reset();
		}
		DROPCOPY_MSG_DOWNLOAD_REQ(SIGNED_LONG traderID)
			: DC_PACKET_WRAPPER(sizeof(DROPCOPY_MSG_DOWNLOAD_REQ)),
			MESSAGE_HEADER(TransCode_DROP_COPY_DOWNLOAD_REQUEST, traderID, sizeof(DROPCOPY_MSG_DOWNLOAD_REQ) - sizeof(DC_PACKET_WRAPPER))
		{
			reset();
		}
		void getString(std::string& str)
		{
			MESSAGE_HEADER::getString(str);
			str.append("|");
			str.append(std::to_string(Sequence_number));
			str.append("|");

		}

		void reset()
		{
			//std::cout << "RESET_CALLED" << std::endl;
			Sequence_number = 0;
		}

		void print() const
		{
			PRINT_BODY_START(DROPCOPY_MSG_DOWNLOAD_REQ)
				MESSAGE_HEADER::print();
			//std::cout << Sequence_number << std::endl;
			PRINT_BODY_END(DROPCOPY_MSG_DOWNLOAD_REQ)
		}
	};

	struct TRADE_CONFIRMATION : public MESSAGE_HEADER
	{
		//CREATE_NNF_DOUBLE (Response_order_number);
		double				 Response_order_number;
		CREATE_NNF_STRING(BrokerID, 5);
		CREATE_NNF_CHAR(Reserved9);
		CREATE_NNF_LONG(TraderNum);
		CREATE_NNF_STRING(AccountNum, 10);
		CREATE_NNF_SHORT(BuySell);
		CREATE_NNF_LONG(OriginalVol);
		CREATE_NNF_LONG(DisclosedVol);
		CREATE_NNF_LONG(RemainingVol);
		CREATE_NNF_LONG(DisclosedVolRemaining);
		CREATE_NNF_LONG(Price);
		INCLUDE_STRUCT(ST_ORDER_FLAGS, orderFlags);
		CREATE_NNF_LONG(Gtd);
		CREATE_NNF_LONG(FillNumber);
		CREATE_NNF_LONG(FillQty);
		CREATE_NNF_LONG(FillPrice);
		CREATE_NNF_LONG(VolFilledToday);
		CREATE_NNF_STRING(ActivityType, 2);
		CREATE_NNF_LONG(ActivityTime);
		double			     OpOrderNumber;
		CREATE_NNF_STRING(OpBrokerId, 5);
		CREATE_NNF_LONG(Token);
		//SIGNED_LONG Token;
		INCLUDE_STRUCT(CONTRACT_DESC, contractDesc);
		CREATE_NNF_CHAR(OpenClose);
		CREATE_NNF_CHAR(OldOpenCLose);
		CREATE_NNF_CHAR(BookType);
		CREATE_NNF_LONG(NewVolume);
		CREATE_NNF_STRING(OldAccountNumber, 10);
		CREATE_NNF_STRING(Participant, 12);
		CREATE_NNF_STRING(OldParticipant, 12);
		INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS, addOrderFlags);
		CREATE_NNF_CHAR(ReservedFiller);
		CREATE_NNF_CHAR(GiveUpTrade);
		CREATE_NNF_CHAR(ReservedFiller2);
		CREATE_NNF_STRING(PAN, 10);
		CREATE_NNF_STRING(OldPAN, 10);
		CREATE_NNF_LONG(AlgoID);
		CREATE_NNF_SHORT(AlgoCategory);
		CREATE_NNF_LONG_LONG(LastActivityReference);
		double nnfField;
		CREATE_NNF_STRING(Reserved, 44);




		TRADE_CONFIRMATION() :MESSAGE_HEADER()
		{
			reset();
		}
		TRADE_CONFIRMATION(TransCode tc, SIGNED_LONG traderID)
			: MESSAGE_HEADER(tc,
				traderID, sizeof(TRADE_CONFIRMATION))
		{
			reset();
		}
		void reset()
		{
			Response_order_number = 0;
			setBrokerID(std::string(5, ' '));
			setReserved9(' ');
			setTraderNum(0);
			//setAccountNum(std::string(10, ' '));
			memset(_AccountNum, ' ', 10);
			setBuySell(0);
			setOriginalVol(0);
			setDisclosedVol(0);
			setRemainingVol(0);
			setDisclosedVolRemaining(0);
			setPrice(0);
			_orderFlags.reset();
			setGtd(0);
			setFillNumber(0);
			setFillQty(0);
			setFillPrice(0);
			setVolFilledToday(0);
			//setActivityType(std::string(2, ' '));
			memset(_ActivityType, ' ', 2);
			setActivityTime(0);
			OpOrderNumber = 0;
			//setOpBrokerId(std::string(5, ' '));
			memset(_OpBrokerId, ' ', 5);
			setToken(0);
			//Token = 0;
			_contractDesc.reset();
			setOpenClose(' ');
			setOldOpenCLose(' ');
			setBookType(' ');
			setNewVolume(0);
			//setOldAccountNumber(std::string(10, ' '));
			memset(_OldAccountNumber, ' ', 10);
			//setParticipant(std::string(12, ' '));
			memset(_Participant, ' ', 12);
			//setOldParticipant(std::string(12, ' '));
			memset(_OldParticipant, ' ', 12);
			_addOrderFlags.reset();
			setReservedFiller(' ');
			setGiveUpTrade(' ');
			setReservedFiller2(' ');
			//setPAN(std::string(10, ' '));
			memset(_PAN, ' ', 10);
			//setOldPAN(std::string(10, ' '));
			memset(_OldPAN, ' ', 10);
			setAlgoID(0);
			setAlgoCategory(0);
			setLastActivityReference(0);
			//setReserved(std::string(52, ' '));
			nnfField = 0;
			memset(_Reserved, ' ', 44);

		}
		void getString(std::string& str)
		{
			MESSAGE_HEADER::getString(str);
			str.append("|");
			// swap a value received from the network into host-byte-order
			auto val = byte_swap<network_endian, host_endian>(Response_order_number);
			//str.append(std::to_string((double)_byteswap_uint64((uint64_t) Response_order_number)));          				str.append("|");
			str.append(std::to_string(val));          				str.append("|");
			str.append(getBrokerID());													str.append("|");
			str.append(std::to_string(getReserved9()));									str.append("|");
			str.append(std::to_string(getTraderNum()));									str.append("|");
			str.append(getAccountNum());												str.append("|");
			str.append(std::to_string(getBuySell()));									str.append("|");
			str.append(std::to_string(getOriginalVol()));								str.append("|");
			str.append(std::to_string(getDisclosedVol()));								str.append("|");
			str.append(std::to_string(getRemainingVol()));								str.append("|");
			str.append(std::to_string(getDisclosedVolRemaining()));						str.append("|");
			str.append(std::to_string(getPrice()));										str.append("|");
			_orderFlags.getString(str);													str.append("|");
			//ST_ORDER_FLAGS::getString();												str.append("|");
			str.append(std::to_string(getGtd()));										str.append("|");
			str.append(std::to_string(getFillNumber()));								str.append("|");
			str.append(std::to_string(getFillQty()));									str.append("|");
			str.append(std::to_string(getFillPrice()));									str.append("|");
			str.append(std::to_string(getVolFilledToday()));							str.append("|");
			str.append((getActivityType()));								str.append("|");
			str.append(std::to_string(getActivityTime()));								str.append("|");
			auto val1 = byte_swap<network_endian, host_endian>(OpOrderNumber);
			//str.append(std::to_string((double)((int64_t)_byteswap_uint64((uint64_t) OpOrderNumber))));									str.append("|");
			str.append(std::to_string(val1));									str.append("|");
			str.append(getOpBrokerId());									str.append("|");
			str.append(std::to_string(getToken()));										str.append("|");
			//str.append(std::to_string(Token));										str.append("|");
			_contractDesc.getString(str);												str.append("|");
			//str.append(std::to_string(getCONTRACT_DESC, contractDesc);  				str.append("|");
			str.append(std::to_string(getOpenClose()));									str.append("|");
			str.append(std::to_string(getOldOpenCLose()));								str.append("|");
			str.append(std::to_string(getBookType()));									str.append("|");
			str.append(std::to_string(getNewVolume()));									str.append("|");
			str.append(getOldAccountNumber());							str.append("|");
			str.append(getParticipant());								str.append("|");
			str.append(getOldParticipant());								str.append("|");
			//str.append(std::to_string(getADDITIONAL_ORDER_FLAGS, addOrderFlags);		str.append("|");
			_addOrderFlags.getString(str);												str.append("|");
			str.append("|");
			str.append(std::to_string(getReservedFiller()));							str.append("|");
			str.append(std::to_string(getGiveUpTrade()));								str.append("|");
			str.append(std::to_string(getReservedFiller2()));							str.append("|");
			str.append((getPAN()));										str.append("|");
			str.append((getOldPAN()));									str.append("|");
			str.append(std::to_string(getAlgoID()));									str.append("|");
			str.append(std::to_string(getAlgoCategory()));									str.append("|");
			str.append(std::to_string(getLastActivityReference()));							str.append("|");
			str.append(std::to_string(byte_swap<network_endian, host_endian>(nnfField)));										str.append("|");
			str.append((getReserved()));									str.append("|");
			//LOG_INFO << "[VAL]" << val;
		}
	};
	struct GIVEUP_RESPONSE :public MESSAGE_HEADER
	{

		CREATE_NNF_SHORT(ReasonCode);
		DOUBLE				 OrderNumber;
		CREATE_NNF_LONG(FillNumber);
		CREATE_NNF_STRING(InstrumentName, 6);
		CREATE_NNF_STRING(Symbol, 10);
		CREATE_NNF_LONG(ExpiryDate);
		CREATE_NNF_LONG(StrikePrice);
		CREATE_NNF_STRING(OptionType, 2);
		CREATE_NNF_SHORT(CALevel);
		CREATE_NNF_LONG(FillVolume);
		CREATE_NNF_LONG(FillPrice);
		CREATE_NNF_STRING(BrokerId, 5);
		CREATE_NNF_CHAR(Filler);
		CREATE_NNF_SHORT(BuySell);
		CREATE_NNF_SHORT(BookType);
		CREATE_NNF_LONG(LastModifiedDateTime);
		CREATE_NNF_CHAR(InitiatedByControl);
		CREATE_NNF_CHAR(OpenClose);
		CREATE_NNF_CHAR(ReservedFiller);
		CREATE_NNF_STRING(Participant, 12);
		CREATE_NNF_CHAR(GiveupFlag);
		CREATE_NNF_CHAR(Deleted);
		GIVEUP_RESPONSE() :MESSAGE_HEADER()
		{
			reset();
		}
		GIVEUP_RESPONSE(SIGNED_LONG traderID)
			: MESSAGE_HEADER(TransCode_SIGN_ON_REQUEST_IN,
				traderID, sizeof(GIVEUP_RESPONSE))
		{
			reset();
		}
		void reset()
		{
			setReasonCode(0);
			OrderNumber = 0;
			setFillNumber(0);
			setInstrumentName(std::string(6, ' '));
			setSymbol(std::string(10, ' '));
			setExpiryDate(0);
			setStrikePrice(0);
			setOptionType(std::string(2, ' '));
			setCALevel(0);
			setFillVolume(0);
			setFillPrice(0);
			setBrokerId(std::string(5, ' '));
			setFiller(' ');
			setBuySell(0);
			setBookType(0);
			setLastModifiedDateTime(0);
			setInitiatedByControl(' ');
			setOpenClose(' ');
			setReservedFiller(' ');
			setParticipant(std::string(12, ' '));
			setGiveupFlag(' ');
			setDeleted(' ');
		}
		void getString(std::string& str)
		{
			str.append(std::to_string(getReasonCode()));
			str.append(std::to_string(OrderNumber));
			str.append(std::to_string(getFillNumber()));
			str.append(getInstrumentName());
			str.append(getSymbol());
			str.append(std::to_string(getExpiryDate()));
			str.append(std::to_string(getStrikePrice()));
			str.append(getOptionType());
			str.append(std::to_string(getCALevel()));
			str.append(std::to_string(getFillVolume()));
			str.append(std::to_string(getFillPrice()));
			str.append((getBrokerId()));
			str.append(std::to_string(getFiller()));
			str.append(std::to_string(getBuySell()));
			str.append(std::to_string(getBookType()));
			str.append(std::to_string(getLastModifiedDateTime()));
			str.append(std::to_string(getInitiatedByControl()));
			str.append(std::to_string(getOpenClose()));
			str.append(std::to_string(getReservedFiller()));
			str.append((getParticipant()));
			str.append(std::to_string(getGiveupFlag()));
			str.append(std::to_string(getDeleted()));
		}
	};
	struct tradeResponseToOMS
	{
		short TransCode;
		INT32 LogTime;
		INT32 TraderId;
		double Response_order_number;
		char BrokerID[5];
		char Reserved9;
		INT32 TraderNum;
		char AccountNum[10];
		short BuySell;
		INT32 OriginalVol;
		INT32 DisclosedVol;
		INT32 RemainingVol;
		INT32 DisclosedVolRemaining;
		INT32 Price;
		char OrderFlags[10];
		INT32 Gtd;
		INT32 FillNumber;
		INT32 FillQty;
		INT32 FillPrice;
		INT32 VolFilledToday;
		char ActivityType[2];
		INT32 ActivityTime;
		double OpOrderNumber;
		char OpBrokerId[5];
		INT32 Token;

		char InstrumentName[6];
		char Symbol[10];
		INT32 ExpiryDate;
		INT32 StrikePrice;
		char OptionType[2];
		short CALevel;

		char OpenClose;
		char OldOpenCLose;
		char BookType;
		INT32 NewVolume;
		char OldAccountNumber[10];
		char Participant[12];
		char OldParticipant[12];

		char ReservedFiller;
		char GiveUpTrade;
		char ReservedFiller2;
		char PAN[10];
		char OldPAN[10];
		INT32 AlgoID;
		short AlgoCategory;
		INT64 LastActivityReference;
		double nnfField;
		char Reserved[44];

		void getString(std::string& str, std::string separater = "|")
		{
			str.append(std::to_string(TransCode));   str.append(separater);
			str.append(std::to_string(LogTime));   str.append(separater);
			str.append(std::to_string(TraderId));   str.append(separater);
			// swap a value received from the network into host-byte-order
			//auto val = byte_swap<network_endian, host_endian>(Response_order_number);
			//str.append(std::to_string((double)_byteswap_uint64((uint64_t) Response_order_number)));          				str.append(separater);
			str.append(std::to_string(Response_order_number));          									str.append(separater);
			str.append(BrokerID);													str.append(separater);
			str.append(std::to_string(Reserved9));									str.append(separater);
			str.append(std::to_string(TraderNum));									str.append(separater);
			str.append(AccountNum);													str.append(separater);
			str.append(std::to_string(BuySell));									str.append(separater);
			str.append(std::to_string(OriginalVol));								str.append(separater);
			str.append(std::to_string(DisclosedVol));								str.append(separater);
			str.append(std::to_string(RemainingVol));								str.append(separater);
			str.append(std::to_string(DisclosedVolRemaining));						str.append(separater);
			str.append(std::to_string(Price));										str.append(separater);
			str.append((OrderFlags));												str.append(separater);
			//ST_ORDER_FLAGS::getString();												str.append(separater);
			str.append(std::to_string(Gtd));										str.append(separater);
			str.append(std::to_string(FillNumber));								str.append(separater);
			str.append(std::to_string(FillQty));									str.append(separater);
			str.append(std::to_string(FillPrice));									str.append(separater);
			str.append(std::to_string(VolFilledToday));							str.append(separater);
			str.append((ActivityType));											str.append(separater);
			str.append(std::to_string(ActivityTime));								str.append(separater);
			//auto val1 = byte_swap<network_endian, host_endian>(OpOrderNumber);
			//str.append(std::to_string((double)((int64_t)_byteswap_uint64((uint64_t) OpOrderNumber))));									str.append(separater);
			str.append(std::to_string(OpOrderNumber));											str.append(separater);
			str.append(OpBrokerId);												str.append(separater);
			str.append(std::to_string(Token));										str.append(separater);
			//str.append(std::to_string(Token));										str.append(separater);
			//_contractDesc.getString(str);												str.append(separater);
			//str.append(std::to_string(getCONTRACT_DESC, contractDesc);  				str.append(separater);
			str.append(Symbol);													str.append(separater);
			str.append(std::to_string(ExpiryDate));								str.append(separater);
			str.append(std::to_string(StrikePrice));							str.append(separater);
			str.append((OptionType));											str.append(separater);
			str.append(std::to_string(CALevel));							str.append(separater);


			str.append(std::to_string(OpenClose));									str.append(separater);
			str.append(std::to_string(OldOpenCLose));								str.append(separater);
			str.append(std::to_string(BookType));									str.append(separater);
			str.append(std::to_string(NewVolume));									str.append(separater);
			str.append(OldAccountNumber);											str.append(separater);
			str.append(Participant);												str.append(separater);
			str.append(OldParticipant);											str.append(separater);
			//str.append(std::to_string(getADDITIONAL_ORDER_FLAGS, addOrderFlags);		str.append(separater);
			//_addOrderFlags.getString(str);												str.append(separater);
			//str.append(separater);
			str.append(std::to_string(ReservedFiller));							str.append(separater);
			str.append(std::to_string(GiveUpTrade));								str.append(separater);
			str.append(std::to_string(ReservedFiller2));							str.append(separater);
			str.append(PAN);														str.append(separater);
			str.append(OldPAN);													str.append(separater);
			str.append(std::to_string(AlgoID));									str.append(separater);
			str.append(std::to_string(AlgoCategory));								str.append(separater);
			str.append(std::to_string(LastActivityReference));						str.append(separater);
			str.append(std::to_string(nnfField));								str.append(separater);
			str.append((Reserved));												str.append(separater);
			//LOG_INFO << "[VAL]" << val;
		}
	};
}
#endif