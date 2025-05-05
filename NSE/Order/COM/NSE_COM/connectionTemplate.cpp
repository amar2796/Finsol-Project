#include "connectionTemplate.h"
namespace FIN
{

	ConnectionTemplate::ConnectionTemplate(ConfigParameter* configDetail)
	{
		configDetails = configDetail;
	}


	bool ConnectionTemplate::ConnectSSLSocket()
	{
		try
		{
			DEVLOG("SSL Connecting to exch IP : " << configDetails->connectHost
				<< " : " << configDetails->connectPort << " : " << configDetails->nic_ip);

			LOG_INFO << "SSL Connecting to exch IP : " << configDetails->connectHost << " : " << configDetails->connectPort << " : " << configDetails->nic_ip;
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

				return false;
			}

			// Create TCP socket
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == -1)
			{
				std::cerr << "Error creating socket" << std::endl;
				LOG_INFO << "Error creating socket ";

				return false;
			}

			if (configDetails->nic_ip != "")
			{
				// Bind to a specific NIC IP address
				sockaddr_in nic_addr;
				nic_addr.sin_family = AF_INET;
				nic_addr.sin_port = 0; // Let the system assign a random port
				if (inet_pton(AF_INET, configDetails->nic_ip.c_str(), &nic_addr.sin_addr) != 1)
				{
					std::cerr << "Invalid NIC IPv4 address: " << configDetails->nic_ip << std::endl;
					LOG_INFO << "Invalid NIC IPv4 address: " << configDetails->nic_ip;
					return false;
				}

				if (bind(sockfd, reinterpret_cast<sockaddr*>(&nic_addr), sizeof(nic_addr)) == -1)
				{
					std::cerr << "Error binding socket to NIC IP" << std::endl;
					LOG_INFO << "Error binding socket to NIC IP";
					return false;
				}
			}

			// Connect to the server
			sockaddr_in serv_addr;
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(configDetails->connectPort);
			if (inet_pton(AF_INET, configDetails->connectHost.c_str(), &serv_addr.sin_addr) != 1)
			{
				std::cerr << "Invalid IPv4 address: " << configDetails->connectHost << std::endl;
				LOG_INFO << "Invalid IPv4 address: " << configDetails->connectHost;

				closesocket(sockfd);
				return false;
			}

			if (connect(sockfd, reinterpret_cast<sockaddr*>(&serv_addr), sizeof(serv_addr)) == -1)
			{
				std::cerr << "Error connecting to server" << std::endl;
				LOG_INFO << "Error connecting to server";

				closesocket(sockfd);
				return false;
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
				return false;
			}
			std::cout << "[INFO] handshake ssl done" << std::endl;
			LOG_INFO << "[INFO] handshake ssl done";
			return true;
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
			return false;
		}
		return false;
	}
	void ConnectionTemplate::CloseSSLSocket()
	{

		SSL_shutdown(ssl);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		closesocket(sockfd);
	}
}
