#ifndef FIN_NNF_TRIMMED_COM_CONNECTION_H
#define FIN_NNF_TRIMMED_COM_CONNECTION_H

#include "messages.h"
#include "errorCodes.h"
#include <openssl/md5.h>
#include "connectionStatus.h"
#include "common.h"
#include "configReader.h"
#include "throttling.h"
#include "color.h"
#include "connection.h"
#include "configParameter.h"

#define SSL_unreachable "ssl connection unreachable"
#define exch_unreachable "Exchange connetion unreachable"
#define exch_notup "Exchange not UP"

namespace FIN {
	namespace NSE {
		namespace TRIMMED {
			namespace COM {

				class ExchangeProcess : public TRIMMED::Connection
				{
				public:
					void init();
					void stop();

					ExchangeProcess(ConfigParameter* configDetails);

				protected:
					ConfigParameter* configDetails;

					void startCommunication();
					void sendGatewayRequest();
					void sendBoxLoginRequest(const std::string& sessionKey, const std::string& ip, const int port);
					void sendSecureBoxLogin_Req(const std::string& sessionKey, const std::string& ip, const int port);
					void sendLogonMesage();
					void sendMessageDownloadHandle(int currStream);
					void sendSystemUpdate();


					virtual void handleMessage(char* data, int value = 0);
					virtual void messageDownloadHandle(char* data, int);
					virtual void disconnectOmsConn();
					virtual void sendRequestDisconnectOmsConn() = 0;

					virtual void onMessage(NSE::TRIMMED::COM::ORDER_NEW_CONFIRMATION_TR* msg) = 0;
					virtual void onMessage(NSE::TRIMMED::COM::ORDER_CXL_CONFIRMATION_TR* msg) = 0;
					virtual void onMessage(NSE::TRIMMED::COM::ORDER_MOD_CONFIRMATION_TR* msg) = 0;
					virtual void onMessage(NSE::TRIMMED::COM::ORDER_MODIFICATION_ERROR* msg) = 0;
					virtual void onMessage(NSE::TRIMMED::COM::ORDER_CANCELATION_ERROR* msg) = 0;

					virtual void onMessage(NSE::TRIMMED::COM::ORDER_ERROR_RESPONSE* msg) = 0;
					virtual void onMessage(NSE::TRIMMED::COM::MS_OE_REQUEST* msg) = 0;
					virtual void onMessage(NSE::TRIMMED::COM::ORDER_CONFIRM_RESPONSE* msg) = 0;
					//            virtual void onMessage ( NSE::TRIMMED::COM::ORDER_ERROR_RESPONSE * msg ) = 0 ;
					virtual void onMessage(NSE::TRIMMED::COM::ORDER_CANCEL_CONFIRMATION* msg) = 0;
					virtual void onMessage(NSE::TRIMMED::COM::ORDER_MODIFICATION_CONFIRMATION* msg) = 0;

					virtual void onMessage(MS_TRADE_CONFIRM* msg) = 0;
					virtual void onMessage(MS_TRADE_CONFIRM_TR* msg) = 0;
					virtual void onLogon() = 0;

					virtual void setExchangeIsUPStatus(bool) = 0;
					virtual void stopConnection() = 0;
					virtual void rejectMessage(long clordid, std::string) = 0;
					virtual bool orderHistoryIsEmpty() = 0;
					virtual std::unordered_set<int> getClOrdIdInstance() = 0;

					HEARTBEAT_MESSAGE* _heartBeatMsg;

					//LOG::NnfFoDCReader _logReader ;
					virtual bool getOmsConnectionStatus() = 0;

