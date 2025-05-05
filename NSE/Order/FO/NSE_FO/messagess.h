#ifndef FIN_NSE_TRIMMED_MESSAGES_H
#define FIN_NSE_TRIMMED_MESSAGES_H

#include <stdlib.h>
#include <iostream>
#include "transcodes.h"
#include <vector>
#include <sstream>
#include "boost/lexical_cast.hpp"
#include <string>
#include "globalDefines.h"

#pragma pack ( push, 2)
namespace FIN {
	namespace NSE {
		namespace TRIMMED {


			// TODO : Inherit DC_PACKET_WRAPPER on every message which is required to be sent to exchange ( For DC )


			// TODO : Should I move this to one direcotry back ???
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
					memset(this, 0, length);
					setLength(length);
					setMD5Checksum(std::string(16, ' '));
				}
				//virtual void getString(std::string &str)=0;
				//{
				//	std::string str;
				//	str.append(std::to_string(getLength()));
				//	str.append(",");
				//	str.append(std::to_string(getSequenceNo ()));
				//	str.append(",");
				//	str.append(getMD5Checksum());
				//	return str;
				//}
				//virtual void fromString(std::string &str)=0;
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
					CREATE_NNF_STRING(Timestamp, 8)
					CREATE_NNF_STRING(Timestamp1, 8)
					CREATE_NNF_STRING(Timestamp2, 8)
					CREATE_NNF_SHORT(MessageLength)

					MESSAGE_HEADER()
				{
					reset();
				}

				MESSAGE_HEADER(TransCode tc, SIGNED_NNF_LONG traderID, size_t msgLength)
				{
					reset();
					setTransCode(tc);
					setTraderId(traderID);
					setMessageLength(msgLength);
				}
				//Added by sachin this setAllValus is just for dummy code
				void setAllValues(short _transCode, long _logTime, std::string _alphachar, long _tradeID, short _errorcode, std::string _timeStamp, std::string _timeStamp1, std::string _timeStamp2, short msgLength)
				{
					//setTransCode (TransCode_BC_OPEN_MESSAGE);
					//setAlphaChar ("AB");
					//setTraderId  (1234);
					//setErrorCode (0);
					//setTimestamp1("1244");
					//setTimestamp2("2344");
					//setTimestamp ("1122");
					//setMessageLength(12);
					//setLogTime(2345);
					setTransCode(_transCode);
					setLogTime(_logTime);
					setAlphaChar(_alphachar);
					setTraderId(_tradeID);
					setErrorCode(_errorcode);
					setTimestamp1(_timeStamp1);
					setTimestamp2(_timeStamp2);
					setTimestamp(_timeStamp);
					setMessageLength(msgLength);

				}
				void getString(std::string& str)
				{

					str.append(std::to_string(getTransCode()));
					str.append(",");
					str.append(std::to_string(getLogTime()));
					str.append(",");
					str.append(getAlphaChar());
					str.append(",");
					str.append(std::to_string(getTraderId()));
					str.append(",");
					str.append(std::to_string(getErrorCode()));
					str.append(",");
					str.append(getTimestamp());
					str.append(",");
					str.append(getTimestamp1());
					str.append(",");
					str.append(getTimestamp2());
					str.append(",");
					str.append(std::to_string(getMessageLength()));

				}
				void fromString(std::string& str)
				{
					std::vector<std::string> result;
					std::stringstream s_stream(str); //create string stream from the string
					while (s_stream.good())
					{
						std::string substr;
						getline(s_stream, substr, ','); //get first string delimited by comma
						result.push_back(substr);
					}

					setTransCode(boost::lexical_cast<short>  (result[0]));
					setLogTime(boost::lexical_cast<long>  (result[1]));
					setAlphaChar((result[2]));
					setTraderId(boost::lexical_cast<long>  (result[3]));
					setErrorCode(boost::lexical_cast<short>  (result[4]));
					setTimestamp((result[5]));
					setTimestamp1((result[6]));
					setTimestamp2((result[7]));
					setMessageLength(boost::lexical_cast<short>  (result[8]));
					print();
				}
				// till here..................
				void reset()
				{
					memset(this, 0, sizeof(MESSAGE_HEADER));
					//setTransCode  ( 0 ) ;
					//setLogTime    ( 0 ) ;
					//setAlphaChar("AA");
					//setTraderId   ( 0 ) ;
					//setErrorCode  ( 0 ) ;
					//setTimestamp  ( "0"); 
					//setTimestamp1 ( "0");
					//setTimestamp2 ( "0");
					//setMessageLength (0) ;
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
					DEVLOG("Seq: " << atof(s1) << "-" << atof(s2));

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
				void fromString(std::string& str)
				{
					std::vector<std::string> result;
					std::stringstream s_stream(str); //create string stream from the string
					while (s_stream.good())
					{
						std::string substr;
						getline(s_stream, substr, ','); //get first string delimited by comma
						result.push_back(substr);
					}

					setTransCode(boost::lexical_cast<short>  (result[0]));
					setLogTime(boost::lexical_cast<long>  (result[1]));
					setAlphaChar((result[2]));
					setTraderId(boost::lexical_cast<long>  (result[3]));
					setErrorCode(boost::lexical_cast<short>  (result[4]));
					setTimestamp((result[5]));
					setTimestamp1((result[6]));
					setTimestamp2((result[7]));
					setMessageLength(boost::lexical_cast<short>  (result[8]));
					print();
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
				TEST_TIME_STAMP(SIGNED_NNF_LONG traderID)
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
		}
	}
}

#pragma pack ( pop )
#endif
