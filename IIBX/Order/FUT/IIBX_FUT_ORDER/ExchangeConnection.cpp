#include "ExchangeConnection.h"

ExchangeConnection::ExchangeConnection(const std::string& fileName, boost::asio::io_service& ioService)
	: _ioService(ioService)
{
	_configFileName = fileName;
	config_param.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
	host_ip = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION+FIN::FIN_CONST::SEP_DOT+FIN::FIN_CONST::HOST_IP, "");
	host_port = config_param.getValue<int>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::HOST_PORT, 0);
	password = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::PASSWORD, "");
}

void ExchangeConnection::init()
{
	config_param.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
	host_ip = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::HOST_IP, "");
	host_port = config_param.getValue<int>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::HOST_PORT, 0);
	password = config_param.getValue<std::string>(FIN::FIN_CONST::SESSION + FIN::FIN_CONST::SEP_DOT + FIN::FIN_CONST::PASSWORD, "");
	firstMsgReceive = false;
	LOG_INFO << "Initilize complete ExchangeConnection";
}

bool ExchangeConnection::createTCPConnection()
{
	try 
	{
		init();

		std::cout << "Connecting to IEC Lookup IP: " << host_ip << " : " << host_port << std::endl;
		LOG_INFO << "Connecting to IEC Lookup IP: " << host_ip << " : " << host_port;

		// Create socket and resolver
		boost::asio::io_context io_context;
		tcp::resolver resolver(io_context);
		tcp::socket socket(io_context);

		// Resolve the host and connect
		std::string port = std::to_string(host_port);
		auto endpoints = resolver.resolve(host_ip, port);
		boost::asio::connect(socket, endpoints);

		if (!socket.is_open()) {
			std::cerr << "Failed to connect to the server." << std::endl;
			return false;
		}

		FIN::setConsoleColor(2);
		std::cout << "Connected to the IEC Lookup Service." << std::endl;
		FIN::setConsoleColor(7);

		// Read loop for receiving data
		const size_t bufferSize = 500;
		char data[bufferSize];

		while (1) {
			boost::system::error_code error;
			size_t length = socket.read_some(boost::asio::buffer(data, bufferSize), error);

			// Error handling
			if (error == boost::asio::error::eof) {
				std::cout << "Connection closed by the server." << std::endl;
				return false;
			}
			else if (error) {
				std::cerr << "Error: " << error.message() << std::endl;
				return false;
			}

			//process response
			int resCode;
			std::memcpy(&resCode, data + 8, sizeof(int));
			if (resCode == 0)
			{
				std::string ip(data + 112, 15);
				host_ip_res = ip;

				int port;
				std::memcpy(&port, data + 127, sizeof(int));
				host_port_res = port;

				LOG_INFO << "IEC response : IP : " << ip << " ,Port : " << port ;

				std::string encryptKey(data + 151, 8);

				generateKeyandIV(encryptKey);

				LOG_INFO << "key :" << key;
				LOG_INFO << "iv : " << IV;

				socket.close();
				std::cout << "Close IEC Lookup Service connection." << std::endl;

				return true;
			}
			else 
			{
				std::cout << "IEC Lookup Response code: " << resCode << std::endl;
				return false;
			}
		}
	}
	catch (std::exception& e) {
		std::string errorMsg = e.what();
		if (errorMsg.find("10060") != std::string::npos)
		{
			FIN::setConsoleColor(4);
			std::cout << "Connection failed: Host did not respond or connection timed out (Error 10060)." << std::endl;
			FIN::setConsoleColor(7);
			LOG_INFO << "Connection failed: Host did not respond or connection timed out (Error 10060).";
			return false;
		}
		else
		{
			std::cout << "Exception Rcvd: " << e.what() << std::endl;
			LOG_INFO << "Exception Rcvd: " << e.what();
			return false;
		}
	}
	return false;
}

std::string ExchangeConnection::escapeNonPrintableChars(const std::string& input) {
	std::ostringstream escaped;
	for (unsigned char c : input) {
		if (std::isprint(c)) {
			escaped << c;  // Print printable characters as is
		}
		else {
			escaped << '\\' << 'x' << std::setw(2) << std::setfill('0') << std::hex << (int)c;
		}
	}
	return escaped.str();
}

void ExchangeConnection::generateKeyandIV(std::string EncryptKey)
{
	key.clear();
	IV.clear();
	for (int i = 0; i < (16 - EncryptKey.size()); i++)
	{
		key.push_back(password[i % password.size()]);
	}
	key += EncryptKey;

	for (int i = 0; i < 16; i++)
	{
		IV.push_back(password[i % password.size()]);
	}
	LOG_INFO << "Generate Key and IV";
}

