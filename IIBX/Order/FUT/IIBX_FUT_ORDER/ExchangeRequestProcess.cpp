#include "ExchangeRequestProcess.h"
namespace FIN {
	ExchangeRequestProcess::ExchangeRequestProcess()
	{
	}

	//msgType=Message Type, seqNum=Numeric Sequence of the Message, cMemCode=Clearing Member Code
	//tMemCode=Trading Member Code, ctclTCode=CTCL Terminal Id
	std::string ExchangeRequestProcess::getMessageHeaderForLogon(std::string msgType, int seqNum, std::string cMemCode,
		std::string tMemCode, std::string dCode, std::string ctclTCode)
	{
		std::string header;
		header.append("IIBX_EXCH_CTCL").append(FIN::PIPE);
		header.append("1.0.1").append(FIN::PIPE);
		header.append(msgType).append(FIN::PIPE);
		header.append(std::to_string(seqNum)).append(FIN::PIPE);
		header.append(cMemCode).append(FIN::PIPE);
		header.append(tMemCode).append(FIN::PIPE);
		header.append(dCode).append(FIN::PIPE);
		header.append(ctclTCode).append(FIN::PIPE);
		return header;
	}

	std::string ExchangeRequestProcess::getMessageHeaderOnly(std::string msgType, int seqNum, std::string cMemCode,
		std::string tMemCode, std::string dCode, std::string ctclTCode)
	{
		std::string header;
		header.append("IIBX_EXCH_CTCL").append(FIN::PIPE);
		header.append("1.0.1").append(FIN::PIPE);
		header.append(msgType).append(FIN::PIPE);
		header.append(std::to_string(seqNum)).append(FIN::PIPE);
		header.append(cMemCode).append(FIN::PIPE);
		header.append(tMemCode).append(FIN::PIPE);
		header.append(dCode).append(FIN::PIPE);
		header.append(ctclTCode);
		return header;
	}

	std::string ExchangeRequestProcess::getMessageHeader(std::string msgType, int seqNum, 
		std::string tMemCode, std::string dCode, std::string ctclTCode)
	{
		std::string header;
		header.append("IIBX_EXCH_CTCL").append(FIN::PIPE);
		header.append("1.0.1").append(FIN::PIPE);
		header.append(msgType).append(FIN::PIPE);
		header.append(std::to_string(seqNum)).append(FIN::PIPE);
		header.append(_fixStringParser.get(FIN::Fix_SenderCompID)).append(FIN::PIPE);
		header.append(tMemCode).append(FIN::PIPE);
		header.append(dCode).append(FIN::PIPE);
		header.append(ctclTCode).append(FIN::PIPE);
		return header;
	}

	std::string ExchangeRequestProcess::getLoginRequest(std::string& msg, std::string& password)
	{	
		msg.append(password);
		return msg;
	}	
	
