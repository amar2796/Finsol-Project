#include "ExchangeConnection.h"

ExchangeConnection::ExchangeConnection(SessionSettings* s, boost::asio::io_service& ioService) :m_ioService(ioService)
{
	/*host_ip = s->ExchangeIp;
	host_port = s->ExchangePort;*/

	sessionSetting = s;
}



void ExchangeConnection::initializeSSLSocket()
{
	// Initialize SSL/TLS library
	SSL_library_init();
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	// Create SSL context for client
	ctx = SSL_CTX_new(TLS_client_method());
	if (ctx == nullptr) {
		ERR_print_errors_fp(stderr);
		LOG_INFO << "SSL_CTX_new initilized error";
		return;
	}

	// Set minimum and maximum protocol versions
	SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
	SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);

	// Load CA certificate file
	if (SSL_CTX_load_verify_locations(ctx, Certificate.c_str(), sessionSetting->certificatePath.c_str()) != 1)
	{
		std::cout << "Error loading CA certificate file " << Certificate << std::endl;
		LOG_INFO << "Error loading CA certificate file " << Certificate;
		return;
	}
	std::cout << "Certificate vefified." << std::endl;
	LOG_INFO << "Certificate verified.";

	// Create a TCP socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("socket creation");
		return;
	}

	// Connect to the server
	std::cout << "Connecting to exchange with Primary ip, port " << sessionSetting->ExchangeIp << " " << std::to_string(sessionSetting->ExchangePort) << std::endl;
	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(sessionSetting->ExchangePort);
	server_addr.sin_addr.s_addr = inet_addr(sessionSetting->ExchangeIp.c_str());
	if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("socket connection");
		closesocket(sockfd);
		return;
	}

	// Create SSL structure
	ssl = SSL_new(ctx);
	if (ssl == nullptr) {
		ERR_print_errors_fp(stderr);
		closesocket(sockfd);
		return;
	}
	// Associate SSL object with file descriptor (socket)
	if (SSL_set_fd(ssl, sockfd) == 0) {
		ERR_print_errors_fp(stderr);
		closesocket(sockfd);
		return;
	}
	// Initiate SSL handshake
	int ssl_ret_code = SSL_connect(ssl); // Capture the return value
	if (ssl_ret_code != 1)
	{
		// Print more specific error message
		int ssl_error = SSL_get_error(ssl, ssl_ret_code);
		if (ssl_error == SSL_ERROR_SSL) {
			// An SSL error occurred, print detailed error information
			char error_buf[256];
			ERR_error_string_n(ERR_get_error(), error_buf, sizeof(error_buf));
			std::cerr << "SSL_connect failed: " << error_buf << std::endl;
		}
		else {
			// Other types of errors, print generic error message
			std::cerr << "SSL_connect failed with error code: " << ssl_error << std::endl;
		}

		// Clean up and return false
		SSL_free(ssl);
		closesocket(sockfd);
		SSL_CTX_free(ctx);
		WSACleanup();
		return;
	}
	isSSLConnect = true;
	std::cout << "Handshake Done." << std::endl;
	LOG_INFO << "Handshake Done.";
	std::cout << "SSL connection established successfully!" << std::endl;
	LOG_INFO << "SSL connection established successfully!";
}

