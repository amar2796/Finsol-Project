#ifndef FIN_NSE_TRIMMED_COM_MESSAGES_H
#define FIN_NSE_TRIMMED_COM_MESSAGES_H
#pragma once
#include <stdlib.h>
#include <iostream>
#include "messagess.h"
#include <vector>
#include <sstream>
#include "stdlib.h" 

#pragma pack ( push, 2)
namespace FIN {
	namespace NSE {
		namespace TRIMMED {
			namespace COM {

				//There is an error in getString function of a message structure I have commented it "errorCode"
				// TODO : Inherit DC_PACKET_WRAPPER on every message which is required to be sent to exchange ( For DC )


				/*
				// TODO : Should I move this to one direcotry back ???
				struct DC_PACKET_WRAPPER
				{
					CREATE_NNF_SHORT( Length ) ;
					CREATE_NNF_LONG ( SequenceNo ) ;
					CREATE_NNF_STRING( MD5Checksum , 16) ;

					DC_PACKET_WRAPPER()
					{
					}

					DC_PACKET_WRAPPER ( size_t length )
					{
						memset ( this, 0, length  ) ;
						setLength ( length ) ;
						setMD5Checksum ( std::string( 16, ' '));
					}

					void print() const
					{
						PRINT_NNF_FIELD ( Length ) ;
						PRINT_NNF_FIELD ( SequenceNo ) ;
						PRINT_NNF_FIELD ( MD5Checksum ) ;
					}

					void reset()
					{
						setSequenceNo(0) ;
						setMD5Checksum ( std::string( 16, ' '));
					}
				};

				struct MESSAGE_HEADER
				{
					CREATE_NNF_SHORT    ( TransCode     )
					CREATE_NNF_LONG     ( LogTime       )
					CREATE_NNF_STRING   ( AlphaChar, 2  )
					CREATE_NNF_LONG     ( TraderId      )
					CREATE_NNF_SHORT    ( ErrorCode     )
					CREATE_NNF_STRING   ( Timestamp , 8 )
					CREATE_NNF_STRING   ( Timestamp1, 8 )
					CREATE_NNF_STRING   ( Timestamp2, 8 )
					CREATE_NNF_SHORT    ( MessageLength )

					MESSAGE_HEADER()
					{
						reset() ;
					}

					MESSAGE_HEADER( TransCode tc, SIGNED_NNF_LONG traderID, size_t msgLength)
					{
						reset() ;
						setTransCode( tc ) ;
						setTraderId( traderID ) ;
						setMessageLength( msgLength ) ;
					}

					void reset()
					{
						memset ( this, 0, sizeof (MESSAGE_HEADER)) ;
						//setTransCode  ( 0 ) ;
						//setLogTime    ( 0 ) ;
						//setTraderId   ( 0 ) ;
						//setErrorCode  ( 0 ) ;
						//setTimestamp  ( "0");
						//setTimestamp1 ( "0");
						//setTimestamp2 ( "0");
						//setMessageLength (0) ;
					}

					short getTCode () { return _TransCode ; }

					void print() const
					{
						PRINT_BODY_START( MESSAGE_HEADER )
						PRINT_NNF_FIELD( TransCode )
							PRINT_NNF_FIELD( LogTime   )
							PRINT_NNF_FIELD( AlphaChar )
							PRINT_NNF_FIELD( TraderId  )
							PRINT_NNF_FIELD( ErrorCode )
							PRINT_NNF_FIELD( Timestamp )
							PRINT_NNF_FIELD( Timestamp1)
							PRINT_NNF_FIELD( Timestamp2)
							PRINT_NNF_FIELD( MessageLength )
						PRINT_BODY_END( MESSAGE_HEADER )
					}
				};

				//// TODO : Add BCAST HEADER whene working on NSE Snapshot

			*/


				struct MS_ERROR_RESPONSE : public MESSAGE_HEADER
				{
					CREATE_NNF_STRING(Key, 14);
					CREATE_NNF_STRING(ErrorMsg, 128);

					MS_ERROR_RESPONSE() : MESSAGE_HEADER()
					{
						reset();
					}

					void getString(std::string& str)
					{
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
						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						print();
					}
					void reset()
					{
						setKey(std::string(14, ' '));
						setErrorMsg(std::string(128, ' '));
					}
					void print() const
					{
						PRINT_BODY_START(MS_ERROR_RESPONSE)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(Key)
							PRINT_NNF_FIELD(ErrorMsg)
							PRINT_BODY_END(MS_ERROR_RESPONSE)
					}
				};

				struct GR_REQUEST : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
				{
					CREATE_NNF_SHORT(BoxID);
					CREATE_NNF_STRING(BrokerID, 5);
					CREATE_NNF_CHAR(Filler);

					GR_REQUEST(SIGNED_NNF_LONG traderID,
						SIGNED_NNF_SHORT boxId, const std::string& brokerId)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MESSAGE_HEADER(TransCode_GR_REQUEST,
							traderID, sizeof(GR_REQUEST) - sizeof(DC_PACKET_WRAPPER))
					{
						reset();
						setBoxID(boxId);
						setBrokerID(brokerId);
					}

					void reset()
					{
						setBoxID(0);
						setBrokerID(std::string(5, ' '));
						setFiller(' ');
					}

					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getBoxID()));
						str.append(",");
						str.append(getBrokerID());
						str.append(",");
						str.append(std::to_string(getFiller()));
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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setBoxID(static_cast<short>(std::stoi(result[9])));

						setBrokerID(result[10]);

						if (isSafeCharConvert(std::stoi(result[11])))
							setFiller(static_cast<char>(std::stoi(result[11])));

						print();
					}

					void print() const
					{
						PRINT_BODY_START(GR_REQUEST)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(BoxID);
						PRINT_NNF_FIELD(BrokerID);
						PRINT_NNF_FIELD(Filler);
						PRINT_BODY_END(GR_REQUEST)
					}
				};

				struct GR_RESPONSE : public MESSAGE_HEADER
				{
					CREATE_NNF_SHORT(BoxID)
						CREATE_NNF_STRING(BrokerID, 5)
						CREATE_NNF_CHAR(Filler)
						CREATE_NNF_STRING(IpAddress, 16)
						CREATE_NNF_LONG(Port)
						CREATE_NNF_STRING(SessionKey, 8)
						CREATE_NNF_STRING(Cryptographic_Key, 32)
						CREATE_NNF_STRING(Cryptographic_IV, 12)

