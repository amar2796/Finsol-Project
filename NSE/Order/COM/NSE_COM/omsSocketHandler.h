#ifndef FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H
#define FIN_ORDER_GTWY_OMS_SOCKET_HANDLER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <atomic>
#include <chrono>
#include "threadSafeQueue.h"
#include "defines.h"
#include "NanoLog.hpp"
#include "orderBook.h"

#pragma comment(lib, "Ws2_32.lib")

namespace FIN {
    namespace OrdGtwy {

        class OmsAcceptor {
        public:
            OmsAcceptor() : _listenSocket(INVALID_SOCKET), _clientSocket(INVALID_SOCKET), _port(0) {
                WSADATA wsaData;
                if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                    throw std::runtime_error("WSAStartup failed");
                }
            }

            ~OmsAcceptor() {
                cleanupSockets();
                WSACleanup();
            }

            void init(int port) {
                _port = port;

                _listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                if (_listenSocket == INVALID_SOCKET) throw std::runtime_error("Socket creation failed");

                sockaddr_in service{};
                service.sin_family = AF_INET;
                service.sin_addr.s_addr = INADDR_ANY;
                service.sin_port = htons(port);

                if (bind(_listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
                    throw std::runtime_error("Bind failed");
                }

                if (listen(_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
                    throw std::runtime_error("Listen failed");
                }
            }

            void accept() {
                cleanupClientSocket();
                DEVLOG("Waiting to accept connection from OMS : " << _port);
                LOG_INFO << "Waiting to accept connection from OMS : " << _port;
                _clientSocket = ::accept(_listenSocket, nullptr, nullptr);
                if (_clientSocket == INVALID_SOCKET) {
                    throw std::runtime_error("Accept failed");
                }
                DEVLOG("Connection accepted from OMS : " << _port);
                LOG_INFO << "Connection accepted from OMS : " << _port;
            }

        protected:
            void cleanupClientSocket() {
                if (_clientSocket != INVALID_SOCKET) {
                    closesocket(_clientSocket);
                    _clientSocket = INVALID_SOCKET;
                }
            }

            void cleanupSockets() {
                cleanupClientSocket();
                if (_listenSocket != INVALID_SOCKET) {
                    closesocket(_listenSocket);
                    _listenSocket = INVALID_SOCKET;
                }
            }

            int _port;
            SOCKET _listenSocket;
            SOCKET _clientSocket;
        };

        class OmsSenderServer : public OmsAcceptor {
        public:
            OmsSenderServer() : OmsAcceptor(), _process(false) {}

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
                send(_clientSocket, msg.c_str(), msg.length(), 0);
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
                closesocket(_clientSocket);
                closesocket(_listenSocket);
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
            std::atomic<bool> _process;
            bool _checkExchange = false;
            std::string logoutMsg = "Logout session";

            void checkHealth() {
                while (_process) 
                {
                    try 
                    {
                        char buf[1] = { 0 };
                        int bytesSent = send(_clientSocket, buf, 1, 0);
                        if (bytesSent == SOCKET_ERROR) {
                            std::cerr << "Failed send heartbeat to oms: " << WSAGetLastError() << std::endl;
                            throw std::runtime_error("Failed send heartbeat to oms.");
                        }
                    }
                    catch (const std::exception& e) {
                        LOG_INFO << "[INFO ]OMS Disconnected [Port]: " << _port << " Reason: " << e.what();
                        _process = false;
                        accept();
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
            }

            void writeToOms() {
                std::string msg;
                while (true) {
                    if (!_process) continue;
                    try {
                        msg = _exchRespQ.top();
                        int bytesSent = send(_clientSocket, msg.c_str(), (int)msg.size(), 0);
                        if (bytesSent == SOCKET_ERROR) {
                            DEVLOG("Send failed: " << WSAGetLastError());
                            LOG_INFO << "Send failed: " << WSAGetLastError();
                            _process = false;
                            break;
                        }
                        _exchRespQ.pop();
                    }
                    catch (const std::exception& e) {
                        DEVLOG("EXCEPTION: " << e.what());
                        LOG_INFO << "EXCEPTION: " << e.what();
                        _process = false;
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                }
            }

            ThreadSafeQueue<std::string> _exchRespQ;
            ThreadSafeQueue<OrderExchResponse> _exchRespQueue;
        };
    }
}

#endif
