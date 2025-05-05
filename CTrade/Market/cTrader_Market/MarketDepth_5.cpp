#include "MarketDepth_5.h"

using namespace FIN;
PRICE MarketData_5Depth::getBuyDepthPrice(const UNSIGNED_INT_16 pos) const
{
	return _buyDepth[pos].getPrice();
}
PRICE MarketData_5Depth::getSellDepthPrice(const UNSIGNED_INT_16 pos) const
{
	return _sellDepth[pos].getPrice();
}
UNSIGNED_INT_16 MarketData_5Depth::getBuyDepthQty(const UNSIGNED_INT_16 pos) const
{
	return _buyDepth[pos].getQty();
}
UNSIGNED_INT_16  MarketData_5Depth::getSellDepthQty(const UNSIGNED_INT_16 pos) const
{
	return _sellDepth[pos].getQty();
}

/*!
  \public member function of MarketData_5Depth
  \delete the price and quantity for given price from buy depth.
  */
void MarketData_5Depth::deleteBuyDepth(const PRICE price)
{

	UNSIGNED_INT_16 pos = 0;
	if (getPositionForBuyPrice(price, pos))
	{
		for (int i = pos; i < DEPTH_SIZE_5 - 1; ++i)
			_buyDepth[i] = _buyDepth[i + 1];
		_buyDepth[DEPTH_SIZE_5 - 1].setPrice(0);
		_buyDepth[DEPTH_SIZE_5 - 1].setQty(0);

	}
}

/*!
  \public member function of MarketData_5Depth
  \delete the price and quantity for given price from sell depth.
  */
void MarketData_5Depth::deleteSellDepth(const PRICE price)
{
	UNSIGNED_INT_16 pos = 0;
	if (getPositionForSellPrice(price, pos))
	{
		for (int i = pos; i < DEPTH_SIZE_5 - 1; ++i)
			_sellDepth[i] = _sellDepth[i + 1];
		_sellDepth[DEPTH_SIZE_5 - 1].setPrice(0);
		_sellDepth[DEPTH_SIZE_5 - 1].setQty(0);
	}
}


/*!
  /public member function of MarketData_5Depth
  /updating buy ladder or depth by updating quantity at given price
  */

void MarketData_5Depth::updateBuyDepth(const PRICE price, const UNSIGNED_INT_16 qty)
{
	UNSIGNED_INT_16 pos = 0;
	if (getPositionForBuyPrice(price, pos))
	{
		_buyDepth[pos].setPrice(price);
		_buyDepth[pos].setQty(qty);
	}
}



/*!
  \public member function of MarketData_5Depth
  \updating sell ladder or depth of MarketData_5Depth by updating quantity at given price
  */
void MarketData_5Depth::updateSellDepth(const PRICE price, const UNSIGNED_INT_16 qty)
{

	UNSIGNED_INT_16 pos = 0;
	if (getPositionForSellPrice(price, pos))
	{
		_sellDepth[pos].setPrice(price);
		_sellDepth[pos].setQty(qty);

	}
}




/*!
  \public member function of MarketData_5Depth
  \shows values of market data
  */
//void MarketData_5Depth::print() const
//{
//	//FIN_DEVLOG("***** MARKET_DATA_START ******");
//	BroadCastMsgHeader::print();
//
//	if (!checkBestBidAsk())
//	{
//		std::cout << "incorrect bid ask" << std::endl;
//	}
//
//	if (!checkCorrectnessBuy())
//	{
//		std::cout << "incorrect buy depth" << std::endl;
//	}
//
//	if (!checkCorrectnessSell())
//	{
//		std::cout << "incorrect sell depth" << std::endl;
//	}
//	std::cin.precision(2);
//
//	FIN_DEVLOG("SecId : " << _secId
//		<< std::endl << "LTP   : " << _ltp
//		<< std::endl << "Volume: " << _volume);
//
//	for (int i = 0; i < 5; ++i)
//	{
//		FIN_DEVLOG(
//			std::left << std::setw(10)
//			<< _buyDepth[i].getQty()
//			<< std::left << std::setw(10)
//			<< _buyDepth[i].getPrice()
//			<< std::left << std::setw(10)
//			<< " | "
//			<< std::left << std::setw(10)
//			<< _sellDepth[i].getPrice()
//			<< std::left << std::setw(10)
//			<< _sellDepth[i].getQty()
//			<< std::left << std::setw(10)
//		);
//	}
//}



bool MarketData_5Depth::checkBestBidAsk() const
{
	return (((_sellDepth[0].getPrice() == 0 && _sellDepth[0].getQty() == 0) || (_buyDepth[0].getPrice() < _sellDepth[0].getPrice()))) ? true : false;
}

