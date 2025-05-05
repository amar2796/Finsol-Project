#include "ExchangeResponseProcess.h"
namespace FIN
{
	namespace IIBX
	{
		ExchangeResponseProcess::ExchangeResponseProcess()
		{
			ordResp = new OmsResponseOrder();
		}
		SIGNED_LONG ExchangeResponseProcess::strinToInt(std::string str)
		{
			int val = 0;
			if (str == "")
			{
				return val;
			}
			else
			{
				try
				{
					val = std::stoi(str);
				}
				catch (std::exception& e)
				{
					std::cout << "Exception Received:" << e.what();
					LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
				}
			}
			return val;
		}

		OmsResponseOrder* ExchangeResponseProcess::getOrderEntryConfirm(const std::string& resp)
		{
			ordResp->reset();

			ordResp->orderStatus = 0;

			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			int count = 0;
			while (getline(f, s, '|'))
			{
				if (count > 9)
				{
					vect.push_back(s);
				}
				count++;
			}
			try {
				strcpy(ordResp->orderNumber, vect[1].c_str());
				ordResp->quantity = stoi(vect[3]);
			}
			catch (std::exception& e) {
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
			}
			return ordResp;
		}
		OmsResponseOrder* ExchangeResponseProcess::getorderEntryReject(std::string& resp)
		{
			ordResp->reset();
			ordResp->orderStatus = 8;
			return ordResp;
		}
		OmsResponseOrder* ExchangeResponseProcess::getorderModConfirm(std::string& resp)
		{
			ordResp->reset();

			ordResp->orderStatus = 5;

			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try {
				strcpy(ordResp->orderNumber, vect[11].c_str());
				ordResp->price = stoll(vect[15]) * priceDevider;
				ordResp->quantity = stoi(vect[14]);
				ordResp->volumeRemaining = stoi(vect[13]);
			}
			catch (std::exception& e) {
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
			}

			return ordResp;
		}
		OmsResponseOrder* ExchangeResponseProcess::getorderModReject(std::string& resp)
		{
			ordResp->reset();
			ordResp->orderStatus = 9;
			return ordResp;
		}
		OmsResponseOrder* ExchangeResponseProcess::getorderCancelConfirm(std::string& resp)
		{
			ordResp->reset();

			ordResp->orderStatus = 4;

			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			int count = 0;
			while (getline(f, s, '|'))
			{
				if (count > 9)
				{
					vect.push_back(s);
				}
				count++;
			}
			try {
				std::strcpy(ordResp->orderNumber, vect[1].c_str());
				ordResp->quantity = stoi(vect[2]);
			}
			catch (std::exception& e) {
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);
			}

			return ordResp;
		}

		OmsResponseOrder* ExchangeResponseProcess::getorderCancelReject(std::string& resp)
		{
			ordResp->reset();
			ordResp->orderStatus = 9;
			return ordResp;
		}
		OmsResponseOrder* ExchangeResponseProcess::getTradeConfirm(std::string& resp)
		{
			ordResp->reset();

			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try
			{

				std::strcpy(ordResp->orderNumber, vect[5].c_str());
				std::strcpy(ordResp->tradeNumber, vect[6].c_str());

				ordResp->lastFilledQty = stoi(vect[8]);
				ordResp->price = stoll(vect[9]) * priceDevider;
			}
			catch (std::exception& e) 
			{
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);

			}
			return ordResp;
		}

		bool ExchangeResponseProcess::getLimitViolationNotification(std::string& resp, OmsResponseOrder& omsRes, std::string cmemcode)
		{
			omsRes.reset();
			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try
			{
				if (vect[4] == "N")
				{
					return false;
				}
				else
				{
					int status = std::stoi(vect[3]);
					if(status == 1)
						status = 31;		// for Limit Violation 31 = CM entered RRM
					else if(status == 2)
						status = 32;		//32 = CM entered LVM
					else if(status == 3)
						status = 33;		//33 = TM entered RRM
					else if(status == 4)
						status = 34;		//34 = TM entered LVM
					omsRes.orderStatus = status;

					std::strcpy(omsRes.tradeNumber, cmemcode.c_str());				//Clearing member code
				}
			}
			catch (std::exception& e)
			{
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);

			}
			return true;
		}

		OmsResponseOrder* ExchangeResponseProcess::getMemberNotification(std::string& resp)
		{
			ordResp->reset();
			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try
			{
				int status = std::stoi(vect[3]);
				if (status == 1)
					status = 11;		// for member 11 = Revoked / Active
				else if(status == 2)
					status = 12;		//12 = Suspended
				else if(status == 3)
					status = 13;		//13 = SquareOffMode
				ordResp->orderStatus = status;
			}
			catch (std::exception& e)
			{
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);

			}
			return ordResp;
		}

		OmsResponseOrder* ExchangeResponseProcess::getClientNotification(std::string& resp)
		{
			ordResp->reset();
			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try
			{
				int status = std::stoi(vect[4]);
				if (status == 1)
					status = 21;		// for client 21 = Revoked / Active
				else if (status == 2)
					status = 22;		//22 = Suspended
				else if (status == 3)
					status = 23;		//23 = SquareOffMode
				ordResp->orderStatus = status;

				std::string clientName = vect[3];
				size_t pos = clientName.find_last_not_of(' ');
				if (pos != std::string::npos) {
					clientName = clientName.substr(0, pos + 1);
				}
				std::strcpy(ordResp->tradeNumber, clientName.c_str());
			}
			catch (std::exception& e)
			{
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);

			}
			return ordResp;
		}

		OmsResponseOrder* ExchangeResponseProcess::getOrderkill(std::string& resp)
		{
			ordResp->reset();

			ordResp->orderStatus = 4;

			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try
			{
				std::strcpy(ordResp->orderNumber, vect[5].c_str());
				ordResp->quantity = stoi(vect[4]);
			}
			catch (std::exception& e)
			{
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);

			}
			return ordResp;
		}

		OmsResponseOrder* ExchangeResponseProcess::getSelfTrade(std::string& resp)
		{
			ordResp->reset();

			ordResp->orderStatus = 4;

			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try
			{
				std::strcpy(ordResp->orderNumber, vect[4].c_str());
				ordResp->quantity = stoi(vect[6]);
			}
			catch (std::exception& e)
			{
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);

			}
			return ordResp;
		}
		
		OmsResponseOrder* ExchangeResponseProcess::getStopLossDetail(std::string& resp)
		{
			ordResp->reset();

			std::vector<std::string> vect;
			std::istringstream f(resp.c_str());
			std::string s;
			while (getline(f, s, '|'))
			{
				vect.push_back(s);
			}
			try
			{
				if (vect[6] == "Y")
				{
					std::strcpy(ordResp->orderNumber, vect[4].c_str());
					ordResp->price = stoll(vect[5]);
				}
			}
			catch (std::exception& e)
			{
				std::cout << "Exception in Parse Body:" << e.what();
				LOG_INFO << std::string("Exception occur: ") + e.what() + " Function: " + __func__ + " File: " + __FILE__ + " Line: " + std::to_string(__LINE__);

			}
			return ordResp;
		}
	}
}