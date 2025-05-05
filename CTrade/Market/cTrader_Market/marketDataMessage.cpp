#include "marketDataMessage.h"
#include <iomanip>
#include <boost/log/trivial.hpp>

namespace FIN
{

	//void MarketData_10Depth::setDepthCount(const SIGNED_INT_16 count)
	//{
	//	DepthCount = count;
	//}
	//PRICE MarketData_10Depth::getBuyDepthPrice(const UNSIGNED_INT_16 pos) const
	//{
	//	return _buyDepth[pos].getPrice();
	//}
	//PRICE MarketData_10Depth::getSellDepthPrice(const UNSIGNED_INT_16 pos) const
	//{
	//	return _sellDepth[pos].getPrice();
	//}
	//UNSIGNED_INT_16 MarketData_10Depth::getBuyDepthQty(const UNSIGNED_INT_16 pos) const
	//{
	//	return _buyDepth[pos].getQty();
	//}
	//UNSIGNED_INT_16  MarketData_10Depth::getSellDepthQty(const UNSIGNED_INT_16 pos) const
	//{
	//	return _sellDepth[pos].getQty();
	//}

	///*!
	//  \public member function of MarketData_10Depth
	//  \delete the price and quantity for given price from buy depth.
	//  */
	//void MarketData_10Depth::deleteBuyDepth(const PRICE price)
	//{

	//	UNSIGNED_INT_16 pos = 0;
	//	if (getPositionForBuyPrice(price, pos))
	//	{
	//		for (int i = pos; i < DEPTH_SIZE_10 - 1; ++i)
	//			_buyDepth[i] = _buyDepth[i + 1];
	//		_buyDepth[DEPTH_SIZE_10 - 1].setPrice(0);
	//		_buyDepth[DEPTH_SIZE_10 - 1].setQty(0);

	//	}
	//}

	///*!
	//  \public member function of MarketData_10Depth
	//  \delete the price and quantity for given price from sell depth.
	//  */
	//void MarketData_10Depth::deleteSellDepth(const PRICE price)
	//{
	//	UNSIGNED_INT_16 pos = 0;
	//	if (getPositionForSellPrice(price, pos))
	//	{
	//		for (int i = pos; i < DEPTH_SIZE_10 - 1; ++i)
	//			_sellDepth[i] = _sellDepth[i + 1];
	//		_sellDepth[DEPTH_SIZE_10 - 1].setPrice(0);
	//		_sellDepth[DEPTH_SIZE_10 - 1].setQty(0);
	//	}
	//}


	///*!
	//  /public member function of MarketData_10Depth
	//  /updating buy ladder or depth by updating quantity at given price
	//  */

	//void MarketData_10Depth::updateBuyDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	//{
	//	UNSIGNED_INT_16 pos = 0;
	//	if (getPositionForBuyPrice(price, pos))
	//	{
	//		_buyDepth[pos].setPrice(price);
	//		_buyDepth[pos].setQty(qty);
	//	}
	//}



	///*!
	//  \public member function of MarketData_10Depth
	//  \updating sell ladder or depth of MarketData_10Depth by updating quantity at given price
	//  */
	//void MarketData_10Depth::updateSellDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	//{

	//	UNSIGNED_INT_16 pos = 0;
	//	if (getPositionForSellPrice(price, pos))
	//	{
	//		_sellDepth[pos].setPrice(price);
	//		_sellDepth[pos].setQty(qty);

	//	}
	//}




	///*!
	//  \public member function of MarketData_10Depth
	//  \shows values of market data
	//  */
	//void MarketData_10Depth::print() const
	//{
	//	//FIN_DEVLOG("***** MARKET_DATA_START ******");
	//	BroadCastMsgHeader::print();

	//	if (!checkBestBidAsk())
	//	{
	//		std::cout << "incorrect bid ask" << std::endl;
	//	}

	//	if (!checkCorrectnessBuy())
	//	{
	//		std::cout << "incorrect buy depth" << std::endl;
	//	}

	//	if (!checkCorrectnessSell())
	//	{
	//		std::cout << "incorrect sell depth" << std::endl;
	//	}
	//	std::cin.precision(2);

	//	FIN_DEVLOG("SecId : " << _secId
	//		<< std::endl << "LTP   : " << _ltp
	//		<< std::endl << "Volume: " << _volume);

	//	for (int i = 0; i < 10; ++i)
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



