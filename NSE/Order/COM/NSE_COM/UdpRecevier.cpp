#include "UdpRecevier.h"

UDP_RECEVIER::UDP_RECEVIER()
{


}

void UDP_RECEVIER::init_rcv(int port)
{
	try
	{
		std::cout << "UNICAST RECEVIER INITIALIZATION" << std::endl;
		this->port = port;
		initializeUdp();
		ConnectUdp();
	}
	catch (std::exception& e)
	{

	}
}

void UDP_RECEVIER::initializeUdp()
{
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)

	{

		std::cout << "Client: WSAStartup failed with error " << WSAGetLastError() << std::endl;
		WSACleanup();
	}

	else
	{
		std::cout << "Client: The Winsock DLL status is " << wsaData.szSystemStatus << std::endl;
		SendingSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (SendingSocket == INVALID_SOCKET)
		{
			std::cout << "Client: Error at socket " << WSAGetLastError() << std::endl;
			WSACleanup();
		}
		else
		{
			std::cout << "Client: socket() is OK! " << std::endl;
		}


	}
}

void UDP_RECEVIER::ConnectUdp()
{

	memset((char*)&ReceiverAddr, 0, sizeof(ReceiverAddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	ReceiverAddr.sin_family = AF_INET;
	ReceiverAddr.sin_port = htons(port);
	ReceiverAddr.sin_addr.s_addr = INADDR_ANY;

	if (bind(SendingSocket, (const struct sockaddr*)&ReceiverAddr,
		sizeof(ReceiverAddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

}

void UDP_RECEVIER::StartReceiving()
{

	int clientSize = sizeof(cliaddr);
	while (true)
	{
		ByteReceived = recvfrom(SendingSocket, ReceiveBuf, BufLength, 0, (struct sockaddr*)&cliaddr, &clientSize);
		if (ByteReceived > 0)

		{
			receiveFromOMS(ReceiveBuf);

		}

		else if (ByteReceived <= 0)
			printf("Server: Connection closed with error code: %ld\n", WSAGetLastError());

		else
			printf("Server: recvfrom() failed with error code: %d\n", WSAGetLastError());

		memset(ReceiveBuf, 0, BufLength);
	}
}

void UDP_RECEVIER::disposeSocket()
{
	closesocket(SendingSocket);
}

//void UDP_RECEVIER::receiveFromOMS(const std::string&)
//{
//}
