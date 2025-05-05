#include "ExchangeConnection.h"


ExchangeConnection::ExchangeConnection(const std::string& fileName, boost::asio::io_service& ioService)
	: _ioService(ioService)
{
	OpenSSL_add_all_algorithms();

	_configFileName = fileName;
	config_param.loadData(_currentPath.getCurrentPath() + FIN::FIN_CONST::FIN_CONFIG_FILE);
	host_ip = config_param.getValue<std::string>("SESSION.HostIP", "");
	host_port = config_param.getValue<int>("SESSION.HostPORT", 0);
	nic_ip = config_param.getValue<std::string>("SESSION.NicIP", "");



}


//void ExchangeConnection::initializeLib()
//{
//	OpenSSL_add_all_algorithms();
//
//	//ctx_encrypt = EVP_CIPHER_CTX_new(); // NULL;
//	//encrypt_EVP_aes_256_gcm_init(&ctx_encrypt, (unsigned char*)gatewayResponse.Key, (unsigned char*)gatewayResponse.IVVector);
//
//
//	//ctx_decrypt = EVP_CIPHER_CTX_new(); // NULL;
//	//decrypt_EVP_aes_256_gcm_init(&ctx_decrypt, (unsigned char*)gatewayResponse.Key, (unsigned char*)gatewayResponse.IVVector);
//
//	count += 1;
//}
void ExchangeConnection::encrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv)
{
	if (!(*ctx = EVP_CIPHER_CTX_new()))
	{
		std::cout << "Error in encrypt initialization " << std::endl;
		LOG_INFO << "Error in encrypt initialization  ";
		ERR_print_errors_fp(stderr);
	}
	if (1 != EVP_EncryptInit_ex(*ctx, EVP_aes_256_gcm(), NULL, key, iv))
	{
		std::cout << "Error in encrypt initialization EVP_EncryptInit_ex" << std::endl;
		LOG_INFO << "Error in encrypt initialization EVP_EncryptInit_ex";
		ERR_print_errors_fp(stderr);
	}
	//std::cout << "[INFO] Encrypt Context initialize " << std::endl;
}
void ExchangeConnection::encrypt(unsigned char* plaintext, int plaintext_len, unsigned char* ciphertext, int* ciphertext_len)
{
	//ctx_encrypt = EVP_CIPHER_CTX_new(); // NULL;
	encrypt_EVP_aes_256_gcm_init(&ctx_encrypt, (unsigned char*)gatewayResponse.Key, (unsigned char*)gatewayResponse.IVVector);

	// std::cout << "[INFO ] encrypt address " << (void *)ctx << " global " << (void *)ctx_encrypt << " value " << ctx_encrypt << std::endl;
	int len;
	if (1 != EVP_EncryptUpdate(ctx_encrypt, ciphertext, &len, plaintext, plaintext_len))
	{
		std::cout << "Error in encrypt  " << std::endl;
		LOG_INFO << "Error in encrypt  ";
		ERR_print_errors_fp(stderr); // Print OpenSSL error stack
	}
	*ciphertext_len = len;
	//std::cout << "Encrypted len " << len << " " << *ciphertext_len << std::endl;
}
void ExchangeConnection::decrypt_EVP_aes_256_gcm_init(EVP_CIPHER_CTX** ctx, unsigned char* key, unsigned char* iv)
{
	if (!(*ctx = EVP_CIPHER_CTX_new()))
	{
		std::cout << "Error in decrypt initialization " << std::endl;
		LOG_INFO << "Error in decrypt initialization  ";
		ERR_print_errors_fp(stderr);
	}

	if (1 != EVP_DecryptInit_ex(*ctx, EVP_aes_256_gcm(), NULL, key, iv))
	{
		std::cout << "Error in decrypt EVP_DecryptInit_ex " << std::endl;
		LOG_INFO << "Error in decrypt EVP_DecryptInit_ex  ";
		ERR_print_errors_fp(stderr);
	}
	//std::cout << "[INFO] Decrypt Context initialize " << std::endl;
	//LOG_INFO << "Decrypt Context initialize   ";
}
int ExchangeConnection::decrypt(unsigned char* ciphertext, int ciphertext_len)
{
	memset(decryptedtext, 0, 1000);
	decryptedtext_len = 0;
	/*if (count == 0)
		initializeLib();*/

	decrypt_EVP_aes_256_gcm_init(&ctx_decrypt, (unsigned char*)gatewayResponse.Key, (unsigned char*)gatewayResponse.IVVector);


	if (1 != EVP_DecryptUpdate(ctx_decrypt, decryptedtext, &decryptedtext_len, ciphertext, ciphertext_len))
	{
		std::cout << " [INFO] Error in decrypt  " << std::endl;
		LOG_INFO << "Error in decrypt  ";
		ERR_print_errors_fp(stderr); // Print OpenSSL error stack
	}

	//std::cout << "Decrypt len " << len << " " << decryptedtext_len << std::endl;

	return 0;
}