void ExchangeConnection::connectSocket()
{
	try
	{
		if (m_socket)
			m_socket.reset();

		std::cout << "Connecting to IEC IP : " << host_ip_res << " : " << host_port_res << std::endl;
		LOG_INFO << "Connecting to IEC IP : " << host_ip_res << " : " << host_port_res;


		m_socket = boost::shared_ptr< boost::asio::ip::tcp::socket>(
			new boost::asio::ip::tcp::socket(_ioService));

		boost::asio::ip::tcp::resolver resolver(_ioService);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
			host_ip_res.c_str(),
			std::to_string(host_port_res));
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

		m_socket->connect(*iterator);
		m_socket->set_option(boost::asio::ip::tcp::no_delay(true));

		_connected = true;
		LOG_INFO << "socket Connected to IEC.";
	}
	catch (std::exception& ex)
	{
		_connected = false;
		std::cout << ex.what() << std::endl;
		LOG_INFO << std::string("Exception occur: ") + ex.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
	}
}

int ExchangeConnection::getFirstTwoByteLength(char* data)
{
	uint8_t byte1 = static_cast<uint8_t>(data[0]);		// Least significant byte
	uint8_t byte2 = static_cast<uint8_t>(data[1]);		// Most significant byte
	uint16_t byteLen = static_cast<uint16_t>(byte1 | (byte2 << 8));
	return byteLen;
}

void ExchangeConnection::receiveFromTarget()
{
	memset(_data, 0, 65535);
	
	size_t length = m_socket->read_some(boost::asio::buffer(_data, 65535));
	
	if (firstMsgReceive)
	{
		int pos = 0;
		while (pos < length)
		{
			int byteLen = getFirstTwoByteLength(_data + pos);
			std::string msg(_data + pos, byteLen + 2);
			LOG_INFO << "From Exchange size : " << length << " In packet length : " << byteLen;
			handleMessage(msg, byteLen);
			pos += byteLen + 2;
		}

		if (pos < length)
		{
			LOG_INFO << "read more byte, incomplete data receive";
			int readMore = length - pos;
			char _moreData[1024];
			std::memcpy(_moreData, _data + pos, readMore);

			int byteLen = getFirstTwoByteLength(_moreData);

			while (readMore < byteLen)
			{
				char buffer[1024];
				size_t bytesReceived = m_socket->read_some(boost::asio::buffer(buffer, sizeof(buffer)));
				std::memcpy(_moreData + readMore, buffer, bytesReceived);
				readMore += bytesReceived;
			}

			std::string msg(_moreData, byteLen + 2);
			handleMessage(msg, byteLen);
		}
	}
	else
	{
		firstMsgReceive = true;
		std::string msg(_data, length);
		handleMessage(msg, length);
	}
}

void ExchangeConnection::start()
{

}
void ExchangeConnection::stop()
{

}


void ExchangeConnection::scheduleHeartbeat()
{
}
void ExchangeConnection::closeSocket()
{
	if(m_socket!=nullptr)
		m_socket->close();
}

bool ExchangeConnection::sendToTarget(std::string str, size_t len)
{
	if (!m_socket->is_open())
		return  false;

	//ciphertext_len = performEncrypt((unsigned char*)str.c_str(), len);
	encrypt(reinterpret_cast<const unsigned char*>(str.c_str()), len, (unsigned char*)key.c_str(), (unsigned char*)IV.c_str(), ciphertext, ciphertext_len);
		

	std::vector<char> buffer(ciphertext_len + 2);

	// Encode the message length in little-endian format
	buffer[0] = static_cast<char>(ciphertext_len & 0xFF);       // Lower byte
	buffer[1] = static_cast<char>((ciphertext_len >> 8) & 0xFF); // Upper byte

	//copy encrypt data
	std::memcpy(buffer.data() + 2, ciphertext, ciphertext_len);

	//// Output the first two bytes of the buffer for verification
	//std::cout << "First byte: " << static_cast<int>(static_cast<unsigned char>(buffer[0])) << "\n";
	//std::cout << "Second byte: " << static_cast<int>(static_cast<unsigned char>(buffer[1])) << "\n";


	/*if (decrypt(ciphertext, ciphertext_len, (unsigned char*)key.c_str(), (unsigned char*)IV.c_str(), decryptedtext, decryptedtext_len)) {
		std::cout << "Decryption successful. Decrypted text: " << decryptedtext_len << " "
			<< std::string(reinterpret_cast<char*>(decryptedtext), decryptedtext_len) << "\n";
	}
	else {
		std::cout << "fails" << std::endl;
	}*/
	//decrypt
	//auto decryptedText_len = performDecrypt(((unsigned char*)ciphertext), (ciphertext_len));
	//char* resMsg;
	//resMsg = new char[decryptedText_len];
	//std::memcpy(resMsg, decryptedtext, decryptedText_len);

	//// Create and write to the file at the specified location
	//std::string filePath = "D:\\SVN_Finsol\\Code\\Gateway\\IIBX\\Order\\IIBX_Future_Order\\x64\\Debug\\logonRequest.txt";
	//// Create and write to the file at the specified location
	//std::ofstream outFile(filePath, std::ios::binary);
	//if (outFile.is_open()) {
	//	// Write the contents of the vector to the file
	//	outFile.write(buffer.data(), buffer.size());
	//	outFile.close();
	//	std::cout << "Buffer written to: " << filePath << std::endl;
	//}
	//else {
	//	std::cerr << "Unable to open file at specified location: " << filePath << std::endl;
	//}

	boost::system::error_code ec;
	auto sentBytes=boost::asio::write(*m_socket, boost::asio::buffer(buffer));

	if (ec)
	{
		std::cout << "Error: " << "Byte size : " << sentBytes << " " << ec.message() << std::endl;
		return false;
	}
	
	LOG_INFO << "[TO EXCHANGE] " << str;		
	return true;
}