bool MarketData_5Depth::checkCorrectnessBuy() const
{
	for (int i = 1; i < DEPTH_SIZE_5; ++i)
	{
		if (_buyDepth[i].getPrice() != 0 && (_buyDepth[i].getPrice() >= _buyDepth[i - 1].getPrice()))
		{
			return false;
		}
		if (_buyDepth[i - 1].getQty() == 0 && _buyDepth[i].getQty() != 0)
		{
			return false;
		}
	}

	return true;
}

bool MarketData_5Depth::checkCorrectnessSell() const
{
	for (int i = 1; i < DEPTH_SIZE_5; ++i)
	{
		if (_sellDepth[i].getQty() != 0 && (_sellDepth[i].getPrice() <= _sellDepth[i - 1].getPrice()))
		{
			return false;
		}
		if (_sellDepth[i - 1].getQty() == 0 && _sellDepth[i].getQty() != 0)
		{
			return false;
		}
	}

	return true;
}



/*!
  \public member function of MarketData_5Depth
  \insert new price and quantity into buy depth.
  */
void MarketData_5Depth::insertBuyDepth(const PRICE price, const UNSIGNED_INT_16 qty)
{
	UNSIGNED_INT_16 pos = 0;
	if (!getPositionForBuyPrice(price, pos))
	{

		if (pos >= DEPTH_SIZE_5)
		{

			return;
		}
		for (int i = DEPTH_SIZE_5 - 1; i > pos; --i)
		{
			_buyDepth[i] = _buyDepth[i - 1];
		}

		_buyDepth[pos].setPrice(price);
		_buyDepth[pos].setQty(qty);

	}

}


/*!
  \private memeber function of MarketData_5Depth
  \ return position for given price into buy depth.
  */
bool MarketData_5Depth::getPositionForBuyPrice(const PRICE price, UNSIGNED_INT_16& pos)
{

	bool found = false;

	for (; (!_buyDepth[pos].isEmpty() && pos < DEPTH_SIZE_5); ++pos)
	{
		if (price == _buyDepth[pos].getPrice())
		{
			found = true;
			break;
		}
		if (price > _buyDepth[pos].getPrice())
		{
			break;
		}
	}

	return found;
}



/*!
  \public member function of MarketData_5Depth
  \insert new price and quantity into sell depth.
  */
void MarketData_5Depth::insertSellDepth(const PRICE price, const UNSIGNED_INT_16 qty)
{
	UNSIGNED_INT_16 pos = 0;
	if (!getPositionForSellPrice(price, pos))
	{
		if (pos >= DEPTH_SIZE_5) return;
		for (int i = (DEPTH_SIZE_5 - 1); i > pos; --i)
			_sellDepth[i] = _sellDepth[i - 1];
		_sellDepth[pos].setPrice(price);
		_sellDepth[pos].setQty(qty);
	}
}



/*!
  \private member function of MarketData_5Depth
  \return position for given price into sell depth.
  */
bool MarketData_5Depth::getPositionForSellPrice(const PRICE price, UNSIGNED_INT_16& pos)
{
	bool found = false;

	for (; (!_sellDepth[pos].isEmpty() && pos < DEPTH_SIZE_5); ++pos)
	{

		if (price == _sellDepth[pos].getPrice())
		{
			found = true;
			break;
		}
		if (price < _sellDepth[pos].getPrice())
		{
			break;
		}
	}

	return found;

}


/*!
  \public member function of market data
  \update last trade price and quantity if any trade happenend.
  */
void MarketData_5Depth::applyTrade(const PRICE price)
{
	_ltp = price;
}

void MarketData_5Depth::deleteCompleteDepth()
{
	for (int i = 0; i < DEPTH_SIZE_5; i++)
	{
		_buyDepth[i].reset();
		_sellDepth[i].reset();
	}
}

//void MarketData_5Depth::setSecId(const std::string& secId)
//{
//	memcpy(_secId, secId.c_str(), 25);
//}
//
//void MarketData_5Depth::setSecId(const char* secId)
//{
//	memcpy(_secId, secId, 25);
//}

void MarketData_5Depth::setVolume(const SIGNED_INT_64 volume)
{
	_volume = volume;
}
void MarketData_5Depth::setLtp(const PRICE price)
{
	_ltp = price;
}
PRICE MarketData_5Depth::getLtp() const
{
	return _ltp;
}
//std::string MarketData_5Depth::getSecId() const
//{
//	return std::string(_secId);
//}
SIGNED_INT_64 MarketData_5Depth::getVolume() const
{
	return _volume;
}
