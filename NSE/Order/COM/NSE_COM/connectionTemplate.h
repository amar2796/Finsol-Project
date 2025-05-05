#ifndef FIN_TEMPLATE_CONNECTION_H
#define FIN_TEMPLATE_CONNECTION_H
#pragma once

#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/evp.h>

#include <ws2tcpip.h>

#include "connectionStatus.h"
#include "common.h"
#include "configParameter.h"

#ifndef TLS1_3_VERSION
#define TLS1_3_VERSION 0x0304
#endif

namespace FIN
{

	class ConnectionTemplate
	{
	public:

		ConnectionTemplate(ConfigParameter* configDetails);

		bool isSSLComplted = false;
		bool isSecureBoxDone = false;

	protected:
		ConfigParameter* configDetails;
		bool ConnectSSLSocket();
		void CloseSSLSocket();
		ConnectionParam& getConnectionParam()
		{
			return m_connectionParam;
		}
		virtual void startCommunication() = 0;

		SSL* ssl;
		SSL_CTX* ctx;
		int sockfd;
	private:

		ConnectionParam m_connectionParam;
		ConnectionStatus m_connectionStatus;
	};
}

#endif