//bool ExchangeConnection::initializeEncryption(const unsigned char* key, const unsigned char* iv) {
//
//	// Create a new EVP_CIPHER_CTX structure
//	encrypt = EVP_CIPHER_CTX_new();
//	if (!encrypt) {
//		std::cout << "Error creating EVP_CIPHER_CTX" << std::endl;
//		return false;
//	}
//
//	// Initialize a symmetric encryption operation with AES 128 CBC encryption algorithm
//	if (EVP_EncryptInit_ex(encrypt, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
//		// Error handling
//		std::cout << "Error in encrypt initialization EVP_EncryptInit_ex" << std::endl;
//		LOG_INFO << "Error in encrypt initialization EVP_EncryptInit_ex";
//		ERR_print_errors_fp(stderr);
//		EVP_CIPHER_CTX_free(encrypt);
//		encrypt = nullptr;
//		return false;
//	}
//
//	//// Set the IV length to 12 bytes (for AES-GCM with a non-standard IV length)
//	//if (EVP_CIPHER_CTX_ctrl(encrypt, EVP_CTRL_GCM_SET_IVLEN, 12, NULL) != 1) {
//	//	// Error handling
//	//	std::cerr << "Error setting IV length to 16 bytes in EVP_CIPHER_CTX_ctrl" << std::endl;
//	//	LOG_INFO << "Error setting IV length to 16 bytes in EVP_CIPHER_CTX_ctrl";
//	//	ERR_print_errors_fp(stderr);
//	//	EVP_CIPHER_CTX_free(encrypt);
//	//	encrypt = nullptr;
//	//	return false;
//	//}
//
//	//// Initialize a symmetric encryption operation with key and IV
//	//if (EVP_EncryptInit_ex(encrypt, NULL, NULL, key, iv) != 1) {
//	//	// Error handling
//	//	std::cout << "Error in encrypt initialization EVP_EncryptInit_ex" << std::endl;
//	//	LOG_INFO << "Error in encrypt initialization EVP_EncryptInit_ex";
//	//	ERR_print_errors_fp(stderr);
//	//	EVP_CIPHER_CTX_free(encrypt);
//	//	encrypt = nullptr;
//	//	return false;
//	//}
//
//	std::cout << "Encrypt Context initialized successfully" << std::endl;
//	LOG_INFO << "Encrypt Context initialized successfully";
//
//	return true;
//}
//
//int ExchangeConnection::performEncrypt(unsigned char* plaintext, int plaintext_len) {
//	int len;
//	ciphertext_len = 0;
//
//	// Perform encryption in chunks
//	if (1 != EVP_EncryptUpdate(encrypt, ciphertext, &len, plaintext, plaintext_len)) {
//		std::cout << "Error in encrypt EVP_EncryptUpdate" << std::endl;
//		LOG_INFO << "Error in encrypt EVP_EncryptUpdate";
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//	ciphertext_len += len;
//
//	// Finalize encryption (handles padding)
//	if (1 != EVP_EncryptFinal_ex(encrypt, ciphertext + ciphertext_len, &len)) {
//		std::cout << "Error in encrypt EVP_EncryptFinal_ex" << std::endl;
//		LOG_INFO << "Error in encrypt EVP_EncryptFinal_ex";
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//	ciphertext_len += len;
//
//	return ciphertext_len;
//}
//
//
//bool ExchangeConnection::initializeDecryption(const unsigned char* key, const unsigned char* iv) {
//	// Create a new EVP_CIPHER_CTX structure
//	decrypt = EVP_CIPHER_CTX_new();
//	if (!decrypt) {
//		std::cout << "Error creating EVP_CIPHER_CTX for decryption" << std::endl;
//		return false;
//	}
//
//	// Initialize a symmetric decryption operation with AES 128 GCM encryption algorithm
//	if (EVP_DecryptInit_ex(decrypt, EVP_aes_128_cbc(), NULL, key, iv) != 1) {
//		std::cout << "Error in decrypt EVP_DecryptInit_ex" << std::endl;
//		LOG_INFO << "Error in decrypt EVP_DecryptInit_ex";
//		ERR_print_errors_fp(stderr);
//		EVP_CIPHER_CTX_free(decrypt);
//		decrypt = nullptr;
//		return false;
//	}
//
//	//// Set the IV length to 12 bytes (for AES 128 GCM)
//	//if (EVP_CIPHER_CTX_ctrl(decrypt, EVP_CTRL_GCM_SET_IVLEN, 12, NULL) != 1) {
//	//	std::cout << "Error in decrypt EVP_CIPHER_CTX_ctrl" << std::endl;
//	//	LOG_INFO << "Error in decrypt EVP_CIPHER_CTX_ctrl";
//	//	ERR_print_errors_fp(stderr);
//	//	EVP_CIPHER_CTX_free(decrypt);
//	//	decrypt = nullptr;
//	//	return false;
//	//}
//
//	//// Initialize a symmetric decryption operation with key and IV
//	//if (EVP_DecryptInit_ex(decrypt, NULL, NULL, key, iv) != 1) {
//	//	std::cout << "Error in decrypt EVP_DecryptInit_ex" << std::endl;
//	//	LOG_INFO << "Error in decrypt EVP_DecryptInit_ex";
//	//	ERR_print_errors_fp(stderr);
//	//	EVP_CIPHER_CTX_free(decrypt);
//	//	decrypt = nullptr;
//	//	return false;
//	//}
//
//	std::cout << "Decrypt Context initialized successfully" << std::endl;
//	LOG_INFO << "Decrypt Context initialized successfully";
//
//	return true;
//}
//
//int ExchangeConnection::performDecrypt(unsigned char* ciphertext, int ciphertext_len) {
//	memset(decryptedtext, 0, sizeof(decryptedtext));
//
//	int len;
//	decryptedtext_len = 0;
//
//	// Perform decryption in chunks
//	if (1 != EVP_DecryptUpdate(decrypt, decryptedtext, &len, ciphertext, ciphertext_len)) {
//		std::cout << "Error in decrypt EVP_DecryptUpdate" << std::endl;
//		LOG_INFO << "Error in decrypt EVP_DecryptUpdate";
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//	decryptedtext_len += len;
//
//	// Finalize decryption (removes padding)
//	if (1 != EVP_DecryptFinal_ex(decrypt, decryptedtext + decryptedtext_len, &len)) {
//		std::cout << "Error in decrypt EVP_DecryptFinal_ex (padding error?)" << std::endl;
//		LOG_INFO << "Error in decrypt EVP_DecryptFinal_ex";
//		ERR_print_errors_fp(stderr);
//		return -1;
//	}
//	decryptedtext_len += len;
//
//	return decryptedtext_len;
//}
//
//



