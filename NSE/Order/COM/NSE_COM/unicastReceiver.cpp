#include "unicastReceiver.h"
#include <iostream>
namespace FIN
{

    UnicastRcvr::UnicastRcvr()
    {
    }

    void UnicastRcvr::startRead()
    {
        WSADATA wsaData;
        SOCKET serverSocket;
        sockaddr_in serverAddr, clientAddr;

        //Initialize Winsock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "Failed to initialize Winsock. Error: " << WSAGetLastError() << std::endl;
        }

        //Create UDP socket
        serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (serverSocket == INVALID_SOCKET) {
            std::cerr << "Failed to create socket. Error: " << WSAGetLastError() << std::endl;
            WSACleanup();
        }

        //Setup server address structure
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        serverAddr.sin_port = htons(_port);

        //Bind socket
        if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Bind failed. Error: " << WSAGetLastError() << std::endl;
            closesocket(serverSocket);
            WSACleanup();
        }

        //Receive data from client
        int clientLen = sizeof(clientAddr);

        while (true)
        {
            ZeroMemory(data_, max_length);
            int len = recvfrom(serverSocket, data_, max_length, 0, (sockaddr*)&clientAddr, &clientLen);
            process(data_, len);

        }

        std::cout << "Issue in reading data from OMS" << std::endl;
    }

}