						GR_RESPONSE() : MESSAGE_HEADER()
					{
						reset();
					}

					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getBoxID()));
						str.append(",");
						str.append(getBrokerID());
						str.append(",");
						str.append(std::to_string(getFiller()));
						str.append(",");
						str.append(getIpAddress());
						str.append(",");
						str.append(std::to_string(getPort()));
						str.append(",");
						str.append(getSessionKey());
						str.append(",");
						str.append(getCryptographic_Key());
						str.append(",");
						str.append(getCryptographic_IV());
					}
					void fromString(std::string& str)
					{
						std::vector<std::string> result;
						std::stringstream s_stream(str); // create string stream from the string
						while (s_stream.good())
						{
							std::string substr;
							getline(s_stream, substr, ','); // get first string delimited by comma
							result.push_back(substr);
						}

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setBoxID(static_cast<short>(std::stoi(result[9])));

						setBrokerID(result[10]);

						if (isSafeCharConvert(std::stoi(result[11])))
							setFiller(static_cast<char>(std::stoi(result[11])));

						setIpAddress(result[12]);

						if (isSafeCharConvert(std::stoi(result[13])))
							setPort(static_cast<char>(std::stoi(result[13])));

						setSessionKey(result[14]);

						print();
					}

					void reset()
					{
						setBoxID(0);
						setBrokerID(std::string(5, ' '));
						setFiller(' ');
						setIpAddress(std::string(16, ' '));
						setPort(0);
						setSessionKey(std::string(8, ' '));
					}

					void print() const
					{
						PRINT_BODY_START(GR_RESPONSE)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(BoxID)
							PRINT_NNF_FIELD(BrokerID)
							PRINT_NNF_FIELD(Filler)
							PRINT_NNF_FIELD(IpAddress)
							PRINT_NNF_FIELD(Port)
							PRINT_NNF_FIELD(SessionKey)
							PRINT_BODY_END(GR_RESPONSE)
					}
				};

				struct BOX_SIGN_ON_REQUEST_IN : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
				{
					CREATE_NNF_SHORT(BoxID)
						CREATE_NNF_STRING(BrokerID, 5)
						CREATE_NNF_STRING(Reserved, 5)
						CREATE_NNF_STRING(SessionKey, 8)

						BOX_SIGN_ON_REQUEST_IN(SIGNED_NNF_LONG traderID,
							SIGNED_NNF_SHORT boxId, const std::string& brokerId)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MESSAGE_HEADER(TransCode_BOX_SIGN_ON_REQUEST_IN,
							traderID, sizeof(BOX_SIGN_ON_REQUEST_IN) - sizeof(DC_PACKET_WRAPPER))
					{
						reset();
						setBoxID(boxId);
						setBrokerID(brokerId);
					}

					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getBoxID()));
						str.append(",");
						str.append(getBrokerID());
						str.append(",");
						str.append(getReserved());
						str.append(",");
						str.append(getSessionKey());

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setBoxID(static_cast<short>(std::stoi(result[9])));

						setBrokerID(result[10]);
						setReserved(result[11]);
						setSessionKey(result[12]);

						print();
					}

					void reset()
					{
						setBoxID(0);
						setBrokerID(std::string(5, 0));
						setReserved(std::string(5, 0));
						setSessionKey(std::string(8, 0));
					}

					void print() const
					{
						PRINT_BODY_START(BOX_SIGN_ON_REQUEST_IN)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(BoxID)
							PRINT_NNF_FIELD(BrokerID)
							PRINT_NNF_FIELD(Reserved)
							PRINT_NNF_FIELD(SessionKey)
							PRINT_BODY_END(BOX_SIGN_ON_REQUEST_IN)
					}
				};

				struct BOX_SIGN_ON_REQUEST_OUT : public MESSAGE_HEADER
				{
					CREATE_NNF_SHORT(BoxID)
						CREATE_NNF_STRING(Reserved, 10)

						BOX_SIGN_ON_REQUEST_OUT()
					{
						reset();
					}

					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getBoxID()));
						str.append(",");
						str.append(getReserved());

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setBoxID(static_cast<short>(std::stoi(result[9])));

						setReserved(result[10]);

						print();
					}

					void reset()
					{
						setBoxID(0);
						setReserved(std::string(10, ' '));
					}

					void print() const
					{
						PRINT_BODY_START(BOX_SIGN_ON_REQUEST_OUT)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(BoxID)
							PRINT_NNF_FIELD(Reserved)
							PRINT_BODY_END(BOX_SIGN_ON_REQUEST_OUT)
					}
				};

				struct MS_BOX_SIGN_OFF : public MESSAGE_HEADER
				{
					CREATE_NNF_SHORT(BoxID)
						MS_BOX_SIGN_OFF()
					{}

					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getBoxID()));
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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setBoxID(static_cast<short>(std::stoi(result[9])));

						print();
					}

					void print() const
					{
						PRINT_BODY_START(MS_BOX_SIGN_OFF)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(BoxID)
							PRINT_BODY_END(MS_BOX_SIGN_OFF)
					}
				};

				struct ST_BROKER_ELIGIBILITY_PER_MKT
				{
					CREATE_NNF_BIT(Reserved1)
						CREATE_NNF_BIT(Reserved2)
						CREATE_NNF_BIT(Reserved3)
						CREATE_NNF_BIT(Reserved4)
						CREATE_NNF_BIT(Auction)
						CREATE_NNF_BIT(Spot)
						CREATE_NNF_BIT(Oddlot)
						CREATE_NNF_BIT(Normal)
						CREATE_NNF_CHAR(Reserved5)

						void reset()
					{
						memset(this, 0, sizeof(ST_BROKER_ELIGIBILITY_PER_MKT));
					}
					void getString(std::string& str)
					{
						str.append(std::to_string(getReserved1()));
						str.append(",");
						str.append(std::to_string(getReserved2()));
						str.append(",");
						str.append(std::to_string(getReserved3()));
						str.append(",");
						str.append(std::to_string(getReserved4()));
						str.append(",");
						str.append(std::to_string(getAuction()));
						str.append(",");
						str.append(std::to_string(getSpot()));
						str.append(",");
						str.append(std::to_string(getOddlot()));
						str.append(",");
						str.append(std::to_string(getNormal()));
						str.append(",");
						str.append(std::to_string(getReserved5()));
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

						if (isSafeUCharConvert(std::stoi(result[0])))
							setReserved1(static_cast<unsigned char>(std::stoi(result[0])));

						if (isSafeUCharConvert(std::stoi(result[1])))
							setReserved2(static_cast<unsigned char>(std::stoi(result[1])));

						if (isSafeUCharConvert(std::stoi(result[2])))
							setReserved3(static_cast<unsigned char>(std::stoi(result[2])));

						if (isSafeUCharConvert(std::stoi(result[3])))
							setReserved4(static_cast<unsigned char>(std::stoi(result[3])));

						if (isSafeUCharConvert(std::stoi(result[4])))
							setAuction(static_cast<unsigned char>(std::stoi(result[4])));

						if (isSafeUCharConvert(std::stoi(result[5])))
							setSpot(static_cast<unsigned char>(std::stoi(result[5])));

						if (isSafeUCharConvert(std::stoi(result[6])))
							setOddlot(static_cast<unsigned char>(std::stoi(result[6])));

						if (isSafeUCharConvert(std::stoi(result[7])))
							setNormal(static_cast<unsigned char>(std::stoi(result[7])));

						if (isSafeUCharConvert(std::stoi(result[8])))
							setReserved5(static_cast<char>(std::stoi(result[8])));

						print();
					}

					void print() const
					{
						PRINT_BODY_START(ST_BROKER_ELIGIBILITY_PER_MKT)
							PRINT_NNF_FIELD(Auction);
						PRINT_NNF_FIELD(Spot);
						PRINT_NNF_FIELD(Oddlot);
						PRINT_NNF_FIELD(Normal);
						PRINT_BODY_END(ST_BROKER_ELIGIBILITY_PER_MKT)

					}
				};

				struct MS_SIGNON : public MESSAGE_HEADER
				{
					CREATE_NNF_LONG(UserID)
						CREATE_NNF_STRING(Reserved2, 8)
						CREATE_NNF_STRING(Password, 8)
						CREATE_NNF_STRING(Reserved3, 8)
						CREATE_NNF_STRING(NewPassword, 8)
						CREATE_NNF_STRING(TraderName, 26)
						CREATE_NNF_LONG(LastPasswordChangeDate)
						CREATE_NNF_STRING(BrokerID, 5)
						CREATE_NNF_CHAR(Reserved)
						CREATE_NNF_SHORT(BranchID)
						CREATE_NNF_LONG(VersionNumber)
						CREATE_NNF_LONG(Batch2StartTime)
						CREATE_NNF_CHAR(HostSwitchContext)
						CREATE_NNF_STRING(Colour, 50)
						CREATE_NNF_CHAR(Reserved1)
						CREATE_NNF_SHORT(UserType)
						CREATE_NNF_DOUBLE(SequenceNumber)
						CREATE_NNF_STRING(WsClassName, 14)
						CREATE_NNF_CHAR(BrokerStatus)
						CREATE_NNF_CHAR(ShowIndex)
						INCLUDE_STRUCT(ST_BROKER_ELIGIBILITY_PER_MKT, BrkrElgbltyPrMkt)
						CREATE_NNF_SHORT(MemberType)
						CREATE_NNF_CHAR(ClearingStatus)
						CREATE_NNF_STRING(BrokerName, 25)
						CREATE_NNF_STRING(Reserved4, 48)

						MS_SIGNON()
					{
						reset();
					}

					MS_SIGNON(SIGNED_NNF_LONG traderID)
						:MESSAGE_HEADER(TransCode_SIGN_ON_REQUEST_IN,
							traderID, sizeof(MS_SIGNON))
					{
						reset();
					}

					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getUserID()));
						str.append(",");
						str.append(getReserved2());
						str.append(",");
						str.append(getPassword());
						str.append(",");
						str.append(getReserved3());
						str.append(",");
						str.append(getNewPassword());
						str.append(",");
						str.append(getTraderName());
						str.append(",");
						str.append(std::to_string(getLastPasswordChangeDate()));
						str.append(",");
						str.append(getBrokerID());
						str.append(",");
						str.append(std::to_string(getReserved()));
						str.append(",");
						str.append(std::to_string(getBranchID()));
						str.append(",");
						str.append(std::to_string(getVersionNumber()));
						str.append(",");
						str.append(std::to_string(getBatch2StartTime()));
						str.append(",");
						str.append(std::to_string(getHostSwitchContext()));
						str.append(",");
						str.append(getColour());
						str.append(",");
						str.append(std::to_string(getReserved1()));
						str.append(",");
						str.append(std::to_string(getUserType()));
						str.append(",");
						str.append(std::to_string(getSequenceNumber()));
						str.append(",");
						str.append(getWsClassName());
						str.append(",");
						str.append(std::to_string(getBrokerStatus()));
						str.append(",");
						str.append(std::to_string(getShowIndex()));
						str.append(",");
						str.append(std::to_string(getMemberType()));
						str.append(",");
						str.append(std::to_string(getClearingStatus()));
						str.append(",");
						str.append(getBrokerName());
						str.append(",");
						str.append(getReserved4());

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setUserID(static_cast<short>(std::stoi(result[9])));

						setReserved2(result[11]);
						setPassword(result[12]);
						setReserved3(result[13]);
						setNewPassword(result[14]);
						setTraderName(result[15]);

						if (isSafeShortConvert(std::stoi(result[16])))
							setLastPasswordChangeDate(static_cast<short>(std::stoi(result[16])));

						setBrokerID(result[17]);

						if (isSafeCharConvert(std::stoi(result[18])))
							setReserved(static_cast<char>(std::stoi(result[18])));

						if (isSafeShortConvert(std::stoi(result[19])))
							setBranchID(static_cast<short>(std::stoi(result[19])));

						if (isSafeShortConvert(std::stoi(result[20])))
							setVersionNumber(static_cast<short>(std::stoi(result[20])));

						if (isSafeShortConvert(std::stoi(result[21])))
							setBatch2StartTime(static_cast<short>(std::stoi(result[21])));

						if (isSafeCharConvert(std::stoi(result[22])))
							setHostSwitchContext(static_cast<char>(std::stoi(result[22])));

						setColour(result[23]);

						if (isSafeCharConvert(std::stoi(result[24])))
							setReserved1(static_cast<char>(std::stoi(result[24])));

						if (isSafeShortConvert(std::stoi(result[25])))
							setUserType(static_cast<short>(std::stoi(result[25])));

						if (isSafeShortConvert(std::stoi(result[26])))
							setSequenceNumber(static_cast<short>(std::stoi(result[26])));

						setWsClassName(result[27]);

						if (isSafeCharConvert(std::stoi(result[28])))
							setBrokerStatus(static_cast<char>(std::stoi(result[28])));

						if (isSafeCharConvert(std::stoi(result[29])))
							setShowIndex(static_cast<char>(std::stoi(result[29])));

						if (isSafeShortConvert(std::stoi(result[30])))
							setMemberType(static_cast<short>(std::stoi(result[30])));

						if (isSafeCharConvert(std::stoi(result[31])))
							setClearingStatus(static_cast<char>(std::stoi(result[31])));

						setBrokerName(result[32]);
						setReserved4(result[33]);

						print();
					}
					void reset()
					{
						setUserID(0);
						setPassword(std::string(8, ' '));
						setNewPassword(std::string(8, ' '));
						setTraderName(std::string(26, ' '));
						setLastPasswordChangeDate(0);
						setBrokerID(std::string(5, ' '));
						setReserved(' ');
						setBranchID(0);
						setVersionNumber(0);
						setBatch2StartTime(0);
						setHostSwitchContext(' ');
						setColour(std::string(50, ' '));
						setReserved1(' ');
						setUserType(0);
						setSequenceNumber(0);
						setWsClassName(std::string(14, ' '));
						setBrokerStatus(0);
						setShowIndex(0);
						_BrkrElgbltyPrMkt.reset();
						setMemberType(0);
						setClearingStatus(0);
						setBrokerName(std::string(25, ' '));
					}

					void print() const
					{
						PRINT_BODY_START(MS_SIGNON)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(UserID)
							PRINT_NNF_FIELD(Password)
							PRINT_NNF_FIELD(NewPassword)
							PRINT_NNF_FIELD(TraderName)
							PRINT_NNF_FIELD(LastPasswordChangeDate)
							PRINT_NNF_FIELD(BrokerID)
							PRINT_NNF_FIELD(Reserved)
							PRINT_NNF_FIELD(BranchID)
							PRINT_NNF_FIELD(VersionNumber)
							PRINT_NNF_FIELD(Batch2StartTime)
							PRINT_NNF_FIELD(HostSwitchContext)
							PRINT_NNF_FIELD(Colour)
							PRINT_NNF_FIELD(Reserved1)
							PRINT_NNF_FIELD(UserType)
							PRINT_NNF_FIELD(SequenceNumber)
							PRINT_NNF_FIELD(WsClassName)
							PRINT_NNF_FIELD(BrokerStatus)
							PRINT_NNF_FIELD(ShowIndex)
							_BrkrElgbltyPrMkt.print();
						PRINT_NNF_FIELD(MemberType)
							PRINT_NNF_FIELD(ClearingStatus)
							PRINT_NNF_FIELD(BrokerName)
							PRINT_BODY_END(MS_SIGNON)
					}
				};

				struct MS_SIGNOFF : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
				{
					MS_SIGNOFF(SIGNED_NNF_LONG traderID)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MESSAGE_HEADER(TransCode_SIGN_OFF_REQUEST_IN,
							traderID, sizeof(MS_SIGNOFF) - sizeof(DC_PACKET_WRAPPER))
					{

					}

				};

				struct SIGNON_REQUETS : public DC_PACKET_WRAPPER, public MS_SIGNON
				{
					SIGNON_REQUETS(SIGNED_NNF_LONG traderID)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MS_SIGNON(traderID)
					{
					}

					void getString(std::string& str)
					{
						//str.append (MS_SIGNON :: getString(str));
						MS_SIGNON::getString(str);

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setUserID(static_cast<short>(std::stoi(result[9])));

						setReserved2(result[11]);
						setPassword(result[12]);
						setReserved3(result[13]);
						setNewPassword(result[14]);
						setTraderName(result[15]);

						if (isSafeShortConvert(std::stoi(result[16])))
							setLastPasswordChangeDate(static_cast<short>(std::stoi(result[16])));

						setBrokerID(result[17]);

						if (isSafeCharConvert(std::stoi(result[18])))
							setReserved(static_cast<char>(std::stoi(result[18])));

						if (isSafeShortConvert(std::stoi(result[19])))
							setBranchID(static_cast<short>(std::stoi(result[19])));

						if (isSafeShortConvert(std::stoi(result[20])))
							setVersionNumber(static_cast<short>(std::stoi(result[20])));

						if (isSafeShortConvert(std::stoi(result[21])))
							setBatch2StartTime(static_cast<short>(std::stoi(result[21])));

						if (isSafeCharConvert(std::stoi(result[22])))
							setHostSwitchContext(static_cast<char>(std::stoi(result[22])));

						setColour(result[23]);

						if (isSafeCharConvert(std::stoi(result[24])))
							setReserved1(static_cast<char>(std::stoi(result[24])));

						if (isSafeShortConvert(std::stoi(result[25])))
							setUserType(static_cast<short>(std::stoi(result[25])));

						if (isSafeShortConvert(std::stoi(result[26])))
							setSequenceNumber(static_cast<short>(std::stoi(result[26])));

						setWsClassName(result[27]);

						if (isSafeCharConvert(std::stoi(result[28])))
							setBrokerStatus(static_cast<char>(std::stoi(result[28])));

						if (isSafeCharConvert(std::stoi(result[29])))
							setShowIndex(static_cast<char>(std::stoi(result[29])));

						if (isSafeShortConvert(std::stoi(result[30])))
							setMemberType(static_cast<short>(std::stoi(result[30])));

						if (isSafeCharConvert(std::stoi(result[31])))
							setClearingStatus(static_cast<char>(std::stoi(result[31])));

						setBrokerName(result[32]);
						setReserved4(result[33]);

						print();
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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeShortConvert(std::stoi(result[9])))
							setUserID(static_cast<short>(std::stoi(result[9])));

						setReserved2(result[10]);
						setPassword(result[11]);
						setReserved3(result[12]);
						setNewPassword(result[13]);
						setTraderName(result[14]);

						if (isSafeShortConvert(std::stoi(result[15])))
							setLastPasswordChangeDate(static_cast<short>(std::stoi(result[15])));

						setBrokerID(result[16]);

						if (isSafeCharConvert(std::stoi(result[17])))
							setReserved(static_cast<char>(std::stoi(result[17])));

						if (isSafeShortConvert(std::stoi(result[18])))
							setBranchID(static_cast<short>(std::stoi(result[18])));

						if (isSafeShortConvert(std::stoi(result[19])))
							setVersionNumber(static_cast<short>(std::stoi(result[19])));

						if (isSafeShortConvert(std::stoi(result[20])))
							setBatch2StartTime(static_cast<short>(std::stoi(result[20])));

						if (isSafeCharConvert(std::stoi(result[21])))
							setHostSwitchContext(static_cast<char>(std::stoi(result[21])));

						setColour(result[22]);

						if (isSafeCharConvert(std::stoi(result[23])))
							setReserved1(static_cast<char>(std::stoi(result[23])));

						if (isSafeShortConvert(std::stoi(result[24])))
							setUserType(static_cast<short>(std::stoi(result[24])));

						if (isSafeShortConvert(std::stoi(result[25])))
							setSequenceNumber(static_cast<short>(std::stoi(result[25])));

						setWsClassName(result[26]);

						if (isSafeCharConvert(std::stoi(result[27])))
							setBrokerStatus(static_cast<char>(std::stoi(result[27])));

						if (isSafeCharConvert(std::stoi(result[28])))
							setShowIndex(static_cast<char>(std::stoi(result[28])));

						if (isSafeShortConvert(std::stoi(result[29])))
							setMemberType(static_cast<short>(std::stoi(result[29])));

						if (isSafeCharConvert(std::stoi(result[30])))
							setClearingStatus(static_cast<char>(std::stoi(result[30])));

						setBrokerName(result[31]);
						setReserved4(result[32]);

						print();
					}
					void print() const
					{
						PRINT_BODY_START(SIGNON_CONFIRMATION)
							MS_SIGNON::print();
						PRINT_BODY_END(SIGNON_CONFIRMATION)
					}
				};

				struct MS_SYSTEM_INFO_REQ : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
				{
					CREATE_NNF_LONG(LastUpdatePortfolioTIme);

					MS_SYSTEM_INFO_REQ(SIGNED_NNF_LONG traderID)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MESSAGE_HEADER(TransCode_SYSTEM_INFORMATION_IN,
							traderID, sizeof(MS_SYSTEM_INFO_REQ) - sizeof(DC_PACKET_WRAPPER))
					{
						reset();
					}


					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getLastUpdatePortfolioTIme()));

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						setLastUpdatePortfolioTIme(std::stol(result[9]));

						print();
					}
					void reset()
					{
						setLastUpdatePortfolioTIme(0);
					}

					void print() const
					{
						PRINT_BODY_START(MS_SYSTEM_INFO_REQ)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(LastUpdatePortfolioTIme)
							PRINT_BODY_END(MS_SYSTEM_INFO_REQ)
					}
				};

				// TODO : 1. Check for Time related fields.
				// TODO : 2. Check for Local DB and Update DB messages.

				struct ST_MARKET_STATUS
				{
					CREATE_NNF_SHORT(Normal)
						CREATE_NNF_SHORT(OddLot)
						CREATE_NNF_SHORT(Spot)
						CREATE_NNF_SHORT(Auction)
						void getString(std::string& str)
					{

						str.append(std::to_string(getNormal()));
						str.append(",");
						str.append(std::to_string(getOddLot()));
						str.append(",");
						str.append(std::to_string(getSpot()));
						str.append(",");
						str.append(std::to_string(getAuction()));

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setNormal(static_cast<short>(std::stoi(result[0])));

						if (isSafeShortConvert(std::stoi(result[1])))
							setOddLot(static_cast<short>(std::stoi(result[1])));

						if (isSafeShortConvert(std::stoi(result[2])))
							setSpot(static_cast<short>(std::stoi(result[2])));

						if (isSafeShortConvert(std::stoi(result[3])))
							setAuction(static_cast<short>(std::stoi(result[3])));

						print();
					}
					void print() const
					{
						PRINT_BODY_START(ST_MARKET_STATUS)
							PRINT_NNF_FIELD(Normal)
							PRINT_NNF_FIELD(OddLot)
							PRINT_NNF_FIELD(Spot)
							PRINT_NNF_FIELD(Auction)
							PRINT_BODY_END(ST_MARKET_STATUS)
					}
				};

				struct ST_STOCK_ELIGIBLE_INDICATORS
				{
					CREATE_NNF_BIT(Reserved1)
						CREATE_NNF_BIT(Reserved2)
						CREATE_NNF_BIT(Reserved3)
						CREATE_NNF_BIT(Reserved4)
						CREATE_NNF_BIT(Reserved5)
						CREATE_NNF_BIT(BooksMerged)
						CREATE_NNF_BIT(MinimumFill)
						CREATE_NNF_BIT(AON)
						CREATE_NNF_CHAR(Reserved)

				public:
					void getString(std::string& str)
					{

						str.append(std::to_string(getReserved1()));
						str.append(",");
						str.append(std::to_string(getReserved2()));
						str.append(",");
						str.append(std::to_string(getReserved3()));
						str.append(",");
						str.append(std::to_string(getReserved4()));
						str.append(",");
						str.append(std::to_string(getReserved5()));
						str.append(",");
						str.append(std::to_string(getBooksMerged()));
						str.append(",");
						str.append(std::to_string(getMinimumFill()));
						str.append(",");
						str.append(std::to_string(getAON()));
						str.append(",");
						str.append(std::to_string(getReserved()));

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
						if (isSafeCharConvert(std::stoi(result[0])))
							setReserved1(static_cast<char>(std::stoi(result[0])));

						if (isSafeCharConvert(std::stoi(result[1])))
							setReserved2(static_cast<char>(std::stoi(result[1])));

						if (isSafeCharConvert(std::stoi(result[2])))
							setReserved3(static_cast<char>(std::stoi(result[2])));

						if (isSafeCharConvert(std::stoi(result[3])))
							setReserved4(static_cast<char>(std::stoi(result[3])));

						if (isSafeCharConvert(std::stoi(result[4])))
							setReserved5(static_cast<char>(std::stoi(result[4])));

						if (isSafeCharConvert(std::stoi(result[5])))
							setBooksMerged(static_cast<char>(std::stoi(result[5])));

						if (isSafeCharConvert(std::stoi(result[6])))
							setMinimumFill(static_cast<char>(std::stoi(result[6])));

						if (isSafeCharConvert(std::stoi(result[7])))
							setAON(static_cast<char>(std::stoi(result[7])));

						if (isSafeCharConvert(std::stoi(result[8])))
							setReserved(static_cast<char>(std::stoi(result[8])));

						print();
					}

					void print() const
					{
						PRINT_BODY_START(ST_STOCK_ELIGIBLE_INDICATORS)
							PRINT_NNF_FIELD(BooksMerged)
							PRINT_NNF_FIELD(MinimumFill)
							PRINT_NNF_FIELD(AON)
							PRINT_BODY_END(ST_STOCK_ELIGIBLE_INDICATORS)
					}
				};


				struct MS_SYSTEM_INFO_DATA : public MESSAGE_HEADER
				{
					INCLUDE_STRUCT(ST_MARKET_STATUS, MarketStatus);
					INCLUDE_STRUCT(ST_MARKET_STATUS, ExMarketStatus);
					INCLUDE_STRUCT(ST_MARKET_STATUS, PlMarketStatus);
					CREATE_NNF_CHAR(UpdatePortfolio)
						CREATE_NNF_CHAR(cFiller)
						CREATE_NNF_LONG(MarketIndex)
						CREATE_NNF_SHORT(DefaultSettlementPeriodNormal)
						CREATE_NNF_SHORT(DefaultSettlementPeriodSpot)
						CREATE_NNF_SHORT(DefaultSettlementPeriodAuction)
						CREATE_NNF_SHORT(CompetitorPeriod)
						CREATE_NNF_SHORT(SolicitorPeriod)
						CREATE_NNF_SHORT(WarningPercent)
						CREATE_NNF_SHORT(VolumeFreezePercent)
						CREATE_NNF_SHORT(SnapQuoteTime)
						CREATE_NNF_SHORT(Reserved)
						CREATE_NNF_LONG(BoardLotQty)
						CREATE_NNF_LONG(TickSize)
						CREATE_NNF_SHORT(MaxGtcDays)
						INCLUDE_STRUCT(ST_STOCK_ELIGIBLE_INDICATORS, StockEligibileIndicator);
						CREATE_NNF_SHORT(DisclosedQtyPercentAllowed)
						CREATE_NNF_LONG(RiskFreeInterestRate)



						void getString(std::string& str)
					{

						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getUpdatePortfolio()));
						str.append(",");
						str.append(std::to_string(getMarketIndex()));
						str.append(",");
						str.append(std::to_string(getDefaultSettlementPeriodNormal()));
						str.append(",");
						str.append(std::to_string(getDefaultSettlementPeriodSpot()));
						str.append(",");
						str.append(std::to_string(getDefaultSettlementPeriodAuction()));
						str.append(",");
						str.append(std::to_string(getCompetitorPeriod()));
						str.append(",");
						str.append(std::to_string(getSolicitorPeriod()));
						str.append(",");
						str.append(std::to_string(getReserved()));
						str.append(",");
						str.append(std::to_string(getBoardLotQty()));
						str.append(",");
						str.append(std::to_string(getTickSize()));
						str.append(",");
						str.append(std::to_string(getMaxGtcDays()));
						str.append(",");
						str.append(std::to_string(getDisclosedQtyPercentAllowed()));
						str.append(",");
						str.append(std::to_string(getRiskFreeInterestRate()));


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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeCharConvert(std::stoi(result[9])))
							setUpdatePortfolio(static_cast<char>(std::stoi(result[9])));

						setMarketIndex(std::stol(result[10]));

						if (isSafeShortConvert(std::stoi(result[11])))
							setDefaultSettlementPeriodNormal(static_cast<short>(std::stoi(result[11])));

						if (isSafeShortConvert(std::stoi(result[12])))
							setDefaultSettlementPeriodSpot(static_cast<short>(std::stoi(result[12])));

						if (isSafeShortConvert(std::stoi(result[13])))
							setDefaultSettlementPeriodAuction(static_cast<short>(std::stoi(result[13])));

						if (isSafeShortConvert(std::stoi(result[14])))
							setCompetitorPeriod(static_cast<short>(std::stoi(result[14])));

						if (isSafeShortConvert(std::stoi(result[15])))
							setSolicitorPeriod(static_cast<short>(std::stoi(result[15])));

						if (isSafeShortConvert(std::stoi(result[16])))
							setWarningPercent(static_cast<short>(std::stoi(result[16])));

						if (isSafeShortConvert(std::stoi(result[17])))
							setVolumeFreezePercent(static_cast<short>(std::stoi(result[17])));

						if (isSafeShortConvert(std::stoi(result[18])))
							setSnapQuoteTime(static_cast<short>(std::stoi(result[18])));

						if (isSafeShortConvert(std::stoi(result[19])))
							setReserved(static_cast<short>(std::stoi(result[19])));
						setBoardLotQty(std::stol(result[20]));
						setTickSize(std::stol(result[21]));

						if (isSafeShortConvert(std::stoi(result[22])))
							setMaxGtcDays(static_cast<short>(std::stoi(result[22])));

						if (isSafeShortConvert(std::stoi(result[23])))
							setDisclosedQtyPercentAllowed(static_cast<short>(std::stoi(result[23])));

						setRiskFreeInterestRate(std::stol(result[24]));

						print();
					}


					void print() const
					{
						PRINT_BODY_START(MS_SYSTEM_INFO_DATA)

							//DEVLOG( "AlphaCar(Stream): " << MESSAGE_HEADER::getStreamNo() );
							MESSAGE_HEADER::print();
						_MarketStatus.print();
						_ExMarketStatus.print();
						_PlMarketStatus.print();
						PRINT_NNF_FIELD(UpdatePortfolio)
							PRINT_NNF_FIELD(MarketIndex)
							PRINT_NNF_FIELD(DefaultSettlementPeriodNormal)
							PRINT_NNF_FIELD(DefaultSettlementPeriodSpot)
							PRINT_NNF_FIELD(DefaultSettlementPeriodAuction)
							PRINT_NNF_FIELD(CompetitorPeriod)
							PRINT_NNF_FIELD(SolicitorPeriod)
							PRINT_NNF_FIELD(WarningPercent)
							PRINT_NNF_FIELD(VolumeFreezePercent)
							PRINT_NNF_FIELD(SnapQuoteTime)
							PRINT_NNF_FIELD(Reserved)
							PRINT_NNF_FIELD(BoardLotQty)
							PRINT_NNF_FIELD(TickSize)
							PRINT_NNF_FIELD(MaxGtcDays)
							_StockEligibileIndicator.print();
						PRINT_NNF_FIELD(DisclosedQtyPercentAllowed)
							PRINT_NNF_FIELD(RiskFreeInterestRate)
							PRINT_BODY_END(MS_SYSTEM_INFO_DATA)
					}
				};

				struct MS_MESSAGE_DOWNLOAD : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
				{
					CREATE_NNF_DOUBLE(SequenceNumber)

					MS_MESSAGE_DOWNLOAD(SIGNED_NNF_LONG traderID)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MESSAGE_HEADER(TransCode_DOWNLOAD_REQUEST,
							traderID, sizeof(MS_MESSAGE_DOWNLOAD) - sizeof(DC_PACKET_WRAPPER))
					{
						reset();
					}

					void reset()
					{
						setSequenceNumber(0);
					}

					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getSequenceNumber()));

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						setSequenceNumber(std::stod(result[9]));

						print();
					}
					void print() const
					{
						PRINT_BODY_START(MS_MESSAGE_DOWNLOAD)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(SequenceNumber)
							PRINT_BODY_END(MS_MESSAGE_DOWNLOAD)
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
						str.append(",");
						str.append(getSymbol());
						str.append(",");
						str.append(std::to_string(getExpiryDate()));
						str.append(",");
						str.append(std::to_string(getStrikePrice()));
						str.append(",");
						str.append(getOptionType());
						str.append(",");
						str.append(std::to_string(getCALevel()));

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

						setInstrumentName(result[0]);
						setSymbol(result[1]);
						setExpiryDate(std::stol(result[2]));
						setStrikePrice(std::stol(result[3]));
						setOptionType(result[4]);

						if (isSafeShortConvert(std::stoi(result[5])))
							setCALevel(static_cast<short>(std::stoi(result[5])));


						print();
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

						str.append(std::to_string(getAON())); str.append(",");
						str.append(std::to_string(getIOC())); str.append(",");
						str.append(std::to_string(getGTC())); str.append(",");
						str.append(std::to_string(getDay())); str.append(",");
						str.append(std::to_string(getMIT())); str.append(",");
						str.append(std::to_string(getSL())); str.append(",");
						str.append(std::to_string(getMarket())); str.append(",");
						str.append(std::to_string(getATO())); str.append(",");
						str.append(std::to_string(getReserved1())); str.append(",");
						str.append(std::to_string(getReserved2())); str.append(",");
						str.append(std::to_string(getReserved3())); str.append(",");
						str.append(std::to_string(getFrozen())); str.append(",");
						str.append(std::to_string(getModified())); str.append(",");
						str.append(std::to_string(getTraded())); str.append(",");
						str.append(std::to_string(getMatchedInd())); str.append(",");
						str.append(std::to_string(getMF())); str.append(",");

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
						if (isSafeUCharConvert(std::stoi(result[0])))
							setAON(static_cast<unsigned char>(std::stoi(result[0])));

						if (isSafeUCharConvert(std::stoi(result[1])))
							setIOC(static_cast<unsigned char>(std::stoi(result[1])));

						if (isSafeUCharConvert(std::stoi(result[2])))
							setGTC(static_cast<unsigned char>(std::stoi(result[2])));

						if (isSafeUCharConvert(std::stoi(result[3])))
							setDay(static_cast<unsigned char>(std::stoi(result[3])));

						if (isSafeUCharConvert(std::stoi(result[4])))
							setMIT(static_cast<unsigned char>(std::stoi(result[4])));

						if (isSafeUCharConvert(std::stoi(result[5])))
							setSL(static_cast<unsigned char>(std::stoi(result[5])));

						if (isSafeUCharConvert(std::stoi(result[6])))
							setMarket(static_cast<unsigned char>(std::stoi(result[6])));

						if (isSafeUCharConvert(std::stoi(result[7])))
							setATO(static_cast<unsigned char>(std::stoi(result[7])));

						if (isSafeUCharConvert(std::stoi(result[8])))
							setReserved1(static_cast<unsigned char>(std::stoi(result[8])));

						if (isSafeUCharConvert(std::stoi(result[9])))
							setReserved2(static_cast<unsigned char>(std::stoi(result[9])));

						if (isSafeUCharConvert(std::stoi(result[10])))
							setReserved3(static_cast<unsigned char>(std::stoi(result[10])));

						if (isSafeUCharConvert(std::stoi(result[11])))
							setFrozen(static_cast<unsigned char>(std::stoi(result[11])));

						if (isSafeUCharConvert(std::stoi(result[12])))
							setModified(static_cast<unsigned char>(std::stoi(result[12])));

						if (isSafeUCharConvert(std::stoi(result[13])))
							setTraded(static_cast<unsigned char>(std::stoi(result[13])));

						if (isSafeUCharConvert(std::stoi(result[14])))
							setMatchedInd(static_cast<unsigned char>(std::stoi(result[14])));

						if (isSafeUCharConvert(std::stoi(result[15])))
							setMF(static_cast<unsigned char>(std::stoi(result[15])));

						print();
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
						CREATE_NNF_BIT(STPC)
						CREATE_NNF_BIT(Reserved4)
						CREATE_NNF_BIT(Reserved5)
						CREATE_NNF_BIT(Reserved6)

						ADDITIONAL_ORDER_FLAGS()
					{
						reset();
					}
					void getString(std::string& str)
					{

						str.append(std::to_string(getReserved1())); str.append(",");
						str.append(std::to_string(getCOL())); str.append(",");
						str.append(std::to_string(getReserved2())); str.append(",");
						str.append(std::to_string(getReserved3())); str.append(",");
						str.append(std::to_string(getSTPC())); str.append(",");
						str.append(std::to_string(getReserved4())); str.append(",");
						str.append(std::to_string(getReserved5())); str.append(",");
						str.append(std::to_string(getReserved6())); str.append(",");
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
						if (isSafeUCharConvert(std::stoi(result[0])))
							setReserved1(static_cast<unsigned char>(std::stoi(result[0])));

						if (isSafeUCharConvert(std::stoi(result[1])))
							setCOL(static_cast<unsigned char>(std::stoi(result[1])));

						if (isSafeUCharConvert(std::stoi(result[2])))
							setReserved2(static_cast<unsigned char>(std::stoi(result[2])));

						if (isSafeUCharConvert(std::stoi(result[3])))
							setReserved3(static_cast<unsigned char>(std::stoi(result[3])));

						if (isSafeUCharConvert(std::stoi(result[4])))
							setSTPC(static_cast<unsigned char>(std::stoi(result[4])));

						if (isSafeUCharConvert(std::stoi(result[5])))
							setReserved4(static_cast<unsigned char>(std::stoi(result[5])));

						if (isSafeUCharConvert(std::stoi(result[6])))
							setReserved5(static_cast<unsigned char>(std::stoi(result[6])));

						if (isSafeUCharConvert(std::stoi(result[7])))
							setReserved6(static_cast<unsigned char>(std::stoi(result[7])));

						print();
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
							PRINT_NNF_FIELD(STPC)
							//PRINT_NNF_FIELD ( Reserved4)
							//PRINT_NNF_FIELD ( Reserved5)
							//PRINT_NNF_FIELD ( Reserved6)
							PRINT_BODY_END(ADDITIONAL_ORDER_FLAGS)
					}
				};

				struct MS_OE_REQUEST : public MESSAGE_HEADER
				{
					CREATE_NNF_CHAR(ParticipantType)
						CREATE_NNF_CHAR(Reserved)
						CREATE_NNF_SHORT(CompetitorPeriod)
						CREATE_NNF_SHORT(SolicitorPeriod)
						CREATE_NNF_CHAR(ModifiedCancelledBy)
						CREATE_NNF_CHAR(Reserved1)
						CREATE_NNF_SHORT(ReasonCode)
						CREATE_NNF_STRING(Reserved2, 4)
						CREATE_NNF_LONG(TokenNo)
						INCLUDE_STRUCT(CONTRACT_DESC, ContractDesc);
					CREATE_NNF_STRING(CounterPartyBrokerId, 5)
						CREATE_NNF_CHAR(Reserved3)
						CREATE_NNF_STRING(Reserved4, 2)
						CREATE_NNF_CHAR(CloseoutFlag)
						CREATE_NNF_CHAR(Reserved5)
						CREATE_NNF_SHORT(OrderType)
						CREATE_NNF_DOUBLE(OrderNumber)
						CREATE_NNF_STRING(AccountNumber, 10)
						CREATE_NNF_SHORT(BookType)
						CREATE_NNF_SHORT(BuySellIndicator)
						CREATE_NNF_LONG(DisclosedVolume)
						CREATE_NNF_LONG(DisclosedVolumeRemaining)
						CREATE_NNF_LONG(TotalVolumeRemaining)
						CREATE_NNF_LONG(Volume)
						CREATE_NNF_LONG(VolumeFilledToday)
						CREATE_NNF_LONG(Price)
						CREATE_NNF_LONG(TriggerPrice)
						CREATE_NNF_LONG(GoodTillDate)
						CREATE_NNF_LONG(EntryDateTime)
						CREATE_NNF_LONG(MinimumFillAONVolume)
						CREATE_NNF_LONG(LastModified)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags)
						CREATE_NNF_SHORT(BranchId)
						CREATE_NNF_LONG(TraderId)
						CREATE_NNF_STRING(BrokerId, 5)
						CREATE_NNF_STRING(cOrdFiller, 24)
						CREATE_NNF_CHAR(OpenClose)
						CREATE_NNF_STRING(Settlor, 12)
						CREATE_NNF_SHORT(ProClientIndicator)
						CREATE_NNF_SHORT(SettlementPeriod)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS, AdditionalOrderFlags)
						CREATE_NNF_CHAR(GiveupFlag)
						//CREATE_NNF_STRING ( ClOrdId, 4)
						CREATE_NNF_LONG(ClOrdId)
						CREATE_NNF_DOUBLE(NnfField)
						CREATE_NNF_DOUBLE(MktReplay)
						CREATE_NNF_STRING(PAN, 10)
						CREATE_NNF_LONG(AlgoID)
						CREATE_NNF_SHORT(Reserved6)
						CREATE_NNF_LONG_LONG(LastActivityReference)
						CREATE_NNF_STRING(Reserved7, 52)

						MS_OE_REQUEST()
					{
						reset();
					}

					MS_OE_REQUEST(TransCode tc, SIGNED_NNF_LONG traderID)
						:MESSAGE_HEADER(tc, traderID, sizeof(MS_OE_REQUEST))
					{
						reset();
					}


					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getReserved()));
						str.append(", ");
						str.append(std::to_string(getCompetitorPeriod()));
						str.append(", ");
						str.append(std::to_string(getSolicitorPeriod()));
						str.append(", ");
						str.append(std::to_string(getModifiedCancelledBy()));
						str.append(", ");
						str.append(std::to_string(getReserved1()));
						str.append(", ");
						str.append(std::to_string(getReasonCode()));
						str.append(", ");
						str.append(getReserved2());
						str.append(", ");
						str.append(std::to_string(getTokenNo()));
						str.append(", ");
						str.append(getCounterPartyBrokerId());
						str.append(", ");
						str.append(std::to_string(getReserved3()));
						str.append(", ");
						str.append(getReserved4());
						str.append(", ");
						str.append(std::to_string(getCloseoutFlag()));
						str.append(", ");
						str.append(std::to_string(getReserved5()));
						str.append(", ");
						str.append(std::to_string(getOrderType()));
						str.append(",  ");
						str.append(std::to_string(getOrderNumber()));
						str.append(", ");
						str.append(getAccountNumber());
						str.append(", ");
						str.append(std::to_string(getBookType()));
						str.append(", ");
						str.append(std::to_string(getBuySellIndicator()));
						str.append(", ");
						str.append(std::to_string(getDisclosedVolume()));
						str.append(",  ");
						str.append(std::to_string(getDisclosedVolumeRemaining()));
						str.append(", ");
						str.append(std::to_string(getTotalVolumeRemaining()));
						str.append(",  ");
						str.append(std::to_string(getVolume()));
						str.append(",  ");
						str.append(std::to_string(getVolumeFilledToday()));
						str.append(",  ");
						str.append(std::to_string(getPrice()));
						str.append(",  ");
						str.append(std::to_string(getTriggerPrice()));
						str.append(",  ");
						str.append(std::to_string(getGoodTillDate()));
						str.append(",  ");
						str.append(std::to_string(getEntryDateTime()));
						str.append(",  ");
						str.append(std::to_string(getMinimumFillAONVolume()));
						str.append(",   ");
						str.append(std::to_string(getLastModified()));
						str.append(",  ");
						str.append(std::to_string(getBranchId()));
						str.append(",  ");
						str.append(std::to_string(getTraderId()));
						str.append(",  ");
						str.append(getBrokerId());
						str.append(",  ");
						str.append(getcOrdFiller());
						str.append(",  ");
						str.append(std::to_string(getOpenClose()));
						str.append(",  ");
						str.append(getSettlor());
						str.append(",  ");
						str.append(std::to_string(getProClientIndicator()));
						str.append(",  ");
						str.append(std::to_string(getSettlementPeriod()));
						str.append(",  ");
						str.append(std::to_string(getGiveupFlag()));
						str.append(",  ");
						str.append(std::to_string(getClOrdId()));
						str.append(",  ");
						str.append(std::to_string(getNnfField()));
						str.append(",  ");
						str.append(std::to_string(getMktReplay()));
						str.append(",  ");
						str.append(getPAN());
						str.append(", ");
						str.append(std::to_string(getAlgoID()));
						str.append(",  ");
						str.append(std::to_string(getReserved6()));
						str.append(",  ");
						str.append(std::to_string(getLastActivityReference()));
						str.append(", ");
						str.append(getReserved7());

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeCharConvert(std::stoi(result[9])))
							setParticipantType(static_cast<char>(std::stoi(result[9])));

						if (isSafeCharConvert(std::stoi(result[10])))
							setReserved(static_cast<char>(std::stoi(result[10])));

						if (isSafeShortConvert(std::stoi(result[11])))
							setCompetitorPeriod(static_cast<short>(std::stoi(result[11])));

						if (isSafeShortConvert(std::stoi(result[12])))
							setSolicitorPeriod(static_cast<short>(std::stoi(result[12])));

						if (isSafeCharConvert(std::stoi(result[13])))
							setModifiedCancelledBy(static_cast<char>(std::stoi(result[13])));

						if (isSafeCharConvert(std::stoi(result[14])))
							setReserved1(static_cast<char>(std::stoi(result[14])));

						if (isSafeShortConvert(std::stoi(result[15])))
							setReasonCode(static_cast<short>(std::stoi(result[15])));

						setReserved2(result[16]);
						setTokenNo(std::stol(result[17]));
						setCounterPartyBrokerId(result[18]);

						if (isSafeCharConvert(std::stoi(result[19])))
							setReserved3(static_cast<char>(std::stoi(result[19])));

						setReserved4(result[20]);

						if (isSafeCharConvert(std::stoi(result[21])))
							setCloseoutFlag(static_cast<char>(std::stoi(result[21])));

						if (isSafeCharConvert(std::stoi(result[22])))
							setReserved5(static_cast<char>(std::stoi(result[22])));

						if (isSafeShortConvert(std::stoi(result[23])))
							setOrderType(static_cast<short>(std::stoi(result[23])));

						setOrderNumber(std::stod(result[24]));
						setAccountNumber(result[25]);

						if (isSafeShortConvert(std::stoi(result[26])))
							setBookType(static_cast<short>(std::stoi(result[26])));

						if (isSafeShortConvert(std::stoi(result[27])))
							setBuySellIndicator(static_cast<short>(std::stoi(result[27])));

						setDisclosedVolume(std::stol(result[28]));
						setDisclosedVolumeRemaining(std::stol(result[29]));
						setTotalVolumeRemaining(std::stol(result[30]));
						setVolume(std::stol(result[31]));
						setVolumeFilledToday(std::stol(result[32]));
						setPrice(std::stol(result[33]));
						setTriggerPrice(std::stol(result[34]));
						setGoodTillDate(std::stol(result[35]));
						setEntryDateTime(std::stol(result[36]));
						setMinimumFillAONVolume(std::stol(result[37]));
						setLastModified(std::stol(result[38]));

						if (isSafeShortConvert(std::stoi(result[39])))
							setBranchId(static_cast<short>(std::stoi(result[39])));

						setTraderId(std::stol(result[40]));
						setBrokerId(result[41]);
						setcOrdFiller(result[42]);

						if (isSafeCharConvert(std::stoi(result[43])))
							setOpenClose(static_cast<char>(std::stoi(result[43])));

						setSettlor(result[44]);

						if (isSafeShortConvert(std::stoi(result[45])))
							setProClientIndicator(static_cast<short>(std::stoi(result[45])));

						if (isSafeShortConvert(std::stoi(result[46])))
							setSettlementPeriod(static_cast<short>(std::stoi(result[46])));

						if (isSafeCharConvert(std::stoi(result[47])))
							setGiveupFlag(static_cast<char>(std::stoi(result[47])));

						setClOrdId(std::stol(result[48]));
						setNnfField(std::stod(result[49]));
						setMktReplay(std::stod(result[50]));
						setPAN(result[51]);
						setAlgoID(std::stol(result[52]));

						if (isSafeShortConvert(std::stoi(result[53])))
							setReserved6(static_cast<short>(std::stoi(result[53])));

						setLastActivityReference(std::stoll(result[54]));
						setReserved7(result[55]);

						print();
					}
					void reset()
					{
						setParticipantType('C');
						setReserved(' ');
						setCompetitorPeriod(0);
						setSolicitorPeriod(0);
						setModifiedCancelledBy('T');
						setReserved1(' ');
						setReasonCode(0);
						setReserved2(std::string(4, ' '));
						setTokenNo(0);
						_ContractDesc.reset();
						setCounterPartyBrokerId(std::string(5, ' '));
						setReserved3(' ');
						setReserved4(std::string(2, ' '));
						setCloseoutFlag(' ');
						setReserved5(' ');
						//setOrderType('');
						setOrderNumber(0);
						setAccountNumber(std::string(10, ' '));
						////setBookType ( 1 ) ; // Regular Lot order
						////setBuySellIndicator ( 0 ) ;
						setDisclosedVolume(0);
						setDisclosedVolumeRemaining(0);
						setTotalVolumeRemaining(0);
						////setVolume ( 0 ) ;
						setVolumeFilledToday(0);
						////setPrice ( 0 ) ;
						////setTriggerPrice ( 0 ) ;
						setGoodTillDate(0);
						setEntryDateTime(0);
						setMinimumFillAONVolume(0);
						setLastModified(0);
						_StOrderFlags.reset();
						////setBranchId ( 0 ) ;
						////setTraderId ( 0 ) ;
						//setBrokerId(std::string(5, ' '));
						setcOrdFiller(std::string(24, ' '));
						setOpenClose(' '); // Open
						setSettlor(std::string(12, ' '));
						////setProClientIndicator ( 0 ) ;
						setSettlementPeriod(0);
						_AdditionalOrderFlags.reset();
						setGiveupFlag('P');
						//setClOrdId(std::string(4, ' '));
						////setNnfField ( 0 ) ;
						setMktReplay(0);
						setReserved6(0);

					}

					void print() const
					{
						PRINT_BODY_START(MS_OE_REQUEST)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(ParticipantType)
							//PRINT_NNF_FIELD ( Reserved )
							PRINT_NNF_FIELD(CompetitorPeriod)
							PRINT_NNF_FIELD(SolicitorPeriod)
							PRINT_NNF_FIELD(ModifiedCancelledBy)
							//PRINT_NNF_FIELD ( Reserved1)
							PRINT_NNF_FIELD(ReasonCode)
							//PRINT_NNF_FIELD ( Reserved2 )
							PRINT_NNF_FIELD(TokenNo)
							_ContractDesc.print();
						PRINT_NNF_FIELD(CounterPartyBrokerId)
							//PRINT_NNF_FIELD ( Reserved3 )
							//PRINT_NNF_FIELD ( Reserved4 )
							PRINT_NNF_FIELD(CloseoutFlag)
							//PRINT_NNF_FIELD ( Reserved5 )
							PRINT_NNF_FIELD(OrderType)
							PRINT_NNF_FIELD(OrderNumber)
							PRINT_NNF_FIELD(AccountNumber)
							PRINT_NNF_FIELD(BookType)
							PRINT_NNF_FIELD(BuySellIndicator)
							PRINT_NNF_FIELD(DisclosedVolume)
							PRINT_NNF_FIELD(DisclosedVolumeRemaining)
							PRINT_NNF_FIELD(TotalVolumeRemaining)
							PRINT_NNF_FIELD(Volume)
							PRINT_NNF_FIELD(VolumeFilledToday)
							PRINT_NNF_FIELD(Price)
							PRINT_NNF_FIELD(TriggerPrice)
							PRINT_NNF_FIELD(GoodTillDate)
							PRINT_NNF_FIELD(EntryDateTime)
							PRINT_NNF_FIELD(MinimumFillAONVolume)
							PRINT_NNF_FIELD(LastModified)
							_StOrderFlags.print();
						PRINT_NNF_FIELD(BranchId)
							PRINT_NNF_FIELD(TraderId)
							PRINT_NNF_FIELD(BrokerId)
							PRINT_NNF_FIELD(cOrdFiller)
							PRINT_NNF_FIELD(OpenClose)
							PRINT_NNF_FIELD(Settlor)
							PRINT_NNF_FIELD(ProClientIndicator)
							PRINT_NNF_FIELD(SettlementPeriod)
							_AdditionalOrderFlags.print();
						//PRINT_NNF_FIELD(GiveupFlag)
						PRINT_NNF_FIELD(ClOrdId)
							PRINT_NNF_FIELD(NnfField)
							PRINT_NNF_FIELD(MktReplay)

							PRINT_NNF_FIELD(PAN)
							PRINT_NNF_FIELD(AlgoID)
							PRINT_NNF_FIELD(LastActivityReference)

							PRINT_BODY_END(MS_OE_REQUEST)
					}
					short getTCode() { return MESSAGE_HEADER::getTCode(); }
				};

				struct NEW_ORDER : public DC_PACKET_WRAPPER, public MS_OE_REQUEST
				{
					NEW_ORDER(SIGNED_NNF_LONG traderID)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MS_OE_REQUEST(TransCode_BOARD_LOT_IN/*TransCode_BOARD_LOT_IN_TR*/, traderID)
					{
						reset();
						setModifiedCancelledBy(' '); // This should be blank in New Order Req
					}
					//str.append(DC_PACKET_WRAPPER::getString(str));

					void getString(std::string& str)
					{
						//str.append(MS_OE_REQUEST :: getString(str));
						MS_OE_REQUEST::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}

					void reset()
					{
						DC_PACKET_WRAPPER::reset();
						MS_OE_REQUEST::reset();
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(NEW_ORDER)
							DC_PACKET_WRAPPER::print();
						MS_OE_REQUEST::fromString(str);
						//MS_OE_REQUEST::print() ;
						PRINT_BODY_END(NEW_ORDER)
					}
					short getTCode() { return MS_OE_REQUEST::getTCode(); }
				};

				struct MODIFY_ORDER : public DC_PACKET_WRAPPER, public MS_OE_REQUEST
				{

					MODIFY_ORDER(SIGNED_NNF_LONG traderID)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MS_OE_REQUEST(TransCode_ORDER_MOD_IN, traderID)
					{
						reset();
					}
					void reset()
					{
						DC_PACKET_WRAPPER::reset();
						MS_OE_REQUEST::reset();
					}

					void getString(std::string& str)
					{
						//str.append( MS_OE_REQUEST::getString(str));
						MS_OE_REQUEST::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(MODIFY_ORDER)
							MS_OE_REQUEST::fromString(str);
						//MS_OE_REQUEST::print() ;
						PRINT_BODY_END(MODIFY_ORDER)
					}

				};

				struct CANCEL_ORDER : public DC_PACKET_WRAPPER, public MS_OE_REQUEST
				{

					CANCEL_ORDER(SIGNED_NNF_LONG traderID)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MS_OE_REQUEST(TransCode_ORDER_CANCEL_IN, traderID)
					{
						reset();
					}
					void reset()
					{
						DC_PACKET_WRAPPER::reset();
						MS_OE_REQUEST::reset();
					}
					void getString(std::string& str)
					{
						//str.append( MS_OE_REQUEST::getString(str));   
						MS_OE_REQUEST::getString(str);
					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(CANCEL_ORDER)
							//MS_OE_REQUEST::print() ;
							MS_OE_REQUEST::fromString(str);
						PRINT_BODY_END(CANCEL_ORDER)
					}
				};

				struct ORDER_CONFIRM_RESPONSE : public MS_OE_REQUEST
				{

					void getString(std::string& str)
					{
						//str.append(MS_OE_REQUEST::getString(str));    
						MS_OE_REQUEST::getString(str);
					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_CONFIRM_RESPONSE)
							MS_OE_REQUEST::fromString(str);
						//MS_OE_REQUEST::print() ;
						PRINT_BODY_END(ORDER_CONFIRM_RESPONSE)
					}
				};

				struct ORDER_FREEZE_RESPONSE : public MS_OE_REQUEST
				{

					void getString(std::string& str)
					{
						MS_OE_REQUEST::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_FREEZE_RESPONSE)
							//MS_OE_REQUEST::print() ;
							MS_OE_REQUEST::fromString(str);
						PRINT_BODY_END(ORDER_FREEZE_RESPONSE)
					}
				};

				struct ORDER_ERROR_RESPONSE : public MS_OE_REQUEST
				{

					void getString(std::string& str)
					{
						MS_OE_REQUEST::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_ERROR_RESPONSE)
							//MS_OE_REQUEST::print() ;
							MS_OE_REQUEST::fromString(str);
						PRINT_BODY_END(ORDER_ERROR_RESPONSE)
					}
				};

				struct ORDER_MODIFICATION_CONFIRMATION : public MS_OE_REQUEST
				{

					void getString(std::string& str)
					{
						MS_OE_REQUEST::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_MODIFICATION_CONFIRMATION)
							MS_OE_REQUEST::fromString(str);
						//MS_OE_REQUEST::print() ;
						PRINT_BODY_END(ORDER_MODIFICATION_CONFIRMATION)
					}
					// 2074
				};

				struct ORDER_CANCELATION_ERROR : public MS_OE_REQUEST
				{

					void getString(std::string& str)
					{
						MS_OE_REQUEST::getString(str);
					}

					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_CANCELATION_ERROR)
							//MS_OE_REQUEST::print() ;
							MS_OE_REQUEST::fromString(str);
						PRINT_BODY_END(ORDER_CANCELATION_ERROR)
					}
					// 2072
				};

				struct ORDER_MODIFICATION_ERROR : public MS_OE_REQUEST
				{

					void getString(std::string& str)
					{
						//str.append(MS_OE_REQUEST::getString(str));
						MS_OE_REQUEST::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{

						PRINT_BODY_START(ORDER_MODIFICATION_ERROR)
							//MS_OE_REQUEST::print() ;
							MS_OE_REQUEST::fromString(str);
						PRINT_BODY_END(ORDER_MODIFICATION_ERROR)
					}
					// 2042
				};
				struct ORDER_CANCEL_CONFIRMATION : public MS_OE_REQUEST
				{

					void getString(std::string& str)
					{
						//str.append(MS_OE_REQUEST::getString(str));    
						MS_OE_REQUEST::getString(str);
					}
					void fromString(std::string& str)
					{
						print(str);
					}

					//void print() const
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_CANCEL_CONFIRMATION)
							MS_OE_REQUEST::fromString(str);
						//MS_OE_REQUEST::print() ;
						PRINT_BODY_END(ORDER_CANCEL_CONFIRMATION)
					}
				};

				struct CONTRACT_DESC_TR
				{
					CREATE_NNF_STRING(InstrumentName, 6)
						CREATE_NNF_STRING(Symbol, 10)
						CREATE_NNF_LONG(ExpiryDate)
						CREATE_NNF_LONG(StrikePrice)
						CREATE_NNF_STRING(OptionType, 2)

						void getString(std::string& str)
					{

						str.append(getInstrumentName());
						str.append(",");
						str.append(getSymbol());
						str.append(",");
						str.append(std::to_string(getExpiryDate()));
						str.append(",");
						str.append(std::to_string(getStrikePrice()));
						str.append(",");
						str.append(getOptionType());
						str.append(",");

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

						setInstrumentName(result[0]);
						setSymbol(result[1]);
						setExpiryDate(std::stol(result[2]));
						setStrikePrice(std::stol(result[3]));
						setOptionType(result[4]);

						print();
					}

					CONTRACT_DESC_TR()
					{
						reset();
					}

					void reset()
					{
						setInstrumentName(std::string(6, ' '));
						setSymbol(std::string(10, ' '));
						setExpiryDate(0);
						setStrikePrice(0);
						setOptionType(std::string(2, ' '));
					}

					void print() const
					{
						PRINT_BODY_START(CONTRACT_DESC_TR)
							PRINT_NNF_FIELD(InstrumentName)
							PRINT_NNF_FIELD(Symbol)
							PRINT_NNF_FIELD(ExpiryDate)
							PRINT_NNF_FIELD(StrikePrice)
							PRINT_NNF_FIELD(OptionType)
							PRINT_BODY_END(CONTRACT_DESC_TR)
					}
				};

