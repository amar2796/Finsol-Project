
#include "unicastReceiver.h"
#include <iostream>
namespace FIN
{

    UnicastRcvr::UnicastRcvr()
    {
    }

    void UnicastRcvr::startRead()
    {
        boost::asio::io_service ioService;
        boost::asio::ip::udp::socket socket(ioService, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), _port));
        while (true)
        {

            boost::asio::ip::udp::endpoint remote_endpoint;
            int len = socket.receive_from(boost::asio::buffer(data_), remote_endpoint);
            process(data_, len);

        }

        std::cout << "Issue in reading data from OMS" << std::endl;
    }

}
