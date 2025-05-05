#ifndef ConnectionForContract_HPP
#define ConnectionForContract_HPP

#include <boost/asio.hpp>
#include <iostream>
#include "Nanolog.hpp"

class ConnectionForContract 
{
public:
    ConnectionForContract(const std::string& ip, unsigned short port)
        : io_context_(), socket_(io_context_), endpoint_(boost::asio::ip::make_address(ip), port) 
    {
        socket_.open(boost::asio::ip::udp::v4());
        std::cout << "Connection completed for contract send" << std::endl;
        LOG_INFO << "Connection complete for contract send";
    }

    void send(const std::string& message) {
        socket_.send_to(boost::asio::buffer(message), endpoint_);
        LOG_INFO << "Contract send : " << message;
    }

private:
    boost::asio::io_context io_context_;
    boost::asio::ip::udp::socket socket_;
    boost::asio::ip::udp::endpoint endpoint_;
};

#endif // ConnectionForContract_HPP