//
bool ExchangeConnection::encrypt(const unsigned char* plaintext, int plaintext_len, const unsigned char* key, const unsigned char* iv,
	unsigned char* ciphertext, int& ciphertext_len) {
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		std::cerr << "Failed to create cipher context\n";
		return false;
	}

	if (EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv) != 1) {
		std::cerr << "Failed to initialize encryption\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	int len = 0;
	if (EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len) != 1) {
		std::cerr << "Encryption failed\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	ciphertext_len = len;

	if (EVP_EncryptFinal_ex(ctx, ciphertext + len, &len) != 1) {
		std::cerr << "Final encryption step failed\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	ciphertext_len += len;

	EVP_CIPHER_CTX_free(ctx);
	return true;
}

bool ExchangeConnection::decrypt(const unsigned char* ciphertext, int ciphertext_len, const unsigned char* key, const unsigned char* iv,
	unsigned char* plaintext, int& plaintext_len) 
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if (!ctx) {
		std::cerr << "Failed to create cipher context\n";
		return false;
	}

	// Initialize decryption with the same key and IV as used in encryption
	if (EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, key, iv) != 1) {
		std::cerr << "Failed to initialize decryption\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}

	int len = 0;
	if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len) != 1) {
		std::cerr << "Decryption failed\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	plaintext_len = len;

	// Final decryption step - handling padding
	if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len) != 1) {
		std::cerr << "Final decryption step failed\n";
		EVP_CIPHER_CTX_free(ctx);
		return false;
	}
	plaintext_len += len;

	// Remove padding if necessary (PKCS7 padding)
	if (plaintext_len > 0 && plaintext[plaintext_len - 1] <= 16) {
		plaintext_len -= plaintext[plaintext_len - 1];  // Remove the padding bytes
	}

	EVP_CIPHER_CTX_free(ctx);
	return true;
}