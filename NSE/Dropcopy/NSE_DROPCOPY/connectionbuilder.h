#ifndef FIN_CONNECTION_BUILDER_H
#define FIN_CONNECTION_BUILDER_H
#include "connection.h"
#include <boost/asio.hpp>
#include "messages.h"
#include "Constants.h"
namespace FIN {
	class ConnectionBuilder : public Connection
	{
	public:
		void init();
		void stop();
		ConnectionBuilder(boost::asio::io_service& ioService);
		//RequestStructure _request;
		FIN::DC_PACKET_WRAPPER msg;
	protected:

		boost::shared_ptr < boost::asio::ip::tcp::socket > _socket;

		void startCommunication();
		void sendLogonRequest();
		void sendDropCopyDownloadReq();
		//void sendUnsequenceMsgRequest(std::string);
		void sendHeartbeat();
		virtual void handleMessage(char* data);
		virtual void onMessage(std::string msg) = 0;
		virtual void onMessage(FIN::TRADE_CONFIRMATION* msg) = 0;
		virtual void onMessageStringSend(FIN::TRADE_CONFIRMATION* msg) = 0;
		virtual void onMessageToTxt(FIN::TRADE_CONFIRMATION* msg) = 0;
		virtual void onMessageToNewTxt(FIN::TRADE_CONFIRMATION* msg, std::string status) = 0;

		virtual void onLogon() = 0;
		bool isLogging = 0;
		std::string strReq;
		std::string strResp;
#define LOG_FO_RESPONSE( RESP )\
            strResp.clear();((( RESP *) (data ))->getString(strResp));LOG_INFO<<"[EX_RCV] "<<strResp;break;
#define LOG_FO_REQUEST( REQ )\
               strReq.clear();((( REQ *) (data - sizeof ( DC_PACKET_WRAPPER )))->getString(strReq));LOG_INFO<<"[EX_SND] "<<strReq;break;

		void printMessages(char* data)
		{
			MESSAGE_HEADER* hdr = (MESSAGE_HEADER*)data;
			//std::cout << "Message TransCode : " << hdr->getTransCode() << std::endl;
			std::string str1;
			//hdr->print();
			switch (hdr->getTransCode())
			{
			case TransCode_SIGN_ON_REQUEST_IN:  LOG_FO_REQUEST(SIGNON_REQUETS);

			case TransCode_HEARTBEAT:
			{
				std::string str;
				(((HEARTBEAT_MESSAGE*)(data - sizeof(DC_PACKET_WRAPPER)))->getString(str));
				// std::cout << "[EX_SND_HRTBT] " << str;
				LOG_INFO << "[EX_SND_HRTBT] " << str;
			} break;
			case TransCode_SIGN_ON_REQUEST_OUT: LOG_FO_RESPONSE(SIGNON_CONFIRMATION);
			case TransCode_DROP_COPY_DOWNLOAD_REQUEST: LOG_FO_REQUEST(DROPCOPY_MSG_DOWNLOAD_REQ);
			case TransCode_TRADE_CONFIRMATION: LOG_FO_RESPONSE(TRADE_CONFIRMATION);
			case TransCode_TRADE_MODIFY_CONFIRM: LOG_FO_RESPONSE(TRADE_CONFIRMATION);
			case TransCode_TRADE_MODIFY_REJECT: LOG_FO_RESPONSE(TRADE_CONFIRMATION);
			case TransCode_TRADE_CANCEL_CONFIRM: LOG_FO_RESPONSE(TRADE_CONFIRMATION);
			case TransCode_TRADE_CANCEL_REJECT: LOG_FO_RESPONSE(TRADE_CONFIRMATION);

			case TransCode_GIVEUP_APP_CONFIRM_TM: LOG_FO_RESPONSE(GIVEUP_RESPONSE);
			case TransCode_GIVEUP_REJ_CONFIRM_TM: LOG_FO_RESPONSE(GIVEUP_RESPONSE);

			default:
			{
				CONSOLE_INFO_LOG("Transaction Code Not Handled for LOG: " << hdr->getTransCode());
				//std::string str;
				//hdr->getString(str);
				//LOG_INFO<< "Writting header" << str;
			}
			}
		}
#undef LOG_FO_RESPONSE
#undef LOG_FO_REQUEST

	};
}
#endif