#pragma pack ( pop )
#pragma pack ( push, 1)
				struct ADDITIONAL_ORDER_FLAGS_TR
				{
					CREATE_NNF_BIT(Reserved1)
						CREATE_NNF_BIT(COL)
						CREATE_NNF_BIT(Reserved2)
						CREATE_NNF_BIT(Reserved3)
						CREATE_NNF_BIT(STPC)
						CREATE_NNF_BIT(Reserved4)
						CREATE_NNF_BIT(Reserved5)
						CREATE_NNF_BIT(Reserved6)

						void getString(std::string& str)
					{

						str.append(std::to_string(getReserved1()));
						str.append(",");
						str.append(std::to_string(getCOL()));
						str.append(",");
						str.append(std::to_string(getReserved2()));
						str.append(",");
						str.append(std::to_string(getReserved3()));
						str.append(",");
						str.append(std::to_string(getSTPC()));
						str.append(",");
						str.append(std::to_string(getReserved4()));
						str.append(",");
						str.append(std::to_string(getReserved5()));
						str.append(",");
						str.append(std::to_string(getReserved6()));

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

						if (isSafeUCharConvert(std::stoi(result[0])))
							setReserved1(static_cast<unsigned char>(std::stoi(result[0])));

						if (isSafeUCharConvert(std::stoi(result[1])))
							setCOL(static_cast<unsigned char>(std::stoi(result[1])));

						if (isSafeUCharConvert(std::stoi(result[2])))
							setReserved2(static_cast<unsigned char>(std::stoi(result[2])));

						if (isSafeUCharConvert(std::stoi(result[3])))
							setReserved3(static_cast<unsigned char>(std::stoi(result[3])));

						if (isSafeUCharConvert(std::stoi(result[4])))
							setSTPC(static_cast<unsigned char>(std::stoi(result[4])));

						if (isSafeUCharConvert(std::stoi(result[5])))
							setReserved4(static_cast<unsigned char>(std::stoi(result[5])));

						if (isSafeUCharConvert(std::stoi(result[6])))
							setReserved5(static_cast<unsigned char>(std::stoi(result[6])));

						if (isSafeUCharConvert(std::stoi(result[7])))
							setReserved6(static_cast<unsigned char>(std::stoi(result[7])));

						print();
					}

					ADDITIONAL_ORDER_FLAGS_TR()
					{
						reset();
					}

					void reset()
					{
						memset(this, 0, sizeof(ADDITIONAL_ORDER_FLAGS_TR));
					}

					void print() const
					{
						PRINT_BODY_START(ADDITIONAL_ORDER_FLAGS_TR)
							//PRINT_NNF_FIELD ( Reserved1)
							PRINT_NNF_FIELD(COL)
							//PRINT_NNF_FIELD ( Reserved2)
							//PRINT_NNF_FIELD ( Reserved3)
							PRINT_NNF_FIELD(STPC)
							//PRINT_NNF_FIELD ( Reserved4)
							//PRINT_NNF_FIELD ( Reserved5)
							//PRINT_NNF_FIELD ( Reserved6)
							PRINT_BODY_END(ADDITIONAL_ORDER_FLAGS_TR)
					}
				};