	std::string ExchangeRequestProcess::getNewOrderReq(std::string& msg, int seq)
	{
		double price = 0;
		msg.append(_fixStringParser.get(FIN::Fix_Symbol)).append(FIN::PIPE);		//contract symbol

		msg.append(_fixStringParser.get(FIN::Fix_OrdQty)).append(FIN::PIPE);					//order qty

		auto orderType = std::stoi(_fixStringParser.get(Fix_OrdType));
			
		if (orderType == Fix_OrdType_Market)								//order price
			msg.append("0").append(FIN::PIPE);
		else
		{
			price = std::stod(_fixStringParser.get(FIN::Fix_Price));
			price = price / priceDevider;
			msg.append(std::to_string(price)).append(FIN::PIPE);
		}

		if (orderType == Fix_OrdType_stop_loss)								//triger price	
		{
			double triggerprice = std::stod(_fixStringParser.get(FIN::Fix_TriggerPrice));
			triggerprice = triggerprice / priceDevider;
			msg.append(std::to_string(triggerprice)).append(FIN::PIPE);					//Trigger price
		}
		else
		{
			msg.append("0").append(FIN::PIPE);
		}

		msg.append(_fixStringParser.get(FIN::Fix_Account)).append(FIN::PIPE);					//client code

		std::string clientCode = _fixStringParser.get(FIN::Fix_SenderCompID);
		if (clientCode == "OWN" || clientCode == "PRO" || clientCode == "Own")			//client type
			msg.append("O").append(FIN::PIPE);					//O=own
		else 
			msg.append("N").append(FIN::PIPE);					//N=normal	

		if (orderType == Fix_OrdType_stop_loss)											//order type
			msg.append("S").append(FIN::PIPE);
		else if (orderType == Fix_OrdType_Market)
			msg.append("M").append(FIN::PIPE);
		else
			msg.append("L").append(FIN::PIPE);

		if (orderType == Fix_OrdType_Market)				//order retention 
			msg.append("IOC").append(FIN::PIPE);
		else
			msg.append("GFD").append(FIN::PIPE);

		msg.append("1").append(FIN::PIPE);			//order day 

		if (_fixStringParser.get(FIN::Fix_Side) == "1")			//Buy/sell 
			msg.append("B");
		else
			msg.append("S");	

		storeSeqNumber[seq - 1] = { std::stoi(_fixStringParser.get(FIN::Fix_ClOrdID)),  std::stoi(std::to_string(price)) };
		return msg;
	}
	
	std::string ExchangeRequestProcess::getModifyOrderReq(std::string& msg, int seq)
	{
		double price = 0;
		int oldQty = 0;
		msg.append(_fixStringParser.get(FIN::Fix_Symbol)).append(FIN::PIPE);		//contract symbol

		if (isAvailableOrderId(_fixStringParser.get(FIN::Fix_OrderID)))				//order qty
		{
			Order& orderDetail = getOrder(_fixStringParser.get(FIN::Fix_OrderID));
			oldQty = orderDetail.originalqty;
			int newQty = std::stoi(_fixStringParser.get(FIN::Fix_OrdQty));
			newQty -= oldQty;
			msg.append(std::to_string(newQty)).append(FIN::PIPE);
		}

		auto orderType = std::stoi(_fixStringParser.get(Fix_OrdType));				//order price
		if (orderType == Fix_OrdType_Market)
			msg.append("0").append(FIN::PIPE);
		else
		{
			price = std::stoll(_fixStringParser.get(FIN::Fix_Price));
			price = price / priceDevider;
			msg.append(std::to_string(price)).append(FIN::PIPE);			 //Price
		}

		if (orderType == Fix_OrdType_stop_loss)											//triger price	
		{
			double triggerprice = std::stod(_fixStringParser.get(FIN::Fix_TriggerPrice));
			triggerprice = triggerprice / priceDevider;
			msg.append(std::to_string(triggerprice)).append(FIN::PIPE);					
		}
		else
		{
			msg.append("0").append(FIN::PIPE);
		}

		msg.append(_fixStringParser.get(FIN::Fix_OrderID)).append(FIN::PIPE);			//order id
							
		msg.append(_fixStringParser.get(FIN::Fix_Account)).append(FIN::PIPE);			//client code

		std::string clientCode = _fixStringParser.get(FIN::Fix_SenderCompID);
		if (clientCode == "OWN" || clientCode == "PRO" || clientCode == "Own")			//client type
			msg.append("O").append(FIN::PIPE);					//O=own
		else
			msg.append("N").append(FIN::PIPE);					//N=normal	

		if (orderType == Fix_OrdType_stop_loss)											//order type
			msg.append("S").append(FIN::PIPE);
		else if (orderType == Fix_OrdType_Market)
			msg.append("M").append(FIN::PIPE);
		else
			msg.append("L").append(FIN::PIPE);

		if (orderType == Fix_OrdType_Market)				//order retention 
			msg.append("IOC").append(FIN::PIPE);
		else
			msg.append("GFD").append(FIN::PIPE);

		msg.append("1").append(FIN::PIPE);				//order day 

		if (_fixStringParser.get(FIN::Fix_Side) == "1")			//Buy/sell 
			msg.append("B");
		else
			msg.append("S");

		storeSeqNumber[seq - 1] = { std::stoi(_fixStringParser.get(FIN::Fix_ClOrdID)),  std::stoi(std::to_string(price)) };

		return msg;
	}	
	