	//bool MarketData_10Depth::checkBestBidAsk() const
	//{
	//	return (((_sellDepth[0].getPrice() == 0 && _sellDepth[0].getQty() == 0) || (_buyDepth[0].getPrice() < _sellDepth[0].getPrice()))) ? true : false;
	//}

	//bool MarketData_10Depth::checkCorrectnessBuy() const
	//{
	//	for (int i = 1; i < DEPTH_SIZE_10; ++i)
	//	{
	//		if (_buyDepth[i].getPrice() != 0 && (_buyDepth[i].getPrice() >= _buyDepth[i - 1].getPrice()))
	//		{
	//			return false;
	//		}
	//		if (_buyDepth[i - 1].getQty() == 0 && _buyDepth[i].getQty() != 0)
	//		{
	//			return false;
	//		}
	//	}

	//	return true;
	//}

	//bool MarketData_10Depth::checkCorrectnessSell() const
	//{
	//	for (int i = 1; i < DEPTH_SIZE_10; ++i)
	//	{
	//		if (_sellDepth[i].getQty() != 0 && (_sellDepth[i].getPrice() <= _sellDepth[i - 1].getPrice()))
	//		{
	//			return false;
	//		}
	//		if (_sellDepth[i - 1].getQty() == 0 && _sellDepth[i].getQty() != 0)
	//		{
	//			return false;
	//		}
	//	}

	//	return true;
	//}



	///*!
	//  \public member function of MarketData_10Depth
	//  \insert new price and quantity into buy depth.
	//  */
	//void MarketData_10Depth::insertBuyDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	//{
	//	UNSIGNED_INT_16 pos = 0;
	//	if (!getPositionForBuyPrice(price, pos))
	//	{

	//		if (pos >= DEPTH_SIZE_10)
	//		{

	//			return;
	//		}
	//		for (int i = DEPTH_SIZE_10 - 1; i > pos; --i)
	//		{
	//			_buyDepth[i] = _buyDepth[i - 1];
	//		}

	//		_buyDepth[pos].setPrice(price);
	//		_buyDepth[pos].setQty(qty);

	//	}

	//}



	///*!
	//  \private memeber function of MarketData_10Depth
	//  \ return position for given price into buy depth.
	//  */
	//bool MarketData_10Depth::getPositionForBuyPrice(const PRICE price, UNSIGNED_INT_16& pos)
	//{

	//	bool found = false;

	//	for (; (!_buyDepth[pos].isEmpty() && pos < DEPTH_SIZE_10); ++pos)
	//	{
	//		if (price == _buyDepth[pos].getPrice())
	//		{
	//			found = true;
	//			break;
	//		}
	//		if (price > _buyDepth[pos].getPrice())
	//		{
	//			break;
	//		}
	//	}

	//	return found;
	//}



	///*!
	//  \public member function of MarketData_10Depth
	//  \insert new price and quantity into sell depth.
	//  */
	//void MarketData_10Depth::insertSellDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	//{
	//	UNSIGNED_INT_16 pos = 0;
	//	if (!getPositionForSellPrice(price, pos))
	//	{
	//		if (pos >= DEPTH_SIZE_10) return;
	//		for (int i = (DEPTH_SIZE_10 - 1); i > pos; --i)
	//			_sellDepth[i] = _sellDepth[i - 1];
	//		_sellDepth[pos].setPrice(price);
	//		_sellDepth[pos].setQty(qty);
	//	}
	//}



	///*!
	//  \private member function of MarketData_10Depth
	//  \return position for given price into sell depth.
	//  */
	//bool MarketData_10Depth::getPositionForSellPrice(const PRICE price, UNSIGNED_INT_16& pos)
	//{
	//	bool found = false;

	//	for (; (!_sellDepth[pos].isEmpty() && pos < DEPTH_SIZE_10); ++pos)
	//	{

	//		if (price == _sellDepth[pos].getPrice())
	//		{
	//			found = true;
	//			break;
	//		}
	//		if (price < _sellDepth[pos].getPrice())
	//		{
	//			break;
	//		}
	//	}

	//	return found;

	//}


	///*!
	//  \public member function of market data
	//  \update last trade price and quantity if any trade happenend.
	//  */
	//void MarketData_10Depth::applyTrade(const PRICE price)
	//{
	//	_ltp = price;
	//}

	//void MarketData_10Depth::deleteCompleteDepth()
	//{
	//	for (int i = 0; i < DEPTH_SIZE_10; i++)
	//	{
	//		_buyDepth[i].reset();
	//		_sellDepth[i].reset();
	//	}
	//}

