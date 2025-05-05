#ifndef FIN_COMMON_SBSCRPTION_HANDLER_H
#define FIN_COMMON_SBSCRPTION_HANDLER_H

#include "tcpServer.h"
#include "path.h"
#include "configReader.h"
#include "common.h"

namespace FIN {


	class securitySubscriptionHandler : public tcpServer
	{

	public:

		virtual void handlingSubscription(std::string) = 0;  // please give the defination in derived class.
		virtual void handlingUnSubscription(std::string) = 0;// please give the defination in derived class.

		void initTCP()
		{
			std::cout << "called initTCP \n";
			ConfigReader _confR;
			_confR.loadData(_currentPath.getCurrentPath()+FIN_CONST::FIN_CONFIG_FILE);

			std::string isPort = "PROCESS-MONITOR.Port";
			int port = _confR.getValue < int >(isPort, 0);
			std::cout << "port" << port << "\n";
			socketCreation(port);
		}

	private:


		void extractingSymbol(std::string msg)
		{
			std::map<std::string, std::string>fixMp;
			msg = msg.substr(10).c_str();
			size_t x;
			std::string str, st, symbol = "";
			while (1)
			{
				x = msg.find(1);
				if (x == -1)
				{
					break;
				}
				str = msg.substr(0, x).c_str();
				msg = msg.substr(x + 1).c_str();
				st = str.substr(0, str.find('=')).c_str();
				fixMp[st] = str.substr(st.length() + 1);

			}
			if (fixMp["263"] == "1")
			{
				symbol = symbol + fixMp["48"] + "," + fixMp["55"] + "," + fixMp["167"] + "," + fixMp["207"] + "," + fixMp["200"];
				handlingSubscription(symbol);
			}
			else if (fixMp["263"] == "2")
			{

				symbol = symbol + fixMp["48"] + "," + fixMp["55"] + "," + fixMp["167"] + "," + fixMp["207"] + "," + fixMp["200"];
				handlingUnSubscription(symbol);
			}

		}


		void readData(SOCKET &socket, char buffer[], int size)
		{
			try
			{
				 std::cout << "waiting to read from Process Monitor \n";
				 BOOST_LOG_TRIVIAL(info) << "waiting to read from Process Monitor";
				while (1)
				{
					int valread = recv(socket, buffer, size, 0);

					if (valread > 0)
					{

						extractingSymbol(std::string(buffer, valread));

					}
					else if (valread == 0)
					{
						acceptConnection();
					}
				}
			}
			catch (std::exception& e)
			{
				FIN_ERROR(e.what());
			}

		}

		CurrentPath _currentPath;
	};
}
#endif