#include "connectionbuilder.h"
#include <thread>

namespace FIN {
	ConnectionBuilder::ConnectionBuilder(boost::asio::io_service& ioService) 
		:Connection(CONSTANT::NSE_DROPCOPY_CONFIG, ioService)
	{
	}
	void ConnectionBuilder::startCommunication()
	{
		
		isLogging = getConnectionParam()._isLoggingEnable;
		DEVLOG("Starting Communication with Exchange");
		LOG_INFO<<"Starting Communication with Exchange";
		_seqNu = 0;
		sendLogonRequest();		
	}
	void ConnectionBuilder::sendLogonRequest()
	{
 		DEVLOG("Sending LogOn Request");
 		LOG_INFO<<"Sending LogOn Request";
		SIGNON_REQUETS signOnReq(getConnectionParam()._traderId);
		
		signOnReq.setUserID(getConnectionParam()._traderId);
		signOnReq.setPassword(getConnectionParam()._password);
		signOnReq.setBrokerID(getConnectionParam()._brokerId);
		
		if (sendToTarget(signOnReq))
		{
			DEVLOG("Logon Request Sent");
			LOG_INFO << "Logon Request Sent";
		}
	}
	void ConnectionBuilder::sendHeartbeat()
	{
		//DEVLOG("Sending Heartbeat");
		//_request._heartbeatRequest.packetType = 'R';
		//_request._heartbeatRequest.packetLength = htons(sizeof(_request._heartbeatRequest.packetType));
		//std::cout << "Heartbeat packet len = " << _request._heartbeatRequest.packetLength << std::endl;
		//sendToTarget(_request._heartbeatRequest);

		///We are doing above process in handler timer in connection.cpp

	}
	void ConnectionBuilder::sendDropCopyDownloadReq()
	{
		DEVLOG("Sending Drop Copy Download Req");
		DROPCOPY_MSG_DOWNLOAD_REQ dropCopyDownReq(getConnectionParam()._traderId);
		dropCopyDownReq.Sequence_number = 0;
		sendToTarget(dropCopyDownReq);
	}
    void ConnectionBuilder::handleMessage(char* data)
    {
        MESSAGE_HEADER* hdr = (MESSAGE_HEADER*)data;
		//std::cout << hdr->getTransCode() << std::endl;
		//LOG_INFO <<"[MS_TRANSCODE] "<< hdr->getTransCode();
        switch (hdr->getTransCode())
        {
		case TransCode_SIGN_ON_REQUEST_OUT: {
			SIGNON_CONFIRMATION* info = (SIGNON_CONFIRMATION*)data;
					
			std::string s1;
			info->getString(s1);
			
			if (info->getErrorCode() == 0) {
				DEVLOG("Logon Success");
				LOG_INFO << "Logon Success";
				onLogon();
				std::string streamAlphaChar = info->getAlphaChar();
				int totalStream = streamAlphaChar[0];				
				for (int i = 1; i <= totalStream; i++)
				{
					DROPCOPY_MSG_DOWNLOAD_REQ downloadReq(getConnectionParam()._traderId);
					downloadReq.setStreamNo(i);
					downloadReq.Sequence_number = 1;
					if (sendToTarget(downloadReq))
					{
						LOG_INFO << "Sending Drop Copy Download Req for stream: "<<i <<" Total_Streams: "<<totalStream;
						std::cout << "Sending Drop Copy Download Req for stream: " << i << " Total_Streams: " << totalStream;
					}
				}
			}
			else {
				std::cout << "Error Received:" << info->getErrorCode() << std::endl;
				LOG_INFO << "Error Received:" << info->getErrorCode();
			}
		}break;
		case TransCode_TRADE_CONFIRMATION: {
			//std::string str;
			onMessageToNewTxt((TRADE_CONFIRMATION*)data, FIN::TAG_11);			//11 = Original Trade
			//onMessageStringSend((TRADE_CONFIRMATION*)data);
			//TRADE_CONFIRMATION* msg = (TRADE_CONFIRMATION*)data;
			//msg->getString(str);
			//LOG_INFO << "[EXCG_TD_CONF] " << str;

		}break;
		case TransCode_TRADE_MODIFY_CONFIRM	: {
			//LOG_INFO<<"TRADE_MODIFY_CONFIRM	"; 
			onMessageToNewTxt((TRADE_CONFIRMATION*)data, FIN::TAG_12);			//12 = Modified Trade
			//onMessageStringSend((TRADE_CONFIRMATION*)data);
			//std::string str;
			//TRADE_CONFIRMATION* msg = (TRADE_CONFIRMATION*)data;
			//msg->getString(str);
			//LOG_INFO << "[EXCG_TD_MOD_C] " << str;
		} break;
		case TransCode_TRADE_MODIFY_REJECT	: {
			//LOG_INFO<<"TRADE_MODIFY_REJECT	"; 
			onMessageToNewTxt((TRADE_CONFIRMATION*)data, FIN::TAG_18);			//18 = Rejected Trade
			//onMessageStringSend((TRADE_CONFIRMATION*)data);
			//std::string str;
			//TRADE_CONFIRMATION* msg = (TRADE_CONFIRMATION*)data;
			//msg->getString(str);
			//LOG_INFO << "[EXCG_TD_MOD_R] " << str;
		} break;
		case TransCode_TRADE_CANCEL_CONFIRM : {
			//LOG_INFO<<"TRADE_CANCEL_CONFIRM  "; 
			onMessageToNewTxt((TRADE_CONFIRMATION*)data, FIN::TAG_13);			//13 = Cancelled Trade
			//onMessageStringSend((TRADE_CONFIRMATION*)data);
			//std::string str;
			//TRADE_CONFIRMATION* msg = (TRADE_CONFIRMATION*)data;
			//msg->getString(str);
			//LOG_INFO << "[EXCG_TD_CAN] " << str;

		} break;
		case TransCode_TRADE_CANCEL_REJECT	: {
			//LOG_INFO<<"TRADE_CANCEL_REJECT	"; 
			onMessageToNewTxt((TRADE_CONFIRMATION*)data, FIN::TAG_18);			//18 = Rejected Trade, 17 = Approved Trade, 19 = Give Up Trade
			//onMessageStringSend((TRADE_CONFIRMATION*)data);
			//std::string str;
			//TRADE_CONFIRMATION* msg = (TRADE_CONFIRMATION*)data;
			//msg->getString(str);
			//LOG_INFO << "[EXCG_TD_CAN_R] " << str;
		} break;

		case TransCode_GIVEUP_APP_CONFIRM_TM: {
			//LOG_INFO<<"GIVEUP_APP_CONFIRM_TM ";std::string str;
			/*GIVEUP_RESPONSE* msg = (GIVEUP_RESPONSE*)data;
			msg->getString(str); 
			LOG_INFO << "[EXCG_GU_C] " << str;*/
		}break;
		case TransCode_GIVEUP_REJ_CONFIRM_TM: {
			//LOG_INFO<<"GIVEUP_REJ_CONFIRM_TM ";std::string str;
			/*GIVEUP_RESPONSE* msg = (GIVEUP_RESPONSE*)data;
			msg->getString(str); 
			LOG_INFO << "[EXCG_GU_R] " << str;*/
			
		}break;
		case TransCode_HEARTBEAT:
		{
			//std::string str;
			//(((HEARTBEAT_MESSAGE*)(data - sizeof(DC_PACKET_WRAPPER)))->getString(str));
			//std::cout << "[EX_RCV_HRTBT] " << str;
			//LOG_INFO << "[EX_RCV_HRTBT] " << str;
		} break;

		default:
		{
			LOG_INFO << "Transaction Code Not Handled for Execution: " << hdr->getTransCode();
			std::cout << "Transaction Code Not Handled for Execution: " << hdr->getTransCode() << std::endl;
		}
        }
    }
}