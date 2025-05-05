#ifndef FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H
#define FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H

#include <boost/asio.hpp>
#include "threadSafeQueue.h"
#include "defines.h"
#include "NanoLog.hpp"
#include "orderBook.h"

namespace FIN {
    namespace OrdGtwy {

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
                DEVLOG("Waiting to accept connection from OMS : " << _port);
                LOG_INFO << " Waiting to accept connection from OMS : " << _port;
                _acc->accept(*_socket);
                DEVLOG("connection accepted from OMS : " << _port);
                LOG_INFO << "connection accepted from OMS : " << _port;
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
                    //std::cout << "DataRcvd: " << dataRcvd << std::endl ;
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
                        //std::cout << "[INBOUND]:  " << message << std::endl ;
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

            void sendResponseToOms(const OrderExchResponse& response)
            {
                response.print();
                _exchRespQueue.push(std::move(response));
            }

            void startOmsSendThread()
            {
                static std::thread t(&OmsSenderServer::writeToOms, this);
            }

            std::string logoutMessage()
            {
                return logoutMsg;
            }

            void sendLogoutMsg(const std::string msg)
            {
                boost::asio::write(*_socket, boost::asio::buffer(msg));
            }

            void stopProcessing()
            {
                _process = false;
            }

            void startOmsHealthThread()
            {
                static std::thread t(&OmsSenderServer::checkHealth, this);
            }

            void accept()
            {
                OmsAcceptor::accept();
                _process = true;
            }

            void closeSocketConnection()
            {
                _socket->close();
                _acc->close();
            }

            void stopConnection()
            {
                _checkExchange = false;
            }

            void startConnection()
            {
                _checkExchange = true;
            }

            bool isOmsConnect()
            {
                return _process;
            }
        private:

            bool _process;
            bool isFirst; 
            bool _checkExchange = false;
            std::string logoutMsg = "Logout session";

            void checkHealth()
            {
                isFirst = true;
                while (true)
                {
                    try
                    {
                        char buf[1];
                        _socket->read_some(boost::asio::buffer(buf, 1));
                    }
                    catch (...)
                    {
                        if (isFirst)
                        {
                            std::cout << "OMS Disconnected [Port]: " << _port << std::endl;
                            std::cout << "Waiting for exchnage..." << std::endl;
                            isFirst = false;
                        }

                        if (_checkExchange)
                        {
                            _process = false;
                            OmsAcceptor::accept();
                            _process = true;
                        }
                    }
                }
            }

            void writeToOms()
            {
                //std::cout << "Start Processing" << std::endl ;
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
                                //std::cout << "[OUTBOUND]: " << msg << std::endl ;
                            }
                            catch (std::exception& e)
                            {
                                DEVLOG("EXCEPTION: " << e.what());
                                LOG_INFO << "EXCEPTION: " << e.what();
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

            ThreadSafeQueue < OrderExchResponse >  _exchRespQueue;
        };

    }
}
#endif