	//void MarketData_10Depth::setSecId(const std::string& secId)
	//{
	//	memcpy(_secId, secId.c_str(), 25);
	//}

	//void MarketData_10Depth::setSecId(const char* secId)
	//{
	//	memcpy(_secId, secId, 25);
	//}

	//void MarketData_10Depth::setVolume(const SIGNED_INT_64 volume)
	//{
	//	_volume = volume;
	//}
	//void MarketData_10Depth::setLtp(const PRICE price)
	//{
	//	_ltp = price;
	//}
	//PRICE MarketData_10Depth::getLtp() const
	//{
	//	return _ltp;
	//}
	//std::string MarketData_10Depth::getSecId() const
	//{
	//	return std::string(_secId);
	//}
	//SIGNED_INT_64 MarketData_10Depth::getVolume() const
	//{
	//	return _volume;
	//}


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

	void MarketData_5Depth::updateBuyDepth(const PRICE price, const int64_t qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (getPositionForBuyPrice(price, pos))
		{
			_buyDepth[pos].setPrice(price);
			_buyDepth[pos].setQty(qty);
		}
		//sorting descending order
		std::sort(_buyDepth, _buyDepth + 5, [](const Depth& a, const Depth& b) {
			return a.getPrice() > b.getPrice();
			});
	}



	/*!
	  \public member function of MarketData_5Depth
	  \updating sell ladder or depth of MarketData_5Depth by updating quantity at given price
	  */
	void MarketData_5Depth::updateSellDepth(const PRICE price, const int64_t qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (getPositionForSellPrice(price, pos))
		{
			_sellDepth[pos].setPrice(price);
			_sellDepth[pos].setQty(qty);
		}

		//sort ascending order
		for (int i = 0; i < 5 - 1; ++i) {
			for (int j = 0; j < 5 - i - 1; ++j) {
				if (_sellDepth[j].getPrice() > _sellDepth[j + 1].getPrice() && _sellDepth[j + 1].getPrice() != 0) {
					// Swap elements if they are in the wrong order
					Depth temp = _sellDepth[j];
					_sellDepth[j] = _sellDepth[j + 1];
					_sellDepth[j + 1] = temp;
				}
			}
		}
	}