	std::string ExchangeRequestProcess::getCancelOrderReq(std::string& msg, int seq)
	{
		double price = 0;

		msg.append(_fixStringParser.get(FIN::Fix_Symbol)).append(FIN::PIPE);		//contract symbol

		msg.append(_fixStringParser.get(FIN::Fix_OrderID)).append(FIN::PIPE);			//order id

		auto orderType = std::stoi(_fixStringParser.get(Fix_OrdType));
		if (orderType == Fix_OrdType_stop_loss)											//order type
			msg.append("S").append(FIN::PIPE);
		else if (orderType == Fix_OrdType_Market)
			msg.append("M").append(FIN::PIPE);
		else
			msg.append("L").append(FIN::PIPE);

		if (_fixStringParser.get(FIN::Fix_Side) == "1")				//Buy/sell 
			msg.append("B");
		else
			msg.append("S");

		price = std::stoll(_fixStringParser.get(FIN::Fix_Price));
		price = price / priceDevider;

		storeSeqNumber[seq - 1] = { std::stoi(_fixStringParser.get(FIN::Fix_ClOrdID)), std::stoi(std::to_string(price)) };

		return msg;
	}

	bool ExchangeRequestProcess::processPendingOrderHistory(std::string msg, OmsResponseOrder& omsResp)
	{
		omsResp.reset();

		std::vector<std::string> vect;
		std::istringstream f(msg.c_str());
		std::string s;
		while (getline(f, s, '|'))
		{
			vect.push_back(s);
		}
		if (std::strcmp(vect[0].c_str(), "Y") == 0)
		{
			return true;
		}
		int ordType;
		if (vect[5] == "B")
			ordType = 1;
		else
			ordType = 2;

		omsResp.orderStatus = 0;
		std::strcpy(omsResp.orderNumber, vect[7].c_str());
		omsResp.quantity = std::stoi(vect[2]);
		omsResp.volumeRemaining = std::stoi(vect[3]);
		omsResp.price = std::stoi(vect[4]) * priceDevider;
		omsResp.orderType = ordType;

		return false;
	}

	bool ExchangeRequestProcess::processReturnOrderHistory(std::string msg, OmsResponseOrder& omsResp)
	{
		omsResp.reset();

		std::vector<std::string> vect;
		std::istringstream f(msg.c_str());
		std::string s;
		while (getline(f, s, '|'))
		{
			vect.push_back(s);
		}
		if (std::strcmp(vect[0].c_str(), "Y") == 0)
		{
			return true;
		}
		int ordType;
		if (vect[5] == "B")
			ordType = 1;
		else
			ordType = 2;

		omsResp.orderStatus = 4;
		std::strcpy(omsResp.orderNumber, vect[7].c_str());
		omsResp.quantity = std::stoi(vect[2]);
		omsResp.volumeRemaining = (std::stoi(vect[2]) - std::stoi(vect[3]));
		omsResp.price = std::stoi(vect[4]) * priceDevider;
		omsResp.orderType = ordType;
		std::memcpy(omsResp.errorMessage, vect[12].c_str(), sizeof(vect[12]));

		return false;
	}

	bool ExchangeRequestProcess::processReturnStopLossOrder(std::string msg, OmsResponseOrder& omsResp)
	{
		omsResp.reset();

		std::vector<std::string> vect;
		std::istringstream f(msg.c_str());
		std::string s;
		while (getline(f, s, '|'))
		{
			vect.push_back(s);
		}
		if (std::strcmp(vect[0].c_str(), "Y") == 0)
		{
			return true;
		}
		int ordType;
		if (vect[5] == "B")
			ordType = 1;
		else
			ordType = 2;

		omsResp.orderStatus = 4;
		std::strcpy(omsResp.orderNumber, vect[7].c_str());
		omsResp.quantity = std::stoi(vect[2]);
		omsResp.volumeRemaining = (std::stoi(vect[2]) - std::stoi(vect[3]));
		omsResp.price = std::stoi(vect[4]) * priceDevider;
		omsResp.orderType = ordType;
		std::memcpy(omsResp.errorMessage, vect[12].c_str(), sizeof(vect[12]));

		return false;
	}