bool ExchangeConnection::sendToSSL(const char* msg, size_t len)
{
	auto bytSent = SSL_write(ssl, msg, len);
	if (bytSent > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ExchangeConnection::readFromSSL()
{
	char buffer[1024]; // Buffer to store received data

	auto bytRec = SSL_read(ssl, buffer, 1024);
	if (bytRec > 0)
	{
		auto header = (MessageHeaderOutCompT*)(buffer);

		if (header->TemplateID == TID_REJECT)
		{
			auto reject = (RejectT*)buffer;
			std::cout << "[REJECT CONFIRM..] " << reject->VarText << std::endl;
			return false;
		}
		else
		{
			auto Gateway_Res = (GatewayResponseT*)buffer;

			////copy key and iv
			//memcpy(key, Gateway_Res->SecurityKey, sizeof(Gateway_Res->SecurityKey));
			//memcpy(iv, Gateway_Res->InitializationVector, sizeof(Gateway_Res->InitializationVector));

			sessionSetting->ExchangeResponsePort = Gateway_Res->GatewaySubID;
			uint32_t ip = Gateway_Res->GatewayID;
			unsigned char octet[4] = { 0,0,0,0 };
			for (int i = 0; i < 4; i++)
			{
				octet[i] = (ip >> (i * 8)) & 0xFF;
			}
			sprintf((char*)sessionSetting->ExchangeResponseIp.c_str(), "%d.%d.%d.%d", octet[3], octet[2], octet[1], octet[0]);

			std::cout << "After Gateway Resposne : Primary IP " << sessionSetting->ExchangeResponseIp.c_str() << " Primary Port " << sessionSetting->ExchangeResponsePort << std::endl;
			//log->logPrint(Gateway_Res);

			//Encryption/Decryption initialize only one time
			std::string temp(Gateway_Res->SecurityKey);
			std::string temp1(Gateway_Res->InitializationVector);
			LOG_INFO << "Key " << temp << " " << temp1;
			LOG_INFO << Gateway_Res->SecurityKey << " " << Gateway_Res->InitializationVector;

			OpenSSL_add_all_algorithms();
			initializeEncryption((unsigned char*)Gateway_Res->SecurityKey, (unsigned char*)Gateway_Res->InitializationVector);
			initializeDecryption((unsigned char*)Gateway_Res->SecurityKey, (unsigned char*)Gateway_Res->InitializationVector);
			LOG_INFO << "Encryption/Decryption Initialization Done";

			return true;
		}
	}
	else
	{
		return false;
	}
}

void ExchangeConnection::CloseSSLSocket()
{
	SSL_shutdown(ssl);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	closesocket(sockfd);
}

void ExchangeConnection::createSocket()
{
	if (m_socket)
		m_socket.reset();

	std::cout << "Connecting to exch IP : " << sessionSetting->ExchangeResponseIp.c_str() << " : " << sessionSetting->ExchangeResponsePort << std::endl;
	LOG_INFO << "Connecting to exch IP : " << sessionSetting->ExchangeResponseIp.c_str() << " : " << int(sessionSetting->ExchangeResponsePort);

	m_socket = boost::shared_ptr< boost::asio::ip::tcp::socket>(
		new boost::asio::ip::tcp::socket(m_ioService));

	boost::asio::ip::tcp::resolver resolver(m_ioService);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
		sessionSetting->ExchangeResponseIp.c_str(),
		std::to_string(sessionSetting->ExchangeResponsePort).c_str());
	boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);
	m_socket->connect(*iterator);
	m_socket->set_option(boost::asio::ip::tcp::no_delay(true));
	LOG_INFO << "Boost socket create sucessfully.";
	std::cout << "Sucessfully create socket." << std::endl;
}

void ExchangeConnection::closeConnectionSocket()
{
	m_socket->close();
	LOG_INFO << "Boost socket close sucessfully.";
}