	/*!
	  \public member function of MarketData_5Depth
	  \shows values of market data
	  */
	void MarketData_5Depth::print() const
	{
		//FIN_DEVLOG("***** MARKET_DATA_START ******");
		BroadCastMsgHeader::print();

		if (!checkBestBidAsk())
		{
			std::cout << "incorrect bid ask" << std::endl;
		}

		if (!checkCorrectnessBuy())
		{
			std::cout << "incorrect buy depth" << std::endl;
		}

		if (!checkCorrectnessSell())
		{
			std::cout << "incorrect sell depth" << std::endl;
		}
		std::cin.precision(2);

		FIN_DEVLOG("SecId : " << _secId
			<< std::endl << "LTP   : " << _ltp
			<< std::endl << "Volume: " << _volume);

		for (int i = 0; i < 5; ++i)
		{
			FIN_DEVLOG(
				std::left << std::setw(10)
				<< _buyDepth[i].getQty()
				<< std::left << std::setw(10)
				<< _buyDepth[i].getPrice()
				<< std::left << std::setw(10)
				<< " | "
				<< std::left << std::setw(10)
				<< _sellDepth[i].getPrice()
				<< std::left << std::setw(10)
				<< _sellDepth[i].getQty()
				<< std::left << std::setw(10)
			);
		}
	}



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
	void MarketData_5Depth::insertBuyDepth(const PRICE price, const int64_t qty)
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
		//sorting descending order
		std::sort(_buyDepth, _buyDepth + 5, [](const Depth& a, const Depth& b) {
			return a.getPrice() > b.getPrice();
			});

	}



	/*!
	  \private memeber function of MarketData_5Depth
	  \ return position for given price into buy depth.
	  */
	bool MarketData_5Depth::getPositionForBuyPrice(const PRICE price, UNSIGNED_INT_16& pos)
	{
		bool found = false;
		for (int i=0; i<5; i++)
		{
			if (price == _buyDepth[i].getPrice())
			{
				found = true;
				pos = i;
				break;
			}
		}
		return found;
	}


	void MarketData_5Depth::deleteBuyDepthViaId(const PRICE price, UNSIGNED_INT_16 pos, const SIZE qty)
	{
		if (_buyDepth[pos].getQty() == qty)
		{
			deleteBuyDepth(price);
		}
		else if(_buyDepth[pos].getQty() > qty)
		{
			SIZE newqty = _buyDepth[pos].getQty();
			newqty -= qty;
			_buyDepth[pos].setQty(newqty);
		}
	}

	void MarketData_5Depth::deleteSellDepthViaId(const PRICE price, UNSIGNED_INT_16 pos, const SIZE qty)
	{
		if (_sellDepth[pos].getQty() == qty)
		{
			deleteSellDepth(price);
		}
		else if (_sellDepth[pos].getQty() > qty)
		{
			SIZE newqty = _sellDepth[pos].getQty();
			newqty -= qty;
			_sellDepth[pos].setQty(newqty);
		}
	}


	/*!
	  \public member function of MarketData_5Depth
	  \insert new price and quantity into sell depth.
	  */
	void MarketData_5Depth::insertSellDepth(const PRICE price, const int64_t qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (!getPositionForSellPrice(price, pos))
		{
			if (pos >= DEPTH_SIZE_5) 
				return;
			for (int i = (DEPTH_SIZE_5 - 1); i > pos; --i)
				_sellDepth[i] = _sellDepth[i - 1];

			_sellDepth[pos].setPrice(price);
			_sellDepth[pos].setQty(qty);
		}
		//sort ascending order
		for (int i = 0; i < 5 - 1; ++i) {
			for (int j = 0; j < 5 - i - 1; ++j) {
				if (_sellDepth[j].getPrice() > _sellDepth[j + 1].getPrice() && _sellDepth[j+1].getPrice()!=0) {
					// Swap elements if they are in the wrong order
					Depth temp = _sellDepth[j];
					_sellDepth[j] = _sellDepth[j + 1];
					_sellDepth[j + 1] = temp;
				}
			}
		}
	}



	/*!
	  \private member function of MarketData_5Depth
	  \return position for given price into sell depth.
	  */
	bool MarketData_5Depth::getPositionForSellPrice(const PRICE price, UNSIGNED_INT_16& pos)
	{
		bool found = false;
		for (int i = 0; i < 5; i++)
		{
			if (price == _sellDepth[i].getPrice())
			{
				found = true;
				pos = i;
				break;
			}
		}
		return found;
		/*bool found = false;

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

		return found;*/

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

	void MarketData_5Depth::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}

	void MarketData_5Depth::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}

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
	std::string MarketData_5Depth::getSecId() const
	{
		return std::string(_secId);
	}
	SIGNED_INT_64 MarketData_5Depth::getVolume() const
	{
		return _volume;
	}


	//! public member function
	// All Getters for BroadCastMsgHeader
	UNSIGNED_INT_16 BroadCastMsgHeader::getMsgLength() const
	{
		return _msgLength;
	}
	SIGNED_INT_8 BroadCastMsgHeader::getMsgType() const
	{
		return _msgType;
	}
	UNSIGNED_INT_16 BroadCastMsgHeader::getExchange() const
	{
		return _exchange;
	}

	SIGNED_INT_32 BroadCastMsgHeader::getUpdateTime() const
	{
		return _updateTime;
	}

	//BroadCastMsgHeader* BroadCastMsgHeader::getBroadCastMsgHeader() const
	//{
	//	return *this;
	//}