	bool ExchangeRequestProcess::processTradeOrderHistory(std::string msg, OmsResponseOrder& omsResp)
	{
		omsResp.reset();

		std::vector<std::string> vect;
		std::istringstream f(msg.c_str());
		std::string s;
		while (getline(f, s, '|'))
		{
			vect.push_back(s);
		}
		if (std::strcmp(vect[0].c_str(), "Y") == 0)
		{
			return true;
		}
		int ordType;
		if (vect[11] == "B")
			ordType = 1;
		else
			ordType = 2;

		//tradeQty[vect[6]] = std::stoi(vect[4]);

		omsResp.orderStatus = 1;
		std::strcpy(omsResp.orderNumber, vect[6].c_str());
		omsResp.quantity = std::stoi(vect[4]);
		omsResp.volumeFilledToday = std::stoi(vect[4]);
		omsResp.price = std::stoi(vect[5]) * priceDevider;
		omsResp.orderType = ordType;
		std::strcpy(omsResp.tradeNumber, vect[2].c_str());

		return false;
	}

	bool ExchangeRequestProcess::processStopLossOrderHistory(std::string msg, OmsResponseOrder& omsResp)
	{
		omsResp.reset();

		std::vector<std::string> vect;
		std::istringstream f(msg.c_str());
		std::string s;
		while (getline(f, s, '|'))
		{
			vect.push_back(s);
		}
		if (std::strcmp(vect[0].c_str(), "Y") == 0)
		{
			return true;
		}
		int ordType;
		if (vect[6] == "B")
			ordType = 1;
		else
			ordType = 2;

		omsResp.orderStatus = 0;
		std::strcpy(omsResp.orderNumber, vect[8].c_str());
		omsResp.quantity = std::stoi(vect[2]);
		omsResp.volumeRemaining = std::stoi(vect[3]);
		omsResp.price = std::stoi(vect[4]) * priceDevider;
		omsResp.orderType = ordType;

		return false;
	}

	bool ExchangeRequestProcess::processMarketPictureBBOHistory(std::string msg, OmsResponseOrder& omsResp, std::unordered_map<std::string, int>& contractClosePrice)
	{
		omsResp.reset();

		std::vector<std::string> vect;
		std::istringstream f(msg.c_str());
		std::string s;
		while (getline(f, s, '|'))
		{
			vect.push_back(s);
		}
		if (std::strcmp(vect[0].c_str(), "Y") == 0)
		{
			return true;
		}
		int ordType;
		if (vect[6] == "B")
			ordType = 1;
		else
			ordType = 2;

		omsResp.orderStatus = 0;
		std::strcpy(omsResp.orderNumber, vect[8].c_str());
		omsResp.quantity = std::stoi(vect[2]);
		omsResp.volumeRemaining = std::stoi(vect[3]);
		omsResp.price = std::stoi(vect[4]) * priceDevider;
		omsResp.orderType = ordType;

		//store close price
		contractClosePrice[vect[1]] = std::stod(vect[7]) * priceDevider;

		return false;
	}

	std::string ExchangeRequestProcess::getCurrentDate()
	{
		std::time_t t = std::time(nullptr);

		std::tm* now = std::localtime(&t);

		std::ostringstream oss;
		oss << std::put_time(now, "%Y%m%d");

		return oss.str();
	}

	std::string ExchangeRequestProcess::getCurrentDateAndTime()
	{
		std::time_t now = std::time(nullptr);

		std::tm* local_time = std::localtime(&now);

		std::stringstream date_time;
		date_time << std::put_time(local_time, "%d-%m-%Y %H:%M:%S");

		return date_time.str();
	}
}