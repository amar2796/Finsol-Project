#include "connectionTemplate.h"
namespace FIN
{

    ConnectionTemplate::ConnectionTemplate(boost::asio::io_service& ioService)
        :m_ioService(ioService)
    {

    }

    void ConnectionTemplate::connectSocket()
    {
        if (m_socket)
            m_socket.reset();

        DEVLOG("Connecting to exch IP : " << m_connectionParam.getConnectHost() << " : " << m_connectionParam.getConnectPort());
        LOG_INFO << "Connecting to exch IP : " << m_connectionParam.getConnectHost() << " : " << m_connectionParam.getConnectPort();
        m_socket = boost::shared_ptr< boost::asio::ip::tcp::socket>(
            new boost::asio::ip::tcp::socket(m_ioService));

        boost::asio::ip::tcp::resolver resolver(m_ioService);
        boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
            m_connectionParam.getConnectHost().c_str(),
            std::to_string(m_connectionParam.getConnectPort()).c_str());
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

        m_socket->connect(*iterator);
        m_socket->set_option(boost::asio::ip::tcp::no_delay(true));

		DEVLOG("Connected to exch IP : " << m_connectionParam.getConnectHost() << " : " << m_connectionParam.getConnectPort());
		LOG_INFO << "Connected to exch IP : " << m_connectionParam.getConnectHost() << " : " << m_connectionParam.getConnectPort();
    }

    void ConnectionTemplate::start()
    {
        while (1)
        {
            try
            {
                connectSocket();
                startCommunication();
            }
            catch (std::exception& e)
            {
                Sleep(30); // Get Retry Interval from the connection param
            }
        }
    }

}
