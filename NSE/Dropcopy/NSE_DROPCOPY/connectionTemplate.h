#ifndef FIN_TEMPLATE_CONNECTION_H
#define FIN_TEMPLATE_CONNECTION_H

#include "connectionStatus.h"
#include "common.h"

#include <boost/asio.hpp>

namespace FIN
{

	class ConnectionTemplate
	{
	public:

		ConnectionTemplate(boost::asio::io_service &ioService);
		void init();
		void stop();
		void start();

	protected:

		void connectSocket();
		void setConnectionStatus(const ConnectionStatus status);
		ConnectionStatus getConnectionStatus() const;
		ConnectionParam& getConnectionParam()
		{
			return m_connectionParam;
		}
		virtual void startCommunication() = 0;

		boost::shared_ptr < boost::asio::ip::tcp::socket > m_socket;
	private:

		boost::asio::io_service& m_ioService;

		ConnectionParam m_connectionParam;
		ConnectionStatus m_connectionStatus;
	};
}

#endif
