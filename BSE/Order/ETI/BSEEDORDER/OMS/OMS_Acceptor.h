#ifndef FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H
#define FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H
#include "../HeaderFile/defines.h"
#include <boost/asio.hpp>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
template <typename T>
class ThreadSafeQueue
{
public:

    T pop()
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty())
        {
            _condVar.wait(mlock);
        }
        auto item = _queue.front();
        _queue.pop();
        return item;
    }

    void pop(T& item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty())
        {
            _condVar.wait(mlock);
        }
        item = _queue.front();
        _queue.pop();
    }

    T& top()
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        while (_queue.empty())
        {
            _condVar.wait(mlock);
        }
        return _queue.front();
    }

    void push(const T& item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push(item);
        mlock.unlock();
        _condVar.notify_one();
    }

    void push(T&& item)
    {
        std::unique_lock<std::mutex> mlock(_mutex);
        _queue.push(std::move(item));
        mlock.unlock();
        _condVar.notify_one();
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
    std::condition_variable _condVar;
};
class OmsAcceptor
{
public:
    OmsAcceptor(boost::asio::io_service& ioService)
        :_ioService(ioService)
    {
        _acc = nullptr;
        _socket = nullptr;
    }

    void init(int port)
    {
        if (_acc != nullptr)
            delete _acc;
        if (_socket != nullptr)
            delete _socket;

        _port = port;
        _acc = new boost::asio::ip::tcp::acceptor(_ioService, boost::asio::ip::tcp::endpoint(
            boost::asio::ip::tcp::v4(), port));

        _socket = new boost::asio::ip::tcp::socket(_ioService);
    }

    void accept()
    {
        init(_port);
        FIN_WARN("Waiting for OMS Connection [Port]: "
            << _port);
        _acc->accept(*_socket);
        FIN_INFO("OMS Connected [Port]: "
            << _port);
    }

protected:
    int _port;
    boost::asio::ip::tcp::acceptor* _acc;
    boost::asio::ip::tcp::socket* _socket;
    boost::asio::io_service& _ioService;
};

class OmsRcvServer : public OmsAcceptor
{
    std::string lastKeyVal;
public:
    OmsRcvServer(boost::asio::io_service& ioService)
        : OmsAcceptor(ioService)
    {
        _connectionAccepted = false;
        lastKeyVal = "";
        lastKeyVal.append(1 + "10=000");
    }

    std::string leftOverString;
    void rcvFromOms()
    {
        if (!_connectionAccepted)
        {
            accept();
            _connectionAccepted = true;
        }

        char buf[1024 * 8];
        int dataLength;
        try
        {
            dataLength = _socket->read_some(boost::asio::buffer(buf, 1024 * 8));

            std::string dataRcvd1(buf, dataLength);
            std::string dataRcvd;
            dataRcvd.append(leftOverString + dataRcvd1);
            //std::cout <<"[INFO] " << "DataRcvd: " << dataRcvd << std::endl ;
            leftOverString = "";

            size_t i;
            for (i = 0; i < dataRcvd.size(); i = i + 6)
            {
                int temp = i;
                i = dataRcvd.find(lastKeyVal, i);
                if (i == std::string::npos)
                {
                    i = temp;
                    break;
                }
                std::string message(dataRcvd, temp, i + 6 - temp);
                std::cout <<"[INFO] " << "[INBOUND]:  " << message << std::endl;
                handleOmsWrite(message);
            }
            if (i != dataRcvd.size())
            {
                leftOverString = std::string(dataRcvd, i, dataRcvd.size() - i);
            }
        }
        catch (...)
        {
            _connectionAccepted = false;
        }
    }

    bool _connectionAccepted;

    virtual void handleOmsWrite(const std::string& str) = 0;
};

class OmsSenderServer : public OmsAcceptor
{
public:

    OmsSenderServer(boost::asio::io_service& ioService)
        : OmsAcceptor(ioService)
    {
        _process = false;
    }

    void sendStringToOms(const std::string& msg)
    {
        _exchRespQ.push(std::move(msg));
    }
    /*
    void sendResponseToOms( const OrderExchResponse &response )
    {
        response.print() ;
       // _exchRespQueue.push ( std::move ( response ) ) ;
    }
    */
    void startOmsSendThread()
    {
        static std::thread t(&OmsSenderServer::writeToOms, this);
    }

    void startOmsHealthThread()
    {
        static std::thread t(&OmsSenderServer::checkHealth, this);
    }

    void stopProcessing()
    {
        _process = false;
    }

    void accept()
    {
        OmsAcceptor::accept();
        _process = true;
    }
private:

    bool _process;

    void checkHealth()
    {
        while (true)
        {
            try
            {
                char buf[1];
                _socket->read_some(boost::asio::buffer(buf, 1));
            }
            catch (...)
            {
                FIN_ERROR("OMS Disconnected [Port]: " << _port);
                _process = false;
                OmsAcceptor::accept();
                _process = true;
            }
        }
    }

    void writeToOms()
    {
        std::cout <<"[INFO] " << "Start Processing" << std::endl;
        std::string msg;
        while (true)
        {
            accept();
            while (true)
            {
                if (_process)
                {
                    try
                    {
                        msg = _exchRespQ.top();
                        boost::asio::write(*_socket, boost::asio::buffer(msg));
                        std::cout <<"[INFO] " << "[OUTBOUND]: " << msg << std::endl;
                    }
                    catch (std::exception& e)
                    {

                        _process = false;
                        break;

                    }
                    if (_process)
                        _exchRespQ.pop();
                }
                else
                {
                    Sleep(10);
                }
            }
            _process = true;
        }
    }

    ThreadSafeQueue < std::string >  _exchRespQ;

};

#endif