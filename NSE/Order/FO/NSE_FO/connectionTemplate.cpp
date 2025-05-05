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

		DEVLOG("Connecting to exch IP : " << m_connectionParam.getConnectHost()
			<< " : " << m_connectionParam.getConnectPort()) << " : " << m_connectionParam.getNicAddress() << std::endl;

		LOG_INFO << "Connecting to exch IP : " << m_connectionParam.getConnectHost() << " : " << m_connectionParam.getConnectPort() << " : " << m_connectionParam.getNicAddress();

		m_socket = boost::shared_ptr< boost::asio::ip::tcp::socket>(
			new boost::asio::ip::tcp::socket(m_ioService));


		if (m_connectionParam.getNicAddress() != "")
		{

			// Bind the socket to the NIC address
			boost::asio::ip::address nic_address = boost::asio::ip::address::from_string(m_connectionParam.getNicAddress());
			boost::asio::ip::tcp::endpoint local_endpoint(nic_address, 0); // Port 0 lets the OS choose the port
			m_socket->open(boost::asio::ip::tcp::v4());
			m_socket->bind(local_endpoint);
		}


		boost::asio::ip::tcp::resolver resolver(m_ioService);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
			m_connectionParam.getConnectHost().c_str(),
			std::to_string(m_connectionParam.getConnectPort()).c_str());
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

		m_socket->connect(*iterator);
		m_socket->set_option(boost::asio::ip::tcp::no_delay(true));
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
				Sleep(30000); // Get Retry Interval from the connection param
			}
		}
	}
	void ConnectionTemplate::ConnectSSLSocket()
	{
		try
		{
			DEVLOG("SSL Connecting to exch IP : " << m_connectionParam.getConnectHost()
				<< " : " << m_connectionParam.getConnectPort() << " : " << m_connectionParam.getNicAddress());

			LOG_INFO << "SSL Connecting to exch IP : " << m_connectionParam.getConnectHost() << " : " << m_connectionParam.getConnectPort() << " : " << m_connectionParam.getNicAddress();
			SSL_library_init();
			//                        SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
			ctx = SSL_CTX_new(TLS_client_method());

			// Set TLS 1.3 as the minimum and maximum protocol versions
			SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
			SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

			// Load CA certificate file
			if (SSL_CTX_load_verify_locations(ctx, FIN::CONSTANT::NSE_ENCRYPTION_FILE.c_str(), nullptr) != 1)
			{
				std::cout << "Error loading CA certificate file " << FIN::CONSTANT::NSE_ENCRYPTION_FILE << std::endl;
				LOG_INFO << "Error loading CA certificate file " << FIN::CONSTANT::NSE_ENCRYPTION_FILE;

				return;
			}

			// Create TCP socket
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1)
			{
				std::cerr << "Error creating socket" << std::endl;
				LOG_INFO << "Error creating socket ";

				return;
			}


			if (m_connectionParam.getNicAddress() != "")
			{
				// Bind to a specific NIC IP address
				sockaddr_in nic_addr;
				nic_addr.sin_family = AF_INET;
				nic_addr.sin_port = 0; // Let the system assign a random port
				if (inet_pton(AF_INET, m_connectionParam.getNicAddress().c_str(), &nic_addr.sin_addr) != 1)
				{
					std::cerr << "Invalid NIC IPv4 address: " << m_connectionParam.getNicAddress() << std::endl;
					LOG_INFO << "Invalid NIC IPv4 address: " << m_connectionParam.getNicAddress();
					return;
				}

				if (bind(sockfd, reinterpret_cast<sockaddr*>(&nic_addr), sizeof(nic_addr)) == -1)
				{
					std::cerr << "Error binding socket to NIC IP" << std::endl;
					LOG_INFO << "Error binding socket to NIC IP";
					return;
				}
			}


			// Connect to the server
			sockaddr_in serv_addr;
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(m_connectionParam.getConnectPort());
			if (inet_pton(AF_INET, m_connectionParam.getConnectHost().c_str(), &serv_addr.sin_addr) != 1)
			{
				std::cerr << "Invalid IPv4 address: " << m_connectionParam.getConnectHost() << std::endl;
				LOG_INFO << "Invalid IPv4 address: " << m_connectionParam.getConnectHost();

				closesocket(sockfd);
				return;
			}

			if (connect(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) == -1)
			{
				std::cerr << "Error connecting to server" << std::endl;
				LOG_INFO << "Error connecting to server";

				closesocket(sockfd);
				return;
			}

			// Set up SSL connection
			ssl = SSL_new(ctx);
			SSL_set_fd(ssl, sockfd);

			// Perform SSL handshake
			if (SSL_connect(ssl) != 1)
			{
				std::cerr << "Error performing SSL handshake" << std::endl;
				LOG_INFO << "Error performing SSL handshake";

				SSL_free(ssl);
				closesocket(sockfd);
				return;
			}
			std::cout << "[INFO ] handshake ssl done" << std::endl;
			LOG_INFO << "[INFO ] handshake ssl done";
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
	}
	void ConnectionTemplate::CloseSSLSocket()
	{

		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		closesocket(sockfd);
	}
	void ConnectionTemplate::CloseSocket()
	{
		m_socket->close();
	}
}
