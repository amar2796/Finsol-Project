#include "handleData.h"

void FIN::HandleData::insertBuyDepth(const PRICE price, const UNSIGNED_INT_32 qty, int32_t pos)
{
	_md.insertBuyDepth(price, qty, pos);
}

void FIN::HandleData::insertSellDepth(const PRICE price, const UNSIGNED_INT_32 qty, int32_t pos)
{
	_md.insertSellDepth(price, qty, pos);
}

void FIN::HandleData::setLTP(const PRICE price)
{
	_md.setLtp(price);
}

void  FIN::HandleData::setOpenPrice(const PRICE open)
{
	_ohlc.setOpenPrice(open);
}

void  FIN::HandleData::setHighPrice(const PRICE high)
{
	_ohlc.setHighPrice(high);
}

void  FIN::HandleData::setClosePrice(const PRICE close)
{
	_ohlc.setClosePrice(close);
}

void  FIN::HandleData::setLowPrice(const PRICE low)
{
	_ohlc.setLowPrice(low);
}

void FIN::HandleData::setVolume(const SIGNED_INT_64 volume)
{
	_md.setVolume(volume);
}

void FIN::HandleData::setOpenInterest(const SIGNED_INT_64 volume)
{
	_md.setOpenInterest(volume);
}

//void FIN::HandleData::setUpperLimit(const PRICE uLimit)
//{
//	_dpr.setUpperLimit(uLimit);
//}
//
//void FIN::HandleData::setLowerLimit(const PRICE lLimit)
//{
//	_dpr.setLowerLimit(lLimit);
//}

void FIN::HandleData::sendToBroadcast(bool flag)
{
	if (flag)
	{
		_publisher.send(_md, sizeof(_md));
		_publisher.send(_ohlc, sizeof(_ohlc));
		//_publisher.send(_dpr, sizeof(_dpr));
	}
	
	return;
}