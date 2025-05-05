#ifndef FIN_CONNECTION_H
#define FIN_CONNECTION_H

#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <Windows.h>
#include <windows.h>
#include <ctime>
#include "connectionTemplate.h"
#include "messages.h"
#include <openssl/md5.h>
#include "CurrentPath.h"
#pragma warning(disable : 4996)
namespace FIN {
	class Connection : public ConnectionTemplate
	{
    public:


        void stop();
        void start();
        Connection(const std::string& fileName, boost::asio::io_service& ioService);

    protected:


        void scheduleHeartbeat();
        void handleTimer(const boost::system::error_code& ec);

        bool sendToTarget(DC_PACKET_WRAPPER& data);
        void receiveFromTarget();

        virtual void handleMessage(char* data) = 0;
        virtual void onLogon() = 0;
        virtual void startCommunication() = 0;

        HEARTBEAT_MESSAGE* _heartBeatMsg;

        boost::asio::io_service& _ioService;
        boost::asio::deadline_timer _timer;

        unsigned char  _hash[16];
        char _data[65535];
        unsigned int _seqNu;
        std::mutex _mutex;
        bool isLoggingEnable;

    private:

        bool _connected;
        std::string _configFileName;

        //SlidingWindowThrottling *_st ;

        std::string _fileName;
        std::ofstream _logFile;

        //TEST_TIME_STAMP _tm ;

        
        //struct timeval currentTime;
        //void writTimestamp()
        //{
        //    _tm.setTimeStamp( getMicrotime() ) ;
        //    writeLog(  ( char *) & _tm , _tm.getLength()  ) ;
        //}

        //long getMicrotime(){
        //    gettimeofday(&currentTime, NULL);
        //    return currentTime.tv_sec * (int)1e6 + currentTime.tv_usec;
        //}

        virtual void printMessages(char* data) {}

	};
}
#endif