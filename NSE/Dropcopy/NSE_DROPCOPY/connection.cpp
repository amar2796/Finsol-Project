#include "connection.h"
#include <cstring>
#pragma warning(disable : 4996)
namespace FIN {
	Connection::Connection(const std::string& fileName, boost::asio::io_service& ioService)
		:ConnectionTemplate(ioService),
		_ioService(ioService),
		_timer(_ioService)
	{
		CurrentPath _currPath;
		_configFileName = _currPath.getCurrentPath() + fileName;		
		getConnectionParam().readConfig(_configFileName);
		getConnectionParam().print();
		_timer.expires_from_now(boost::posix_time::seconds(30));
		_timer.async_wait(boost::bind(&Connection::handleTimer, this, boost::asio::placeholders::error));
		_heartBeatMsg = new HEARTBEAT_MESSAGE(getConnectionParam()._traderId);
		_connected = false;
		isLoggingEnable = getConnectionParam()._isLoggingEnable;
	}
	void Connection::start()
	{
		while (1)
		{
			connectSocket(); 
			_connected = true;
			startCommunication(); 
			while (1)
			{
				try
				{
					receiveFromTarget();
				}
				catch (std::exception& e)
				{
					DEVLOG("Exception Rcvd : " << e.what());
					break;
				}
			}
			std::cout << "Reconnecting after 15 Seconds" << std::endl;
			LOG_INFO << "Reconnecting after 15 Seconds";
			_connected = false;
			getConnectionParam().readConfig(_configFileName);
			Sleep(15000);
		}
	}

	void Connection::stop()
	{
	}

	void Connection::scheduleHeartbeat()
	{
	}
	void Connection::handleTimer(const boost::system::error_code& ec)
	{
		if (!ec)
		{
			sendToTarget(*_heartBeatMsg);
			_timer.expires_from_now(boost::posix_time::seconds(30));
			_timer.async_wait(boost::bind(&Connection::handleTimer, this, boost::asio::placeholders::error));
		}
	}
	 bool Connection::sendToTarget(DC_PACKET_WRAPPER& request)
	{
		if (!_connected) return  false;
	
		MD5_CTX ctx;
		MD5_Init(&ctx);
		MD5_Update(&ctx, (uint8_t*)&request + sizeof(DC_PACKET_WRAPPER), request.getLength() - sizeof(DC_PACKET_WRAPPER));
		MD5_Final(_hash, &ctx);
		
		std::string str(_hash, _hash + sizeof _hash / sizeof _hash[0]);
		
		request.setMD5Checksum(str);
		request.setSequenceNo(++_seqNu);
		//std::cout << "Reqquest Sequnence number = " << request.getSequenceNo();
		boost::asio::write(*m_socket, boost::asio::buffer(&request, request.getLength()));
		//std::cout << "Sent Packet Length = " << request.getLength() << std::endl;
		
		//writTimestamp() ;
		//writeLog( ( char *) &request , request.getLength() ) ;
		printMessages((char*)&request + sizeof(DC_PACKET_WRAPPER));
		//printMessages( (char *) &request ) ;
	
		return true;
	}

	
	void Connection::receiveFromTarget()
	{
		boost::system::error_code ec;
		memset(_data, 0, 65535);
		int dataRcvd = m_socket->read_some(boost::asio::buffer(_data, 65535),ec);
		//LOG_INFO << "Byte RCVD:" << dataRcvd;
		//std::cout << "Data Received" << dataRcvd << "and error is: "<<ec<<std::endl;
		//std::cout << "data " << dataRcvd << std::endl;
		DC_PACKET_WRAPPER* hdr = nullptr;
		int pos = 0;
		int length = 0;
		while (pos < dataRcvd)
		{
			hdr = (DC_PACKET_WRAPPER*)(_data + pos);
			length = hdr->getLength();
			if (length == 0)
			{
				int leftbyte = dataRcvd - pos;
				memcpy(_data, (char*)_data + pos, leftbyte);
				dataRcvd += m_socket->read_some(boost::asio::buffer(_data + leftbyte, 65535 - leftbyte));
				pos = 0;
				std::cout << "Found Length : " << length << std::endl;
				continue;
			}
			else if (pos + length > dataRcvd)
			{
				LOG_INFO << "Length>dataRcvd";
				memcpy(_data, (char*)_data + pos, length);
				dataRcvd = dataRcvd - pos;
				dataRcvd += m_socket->read_some(boost::asio::buffer((char*)_data + dataRcvd, 65535 - dataRcvd));
				pos = 0;
				continue;
			}

			//writTimestamp() ;
			//writeLog( ( char *)_data + pos , hdr->getLength() ) ;			
			if(isLoggingEnable)
				printMessages(_data + pos + sizeof(DC_PACKET_WRAPPER));
			handleMessage(_data + pos + sizeof(DC_PACKET_WRAPPER));

			pos += hdr->getLength();
		}
		///	handleMessage(_data, respCheck->packetType, respCheck->msgType);

	}
}