bool ExchangeConnection::initializeEncryption(unsigned char* key, unsigned char* iv) {

	//std::cout << "Key " << key << " IV " << iv << std::endl;
	// Create a new EVP_CIPHER_CTX structure
	encrypt = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(encrypt);


	// Initialize a symmetric encryption operation with AES 256 GCM encryption algorithm
	if (EVP_EncryptInit_ex(encrypt, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		// Error handling
		std::cout << "Error in encrypt initialization EVP_EncryptInit_ex" << std::endl;
		LOG_INFO << "Error in encrypt initialization EVP_EncryptInit_ex";
		ERR_print_errors_fp(stderr);
		EVP_CIPHER_CTX_free(encrypt);
		encrypt = nullptr;
		return false;
	}

	// Set the IV length to 16 bytes
	if (EVP_CIPHER_CTX_ctrl(encrypt, EVP_CTRL_GCM_SET_IVLEN, 16, NULL) != 1) {
		// Error handling
		std::cout << "Error in encrypt initialization EVP_CIPHER_CTX_ctrl" << std::endl;
		LOG_INFO << "Error in encrypt initialization EVP_CIPHER_CTX_ctrl";
		ERR_print_errors_fp(stderr);
		EVP_CIPHER_CTX_free(encrypt);
		encrypt = nullptr;
		return false;
	}

	// Initialize a symmetric encryption operation with key and IV
	if (EVP_EncryptInit_ex(encrypt, NULL, NULL, key, iv) != 1) {
		// Error handling
		std::cout << "Error in encrypt initialization EVP_EncryptInit_ex" << std::endl;
		LOG_INFO << "Error in encrypt initialization EVP_EncryptInit_ex";
		ERR_print_errors_fp(stderr);
		EVP_CIPHER_CTX_free(encrypt);
		encrypt = nullptr;
		return false;
	}
	std::cout << "Encrypt Context initialize " << std::endl;
	LOG_INFO << "Encrypt Context initialize ";

	return true;
}

int ExchangeConnection::performEncrypt(unsigned char* plaintext, int plaintext_len)
{
	//LOG_INFO << "performencrpt " << (char*)plaintext << " : " << plaintext_len << " : " << (char*)ciphertext << " : " << *ciphertext_len;
	int len;
	if (1 != EVP_EncryptUpdate(encrypt, ciphertext, &len, plaintext, plaintext_len))
	{
		std::cout << "Error in encrypt  " << std::endl;
		LOG_INFO << "Error in encrypt  ";
		ERR_print_errors_fp(stderr); // Print OpenSSL error stack
	}
	return len;
}

bool ExchangeConnection::initializeDecryption(unsigned char* key, unsigned char* iv) {
	//std::cout << "Key " << key << " IV " << iv << std::endl;
	// Create a new EVP_CIPHER_CTX structure
	decrypt = EVP_CIPHER_CTX_new();
	EVP_CIPHER_CTX_init(decrypt);

	// Initialize a symmetric decryption operation with AES 256 GCM encryption algorithm
	if (EVP_DecryptInit_ex(decrypt, EVP_aes_256_gcm(), NULL, NULL, NULL) != 1) {
		// Error handling
		std::cout << "Error in decrypt EVP_DecryptInit_ex " << std::endl;
		LOG_INFO << "Error in decrypt EVP_DecryptInit_ex  ";
		ERR_print_errors_fp(stderr);
		EVP_CIPHER_CTX_free(decrypt);
		decrypt = nullptr;
		return false;
	}

	// Set the IV length to 16 bytes
	if (EVP_CIPHER_CTX_ctrl(decrypt, EVP_CTRL_GCM_SET_IVLEN, 16, NULL) != 1) {
		// Error handling
		std::cout << "Error in decrypt EVP_CIPHER_CTX_ctrl " << std::endl;
		LOG_INFO << "Error in decrypt EVP_CIPHER_CTX_ctrl  ";
		ERR_print_errors_fp(stderr);
		EVP_CIPHER_CTX_free(decrypt);
		decrypt = nullptr;
		return false;
	}

	// Initialize a symmetric decryption operation with key and IV
	if (EVP_DecryptInit_ex(decrypt, NULL, NULL, key, iv) != 1) {
		// Error handling
		std::cout << "Error in decrypt EVP_DecryptInit_ex " << std::endl;
		LOG_INFO << "Error in decrypt EVP_DecryptInit_ex  ";
		ERR_print_errors_fp(stderr);
		EVP_CIPHER_CTX_free(decrypt);
		decrypt = nullptr;
		return false;
	}

	std::cout << "Decrypt Context initialize " << std::endl;
	LOG_INFO << "Decrypt Context initialize   ";

	return true;
}

int ExchangeConnection::performDecrypt(unsigned char* ciphertext, int ciphertext_len, unsigned char* plaintext, int* plaintext_len)
{
	int len;
	if (1 != EVP_DecryptUpdate(decrypt, plaintext, &len, ciphertext, ciphertext_len))
	{
		std::cout << " [INFO] Error in decrypt  " << std::endl;
		LOG_INFO << "Error in decrypt  ";
		ERR_print_errors_fp(stderr); // Print OpenSSL error stack
	}

	*plaintext_len = len;
	//LOG_INFO << "Decrypt len " << len << " " << *plaintext_len;

	return 0;
}

//int ExchangeConnection::performDecrypt(unsigned char* ciphertext, int ciphertext_len)
//{
//	memset(decryptedtext, 0, 1000);
//	int len;
//	if (1 != EVP_DecryptUpdate(decrypt, decryptedtext, &len, ciphertext, ciphertext_len))
//	{
//		std::cout << " [INFO] Error in decrypt  " << std::endl;
//		LOG_INFO << "Error in decrypt  ";
//		ERR_print_errors_fp(stderr); // Print OpenSSL error stack
//	}
//
//	return len;
//}

bool ExchangeConnection::sendToTarget(const char* msg, size_t len)
{
	if (m_socket->is_open())
	{
		auto sendBytes = boost::asio::write(*m_socket, boost::asio::buffer(msg, len));
		if (sendBytes < 0)
		{
			std::cout << "Message not connect" << std::endl;
			LOG_INFO << "Message not connect";
			return false;
		}
	}
	else
	{
		std::cout << "Exchange not connect" << std::endl;
		LOG_INFO << "Exchange not connect";
		return false;
	}
	return true;
}

bool ExchangeConnection::sendToExchange(unsigned char* msg, size_t len)
{
	try
	{
		// Perform encryption
		int ciphertext_len = performEncrypt((msg + msg_header_comp_size), (len - msg_header_comp_size));

		std::vector<char> temp(msg_header_comp_size + ciphertext_len);
		std::memcpy(temp.data(), msg, msg_header_comp_size);
		std::memcpy(temp.data() + msg_header_comp_size, ciphertext, ciphertext_len);

		sendToTarget(temp.data(), (msg_header_comp_size + ciphertext_len));

		resetHeartbeat = true;
		return true;
	}
	catch (const std::exception& ex)
	{
		LOG_INFO << "Exception caught: " << ex.what();
	}
	return false;
}

void ExchangeConnection::closeSocket()
{
	m_socket->close();
}

void ExchangeConnection::shutdownSocket()
{
	m_socket->shutdown(ip::tcp::socket::shutdown_both);
}
