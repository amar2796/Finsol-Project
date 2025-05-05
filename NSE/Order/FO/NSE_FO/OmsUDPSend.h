#pragma once
#include <iostream>
#include "threadSafeQueue.h"
#include "sender.h"
#include <boost/asio.hpp>
#include "Nanolog.hpp"
#include "UdpSender.h"
using namespace FIN;
class OmsUDPSend
{
private:
    // UnicastSender *sender ;
    std::string heartbeat = "H";
    UDP_Sender* sender;
    ThreadSafeQueue<std::string> _exchResponseQ;
    std::atomic<bool> _isConnected;
    std::string msg;
    void omsWrite()
    {
        while (true)
        {
            if (_isConnected)
            {

                msg = (_exchResponseQ.top());
                sender->sendToOMS(msg.c_str(), msg.size());
                LOG_INFO << "[UDP to OMS] " << msg;
                _exchResponseQ.pop();
            }
            else
            {
                Sleep(1);
            }
        }
    }
    void send_heartbeat()
    {
        while (true)
        {
            if (_isConnected)
            {
                sender->sendToOMS(heartbeat.c_str(), heartbeat.size());
                //LOG_INFO << "[UDP to OMS] " << heartbeat;
                Sleep(10);
            }
            else
            {
                Sleep(1);
            }
        }
    }

public:
    void connect(boost::asio::io_service& ioService, const std::string& ip, const int port)
    {
        // sender = new UnicastSender (ioService, ip, port) ;
        //sender = new UnicastSenderNative(ip, port);

        sender = new UDP_Sender;
        sender->init(ip, port);
        static std::thread tOmsWrite(&OmsUDPSend::omsWrite, this);
        static std::thread tOmsHeartbeat(&OmsUDPSend::send_heartbeat, this);
    }

    void send(std::string& msg)
    {
        _exchResponseQ.push(msg);
    }
    void isConnected(bool val)
    {
        _isConnected = val;
    }
};