					std::string strReq;
					std::string strResp;
#define LOG_COM_RESPONSE( RESP )\
            strResp.clear();((( COM::RESP *) (data ))->getString(strResp));LOG_INFO<<"[EX_RCV] "<<strResp;break;
#define LOG_COM_REQUEST( REQ )\
               strReq.clear();((( COM::REQ *) (data - sizeof ( DC_PACKET_WRAPPER )))->getString(strReq));LOG_INFO<<"[EX_SND] "<<strReq;break;
					void printMessages(char* data)
					{
						MESSAGE_HEADER* hdr = (MESSAGE_HEADER*)data;

						switch (hdr->getTransCode())
						{
						case TransCode_SIGN_ON_REQUEST_IN:  LOG_COM_REQUEST(SIGNON_REQUETS);
						case TransCode_GR_REQUEST:  LOG_COM_REQUEST(GR_REQUEST);
						case TransCode_BOARD_LOT_IN_TR:  LOG_COM_REQUEST(ORDER_NEW_REQUEST_TR);
						case TransCode_ORDER_CANCEL_IN_TR:  LOG_COM_REQUEST(ORDER_CANCEL_TR);
						case TransCode_ORDER_MOD_IN_TR:  LOG_COM_REQUEST(ORDER_MODIFY_TR);
						case TransCode_BOX_SIGN_ON_REQUEST_IN:  LOG_COM_REQUEST(BOX_SIGN_ON_REQUEST_IN);
						case TransCode_SYSTEM_INFORMATION_IN:  LOG_COM_REQUEST(MS_SYSTEM_INFO_REQ);
						case TransCode_ORDER_MOD_IN:LOG_COM_REQUEST(MODIFY_ORDER);
						case TransCode_BOARD_LOT_IN: LOG_COM_REQUEST(NEW_ORDER);
						case TransCode_ORDER_CANCEL_IN: LOG_COM_REQUEST(CANCEL_ORDER);
						case TransCode_DOWNLOAD_REQUEST:  LOG_COM_REQUEST(MS_MESSAGE_DOWNLOAD);
						case TransCode_HEADER_RECORD: {
							std::string str;
							hdr->getString(str);
							LOG_INFO << "[EX_RCV_HEADER_RECORD]" << str;
						}break;
						case TransCode_TRAILER_RECORD: {
							std::string str;
							hdr->getString(str);
							LOG_INFO << "[EX_RCV_TRAILER_RECORD]" << str;

						}break;
						case TransCode_MESSAGE_RECORD: {
							std::string str;
							hdr->getString(str);
							LOG_INFO << "[EX_RCV_MSG_RECORD]" << str;
						}break;

						case TransCode_HEARTBEAT:
						{
							std::string str;
							(((HEARTBEAT_MESSAGE*)(data - sizeof(DC_PACKET_WRAPPER)))->getString(str));
							LOG_INFO << "[EX_HRTBT] " << str;
						} break;
						case TransCode_SIGN_ON_REQUEST_OUT: LOG_COM_RESPONSE(SIGNON_CONFIRMATION);
						case TransCode_GR_RESPONSE: LOG_COM_RESPONSE(GR_RESPONSE);
						case TransCode_ORDER_CONFIRMATION:LOG_COM_RESPONSE(ORDER_CONFIRM_RESPONSE);
						case TransCode_ORDER_CONFIRMATION_TR: LOG_COM_RESPONSE(ORDER_NEW_CONFIRMATION_TR);
						case TransCode_ORDER_ERROR: LOG_COM_RESPONSE(ORDER_ERROR_RESPONSE);
						case TransCode_BOX_SIGN_ON_REQUEST_OUT: LOG_COM_RESPONSE(MS_BOX_SIGN_OFF);
						case TransCode_TRADE_CONFIRMATION:LOG_COM_RESPONSE(MS_TRADE_CONFIRM);
						case TransCode_TRADE_CONFIRMATION_TR: LOG_COM_RESPONSE(MS_TRADE_CONFIRM_TR);
						case TransCode_ORDER_CANCEL_CONFIRMATION: LOG_COM_RESPONSE(ORDER_CANCEL_CONFIRMATION);
						case TransCode_BATCH_ORDER_CANCEL: LOG_COM_RESPONSE(ORDER_CANCEL_CONFIRMATION);
						case TransCode_ORDER_CXL_CONFIRMATION_TR: LOG_COM_RESPONSE(ORDER_CXL_CONFIRMATION_TR);
						case TransCode_ORDER_MOD_CONFIRMATION:LOG_COM_RESPONSE(ORDER_MODIFICATION_CONFIRMATION);
						case TransCode_ORDER_MOD_CONFIRMATION_TR: LOG_COM_RESPONSE(ORDER_MOD_CONFIRMATION_TR);
						case TransCode_ORDER_MOD_REJECT: LOG_COM_RESPONSE(ORDER_MODIFICATION_ERROR);
						case TransCode_ORDER_CANCEL_REJECT: LOG_COM_RESPONSE(ORDER_CANCELATION_ERROR);
						case TransCode_SYSTEM_INFORMATION_OUT: LOG_COM_RESPONSE(MS_SYSTEM_INFO_DATA);
						case TransCode_PRICE_CONFIRMATION: LOG_COM_RESPONSE(MS_OE_REQUEST);
						default:
						{
							//LOG_INFO << "Transaction Code Not Handled for LOG: " << hdr->getTransCode();
							/*	std::string str;
								hdr->getString(str);
								LOG_INFO << "Writting header" << str;*/
						}
						}
					}
#undef LOG_COM_RESPONSE
#undef LOG_COM_REQUEST
				};
			}
		}
	}
}

#endif