#pragma pack ( pop )
#pragma pack ( push, 2)
				struct MS_OE_REQUEST_TR
				{
					CREATE_NNF_SHORT(TransCode)
						CREATE_NNF_LONG(UserID)
						CREATE_NNF_SHORT(ReasonCode)
						CREATE_NNF_LONG(TokenNo)
						INCLUDE_STRUCT(CONTRACT_DESC_TR, ContractDesc)
						CREATE_NNF_STRING(AccountNumber, 10)
						CREATE_NNF_SHORT(BookType)
						CREATE_NNF_SHORT(BuySellIndicator)
						CREATE_NNF_LONG(DisclosedVolume)
						CREATE_NNF_LONG(Volume)
						CREATE_NNF_LONG(Price)
						CREATE_NNF_LONG(GoodTillDate)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags)
						CREATE_NNF_SHORT(BranchId)
						CREATE_NNF_LONG(TraderId)
						CREATE_NNF_STRING(BrokerId, 5)
						CREATE_NNF_CHAR(OpenClose)
						CREATE_NNF_STRING(Settlor, 12)
						CREATE_NNF_SHORT(ProClientIndicator)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS_TR, AdditionalOrderFlags)
						CREATE_NNF_LONG(ClOrdId)
						CREATE_NNF_DOUBLE(NnfField)
						CREATE_NNF_STRING(PAN, 10)
						CREATE_NNF_LONG(AlgoID)
						//CREATE_NNF_SHORT( AlgoCategory                         )                     
						CREATE_NNF_SHORT(Reserved1)
						CREATE_NNF_STRING(Reserved2, 32)

						MS_OE_REQUEST_TR(int32_t traderID)
					{

						setTransCode(TransCode_BOARD_LOT_IN_TR);
						setUserID(traderID);
						memset(_AccountNumber, ' ', 10);
						memset(_BrokerId, ' ', 5);
						memset(_Settlor, ' ', 12);
						memset(_PAN, ' ', 10);

						setBookType(1);
						setOpenClose('O');

						setTraderId(traderID);

						setAlgoID(0);
						setReserved1(0);
					}

					void getStringWithValue(std::string& str)
					{
						str.append("TransCode: ");
						str.append(std::to_string(getTransCode()));
						str.append(", UserID: ");
						str.append(std::to_string(getUserID()));
						str.append(", ReasonCode: ");
						str.append(std::to_string(getReasonCode()));
						str.append(", TokenNo: ");
						str.append(std::to_string(getTokenNo()));
						str.append(", AccountNumber: ");
						str.append(getAccountNumber());
						str.append(", BookType: ");
						str.append(std::to_string(getBookType()));
						str.append(", BuySellIndicator: ");
						str.append(std::to_string(getBuySellIndicator()));
						str.append(", DisclosedVolume: ");
						str.append(std::to_string(getDisclosedVolume()));
						str.append(", Volume: ");
						str.append(std::to_string(getVolume()));
						str.append(", Price: ");
						str.append(std::to_string(getPrice()));
						str.append(", GoodTillDate: ");
						str.append(std::to_string(getGoodTillDate()));
						str.append(", BranchId: ");
						str.append(std::to_string(getBranchId()));
						str.append(", TraderId: ");
						str.append(std::to_string(getTraderId()));
						str.append(", BrokerId: ");
						str.append(getBrokerId());
						str.append(", OpenClose: ");
						str.append(std::to_string(getOpenClose()));
						str.append(", Settlor: ");
						str.append(getSettlor());
						str.append(", ProClientIndicator: ");
						str.append(std::to_string(getProClientIndicator()));
						str.append(", ClOrdId: ");
						str.append(std::to_string(getClOrdId()));
						str.append(", NnfField: ");
						str.append(std::to_string(getNnfField()));
						str.append(", PAN: ");
						str.append(getPAN());
						str.append(", AlgoID: ");
						str.append(std::to_string(getAlgoID()));
						str.append(", Reserved1: ");
						str.append(std::to_string(getReserved1()));
						str.append(", Reserved2: ");
						str.append(getReserved2());

						str.append(", CONTRACT_DESC_TR: ");
						std::string val1;
						getContractDesc().getString(val1);
						str.append(val1);

						str.append(", ST_ORDER_FLAGS: ");
						std::string val2;
						getStOrderFlags().getString(val2);
						str.append(val2);

						str.append(", ADDITIONAL_ORDER_FLAGS_TR: ");
						std::string val3;
						getAdditionalOrderFlags().getString(val3);
						str.append(val3);
					}


					void getString(std::string& str)
					{

						str.append(std::to_string(getTransCode()));
						str.append(",");
						str.append(std::to_string(getUserID()));
						str.append(",");
						str.append(std::to_string(getReasonCode()));
						str.append(",");
						str.append(std::to_string(getTokenNo()));
						str.append(",");
						str.append(getAccountNumber());
						str.append(",");
						str.append(std::to_string(getBookType()));
						str.append(",");
						str.append(std::to_string(getBuySellIndicator()));
						str.append(",");
						str.append(std::to_string(getDisclosedVolume()));
						str.append(",");
						str.append(std::to_string(getVolume()));
						str.append(",");
						str.append(std::to_string(getPrice()));
						str.append(",");
						str.append(std::to_string(getGoodTillDate()));
						str.append(",");
						str.append(std::to_string(getBranchId()));
						str.append(",");
						str.append(std::to_string(getTraderId()));
						str.append(",");
						str.append(getBrokerId());
						str.append(",");
						str.append(std::to_string(getOpenClose()));
						str.append(",");
						str.append(getSettlor());
						str.append(",");
						str.append(std::to_string(getProClientIndicator()));
						str.append(",");
						str.append(std::to_string(getClOrdId()));
						str.append(",");
						str.append(std::to_string(getNnfField()));
						str.append(",");
						str.append(getPAN());
						str.append(",");
						str.append(std::to_string(getAlgoID()));
						str.append(",");
						str.append(std::to_string(getReserved1()));
						str.append(",");
						str.append(getReserved2());


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
						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setUserID(std::stol(result[1]));

						if (isSafeShortConvert(std::stoi(result[2])))
							setReasonCode(static_cast<short>(std::stoi(result[2])));

						setTokenNo(std::stol(result[3]));
						setAccountNumber(result[4]);

						if (isSafeShortConvert(std::stoi(result[5])))
							setBookType(static_cast<short>(std::stoi(result[5])));

						if (isSafeShortConvert(std::stoi(result[6])))
							setBuySellIndicator(static_cast<short>(std::stoi(result[6])));

						setDisclosedVolume(std::stol(result[7]));
						setVolume(std::stol(result[8]));
						setPrice(std::stol(result[9]));
						setGoodTillDate(std::stol(result[10]));

						if (isSafeShortConvert(std::stoi(result[11])))
							setBranchId(static_cast<short>(std::stoi(result[11])));

						setTraderId(std::stol(result[12]));
						setBrokerId(result[13]);

						if (isSafeCharConvert(std::stoi(result[14])))
							setOpenClose(static_cast<char>(std::stoi(result[14])));

						setSettlor(result[15]);

						if (isSafeShortConvert(std::stoi(result[16])))
							setProClientIndicator(static_cast<short>(std::stoi(result[16])));

						setClOrdId(std::stol(result[17]));
						setNnfField(std::stod(result[18]));
						setPAN(result[19]);
						setAlgoID(std::stol(result[20]));

						if (isSafeShortConvert(std::stoi(result[21])))
							setReserved1(static_cast<short>(std::stoi(result[21])));

						setReserved2(result[22]);

						print();
					}
					void reset()
					{
						_ContractDesc.reset();
						_StOrderFlags.reset();
						_AdditionalOrderFlags.reset();
						setTokenNo(0);
						memset(_AccountNumber, ' ', 10);
						setBuySellIndicator(0);
						setDisclosedVolume(0);
						setVolume(0);
						setPrice(0);
						setGoodTillDate(0);
						setProClientIndicator(0);
						memset(_Settlor, ' ', 12);
						memset(_PAN, ' ', 10);
						setClOrdId(0);
						setNnfField(0);
					}

					void print() const
					{
						PRINT_NNF_FIELD(TransCode)
							PRINT_NNF_FIELD(UserID)
							PRINT_NNF_FIELD(ReasonCode)
							PRINT_NNF_FIELD(TokenNo)
							_ContractDesc.print();
						PRINT_NNF_FIELD(AccountNumber)
							PRINT_NNF_FIELD(BookType)
							PRINT_NNF_FIELD(BuySellIndicator)
							PRINT_NNF_FIELD(DisclosedVolume)
							PRINT_NNF_FIELD(Volume)
							PRINT_NNF_FIELD(Price)
							PRINT_NNF_FIELD(GoodTillDate)
							_StOrderFlags.print();
						PRINT_NNF_FIELD(BranchId)
							PRINT_NNF_FIELD(TraderId)
							PRINT_NNF_FIELD(BrokerId)
							PRINT_NNF_FIELD(OpenClose)
							PRINT_NNF_FIELD(Settlor)
							PRINT_NNF_FIELD(ProClientIndicator)
							_AdditionalOrderFlags.print();
						PRINT_NNF_FIELD(ClOrdId)
							//PRINT_NNF_FIELD( NnfField )
							std::cout << "[" << std::this_thread::get_id() << "]" << "(" << __FILE__ << ":" << __LINE__ << "): " << "NnfField: " << (int64_t)getNnfField() << std::endl;
						PRINT_NNF_FIELD(PAN)
							PRINT_NNF_FIELD(AlgoID)
					}
				};
				struct SECURE_BOX_REGISTRATION_REQUEST_IN : public DC_PACKET_WRAPPER, public MESSAGE_HEADER
				{
					CREATE_NNF_SHORT(BoxID)

						SECURE_BOX_REGISTRATION_REQUEST_IN(SIGNED_NNF_LONG traderID,
							SIGNED_NNF_SHORT boxId, const std::string& brokerId)
						: DC_PACKET_WRAPPER(sizeof(*this)),
						MESSAGE_HEADER(TransCode_SECURE_BOX_IN,
							traderID, sizeof(SECURE_BOX_REGISTRATION_REQUEST_IN) - sizeof(DC_PACKET_WRAPPER))
					{
						reset();
						setBoxID(boxId);
					}
					void getString(std::string& str)
					{
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getBoxID()));
						str.append(",");
						str.append(std::to_string(getBoxID()));
					}
					void reset()
					{
						setBoxID(0);
					}

					void print() const
					{
						PRINT_BODY_START(SECURE_BOX_REGISTRATION_REQUEST_IN)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(BoxID)
					}
				};
				struct ORDER_NEW_REQUEST_TR : public DC_PACKET_WRAPPER, public MS_OE_REQUEST_TR
				{

					ORDER_NEW_REQUEST_TR(int32_t traderId)
						: DC_PACKET_WRAPPER(sizeof(ORDER_NEW_REQUEST_TR)),
						MS_OE_REQUEST_TR(traderId)
					{
					}

					void getString(std::string& str)
					{
						MS_OE_REQUEST_TR::getString(str);

					}

					void getStringWithValue(std::string& str)
					{
						MS_OE_REQUEST_TR::getStringWithValue(str);
					}

					void fromString(std::string& str)
					{
						print(str);
					}
					void reset()
					{
						DC_PACKET_WRAPPER::reset();
						MS_OE_REQUEST_TR::reset();
					}

					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_NEW_REQUEST_TR)
							//MS_OE_REQUEST_TR::print() ;
							MS_OE_REQUEST_TR::fromString(str);
						PRINT_BODY_END(ORDER_NEW_REQUEST_TR)
					}
				};

