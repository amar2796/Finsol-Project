#ifndef FIN_TEMPLATE_CONNECTION_H
#define FIN_TEMPLATE_CONNECTION_H

#include "connectionStatus.h"
#include "common.h"
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>
#include <boost/asio.hpp>

#ifndef TLS1_3_VERSION
#define TLS1_3_VERSION 0x0304
#endif
namespace FIN
{

	class ConnectionTemplate
	{
	public:

		ConnectionTemplate(boost::asio::io_service& ioService);
		void init();
		void stop();
		void start();
		bool isSSLComplted = false;
		bool isSecureBoxDone = false;

	protected:

		void connectSocket();
		void ConnectSSLSocket();
		void CloseSSLSocket();

		void CloseSocket();


		void setConnectionStatus(const ConnectionStatus status);
		ConnectionStatus getConnectionStatus() const;
		ConnectionParam& getConnectionParam()
		{
			return m_connectionParam;
		}
		virtual void startCommunication() = 0;

		boost::shared_ptr < boost::asio::ip::tcp::socket > m_socket;
		SSL* ssl;
		SSL_CTX* ctx;
		int sockfd;
	private:

		boost::asio::io_service& m_ioService;

		ConnectionParam m_connectionParam;
		ConnectionStatus m_connectionStatus;
	};
}

#endif