void ExchangeConnection::connectSocket()
{
	try
	{
		if (m_socket)
			m_socket.reset();

		std::cout << "[INFO ]Connecting to exch IP : " << host_ip
			<< " : " << host_port << std::endl;

		LOG_INFO << "Connecting to exch IP : " + host_ip + " : " + std::to_string(host_port);

		m_socket = boost::shared_ptr< boost::asio::ip::tcp::socket>(
			new boost::asio::ip::tcp::socket(_ioService));

		boost::asio::ip::tcp::resolver resolver(_ioService);
		boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(),
			host_ip.c_str(),
			std::to_string(host_port));
		boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);


		if (nic_ip != "")
		{
			boost::asio::ip::address_v4 interface_address = boost::asio::ip::address_v4::from_string(nic_ip);
			boost::asio::ip::tcp::endpoint endpoint(interface_address, 0);
			m_socket->open(endpoint.protocol());
			m_socket->bind(endpoint);
		}

		m_socket->connect(*iterator);
		m_socket->set_option(boost::asio::ip::tcp::no_delay(true));
	}
	catch (std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}

void ExchangeConnection::receiveFromTarget()
{

	memset(_data, 0, 65535);
	int dataRcvd = m_socket->read_some(boost::asio::buffer(_data, 65535));
	MessageHeaderOutCompT* header = nullptr;
	int pos = 0;
	int length = 0;

	while (pos < dataRcvd)
	{
		header = (MessageHeaderOutCompT*)(_data + pos);
		length = header->BodyLen;
		if (length <= 0)
		{
			LOG_INFO << " Packet length " << length << " total packet " << dataRcvd << " pos " << pos;

			int leftbyte = dataRcvd - pos;
			memcpy(_data, (char*)_data + pos, leftbyte);
			dataRcvd = leftbyte;
			dataRcvd += m_socket->read_some(boost::asio::buffer((char*)_data + dataRcvd, 65535 - leftbyte));
			pos = 0;
		}
		else if (pos + length > dataRcvd)
		{
			LOG_INFO << " Packet length is greater than  datarecvd len  " << length << " total packet " << dataRcvd << " pos " << pos;

			int needMoreBytes = (pos + length) - dataRcvd;
			memcpy(_data, (char*)_data + pos, (dataRcvd - pos));
			dataRcvd = dataRcvd - pos;
			dataRcvd += m_socket->read_some(boost::asio::buffer((char*)_data + dataRcvd, needMoreBytes));
			pos = 0;
			continue;

		}


		handleMessage((unsigned char*)(_data + pos), header->TemplateID, length);
		pos += header->BodyLen;

		//LOG_INFO << "TEMPLATE ID AND LEN PROCESSED " << header->BodyLen << " AND " << header->TemplateID;


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
	m_socket->close();

}
bool ExchangeConnection::sendToTarget(char* data, size_t len)
{


	if (!_connected) return  false;
	boost::system::error_code ec;

	auto buffer_Data = boost::asio::buffer(&data, len);

	auto sentBytes = boost::asio::write(*m_socket, buffer_Data, ec);
	std::cout << sentBytes << " " << ec.message() << std::endl;
	LOG_INFO << sentBytes << " " << ec.message();
	auto dadtad = (Derivatives::ConnectionGatewayRequestT*)buffer_Data.data();

	return false;
}
bool ExchangeConnection::sendToTarget(ConnectionGatewayRequestT data, size_t len)
{


	if (!_connected) return  false;
	boost::system::error_code ec;

	auto buffer_Data = boost::asio::buffer(&data, len);

	auto sentBytes = boost::asio::write(*m_socket, buffer_Data, ec);
	std::cout << sentBytes << " " << ec.message() << std::endl;
	LOG_INFO << sentBytes << " " << ec.message();
	auto dadtad = (Derivatives::ConnectionGatewayRequestT*)buffer_Data.data();

	return false;
}

bool ExchangeConnection::sendToTarget(boost::asio::mutable_buffers_1 _asio_buffer)
{
	if (!_connected) return  false;
	boost::system::error_code ec;

	auto sentBytes = boost::asio::write(*m_socket, _asio_buffer, ec);
	if (ec.value() == boost::system::errc::success)
	{
		return true;
	}
	return false;
}
bool ExchangeConnection::sendEncryptedTarget(unsigned char* request_buffer, int len)

{
	memset(ciphertext, 0, 128);
	ciphertext_len = 0;

	encrypt(request_buffer + 24, len - 24, ciphertext, &ciphertext_len);
	memmove(ciphertext + 24, ciphertext, /*ciphertext_len + 24*/len); // +1 for null terminator
	memcpy(ciphertext, request_buffer, 24);

	//std::cout << ciphertext_len << " header + encrypted  " << 24 + ciphertext_len << std::endl;

	//auto buffer_Data = boost::asio::buffer(&Logon_Req, Logon_Req.MessageHeaderIn.BodyLen);
	auto buffer_Data = boost::asio::buffer(ciphertext, ciphertext_len + 24);

	if (!_connected) return  false;
	boost::system::error_code ec;

	auto sentBytes = boost::asio::write(*m_socket, buffer_Data, ec);
	if (ec.value() == boost::system::errc::success)
	{
		return true;
	}
	return false;
}