#pragma pack ( pop )
#pragma pack ( push, 2)
				struct MS_OE_RESPONSE_TR
				{
					CREATE_NNF_SHORT(TransCode)
						CREATE_NNF_TIME(LogTime)
						CREATE_NNF_LONG(UserID)
						CREATE_NNF_SHORT(ErrorCode)
						CREATE_NNF_DOUBLE(Timestamp1)
						CREATE_NNF_CHAR(Timestamp2)
						CREATE_NNF_CHAR(ModifiedCancelledBy)
						CREATE_NNF_SHORT(ReasonCode)
						CREATE_NNF_LONG(TokenNo)
						INCLUDE_STRUCT(CONTRACT_DESC_TR, ContractDesc)
						CREATE_NNF_CHAR(CloseoutFlag)
						CREATE_NNF_DOUBLE(OrderNumber)
						CREATE_NNF_STRING(AccountNumber, 10)
						CREATE_NNF_SHORT(BookType)
						CREATE_NNF_SHORT(BuySellIndicator)
						CREATE_NNF_LONG(DisclosedVolume)
						CREATE_NNF_LONG(DisclosedVolumeRemaining)
						CREATE_NNF_LONG(TotalVolumeRemaining)
						CREATE_NNF_LONG(Volume)
						CREATE_NNF_LONG(VolumeFilledToday)
						CREATE_NNF_LONG(Price)
						CREATE_NNF_LONG(GoodTillDate)
						CREATE_NNF_LONG(EntryDateTime)
						CREATE_NNF_LONG(LastModified)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags)
						CREATE_NNF_SHORT(BranchId)
						CREATE_NNF_LONG(TraderId)
						CREATE_NNF_STRING(BrokerId, 5)
						CREATE_NNF_CHAR(OpenClose)
						CREATE_NNF_STRING(Settlor, 12)
						CREATE_NNF_SHORT(ProClientIndicator)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS_TR, AdditionalOrderFlags)
						CREATE_NNF_LONG(ClOrdId)
						CREATE_NNF_DOUBLE(NnfField)
						CREATE_NNF_LONG_LONG(TimeStamp)
						CREATE_NNF_STRING(PAN, 10)
						CREATE_NNF_LONG(AlgoID)
						//CREATE_NNF_SHORT   ( AlgoCategory                   )       
						CREATE_NNF_SHORT(Reserved1)
						CREATE_NNF_LONG_LONG(LastActivityReference)
						CREATE_NNF_STRING(Reserved2, 52)
						//errorcode
						void getString(std::string& str)
					{

						str.append(std::to_string(getTransCode()));
						str.append(",");
						//str.append(std::to_string(getLogTime()));
						//str.append(",");
						str.append(std::to_string(getUserID()));
						str.append(",");
						str.append(std::to_string(getErrorCode()));
						str.append(",");
						str.append(std::to_string(getTimestamp1()));
						str.append(",");
						str.append(std::to_string(getTimestamp2()));
						str.append(",");
						str.append(std::to_string(getModifiedCancelledBy()));
						str.append(",");
						str.append(std::to_string(getReasonCode()));
						str.append(",");
						str.append(std::to_string(getTokenNo()));
						str.append(",");
						str.append(std::to_string(getCloseoutFlag()));
						str.append(",");
						str.append(std::to_string(getOrderNumber()));
						str.append(",");
						str.append(getAccountNumber());
						str.append(",");
						str.append(std::to_string(getBookType()));
						str.append(",");
						str.append(std::to_string(getBuySellIndicator()));
						str.append(",");
						str.append(std::to_string(getDisclosedVolume()));
						str.append(",");
						str.append(std::to_string(getDisclosedVolumeRemaining()));
						str.append(",");
						str.append(std::to_string(getVolume()));
						str.append(",");
						str.append(std::to_string(getVolumeFilledToday()));
						str.append(",");
						str.append(std::to_string(getPrice()));
						str.append(",");
						str.append(std::to_string(getGoodTillDate()));
						str.append(",");
						str.append(std::to_string(getEntryDateTime()));
						str.append(",");
						str.append(std::to_string(getLastModified()));
						str.append(",");
						str.append(std::to_string(getBranchId()));
						str.append(",");
						str.append(std::to_string(getTraderId()));
						str.append(",");
						str.append(getBrokerId());
						str.append(",");
						str.append(std::to_string(getOpenClose()));
						str.append(",");
						str.append(getSettlor());
						str.append(",");
						str.append(std::to_string(getProClientIndicator()));
						str.append(",");
						str.append(std::to_string(getClOrdId()));
						str.append(",");
						str.append(std::to_string(getNnfField()));
						str.append(",");
						str.append(std::to_string(getTimeStamp()));
						str.append(",");
						str.append(getPAN());
						str.append(",");
						str.append(std::to_string(getAlgoID()));
						str.append(",");
						str.append(std::to_string(getReserved1()));
						str.append(",");
						str.append(std::to_string(getLastActivityReference()));
						str.append(",");
						str.append(getReserved2());

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
						setTransCode(static_cast<short>(std::stoi(result[0])));
						// setLogTime(std::stol(result[1]));
						setUserID(std::stol(result[1]));
						setErrorCode(static_cast<short>(std::stoi(result[2])));
						setTimestamp1(std::stod(result[3]));
						setTimestamp2(static_cast<char>(std::stoi(result[4])));
						setModifiedCancelledBy(static_cast<char>(std::stoi(result[5])));
						setReasonCode(static_cast<short>(std::stoi(result[6])));
						setTokenNo(std::stol(result[7]));
						setCloseoutFlag(static_cast<char>(std::stoi(result[8])));
						setOrderNumber(std::stod(result[9]));
						setAccountNumber(result[10]);
						setBookType(static_cast<short>(std::stoi(result[11])));
						setBuySellIndicator(static_cast<short>(std::stoi(result[12])));
						setDisclosedVolume(std::stol(result[13]));
						setDisclosedVolumeRemaining(std::stol(result[14]));
						setTotalVolumeRemaining(std::stol(result[15]));
						setVolume(std::stol(result[16]));
						setVolumeFilledToday(std::stol(result[17]));
						setPrice(std::stol(result[18]));
						setGoodTillDate(std::stol(result[19]));
						setEntryDateTime(std::stol(result[20]));
						setLastModified(std::stol(result[21]));
						setBranchId(static_cast<short>(std::stoi(result[22])));
						setTraderId(std::stol(result[23]));
						setBrokerId(result[24]);
						setOpenClose(static_cast<char>(std::stoi(result[25])));
						setSettlor(result[26]);
						setProClientIndicator(static_cast<short>(std::stoi(result[27])));
						setClOrdId(std::stol(result[28]));
						setNnfField(std::stod(result[29]));
						setTimeStamp(std::stoll(result[30]));
						setPAN(result[31]);
						setAlgoID(std::stol(result[32]));
						setReserved1(static_cast<short>(std::stoi(result[33])));
						setLastActivityReference(std::stoll(result[34]));
						setReserved2(result[35]);


						print();
					}
					void print() const
					{
						PRINT_BODY_START(MS_OE_RESPONSE_TR)
							PRINT_NNF_FIELD(TransCode)
							//DEVLOG ( "LogTime: " << getLogTimeAsUtc() ) ;
							PRINT_NNF_FIELD(UserID)
							PRINT_NNF_FIELD(ErrorCode)
							PRINT_NNF_FIELD(Timestamp1)
							PRINT_NNF_FIELD(Timestamp2)
							PRINT_NNF_FIELD(ModifiedCancelledBy)
							PRINT_NNF_FIELD(ReasonCode)
							PRINT_NNF_FIELD(TokenNo)
							_ContractDesc.print();
						PRINT_NNF_FIELD(CloseoutFlag)
							PRINT_NNF_FIELD(OrderNumber)
							PRINT_NNF_FIELD(AccountNumber)
							PRINT_NNF_FIELD(BookType)
							PRINT_NNF_FIELD(BuySellIndicator)
							PRINT_NNF_FIELD(DisclosedVolume)
							PRINT_NNF_FIELD(DisclosedVolumeRemaining)
							PRINT_NNF_FIELD(TotalVolumeRemaining)
							PRINT_NNF_FIELD(Volume)
							PRINT_NNF_FIELD(VolumeFilledToday)
							PRINT_NNF_FIELD(Price)
							PRINT_NNF_FIELD(GoodTillDate)
							PRINT_NNF_FIELD(EntryDateTime)
							PRINT_NNF_FIELD(LastModified)
							_StOrderFlags.print();
						PRINT_NNF_FIELD(BranchId)
							PRINT_NNF_FIELD(TraderId)
							PRINT_NNF_FIELD(BrokerId)
							PRINT_NNF_FIELD(OpenClose)
							PRINT_NNF_FIELD(Settlor)
							PRINT_NNF_FIELD(ProClientIndicator)
							_AdditionalOrderFlags.print();
						PRINT_NNF_FIELD(ClOrdId)
							PRINT_NNF_FIELD(NnfField)
							PRINT_NNF_FIELD(TimeStamp)
							//PRINT_NNF_FIELD( Reserved ) 
							std::cout << "ExchangeOrderId for ClOrdId: " << getClOrdId() << " is " << (int64_t)getOrderNumber() << std::endl;
						PRINT_NNF_FIELD(PAN)
							PRINT_NNF_FIELD(AlgoID)
							//   PRINT_NNF_FIELD( AlgoCategory )
							PRINT_NNF_FIELD(LastActivityReference)
							PRINT_BODY_END(MS_OE_RESPONSE_TR)
					}
				};

				struct ORDER_NEW_CONFIRMATION_TR : public MS_OE_RESPONSE_TR
				{

					void getString(std::string& str)
					{
						MS_OE_RESPONSE_TR::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_NEW_CONFIRMATION_TR)
							//MS_OE_RESPONSE_TR::print() ;
							MS_OE_RESPONSE_TR::fromString(str);
						PRINT_BODY_END(ORDER_NEW_CONFIRMATION_TR)
					}
				};

				struct ORDER_MOD_CONFIRMATION_TR : public MS_OE_RESPONSE_TR
				{

					void getString(std::string& str)
					{
						//str.append( MS_OE_RESPONSE_TR :: getString(str));
						MS_OE_RESPONSE_TR::getString(str);

					}
					void fromString(std::string& str)
					{
						print(str);
					}
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_MOD_CONFIRMATION_TR)
							//MS_OE_RESPONSE_TR::print() ;
							MS_OE_RESPONSE_TR::fromString(str);
						PRINT_BODY_END(ORDER_MOD_CONFIRMATION_TR)
					}
					// 20074
				};

				struct ORDER_CXL_CONFIRMATION_TR : public MS_OE_RESPONSE_TR
				{

					void getString(std::string& str)
					{
						//str.append(MS_OE_RESPONSE_TR :: getString(str));   
						MS_OE_RESPONSE_TR::getString(str);
					}

					void fromString(std::string& str)
					{
						print(str);
					}
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_CXL_CONFIRMATION_TR)
							//   MS_OE_RESPONSE_TR::print() ;
							MS_OE_RESPONSE_TR::fromString(str);
						PRINT_BODY_END(ORDER_CXL_CONFIRMATION_TR)
					}
					// 20075
				};