//All Setters for BroadCastMsgHeader

	void BroadCastMsgHeader::setMsgLength(const UNSIGNED_INT_16 msgLength)
	{
		_msgLength = msgLength;
	}

	void BroadCastMsgHeader::setMsgType(const SIGNED_INT_8 msgType)
	{
		_msgType = msgType;
	}

	void BroadCastMsgHeader::setExchange(const UNSIGNED_INT_16 exchange)
	{
		_exchange = exchange;
	}

	void BroadCastMsgHeader::setUpdateTime(const SIGNED_INT_32 time)
	{
		_updateTime = time;
	}

	void BroadCastMsgHeader::print() const
	{
		std::cout << "------ Header ------" << std::endl;
		std::cout << "MsgLength : " << _msgLength << std::endl;
		std::cout << "MsgType   : " << char(_msgType + 48) << std::endl;
		std::cout << "Exch      : " << _exchange << std::endl;
		std::cout << "UpdateTime: " << _updateTime << std::endl;
		std::cout << "------ Header ------" << std::endl;
	}

	/*!
	  /Public member function of Depth
	  */

	void Depth::print() const
	{
		std::cout.precision(2);
		std::cout << "Price : " << _price << std::endl;
		std::cout << "Qty : " << _qty << std::endl;
	}

	void Depth::reset()
	{
		_price = 0;
		_qty = 0;
	}

	bool Depth::isEmpty() const
	{
		return (_price == 0 && _qty == 0);
	}

	PRICE Depth::getPrice() const
	{
		return _price;
	}

	INT_32 Depth::getQty() const
	{
		return _qty;
	}

	void Depth::setPrice(const PRICE price)
	{
		_price = price;
	}

	void Depth::setQty(const INT_32 qty)
	{
		_qty = qty;
	}

	PRICE MarketData::getBuyDepthPrice(const UNSIGNED_INT_16 pos) const
	{
		return _buyDepth[pos].getPrice();
	}
	PRICE MarketData::getSellDepthPrice(const UNSIGNED_INT_16 pos) const
	{
		return _sellDepth[pos].getPrice();
	}
	UNSIGNED_INT_16 MarketData::getBuyDepthQty(const UNSIGNED_INT_16 pos) const
	{
		return _buyDepth[pos].getQty();
	}
	UNSIGNED_INT_16  MarketData::getSellDepthQty(const UNSIGNED_INT_16 pos) const
	{
		return _sellDepth[pos].getQty();
	}

	/*!
	  \public member function of MarketData
	  \delete the price and quantity for given price from buy depth.
	  */
	void MarketData::deleteBuyDepth(const PRICE price)
	{

		UNSIGNED_INT_16 pos = 0;
		if (getPositionForBuyPrice(price, pos))
		{
			for (int i = pos; i < DEPTH_SIZE_COMMON - 1; ++i)
				_buyDepth[i] = _buyDepth[i + 1];
			_buyDepth[DEPTH_SIZE_COMMON - 1].setPrice(0);
			_buyDepth[DEPTH_SIZE_COMMON - 1].setQty(0);

		}
	}

	/*!
	  \public member function of MarketData
	  \delete the price and quantity for given price from sell depth.
	  */
	void MarketData::deleteSellDepth(const PRICE price)
	{
		UNSIGNED_INT_16 pos = 0;
		if (getPositionForSellPrice(price, pos))
		{
			for (int i = pos; i < DEPTH_SIZE_COMMON - 1; ++i)
				_sellDepth[i] = _sellDepth[i + 1];
			_sellDepth[DEPTH_SIZE_COMMON - 1].setPrice(0);
			_sellDepth[DEPTH_SIZE_COMMON - 1].setQty(0);
		}
	}


	/*!
	  /public member function of MarketData
	  /updating buy ladder or depth by updating quantity at given price
	  */

	void MarketData::updateBuyDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (getPositionForBuyPrice(price, pos))
		{
			_buyDepth[pos].setPrice(price);
			_buyDepth[pos].setQty(qty);
		}
	}



	/*!
	  \public member function of MarketData
	  \updating sell ladder or depth of MarketData by updating quantity at given price
	  */
	void MarketData::updateSellDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	{

		UNSIGNED_INT_16 pos = 0;
		if (getPositionForSellPrice(price, pos))
		{
			_sellDepth[pos].setPrice(price);
			_sellDepth[pos].setQty(qty);

		}
	}




	/*!
	  \public member function of MarketData
	  \shows values of market data
	  */
	void MarketData::print() const
	{
		//FIN_DEVLOG("***** MARKET_DATA_START ******");
		BroadCastMsgHeader::print();

		if (!checkBestBidAsk())
		{
			std::cout << "incorrect bid ask" << std::endl;
		}

		if (!checkCorrectnessBuy())
		{
			std::cout << "incorrect buy depth" << std::endl;
		}

		if (!checkCorrectnessSell())
		{
			std::cout << "incorrect sell depth" << std::endl;
		}
		std::cin.precision(2);
		//FIN_DEVLOG("SecId : " << _secId
		//	<< std::endl << "LTP   : " << ltp
		//	<< std::endl << "Volume: " << volume
		//	<< std::endl << "OPEN  : " << open
		//	<< std::endl << "HIGH  : " << high
		//	<< std::endl << "LOW   : " << low
		//	<< std::endl << "CLOSE : " << close);

		//for ( int i = 0 ; i < 10 ; ++i )
		//{ 
		//	FIN_DEVLOG(
		//		std::left << std::setw(10)
		//		<< _buyDepth[i].getQty()
		//		<< std::left << std::setw(10)
		//		<< buyDepth[i].getPrice()
		//		<< std::left << std::setw(10)
		//		<< " | "
		//		<< std::left << std::setw(10)
		//		<< sellDepth[i].getPrice()
		//		<< std::left << std::setw(10)
		//		<< sellDepth[i].getQty()
		//		<< std::left << std::setw(10)
		//	);
		//}
		//FIN_DEVLOG("****** MARKET_DATA_END *******");

		FIN_DEVLOG("SecId : " << _secId
			<< std::endl << "LTP   : " << _ltp
			<< std::endl << "Volume: " << _volume);

		for (int i = 0; i < DEPTH_SIZE_COMMON; ++i)
		{
			FIN_DEVLOG(
				std::left << std::setw(10)
				<< _buyDepth[i].getQty()
				<< std::left << std::setw(10)
				<< _buyDepth[i].getPrice()
				<< std::left << std::setw(10)
				<< " | "
				<< std::left << std::setw(10)
				<< _sellDepth[i].getPrice()
				<< std::left << std::setw(10)
				<< _sellDepth[i].getQty()
				<< std::left << std::setw(10)
			);
		}
	}



	bool MarketData::checkBestBidAsk() const
	{
		return (((_sellDepth[0].getPrice() == 0 && _sellDepth[0].getQty() == 0) || (_buyDepth[0].getPrice() < _sellDepth[0].getPrice()))) ? true : false;
	}

	bool MarketData::checkCorrectnessBuy() const
	{
		for (int i = 1; i < DEPTH_SIZE_COMMON; ++i)
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

	bool MarketData::checkCorrectnessSell() const
	{
		for (int i = 1; i < DEPTH_SIZE_COMMON; ++i)
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
	  \public member function of marketData
	  \insert new price and quantity into buy depth.
	  */
	void MarketData::insertBuyDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (!getPositionForBuyPrice(price, pos))
		{

			if (pos >= DEPTH_SIZE_COMMON)
			{

				return;
			}
			for (int i = DEPTH_SIZE_COMMON - 1; i > pos; --i)
			{
				_buyDepth[i] = _buyDepth[i - 1];
			}

			_buyDepth[pos].setPrice(price);
			_buyDepth[pos].setQty(qty);

		}

	}



	/*!
	  \private memeber function of MarketData
	  \ return position for given price into buy depth.
	  */
	bool MarketData::getPositionForBuyPrice(const PRICE price, UNSIGNED_INT_16& pos)
	{

		bool found = false;

		for (; (!_buyDepth[pos].isEmpty() && pos < DEPTH_SIZE_COMMON); ++pos)
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
	  \public member function of marketData
	  \insert new price and quantity into sell depth.
	  */
	void MarketData::insertSellDepth(const PRICE price, const UNSIGNED_INT_16 qty)
	{
		UNSIGNED_INT_16 pos = 0;
		if (!getPositionForSellPrice(price, pos))
		{
			if (pos >= DEPTH_SIZE_COMMON) return;
			for (int i = (DEPTH_SIZE_COMMON - 1); i > pos; --i)
				_sellDepth[i] = _sellDepth[i - 1];
			_sellDepth[pos].setPrice(price);
			_sellDepth[pos].setQty(qty);
		}
	}



	/*!
	  \private member function of marketData
	  \return position for given price into sell depth.
	  */
	bool MarketData::getPositionForSellPrice(const PRICE price, UNSIGNED_INT_16& pos)
	{
		bool found = false;

		for (; (!_sellDepth[pos].isEmpty() && pos < DEPTH_SIZE_COMMON); ++pos)
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
	void MarketData::applyTrade(const PRICE price)
	{
		_ltp = price;
	}

	void MarketData::deleteCompleteDepth()
	{
		for (int i = 0; i < DEPTH_SIZE_COMMON; i++)
		{
			_buyDepth[i].reset();
			_sellDepth[i].reset();
		}
	}

	void MarketData::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}

	void MarketData::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}

	void MarketData::setVolume(const SIGNED_INT_64 volume)
	{
		_volume = volume;
	}
	void MarketData::setLtp(const PRICE price)
	{
		_ltp = price;
	}
	PRICE MarketData::getLtp() const
	{
		return _ltp;
	}
	std::string MarketData::getSecId() const
	{
		return std::string(_secId);
	}
	SIGNED_INT_64 MarketData::getVolume() const
	{
		return _volume;
	}

	void  OHLC::print() const
	{
		std::cout << "***** OHLC_START ******\n";
		std::cout << "secID : " << _secId;
		BroadCastMsgHeader::print();
		std::cout << "OpenPrice : " << _openPrice << "\n";
		std::cout << "HighPrice : " << _highPrice << "\n";
		std::cout << "ClosePrice: " << _closePrice << "\n";
		std::cout << "LowPrice  : " << _lowPrice << "\n";
		std::cout << "****** OHLC_END *******" << "\n";
	}
	void  OHLC::setOpenPrice(const PRICE open)
	{
		_openPrice = open;
	}
	void  OHLC::setHighPrice(const PRICE high)
	{
		_highPrice = high;
	}
	void  OHLC::setClosePrice(const PRICE close)
	{
		_closePrice = close;
	}
	void  OHLC::setLowPrice(const PRICE low)
	{
		_lowPrice = low;
	}
	void  OHLC::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	void  OHLC::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	PRICE OHLC::getOpenPrice() const
	{
		return _openPrice;
	}
	PRICE OHLC::getHighPrice() const
	{
		return _highPrice;
	}
	PRICE OHLC::getClosePrice() const
	{
		return _closePrice;
	}
	PRICE OHLC::getLowPrice() const
	{
		return _lowPrice;
	}
	std::string OHLC::getSecId() const
	{
		return std::string(_secId);
	}

	void DPR::print() const
	{
		std::cout.precision(2);
		std::cout << "upperLimit: " << _upperLimit;
		std::cout << "\nlowerLimit: " << _lowerLimit << "\n";

	}
	void DPR::setUpperLimit(const PRICE upper)
	{
		_upperLimit = upper;
	}
	void DPR::setLowerLimit(const PRICE lower)
	{
		_lowerLimit = lower;
	}
	void DPR::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	void DPR::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	PRICE DPR::getUpperLimit() const
	{
		return _upperLimit;
	}
	PRICE DPR::getLowerLimit() const
	{
		return _lowerLimit;
	}
	std::string DPR::getSecId() const
	{
		return std::string(_secId);
	}
	void MarketNotice::print() const
	{

	}
	void MarketNotice::setNoticeProducer(const SIGNED_INT_8 notice)
	{
		_noticeProducer = notice;
	}
	void MarketNotice::setMessage(const std::string& message)
	{
		memcpy(_message, message.c_str(), 250);
	}
	void MarketNotice::setMessage(const char* message)
	{
		memcpy(_message, message, 250);
	}
	SIGNED_INT_8 MarketNotice::getNoticeProducer() const
	{
		return _noticeProducer;
	}
	std::string MarketNotice::getMessage() const
	{
		return std::string(_message);
	}

	void SymbolNotice::print() const
	{
		std::cout << "***** SYMBOL_NOTICE_START ******\n";
		BroadCastMsgHeader::print();
		std::cout << "NoticeProducer: " << _noticeProducer << std::endl <<
			"SecurityID    : " << _secId << std::endl <<
			"Message       : " << _message << std::endl;
		std::cout << "****** SYMBOL_NOTICE_END *******\n";

	}
	void SymbolNotice::setNoticeProducer(const SIGNED_INT_8 notice)
	{
		_noticeProducer = notice;
	}
	void SymbolNotice::setSecId(const char* secId)
	{
		memcpy(_secId, secId, 25);
	}
	void SymbolNotice::setSecId(const std::string& secId)
	{
		memcpy(_secId, secId.c_str(), 25);
	}
	void SymbolNotice::setMessage(const std::string& message)
	{
		memcpy(_message, message.c_str(), 250);
	}
	void SymbolNotice::setMessage(const char* message)
	{
		memcpy(_message, message, 250);
	}
	SIGNED_INT_8 SymbolNotice::getNoticeProducer() const
	{
		return _noticeProducer;
	}
	std::string SymbolNotice::getSecId() const
	{
		return std::string(_secId);
	}
	std::string SymbolNotice::getMessage() const
	{
		return std::string(_message);
	}
	void ModuleConnectivity::print() const
	{
		std::cout << "***** MODULE_CONNECTIVITY_START ******\n";
		BroadCastMsgHeader::print();
		std::cout <<
			"ModuleType : " << getModuleTypeStr(_moduleType) << std::endl <<
			"ModuleState: " << getModuleStateStr(_moduleState) << std::endl;
		std::cout << "****** MODULE_CONNECTIVITY_END *******\n";
	}
	void ModuleConnectivity::setModuleType(const SIGNED_INT_8 moduletype)
	{
		_moduleType = moduletype;
	}
	void ModuleConnectivity::setModuleState(const SIGNED_INT_8 moduleState)
	{
		_moduleState = moduleState;
	}
	SIGNED_INT_8 ModuleConnectivity::getModuleType() const
	{
		return _moduleType;
	}
	SIGNED_INT_8 ModuleConnectivity::getModuleState() const
	{
		return _moduleState;
	}

	void SessionInfo::print() const
	{

	}
	void SessionInfo::setMarketType(const SIGNED_INT_8 marketType)
	{
		_marketType = marketType;
	}
	void SessionInfo::setMarketSession(const SIGNED_INT_8 session)
	{
		_marketSession = session;
	}
	SIGNED_INT_8 SessionInfo::getMarketType() const
	{
		return _marketType;
	}
	SIGNED_INT_8 SessionInfo::getMarketSession() const
	{
		return _marketSession;
	}
	namespace BROADCAST
	{


		MarketData::MarketData( const std::string& secId)
			: BroadCastMsgHeader(sizeof(MarketData), BroadCastMsgType_MarketData),
			ltp(0),
			volume(0)
		{
			memcpy(_secId, secId.c_str(), 25);
		}

		MarketData_default::MarketData_default( const std::string& secId)
			: BroadCastMsgHeader(sizeof(MarketData), BroadCastMsgType_MarketData),
			ltp(0),
			volume(0)
		{
			memcpy(_secId, secId.c_str(), 25);
		}



		void MarketData::print() const
		{
			std::cout << "***** MARKET_DATA_START ******\n";
			BroadCastMsgHeader::print();
			std::cout
				<< "SecurityID: " << _secId << "\n"
				<< "LTP: " << ltp
				<< " |V: " << volume << "\n"
				<< "LTT: " << lastTradeTime << "\n"
				<< "OI : " << openInterest << "\n"
				<< "DC :" << depthCount
				<< std::endl;
			for (int i = 0; i < this->depthCount; ++i)
			{
				std::cout
					<< std::left << std::setw(10)
					<< buyDepth[i].qty
					<< std::left << std::setw(10)
					<< buyDepth[i].price
					<< std::left << std::setw(10)
					<< " | "
					<< std::left << std::setw(10)
					<< sellDepth[i].price
					<< std::left << std::setw(10)
					<< sellDepth[i].qty
					<< std::left << std::setw(10)
					<< std::endl;
			}
			std::cout << "****** MARKET_DATA_END *******";// << std::endl;
		}

		void MarketData_default::print() const
		{
			std::cout << "***** MARKET_DATA_START ******\n";
			BroadCastMsgHeader::print();
			std::cout
				<< "SecurityID: " << _secId << "\n"
				<< "LTP: " << ltp
				<< " |V: " << volume << "\n"
				<< "LTT: " << lastTradeTime << "\n"
				<< "OI : " << openInterest
				<< std::endl;
			for (int i = 0; i < DEPTH_SIZE_DEFAULT; ++i)
			{
				std::cout
					<< std::left << std::setw(10)
					<< buyDepth[i].qty
					<< std::left << std::setw(10)
					<< buyDepth[i].price
					<< std::left << std::setw(10)
					<< " | "
					<< std::left << std::setw(10)
					<< sellDepth[i].price
					<< std::left << std::setw(10)
					<< sellDepth[i].qty
					<< std::left << std::setw(10)
					<< std::endl;
			}
			std::cout << "****** MARKET_DATA_END *******";// << std::endl;
		}

		std::string MarketData::getString() const
		{
			std::string mdAsString((char*)this, sizeof(MarketData));
			return mdAsString;
		}

		void MarketData::fromString(const std::string& s)
		{
			size_t size = sizeof(*this);
			if (s.size() <= size)
				size = s.size();
			memcpy(this, s.c_str(), size);
		}




		MarketData_default::Depth::Depth()
		{
			price = 0;
			qty = 0;
			//noOfOrders = 0;
		}
		MarketData::Depth::Depth()
		{
			price = 0;
			qty = 0;
			//noOfOrders = 0;
		}
		void MarketData::Depth::reset()
		{
			price = 0;
			qty = 0;
		}

		bool MarketData::Depth::isEmpty() const
		{
			return (price == 0 && qty == 0);
		}

		PRICE MarketData::Depth::getPrice() const
		{
			return price;
		}

		uint16_t MarketData::Depth::getQty() const
		{
			return qty;
		}

		void MarketData::Depth::print() const
		{
			std::cout << "Price : " << price << std::endl;
			std::cout << "Qty : " << qty << std::endl;
			//std::cout << "NoOfOrd : " << noOfOrders << std::endl;
		}
	}
}