#pragma pack ( pop )
#pragma pack ( push, 2)
				struct MS_TRADE_CONFIRM :public MESSAGE_HEADER
				{
					CREATE_NNF_DOUBLE(ResponseOrderNumber)
						CREATE_NNF_STRING(BrokerId, 5)
						CREATE_NNF_CHAR(Reserved1)
						CREATE_NNF_LONG(TraderNumber)
						CREATE_NNF_STRING(AccountNumber, 10)
						CREATE_NNF_SHORT(BuySellIndicator)
						CREATE_NNF_LONG(OriginalVolume)
						CREATE_NNF_LONG(DisclosedVolume)
						CREATE_NNF_LONG(RemainingVolume)
						CREATE_NNF_LONG(DisclosedVolumeRemaining)
						CREATE_NNF_LONG(Price)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags)

						CREATE_NNF_LONG(GoodTillDate)
						CREATE_NNF_LONG(FillNumber)
						CREATE_NNF_LONG(FillQuantity)
						CREATE_NNF_LONG(FillPrice)
						CREATE_NNF_LONG(VolumeFilledToday)
						CREATE_NNF_STRING(ActivityType, 2)
						CREATE_NNF_LONG(ActivityTime)
						CREATE_NNF_DOUBLE(CounterTraderOrderNumber)
						CREATE_NNF_STRING(CounterBrokerId, 5)
						CREATE_NNF_LONG(Token)
						INCLUDE_STRUCT(CONTRACT_DESC, ContractDesc)
						CREATE_NNF_CHAR(OpenClose)
						CREATE_NNF_CHAR(OldOpenClose)
						CREATE_NNF_CHAR(BookType)
						CREATE_NNF_LONG(Reserved2)
						CREATE_NNF_STRING(OldAccountNumber, 10)
						CREATE_NNF_STRING(Participant, 12)
						CREATE_NNF_STRING(OldParticipant, 12)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS, AdditionalOrderFlags)
						CREATE_NNF_CHAR(Reserved3)
						CREATE_NNF_CHAR(Reserved4)
						CREATE_NNF_CHAR(Reserved5)
						CREATE_NNF_CHAR(ReservedFiller2)
						CREATE_NNF_STRING(PAN, 10)
						CREATE_NNF_STRING(OldPAN, 10)
						CREATE_NNF_LONG(AlgoID)
						CREATE_NNF_SHORT(Reserved6)
						CREATE_NNF_LONG_LONG(LastActivityReference)
						CREATE_NNF_STRING(Reserved7, 52)


						void getString(std::string& str)
					{
						str.append(std::to_string(getResponseOrderNumber()));  	str.append(",");
						str.append((getBrokerId()));  	str.append(",");
						str.append(std::to_string(getReserved1()));  	str.append(",");
						str.append(std::to_string(getTraderNumber()));  	str.append(",");
						str.append((getAccountNumber()));		str.append(",");
						str.append(std::to_string(getBuySellIndicator()));		str.append(",");
						str.append(std::to_string(getOriginalVolume()));  	str.append(",");
						str.append(std::to_string(getDisclosedVolume()));  	str.append(",");
						str.append(std::to_string(getRemainingVolume()));  	str.append(",");
						str.append(std::to_string(getDisclosedVolumeRemaining()));  	str.append(",");
						str.append(std::to_string(getPrice()));		str.append(",");
						getStOrderFlags().getString(str);						str.append(",");
						str.append(std::to_string(getGoodTillDate())); 	str.append(",");
						str.append(std::to_string(getFillNumber()));		str.append(",");
						str.append(std::to_string(getFillQuantity()));		str.append(",");
						str.append(std::to_string(getFillPrice()));			str.append(",");
						str.append(std::to_string(getVolumeFilledToday())); 		str.append(",");
						str.append((getActivityType()));		str.append(",");
						str.append(std::to_string(getActivityTime()));		str.append(",");
						str.append(std::to_string(getCounterTraderOrderNumber()));		str.append(",");
						str.append((getCounterBrokerId()));		str.append(",");
						str.append(std::to_string(getToken()));			str.append(",");
						getContractDesc().getString(str);						str.append(",");
						str.append(std::to_string(getOpenClose()));			str.append(",");
						str.append(std::to_string(getOldOpenClose()));		str.append(",");
						str.append(std::to_string(getBookType()));		str.append(",");
						str.append(std::to_string(getReserved2()));		str.append(",");
						str.append((getOldAccountNumber()));		str.append(",");
						str.append((getParticipant()));		str.append(",");
						str.append((getOldParticipant()));		str.append(",");
						getAdditionalOrderFlags().getString(str);						str.append(",");
						str.append(std::to_string(getReserved3()));  	str.append(",");
						str.append(std::to_string(getReserved4()));		str.append(",");
						str.append(std::to_string(getReserved5()));		str.append(",");
						str.append(std::to_string(getReservedFiller2()));		str.append(",");
						str.append((getPAN()));		str.append(",");
						str.append((getOldPAN()));		str.append(",");
						str.append(std::to_string(getAlgoID()));		str.append(",");
						str.append(std::to_string(getReserved6()));		str.append(",");
						str.append(std::to_string(getLastActivityReference()));		str.append(",");
						str.append((getReserved7()));     str.append(",");
					}
					void print() const
					{
						DEVLOG("PLEASE WRITE CODE TO PRINT");
					}
				};


#pragma pack ( pop )
#pragma pack ( push, 2)
				struct MS_TRADE_CONFIRM_TR
				{
					CREATE_NNF_SHORT(TransCode)
						CREATE_NNF_TIME(LogTime)
						CREATE_NNF_LONG(UserID)
						CREATE_NNF_LONG_LONG(TimeStamp)
						CREATE_NNF_DOUBLE(Timestamp1)
						CREATE_NNF_DOUBLE(Timestamp2)
						CREATE_NNF_DOUBLE(ResponseOrderNumber)
						CREATE_NNF_STRING(BrokerId, 5)
						CREATE_NNF_CHAR(Reserved)
						CREATE_NNF_STRING(AccountNumber, 10)
						CREATE_NNF_SHORT(BuySellIndicator)
						CREATE_NNF_LONG(OriginalVolume)
						CREATE_NNF_LONG(DisclosedVolume)
						CREATE_NNF_LONG(RemainingVolume)
						CREATE_NNF_LONG(DisclosedVolumeRemaining)
						CREATE_NNF_LONG(Price)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags)
						CREATE_NNF_LONG(GoodTillDate)
						CREATE_NNF_LONG(FillNumber)
						CREATE_NNF_LONG(FillQty)
						CREATE_NNF_LONG(FillPrice)
						CREATE_NNF_LONG(VolumeFilledToday)
						CREATE_NNF_STRING(ActivityType, 2)
						CREATE_NNF_LONG(ActivityTime)
						CREATE_NNF_LONG(TokenNo)
						INCLUDE_STRUCT(CONTRACT_DESC_TR, ContractDesc)
						CREATE_NNF_CHAR(OpenClose)
						CREATE_NNF_CHAR(BookType)
						CREATE_NNF_STRING(Participant, 12)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS_TR, AdditionalOrderFlags)
						CREATE_NNF_STRING(PAN, 10)
						CREATE_NNF_LONG(AlgoID)
						CREATE_NNF_SHORT(Reserved1)
						CREATE_NNF_LONG_LONG(LastActivityReference)
						CREATE_NNF_STRING(Reserved2, 52)
						//errorcode
						void getString(std::string& str)
					{
						str.append(std::to_string(getTransCode())); str.append(",");
						str.append(std::to_string(getLogTimeAsUtc())); str.append(",");
						str.append(std::to_string(getUserID())); str.append(",");
						str.append(std::to_string(getTimeStamp())); str.append(",");
						str.append(std::to_string(getTimestamp1())); str.append(",");
						str.append(std::to_string(getTimestamp2())); str.append(",");
						str.append(std::to_string(getResponseOrderNumber())); str.append(",");
						str.append(getBrokerId()); str.append(",");
						str.append(std::to_string(getReserved())); str.append(",");
						str.append(getAccountNumber()); str.append(",");
						str.append(std::to_string(getBuySellIndicator())); str.append(",");
						str.append(std::to_string(getOriginalVolume())); str.append(",");
						str.append(std::to_string(getDisclosedVolume())); str.append(",");
						str.append(std::to_string(getRemainingVolume())); str.append(",");
						str.append(std::to_string(getDisclosedVolumeRemaining())); str.append(",");
						str.append(std::to_string(getPrice())); str.append(",");
						getStOrderFlags().getString(str); str.append(",");
						str.append(std::to_string(getGoodTillDate())); str.append(",");
						str.append(std::to_string(getFillNumber())); str.append(",");
						str.append(std::to_string(getFillQty())); str.append(",");
						str.append(std::to_string(getFillPrice())); str.append(",");
						str.append(std::to_string(getVolumeFilledToday())); str.append(",");
						str.append(getActivityType()); str.append(",");
						str.append(std::to_string(getActivityTime())); str.append(",");
						str.append(std::to_string(getTokenNo())); str.append(",");
						getContractDesc().getString(str); str.append(",");
						str.append(std::to_string(getOpenClose())); str.append(",");
						str.append(std::to_string(getBookType())); str.append(",");
						str.append(getParticipant()); str.append(",");
						getAdditionalOrderFlags().getString(str); str.append(",");
						str.append(getPAN()); str.append(",");
						str.append(std::to_string(getAlgoID())); str.append(",");
						str.append(std::to_string(getReserved1())); str.append(",");
						str.append(std::to_string(getLastActivityReference())); str.append(",");
						str.append(getReserved2()); str.append(",");

					}
					void print() const
					{
						PRINT_BODY_START(MS_TRADE_CONFIRM_TR)

							PRINT_NNF_FIELD(TransCode)
							std::cout << "[" << std::this_thread::get_id() << "]" << "(" << __FILE__ << ":" << __LINE__ << "): LogTime: " << getLogTimeAsUtc() << std::endl;
						PRINT_NNF_FIELD(UserID)
							PRINT_NNF_FIELD(TimeStamp)
							PRINT_NNF_FIELD(Timestamp1)
							PRINT_NNF_FIELD(Timestamp2)
							PRINT_NNF_FIELD(ResponseOrderNumber)
							std::cout << "ResponseOrderId: " << " is " << (int64_t)getResponseOrderNumber() << std::endl;
						PRINT_NNF_FIELD(BrokerId)
							PRINT_NNF_FIELD(Reserved)
							PRINT_NNF_FIELD(AccountNumber)
							PRINT_NNF_FIELD(BuySellIndicator)
							PRINT_NNF_FIELD(OriginalVolume)
							PRINT_NNF_FIELD(DisclosedVolume)
							PRINT_NNF_FIELD(RemainingVolume)
							PRINT_NNF_FIELD(DisclosedVolumeRemaining)
							PRINT_NNF_FIELD(Price)
							_StOrderFlags.print();
						PRINT_NNF_FIELD(GoodTillDate)
							PRINT_NNF_FIELD(FillNumber)
							PRINT_NNF_FIELD(FillQty)
							PRINT_NNF_FIELD(FillPrice)
							PRINT_NNF_FIELD(VolumeFilledToday)
							PRINT_NNF_FIELD(ActivityType)
							PRINT_NNF_FIELD(ActivityTime)
							PRINT_NNF_FIELD(TokenNo)
							_ContractDesc.print();
						PRINT_NNF_FIELD(OpenClose)
							PRINT_NNF_FIELD(BookType)
							PRINT_NNF_FIELD(Participant)
							_AdditionalOrderFlags.print();
						PRINT_NNF_FIELD(PAN)
							PRINT_NNF_FIELD(AlgoID)
							PRINT_NNF_FIELD(LastActivityReference)


							PRINT_BODY_END(MS_TRADE_CONFIRM_TR)
					}
				};
#pragma pack ( pop )
#pragma pack ( push, 2)

				struct MS_OM_REQUEST_TR //: public DC_PACKET_WRAPPER
				{
						CREATE_NNF_SHORT(TransCode)
						CREATE_NNF_LONG(UserID)
						CREATE_NNF_CHAR(ModifiedCancelledBy)
						CREATE_NNF_LONG(TokenNo)
						INCLUDE_STRUCT(CONTRACT_DESC_TR, ContractDesc)
						CREATE_NNF_DOUBLE(OrderNumber)
						CREATE_NNF_STRING(AccountNumber, 10)
						CREATE_NNF_SHORT(BookType)
						CREATE_NNF_SHORT(BuySellIndicator)
						CREATE_NNF_LONG(DisclosedVolume)
						CREATE_NNF_LONG(DisclosedVolumeRemaining)
						CREATE_NNF_LONG(TotalVolumeRemaining)
						CREATE_NNF_LONG(Volume)
						CREATE_NNF_LONG(VolumeFilledToday)
						CREATE_NNF_LONG(Price)
						CREATE_NNF_LONG(GoodTillDate)
						CREATE_NNF_LONG(EntryDateTime)
						CREATE_NNF_LONG(LastModified)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags)
						CREATE_NNF_SHORT(BranchId)
						CREATE_NNF_LONG(TraderId)
						CREATE_NNF_STRING(BrokerId, 5)
						CREATE_NNF_CHAR(OpenClose)
						CREATE_NNF_STRING(Settlor, 12)
						CREATE_NNF_SHORT(ProClientIndicator)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS_TR, AdditionalOrderFlags)
						CREATE_NNF_LONG(ClOrdId)
						CREATE_NNF_DOUBLE(NnfField)
						CREATE_NNF_STRING(PAN, 10)
						CREATE_NNF_LONG(AlgoID)
						CREATE_NNF_SHORT(Reserved1)
						CREATE_NNF_LONG_LONG(LastActivityReference)
						CREATE_NNF_STRING(Reserved, 24)

						MS_OM_REQUEST_TR(TransCode tc, int32_t traderID)
					{
						setUserID(traderID);
						setTransCode(tc);
						memset(_AccountNumber, ' ', 10);
						memset(_BrokerId, ' ', 5);
						memset(_Settlor, ' ', 12);
						memset(_PAN, ' ', 10);

						setBookType(1);
						setOpenClose('O');

						setTraderId(traderID);
						setAlgoID(0);
						setReserved1(0);
						//setAlgoCategory(0) ;
					}

					void getString(std::string& str)
					{

						str.append(std::to_string(getTransCode()));
						str.append(",");
						str.append(std::to_string(getUserID()));
						str.append(",");
						str.append(std::to_string(getModifiedCancelledBy()));
						str.append(",");
						str.append(std::to_string(getTokenNo()));
						str.append(",");
						str.append(std::to_string(getOrderNumber()));
						str.append(",");
						str.append(getAccountNumber());
						str.append(",");
						str.append(std::to_string(getBookType()));
						str.append(",");
						str.append(std::to_string(getBuySellIndicator()));
						str.append(",");
						str.append(std::to_string(getDisclosedVolume()));
						str.append(",");
						str.append(std::to_string(getDisclosedVolumeRemaining()));
						str.append(",");
						str.append(std::to_string(getTotalVolumeRemaining()));
						str.append(",");
						str.append(std::to_string(getVolume()));
						str.append(",");
						str.append(std::to_string(getVolumeFilledToday()));
						str.append(",");
						str.append(std::to_string(getPrice()));
						str.append(",");
						str.append(std::to_string(getGoodTillDate()));
						str.append(",");
						str.append(std::to_string(getEntryDateTime()));
						str.append(",");
						str.append(std::to_string(getLastModified()));
						str.append(",");
						str.append(std::to_string(getBranchId()));
						str.append(",");
						str.append(std::to_string(getTraderId()));
						str.append(",");
						str.append(getBrokerId());
						str.append(",");
						str.append(std::to_string(getOpenClose()));
						str.append(",");
						str.append(getSettlor());
						str.append(",");
						str.append(std::to_string(getProClientIndicator()));
						str.append(",");
						str.append(std::to_string(getClOrdId()));
						str.append(",");
						str.append(std::to_string(getNnfField()));
						str.append(",");
						str.append(getPAN());
						str.append(",");
						str.append(std::to_string(getAlgoID()));
						str.append(",");
						str.append(std::to_string(getReserved1()));
						str.append(",");
						str.append(std::to_string(getLastActivityReference()));
						str.append(",");
						str.append(getReserved());

					}

					void getStringWithValue(std::string& str)
					{
						str.append("TransCode: ").append(std::to_string(getTransCode()));
						str.append(", UserID: ").append(std::to_string(getUserID()));
						str.append(", ModifiedCancelledBy: ").append(std::to_string(getModifiedCancelledBy()));
						str.append(", TokenNo: ").append(std::to_string(getTokenNo()));

						str.append(", OrderNumber: ").append(std::to_string(getOrderNumber()));
						str.append(", AccountNumber: ").append(getAccountNumber());
						str.append(", BookType: ").append(std::to_string(getBookType()));
						str.append(", BuySellIndicator: ").append(std::to_string(getBuySellIndicator()));

						str.append(", DisclosedVolume: ").append(std::to_string(getDisclosedVolume()));
						str.append(", DisclosedVolumeRemaining: ").append(std::to_string(getDisclosedVolumeRemaining()));
						str.append(", TotalVolumeRemaining: ").append(std::to_string(getTotalVolumeRemaining()));
						str.append(", Volume: ").append(std::to_string(getVolume()));
						str.append(", VolumeFilledToday: ").append(std::to_string(getVolumeFilledToday()));

						str.append(", Price: ").append(std::to_string(getPrice()));
						str.append(", GoodTillDate: ").append(std::to_string(getGoodTillDate()));
						str.append(", EntryDateTime: ").append(std::to_string(getEntryDateTime()));
						str.append(", LastModified: ").append(std::to_string(getLastModified()));

						str.append(", BranchId: ").append(std::to_string(getBranchId()));
						str.append(", TraderId: ").append(std::to_string(getTraderId()));
						str.append(", BrokerId: ").append(getBrokerId());
						str.append(", OpenClose: ").append(std::to_string(getOpenClose()));

						str.append(", Settlor: ").append(getSettlor());
						str.append(", ProClientIndicator: ").append(std::to_string(getProClientIndicator()));

						str.append(", ClOrdId: ").append(std::to_string(getClOrdId()));
						str.append(", NnfField: ").append(std::to_string(getNnfField()));
						str.append(", PAN: ").append(getPAN());
						str.append(", AlgoID: ").append(std::to_string(getAlgoID()));
						str.append(", Reserved1: ").append(std::to_string(getReserved1()));
						str.append(", LastActivityReference: ").append(std::to_string(getLastActivityReference()));
						str.append(", Reserved: ").append(getReserved());

						str.append(", CONTRACT_DESC_TR: ");
						std::string val1;
						getContractDesc().getString(val1);
						str.append(val1);

						str.append(", ST_ORDER_FLAGS: ");
						std::string val2;
						getStOrderFlags().getString(val2);
						str.append(val2);

						str.append(", ADDITIONAL_ORDER_FLAGS_TR: ");
						std::string val3;
						getAdditionalOrderFlags().getString(val3);
						str.append(val3);
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
						setTransCode(static_cast<short>(std::stoi(result[0])));
						setUserID(std::stol(result[1]));
						setModifiedCancelledBy(static_cast<char>(std::stoi(result[2])));
						setTokenNo(std::stol(result[3]));
						setOrderNumber(std::stod(result[4]));
						setAccountNumber(result[5]);
						setBookType(static_cast<short>(std::stoi(result[6])));
						setBuySellIndicator(static_cast<short>(std::stoi(result[7])));
						setDisclosedVolume(std::stol(result[8]));
						setDisclosedVolumeRemaining(std::stol(result[9]));
						setTotalVolumeRemaining(std::stol(result[10]));
						setVolume(std::stol(result[11]));
						setVolumeFilledToday(std::stol(result[12]));
						setPrice(std::stol(result[13]));
						setGoodTillDate(std::stol(result[14]));
						setEntryDateTime(std::stol(result[15]));
						setLastModified(std::stol(result[16]));
						setBranchId(static_cast<short>(std::stoi(result[17])));
						setTraderId(std::stol(result[18]));
						setBrokerId(result[19]);
						setOpenClose(static_cast<char>(std::stoi(result[20])));
						setSettlor(result[21]);
						setProClientIndicator(static_cast<short>(std::stoi(result[22])));
						setClOrdId(std::stol(result[23]));
						setNnfField(std::stod(result[24]));
						setPAN(result[25]);
						setAlgoID(std::stol(result[26]));
						setReserved1(static_cast<short>(std::stoi(result[27])));
						setLastActivityReference(std::stoll(result[28]));
						setReserved(result[29]);

						print();
					}
					void reset()
					{
						_ContractDesc.reset();
						_StOrderFlags.reset();
						_AdditionalOrderFlags.reset();
						setTokenNo(0);
						memset(_AccountNumber, ' ', 10);
						setBuySellIndicator(0);
						setDisclosedVolume(0);
						setVolume(0);
						setPrice(0);
						setGoodTillDate(0);
						setProClientIndicator(0);
						memset(_Settlor, ' ', 12);
						memset(_PAN, ' ', 10);
						setClOrdId(0);
						setNnfField(0);
					}

					void print() const
					{
						PRINT_BODY_START(MS_OM_REQUEST_TR)
							PRINT_NNF_FIELD(TransCode)
							PRINT_NNF_FIELD(UserID)
							PRINT_NNF_FIELD(ModifiedCancelledBy)
							PRINT_NNF_FIELD(TokenNo)
							_ContractDesc.print();
						PRINT_NNF_FIELD(OrderNumber)
							PRINT_NNF_FIELD(AccountNumber)
							PRINT_NNF_FIELD(BookType)
							PRINT_NNF_FIELD(BuySellIndicator)
							PRINT_NNF_FIELD(DisclosedVolume)
							PRINT_NNF_FIELD(DisclosedVolumeRemaining)
							PRINT_NNF_FIELD(TotalVolumeRemaining)
							PRINT_NNF_FIELD(Volume)
							PRINT_NNF_FIELD(VolumeFilledToday)
							PRINT_NNF_FIELD(Price)
							PRINT_NNF_FIELD(GoodTillDate)
							PRINT_NNF_FIELD(EntryDateTime)
							PRINT_NNF_FIELD(LastModified)
							_StOrderFlags.print();
						PRINT_NNF_FIELD(BranchId)
							PRINT_NNF_FIELD(TraderId)
							PRINT_NNF_FIELD(BrokerId)
							PRINT_NNF_FIELD(OpenClose)
							PRINT_NNF_FIELD(Settlor)
							PRINT_NNF_FIELD(ProClientIndicator)
							_AdditionalOrderFlags.print();
						PRINT_NNF_FIELD(ClOrdId)
							PRINT_NNF_FIELD(NnfField)
							PRINT_NNF_FIELD(PAN)
							PRINT_NNF_FIELD(AlgoID)
							//PRINT_NNF_FIELD( AlgoCategory )
							PRINT_NNF_FIELD(LastActivityReference)
							PRINT_BODY_END(MS_OM_REQUEST_TR)
					}
				};

				struct ORDER_CANCEL_TR : public DC_PACKET_WRAPPER, public MS_OM_REQUEST_TR
				{
					// 20070
					ORDER_CANCEL_TR(int32_t traderID)
						: DC_PACKET_WRAPPER(sizeof(ORDER_CANCEL_TR)),
						MS_OM_REQUEST_TR(TransCode_ORDER_CANCEL_IN_TR, traderID)
					{
					}

					void getStringWithValue(std::string& str)
					{
						MS_OM_REQUEST_TR::getStringWithValue(str);
					}

					void getString(std::string& str)
					{
						//str.append(MS_OM_REQUEST_TR ::getString(str));    
						MS_OM_REQUEST_TR::getString(str);
					}
					void reset()
					{
						DC_PACKET_WRAPPER::reset();
						MS_OM_REQUEST_TR::reset();
					}
					void fromString(std::string& str)
					{
						print(str);
					}
					//void print(std::string & str) const
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_CANCEL_TR)
							//MS_OM_REQUEST_TR::print() ;
							MS_OM_REQUEST_TR::fromString(str);
						PRINT_BODY_END(ORDER_CANCEL_TR)
					}
				};

				// 20040
				struct ORDER_MODIFY_TR : public DC_PACKET_WRAPPER, public MS_OM_REQUEST_TR
				{
					ORDER_MODIFY_TR(int32_t traderID)
						: DC_PACKET_WRAPPER(sizeof(ORDER_MODIFY_TR)),
						MS_OM_REQUEST_TR(TransCode_ORDER_MOD_IN_TR, traderID)
					{
						getStOrderFlags().setModified(true);
					}

					void getStringWithValue(std::string& str)
					{

						MS_OM_REQUEST_TR::getStringWithValue(str);

					}

					void getString(std::string& str)
					{

						MS_OM_REQUEST_TR::getString(str);

					}
					void reset()
					{
						DC_PACKET_WRAPPER::reset();
						MS_OM_REQUEST_TR::reset();
						getStOrderFlags().setModified(true);
					}

					void fromString(std::string& str)
					{
						print(str);
					}
					void print(std::string& str)
					{
						PRINT_BODY_START(ORDER_MODIFY_TR)
							//MS_OM_REQUEST_TR::print() ;
							MS_OM_REQUEST_TR::fromString(str);
						PRINT_BODY_END(ORDER_MODIFY_TR)
					}
				};

				struct ORDER_QUICK_CANCEL_TR : public DC_PACKET_WRAPPER, public MS_OM_REQUEST_TR
				{
					// 20060
				};


				struct MS_SPD_LEG_INFO
				{
					CREATE_NNF_LONG(Token)
						INCLUDE_STRUCT(CONTRACT_DESC, ContractDesc)
						CREATE_NNF_STRING(OpBrokerId, 5)
						CREATE_NNF_CHAR(FillerX)
						CREATE_NNF_SHORT(OrderType)
						CREATE_NNF_SHORT(BuySell)
						CREATE_NNF_LONG(DisclosedVol)
						CREATE_NNF_LONG(DisclosedVolRemaining)
						CREATE_NNF_LONG(TotalVolRemaining)
						CREATE_NNF_LONG(Volume)
						CREATE_NNF_LONG(VolumeFilledToday)
						CREATE_NNF_LONG(Price)
						CREATE_NNF_LONG(TriggerPrice)
						CREATE_NNF_LONG(MinFillAon)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags)
						CREATE_NNF_CHAR(OpenClose)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS_TR, AdditionalOrderFlags)
						CREATE_NNF_CHAR(GiveupFlag)
						CREATE_NNF_CHAR(FillerY)

						MS_SPD_LEG_INFO()
					{
						memset(_OpBrokerId, ' ', 5);

						setFillerX(' ');
						setFillerY(' ');
						setOpenClose('O');
						setGiveupFlag('P');
					}
					void getString(std::string& str)
					{

						str.append(std::to_string(getToken()));
						str.append(",");
						str.append(getOpBrokerId());
						str.append(",");
						str.append(std::to_string(getFillerX()));
						str.append(",");
						str.append(std::to_string(getOrderType()));
						str.append(",");
						str.append(std::to_string(getBuySell()));
						str.append(",");
						str.append(std::to_string(getDisclosedVol()));
						str.append(",");
						str.append(std::to_string(getDisclosedVolRemaining()));
						str.append(",");
						str.append(std::to_string(getVolume()));
						str.append(",");
						str.append(std::to_string(getVolumeFilledToday()));
						str.append(",");
						str.append(std::to_string(getPrice()));
						str.append(",");
						str.append(std::to_string(getTriggerPrice()));
						str.append(",");
						str.append(std::to_string(getMinFillAon()));
						str.append(",");
						str.append(std::to_string(getOpenClose()));
						str.append(",");
						str.append(std::to_string(getGiveupFlag()));
						str.append(",");
						str.append(std::to_string(getFillerY()));

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


						setToken(std::stol(result[0]));
						setOpBrokerId(result[1]);

						if (isSafeCharConvert(std::stoi(result[2])))
							setFillerX(static_cast<char>(std::stoi(result[2])));

						if (isSafeShortConvert(std::stoi(result[3])))
							setOrderType(static_cast<short>(std::stoi(result[3])));

						if (isSafeShortConvert(std::stoi(result[4])))
							setBuySell(static_cast<short>(std::stoi(result[4])));

						setDisclosedVol(std::stol(result[5]));
						setDisclosedVolRemaining(std::stol(result[6]));
						setTotalVolRemaining(std::stol(result[7]));
						setVolume(std::stol(result[8]));
						setVolumeFilledToday(std::stol(result[9]));
						setPrice(std::stol(result[10]));
						setTriggerPrice(std::stol(result[11]));
						setMinFillAon(std::stol(result[12]));

						if (isSafeCharConvert(std::stoi(result[13])))
							setOpenClose(static_cast<char>(std::stoi(result[13])));

						if (isSafeCharConvert(std::stoi(result[14])))
							setGiveupFlag(static_cast<char>(std::stoi(result[14])));

						if (isSafeCharConvert(std::stoi(result[15])))
							setFillerY(static_cast<char>(std::stoi(result[15])));

						print();
					}
					void print() const
					{
						PRINT_BODY_START(MS_SPD_LEG_INFO)
							PRINT_NNF_FIELD(Token)
							_ContractDesc.print();
						PRINT_NNF_FIELD(OpBrokerId)
							PRINT_NNF_FIELD(OrderType)
							PRINT_NNF_FIELD(BuySell)
							PRINT_NNF_FIELD(DisclosedVol)
							PRINT_NNF_FIELD(DisclosedVolRemaining)
							PRINT_NNF_FIELD(TotalVolRemaining)
							PRINT_NNF_FIELD(Volume)
							PRINT_NNF_FIELD(VolumeFilledToday)
							PRINT_NNF_FIELD(Price)
							PRINT_NNF_FIELD(TriggerPrice)
							PRINT_NNF_FIELD(MinFillAon)
							_StOrderFlags.print();
						PRINT_NNF_FIELD(OpenClose)
							_AdditionalOrderFlags.print();
						PRINT_NNF_FIELD(GiveupFlag)
							PRINT_BODY_END(MS_SPD_LEG_INFO)
					}
				};

				struct MS_SPD_OE_REQUEST : public MESSAGE_HEADER
				{

					CREATE_NNF_CHAR(ParticipantType1)
						CREATE_NNF_CHAR(Filler1)
						CREATE_NNF_SHORT(CompetitorPeriod1)
						CREATE_NNF_SHORT(SolicitorPeriod1)
						CREATE_NNF_CHAR(ModCxlBy1)
						CREATE_NNF_CHAR(Filler9)
						CREATE_NNF_SHORT(ReasonCode1)
						CREATE_NNF_STRING(StartAlpha1, 2)
						CREATE_NNF_STRING(EndAlpha1, 2)
						CREATE_NNF_LONG(Token1)
						INCLUDE_STRUCT(CONTRACT_DESC, ContractDesc1)
						CREATE_NNF_STRING(OpBrokerId1, 5)
						CREATE_NNF_CHAR(Fillerx1)
						CREATE_NNF_STRING(FillerOptions1, 3)
						CREATE_NNF_CHAR(Fillery1)
						CREATE_NNF_SHORT(OrderType1)
						CREATE_NNF_DOUBLE(OrderNumber1)
						CREATE_NNF_STRING(AccountNumber1, 10)
						CREATE_NNF_SHORT(BookType1)
						CREATE_NNF_SHORT(BuySell1)
						CREATE_NNF_LONG(DisclosedVol1)
						CREATE_NNF_LONG(DisclosedVolRemaining1)
						CREATE_NNF_LONG(TotalVolRemaining1)
						CREATE_NNF_LONG(Volume1)
						CREATE_NNF_LONG(VolumeFilledToday1)
						CREATE_NNF_LONG(Price1)
						CREATE_NNF_LONG(TriggerPrice1)
						CREATE_NNF_LONG(GoodTillDate1)
						CREATE_NNF_LONG(EntryDateTime1)
						CREATE_NNF_LONG(MinFillAon1)
						CREATE_NNF_LONG(LastModified1)
						INCLUDE_STRUCT(ST_ORDER_FLAGS, StOrderFlags1)
						CREATE_NNF_SHORT(BranchId1)
						CREATE_NNF_LONG(TraderId1)
						CREATE_NNF_STRING(BrokerId1, 5)
						CREATE_NNF_STRING(cOrdFiller, 24)
						CREATE_NNF_CHAR(OpenClose1)
						CREATE_NNF_STRING(Settlor1, 12)
						CREATE_NNF_SHORT(ProClient1)
						CREATE_NNF_SHORT(SettlementPeriod1)
						INCLUDE_STRUCT(ADDITIONAL_ORDER_FLAGS_TR, AdditionalOrderFlags1)
						CREATE_NNF_CHAR(GiveupFlag1)
						CREATE_NNF_STRING(Filler, 4)
						CREATE_NNF_DOUBLE(NnfField)
						CREATE_NNF_DOUBLE(MktReplay)
						CREATE_NNF_STRING(PAN, 10)
						CREATE_NNF_LONG(AlgoID)
						CREATE_NNF_SHORT(AlgoCategory)
						CREATE_NNF_STRING(Reserved, 60)
						CREATE_NNF_LONG(PriceDiff)
						INCLUDE_STRUCT(MS_SPD_LEG_INFO, Leg2)
						INCLUDE_STRUCT(MS_SPD_LEG_INFO, Leg3)

						MS_SPD_OE_REQUEST(SIGNED_NNF_LONG traderID, TransCode t)
						: MESSAGE_HEADER(t, traderID, sizeof(MS_SPD_OE_REQUEST))
					{
						_Filler1 = ' ';
						_ModCxlBy1 = ' ';
						_Filler9 = ' ';
						_Fillerx1 = ' ';
						_Fillery1 = ' ';
						_OpenClose1 = ' ';
						_GiveupFlag1 = 'P';
						_ParticipantType1 = ' ';
						memset(_StartAlpha1, ' ', 2);
						memset(_EndAlpha1, ' ', 2);
						memset(_OpBrokerId1, ' ', 5);
						memset(_FillerOptions1, ' ', 3);
						memset(_AccountNumber1, ' ', 10);
						memset(_BrokerId1, ' ', 5);
						memset(_cOrdFiller, ' ', 24);
						memset(_Settlor1, ' ', 12);
						memset(_Filler, ' ', 4);
						memset(_PAN, ' ', 10);
						memset(_Reserved, ' ', 60);

						setBookType1(1);
						setOpenClose1('O');

						setTraderId1(traderID);
						setBrokerId1("90012");
						setBranchId1(1);

						setAlgoID(0);
						setAlgoCategory(0);


						getStOrderFlags1().setIOC(true);
						getStOrderFlags1().setDay(true);

						getLeg2().getStOrderFlags().setIOC(true);
						getLeg2().getStOrderFlags().setDay(true);

						//getLeg3().getStOrderFlags().setIOC( true ) ;
						//getLeg3().getStOrderFlags().setDay( true ) ;
					}

					void getString(std::string& str)
					{

						//str.append(MESSAGE_HEADER ::getString(str));
						MESSAGE_HEADER::getString(str);
						str.append(",");
						str.append(std::to_string(getParticipantType1()));
						str.append(",");
						str.append(std::to_string(getFiller1()));
						str.append(",");
						str.append(std::to_string(getCompetitorPeriod1()));
						str.append(",");
						str.append(std::to_string(getSolicitorPeriod1()));
						str.append(",");
						str.append(std::to_string(getModCxlBy1()));
						str.append(",");
						str.append(std::to_string(getFiller9()));
						str.append(",");
						str.append(std::to_string(getReasonCode1()));
						str.append(",");
						str.append(getStartAlpha1());
						str.append(",");
						str.append(getEndAlpha1());
						str.append(",");
						str.append(std::to_string(getToken1()));
						str.append(",");
						str.append(getOpBrokerId1());
						str.append(",");
						str.append(std::to_string(getFillerx1()));
						str.append(",");
						str.append(getFillerOptions1());
						str.append(",");
						str.append(std::to_string(getFillery1()));
						str.append(",");
						str.append(std::to_string(getOrderType1()));
						str.append(",");
						str.append(std::to_string(getOrderNumber1()));
						str.append(",");
						str.append(getAccountNumber1());
						str.append(",");
						str.append(std::to_string(getBookType1()));
						str.append(",");
						str.append(std::to_string(getBuySell1()));
						str.append(",");
						str.append(std::to_string(getDisclosedVol1()));
						str.append(",");
						str.append(std::to_string(getDisclosedVolRemaining1()));
						str.append(",");
						str.append(std::to_string(getTotalVolRemaining1()));
						str.append(",");
						str.append(std::to_string(getVolume1()));
						str.append(",");
						str.append(std::to_string(getVolumeFilledToday1()));
						str.append(",");
						str.append(std::to_string(getPrice1()));
						str.append(",");
						str.append(std::to_string(getTriggerPrice1()));
						str.append(",");
						str.append(std::to_string(getGoodTillDate1()));
						str.append(",");
						str.append(std::to_string(getEntryDateTime1()));
						str.append(",");
						str.append(std::to_string(getMinFillAon1()));
						str.append(",");
						str.append(std::to_string(getLastModified1()));
						str.append(",");
						str.append(std::to_string(getBranchId1()));
						str.append(",");
						str.append(std::to_string(getTraderId1()));
						str.append(",");
						str.append(getBrokerId1());
						str.append(",");
						str.append(getcOrdFiller());
						str.append(",");
						str.append(std::to_string(getOpenClose1()));
						str.append(",");
						str.append(getSettlor1());
						str.append(",");
						str.append(std::to_string(getProClient1()));
						str.append(",");
						str.append(std::to_string(getSettlementPeriod1()));
						str.append(",");
						str.append(std::to_string(getGiveupFlag1()));
						str.append(",");
						str.append(getFiller());
						str.append(",");
						str.append(std::to_string(getNnfField()));
						str.append(",");
						str.append(std::to_string(getMktReplay()));
						str.append(",");
						str.append(getPAN());
						str.append(",");
						str.append(std::to_string(getAlgoID()));
						str.append(",");
						str.append(std::to_string(getAlgoCategory()));
						str.append(",");
						str.append(getReserved());
						str.append(",");
						str.append(std::to_string(getPriceDiff()));

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

						if (isSafeShortConvert(std::stoi(result[0])))
							setTransCode(static_cast<short>(std::stoi(result[0])));

						setLogTime(std::stol(result[1]));
						setAlphaChar(result[2]);
						setTraderId(std::stol(result[3]));

						if (isSafeShortConvert(std::stoi(result[4])))
							setErrorCode(static_cast<short>(std::stoi(result[4])));

						setTimestamp(result[5]);
						setTimestamp1(result[6]);
						setTimestamp2(result[7]);

						if (isSafeShortConvert(std::stoi(result[8])))
							setMessageLength(static_cast<short>(std::stoi(result[8])));

						if (isSafeCharConvert(std::stoi(result[9])))
							setParticipantType1(static_cast<char>(std::stoi(result[9])));

						if (isSafeCharConvert(std::stoi(result[10])))
							setFiller1(static_cast<char>(std::stoi(result[10])));

						if (isSafeShortConvert(std::stoi(result[11])))
							setCompetitorPeriod1(static_cast<short>(std::stoi(result[11])));

						if (isSafeShortConvert(std::stoi(result[12])))
							setSolicitorPeriod1(static_cast<short>(std::stoi(result[12])));

						if (isSafeCharConvert(std::stoi(result[13])))
							setModCxlBy1(static_cast<char>(std::stoi(result[13])));

						if (isSafeCharConvert(std::stoi(result[14])))
							setFiller9(static_cast<char>(std::stoi(result[14])));

						if (isSafeShortConvert(std::stoi(result[15])))
							setReasonCode1(static_cast<short>(std::stoi(result[15])));

						setStartAlpha1(result[16]);
						setEndAlpha1(result[17]);
						setToken1(std::stol(result[18]));
						setOpBrokerId1(result[19]);

						if (isSafeCharConvert(std::stoi(result[20])))
							setFillerx1(static_cast<char>(std::stoi(result[20])));

						setFillerOptions1(result[21]);

						if (isSafeCharConvert(std::stoi(result[22])))
							setFillery1(static_cast<char>(std::stoi(result[22])));

						if (isSafeShortConvert(std::stoi(result[23])))
							setOrderType1(static_cast<short>(std::stoi(result[23])));

						setOrderNumber1(std::stod(result[24]));
						setAccountNumber1(result[25]);

						if (isSafeShortConvert(std::stoi(result[26])))
							setBookType1(static_cast<short>(std::stoi(result[26])));

						if (isSafeShortConvert(std::stoi(result[27])))
							setBuySell1(static_cast<short>(std::stoi(result[27])));

						setDisclosedVol1(std::stol(result[28]));
						setDisclosedVolRemaining1(std::stol(result[29]));
						setTotalVolRemaining1(std::stol(result[30]));
						setVolume1(std::stol(result[31]));
						setVolumeFilledToday1(std::stol(result[32]));
						setPrice1(std::stol(result[33]));
						setTriggerPrice1(std::stol(result[34]));
						setGoodTillDate1(std::stol(result[35]));
						setEntryDateTime1(std::stol(result[36]));
						setMinFillAon1(std::stol(result[37]));
						setLastModified1(std::stol(result[38]));

						if (isSafeShortConvert(std::stoi(result[39])))
							setBranchId1(static_cast<short>(std::stoi(result[39])));

						setTraderId1(std::stol(result[40]));
						setBrokerId1(result[41]);
						setcOrdFiller(result[42]);

						if (isSafeCharConvert(std::stoi(result[43])))
							setOpenClose1(static_cast<char>(std::stoi(result[43])));

						setSettlor1(result[44]);

						if (isSafeShortConvert(std::stoi(result[45])))
							setProClient1(static_cast<short>(std::stoi(result[45])));

						if (isSafeShortConvert(std::stoi(result[46])))
							setSettlementPeriod1(static_cast<short>(std::stoi(result[46])));

						if (isSafeCharConvert(std::stoi(result[47])))
							setGiveupFlag1(static_cast<char>(std::stoi(result[47])));

						setFiller(result[48]);
						setNnfField(std::stod(result[49]));
						setMktReplay(std::stod(result[50]));
						setPAN(result[51]);
						setAlgoID(std::stol(result[52]));

						if (isSafeShortConvert(std::stoi(result[53])))
							setAlgoCategory(static_cast<short>(std::stoi(result[53])));

						setReserved(result[54]);
						setPriceDiff(std::stol(result[55]));


						print();
					}
					void print() const
					{
						PRINT_BODY_START(MS_SPD_OE_REQUEST)
							MESSAGE_HEADER::print();
						PRINT_NNF_FIELD(ParticipantType1)
							PRINT_NNF_FIELD(Filler1)
							PRINT_NNF_FIELD(CompetitorPeriod1)
							PRINT_NNF_FIELD(SolicitorPeriod1)
							PRINT_NNF_FIELD(ModCxlBy1)
							PRINT_NNF_FIELD(ReasonCode1)
							PRINT_NNF_FIELD(StartAlpha1)
							PRINT_NNF_FIELD(EndAlpha1)
							PRINT_NNF_FIELD(Token1)
							_ContractDesc1.print();
						PRINT_NNF_FIELD(OpBrokerId1)
							PRINT_NNF_FIELD(Fillerx1)
							PRINT_NNF_FIELD(FillerOptions1)
							PRINT_NNF_FIELD(Fillery1)
							PRINT_NNF_FIELD(OrderType1)
							PRINT_NNF_FIELD(OrderNumber1)
							PRINT_NNF_FIELD(AccountNumber1)
							PRINT_NNF_FIELD(BookType1)
							PRINT_NNF_FIELD(BuySell1)
							PRINT_NNF_FIELD(DisclosedVol1)
							PRINT_NNF_FIELD(DisclosedVolRemaining1)
							PRINT_NNF_FIELD(TotalVolRemaining1)
							PRINT_NNF_FIELD(Volume1)
							PRINT_NNF_FIELD(VolumeFilledToday1)
							PRINT_NNF_FIELD(Price1)
							PRINT_NNF_FIELD(TriggerPrice1)
							PRINT_NNF_FIELD(GoodTillDate1)
							PRINT_NNF_FIELD(EntryDateTime1)
							PRINT_NNF_FIELD(MinFillAon1)
							PRINT_NNF_FIELD(LastModified1)
							_StOrderFlags1.print();
						PRINT_NNF_FIELD(BranchId1)
							PRINT_NNF_FIELD(TraderId1)
							PRINT_NNF_FIELD(BrokerId1)
							PRINT_NNF_FIELD(cOrdFiller)
							PRINT_NNF_FIELD(OpenClose1)
							PRINT_NNF_FIELD(Settlor1)
							PRINT_NNF_FIELD(ProClient1)
							PRINT_NNF_FIELD(SettlementPeriod1)
							_AdditionalOrderFlags1.print();
						PRINT_NNF_FIELD(GiveupFlag1)
							PRINT_NNF_FIELD(NnfField)
							PRINT_NNF_FIELD(MktReplay)
							PRINT_NNF_FIELD(PAN)
							PRINT_NNF_FIELD(AlgoID)
							PRINT_NNF_FIELD(AlgoCategory)
							PRINT_NNF_FIELD(PriceDiff)
							_Leg2.print();
						_Leg3.print();
						PRINT_BODY_END(MS_SPD_OE_REQUEST)
					}
				};

				struct MULTI_LEG_TWO_L : public DC_PACKET_WRAPPER, MS_SPD_OE_REQUEST
				{
					MULTI_LEG_TWO_L(SIGNED_NNF_LONG traderID, TransCode t)
						:
						DC_PACKET_WRAPPER(sizeof(MULTI_LEG_TWO_L)),
						MS_SPD_OE_REQUEST(traderID, t)
					{}

					void getString(std::string& str)
					{
						MS_SPD_OE_REQUEST::getString(str);
					}
					void fromString(std::string& str)
					{
						print(str);
					}
					void print(std::string& str)
					{
						PRINT_BODY_START(MULTI_LEG_TWO_L)
							//MS_SPD_OE_REQUEST::print() ;
							MS_SPD_OE_REQUEST::fromString(str);
						PRINT_BODY_END(MULTI_LEG_TWO_L)
					}
				};

			}
		}
	}
}

#pragma pack ( pop )
#endif
