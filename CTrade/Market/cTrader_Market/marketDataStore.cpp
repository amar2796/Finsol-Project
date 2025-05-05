#include "marketDataStore.h"


namespace FIN {
	namespace MDGtwy {

		UpdateRecord::UpdateRecord()
		{
			reset();
		}

		void UpdateRecord::reset()
		{
			secId = "";
			price = 0;
			qty = 0;
			//	entryPosition = 0 ;
			/*updateType = MDUpdateType_INVALID;
			updateAction = MDUpdateAction_INVALID;*/
			updateTime = 0;
		}

		PRICE UpdateRecord::getPrice() const
		{
			return price;
		}

		int32_t UpdateRecord::getMDenteryId() const
		{
			return MDEntryId;
		}

		SIZE UpdateRecord::getQty() const
		{
			return qty;
		}

		void UpdateRecord::print() const
		{
			std::cout << "------ UPDATE RECORD ------" << std::endl;
			std::cout << "Update Record SecId : " << secId << std::endl;
			//std::cout << "Action : " << updateAction << " " << getName(updateAction) << std::endl;
			//std::cout << "Type   : " << updateType << " " << getName(updateType) << std::endl;
			std::cout << "Price  : " << price << std::endl;
			std::cout << "Qty    : " << qty << std::endl;

		}

		void MarketDataStore::init(const uint16_t exch)
		{
			_exchangeName = exch;
			//_securityFileHandler.init();
		}

		void MarketDataStore::reset()
		{
			//_qUpdates.putToSleep();
			//_qUpdates.start();

			_mDataMap.clear();
			_registeredSymbols.clear();

			std::cout << "MD Store Reset Done" << std::endl;
		}

		void MarketDataStore::addUpdateRecords(const UpdateRecordsPtr ptr)
		{

			UpdateRecords::iterator itr = ptr->begin();
			for (; itr != ptr->end(); ++itr)
			{
				applyUpdate(*(*itr));
			}

			messageUpdatesProcessed();
		}

		void MarketDataStore::startProcessingData()
		{

			SymbolDataStore* s;
			//ProfTimer t;

			while (true)
			{
				Sleep(100000); continue;

			}
		}

		void MarketDataStore::applyUpdate(const UpdateRecord& uRec)
		{
			if (_mDataMap.find(uRec.secId) != _mDataMap.end())
			{
				_mDataMap[uRec.secId]->applyUpdate(uRec);
				_registeredSymbols.insert(_mDataMap[uRec.secId]);
			}
			else
			{
				//std::cout << "unsub ni hua " << uRec.secId << std::endl;
			}
		}

		bool MarketDataStore::addSymbol(std::vector<std::string>& securityId)
		{
			std::ostringstream oss;
			std::copy(securityId.begin(), securityId.end(), std::ostream_iterator<std::string>(oss, ","));
			std::string line = oss.str();
			if (!line.empty())
				line.erase(line.length() - 1);



			if (!_securityFileHandler.addSymbolForSubscription(line, securityId))
			{
				std::cout << "Symbol Already  Subscribed : " << securityId[0] << std::endl;
				//LOG_INFO << "Symbol Already  Subscribed : " << securityId[0];
				return false;
			}

			return true;
		}

		bool MarketDataStore::deleteSymbol(const std::string& symbol)
		{
			if (_securityFileHandler.symbolForUnSubscription(symbol))
			{
				_mDataMap.erase(symbol);
				return true;
			}
			return false;
		}

		bool MarketDataStore::isSymbolSubscribed(const std::string& symbol)
		{
			return _securityFileHandler.isSymbolSubscribed(symbol);
		}
		bool MarketDataStore::isValidSymbol(const std::string& symbol)
		{
			if (_securityFileHandler._filttered.empty())
				return true;
			else
				return _securityFileHandler._filttered.find(symbol) != _securityFileHandler._filttered.end();

			return false;
		}

		SymbolDataStore::SymbolDataStore( const std::string& secId)
			:_md( secId), _md5( secId), _mdSend( secId), _ohlc( secId), _md_send_default( secId)
		{
			//_md10.setDepthCount(depth);
		}

		void SymbolDataStore::print() const
		{
			_mdSend.print();
		}

		void SymbolDataStore::logInvalidMessgae(const UpdateRecord& uRec)
		{
			time_t t = time(NULL);
			std::cout << "[WARNING] Check for INVALID CASE in Logs at: " << t << std::endl;
			/*LOG_INFO << "[WARNING] Check for INVALID CASE in Logs at: " << t;
			LOG_INFO << "UpdateAction : " << getName(uRec.updateAction);
			LOG_INFO << "UpdateType   : " << getName(uRec.updateType);
			LOG_INFO << "SecId        : " << uRec.secId;
			LOG_INFO << "Price        : " << uRec.price;
			LOG_INFO << "Qty          : " << uRec.qty;*/
		}

		void SymbolDataStore::applyUpdate(const UpdateRecord& uRec)
		{
			switch (uRec.updateAction)
			{
			case MDUpdateAction_NEW: applyNew(uRec); break;
			//case MDUpdateAction_CHANGE: applyChange(uRec); break;
			case MDUpdateAction_DELETE: applyDelete(uRec); break;
			default: logInvalidMessgae(uRec);
			}

			

			// Now update time is not important 

			/*if (_ohlcUpdated)
				_ohlc.setUpdateTime(uRec.updateTime);
			else
				_md.setUpdateTime(uRec.updateTime);*/
		}

		void SymbolDataStore::applyNew(const UpdateRecord& uRec)
		{
			switch (uRec.updateType)
			{
			case MDUpdateType_BID:
			{
				bool isNew = isPriceNew(uRec.getPrice(), 'B');
				addUpdateNormalQty(uRec.getMDenteryId(), uRec.getPrice(), uRec.getQty(), 'B');

				if (!isNew)
				{
						_md5.insertBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));
				}
				else
				{
						_md5.updateBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));
				}
			}
			break;
			case MDUpdateType_OFFER:
			{
				bool isNew = isPriceNew(uRec.getPrice(), 'S');
				addUpdateNormalQty(uRec.getMDenteryId(), uRec.getPrice(), uRec.getQty(), 'S');

				if (!isNew)
				{
						_md5.insertSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));
				}
				else
				{
						_md5.updateSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));

				}
			}
			break;
			/*case MDUpdateType_IMPLIED_BID:
			{
				bool isNew = isPriceNew(uRec.getPrice(), 'B');
				addUpdateImpliedQty(uRec.getPrice(), uRec.getQty(), 'B');

				if (isNew)
				{
						_md5.insertBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));

				}
				else
				{
						_md5.updateBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));


				}

			}
			break;
			case MDUpdateType_IMPLIED_OFFER:
			{
				bool isNew = isPriceNew(uRec.getPrice(), 'S');
				addUpdateImpliedQty(uRec.getPrice(), uRec.getQty(), 'S');

				if (isNew)
				{
						_md5.insertSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));


				}
				else
				{
						_md5.updateSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));


				}
			}
			break;
			case MDUpdateType_TRADE:
				
					_md5.applyTrade(uRec.getPrice());


				break;

			case MDUpdateType_OPEN: _ohlc.setOpenPrice(uRec.getPrice()); _ohlcUpdated = true; break;
			case MDUpdateType_CLOSE: _ohlc.setClosePrice(uRec.getPrice()); _ohlcUpdated = true; break;
			case MDUpdateType_HIGH: _ohlc.setHighPrice(uRec.getPrice()); _ohlcUpdated = true; break;
			case MDUpdateType_LOW: _ohlc.setLowPrice(uRec.getPrice()); _ohlcUpdated = true; break;

			case MDUpdateType_VOLUME:
				
					_md5.setVolume(uRec.getQty());
				break;*/

			default: logInvalidMessgae(uRec);
			}
		}


		/*void SymbolDataStore::applyChange(const UpdateRecord& uRec)
		{
			switch (uRec.updateType)
			{
			case MDUpdateType_BID:
			{
				addUpdateNormalQty(uRec.getPrice(), uRec.getQty(), 'B');

				
					_md5.updateBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));

			}break;
			case MDUpdateType_OFFER:
			{
				addUpdateNormalQty(uRec.getPrice(), uRec.getQty(), 'S');
				
					_md5.updateSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));


			}break;
			case MDUpdateType_IMPLIED_BID:
			{
				addUpdateImpliedQty(uRec.getPrice(), uRec.getQty(), 'B');
	
					_md5.updateBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));


			}break;
			case MDUpdateType_IMPLIED_OFFER:
			{
				addUpdateImpliedQty(uRec.getPrice(), uRec.getQty(), 'S');
				
					_md5.updateSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));


			}
			break;
			case MDUpdateType_TRADE:
				
					_md5.applyTrade(uRec.getPrice());


				break;
			case MDUpdateType_OPEN: _ohlc.setOpenPrice(uRec.getPrice()); _ohlcUpdated = true; break;
			case MDUpdateType_CLOSE: _ohlc.setClosePrice(uRec.getPrice()); _ohlcUpdated = true; break;
			case MDUpdateType_HIGH: _ohlc.setHighPrice(uRec.getPrice()); _ohlcUpdated = true; break;
			case MDUpdateType_LOW: _ohlc.setLowPrice(uRec.getPrice()); _ohlcUpdated = true; break;

			case MDUpdateType_VOLUME:
				
					_md5.setVolume(uRec.getQty());
				break;


			default: logInvalidMessgae(uRec);
			}
		}*/

		void SymbolDataStore::applyDelete(const UpdateRecord& uRec)
		{
			int32_t id=uRec.MDEntryId;
			checkBuySellViaId(id);
			//switch (uRec.updateType)
			//{
			//case MDUpdateType_BID:
			//{

			//	deleteNormalQty(uRec.getMDenteryId(), 'B');
			//	/*bool isNew = isPriceNew(uRec.getPrice(), 'B');
			//	if (isNew)
			//	{
			//			_md5.deleteBuyDepth(uRec.getPrice());
			//	}
			//	else
			//	{
			//			_md5.updateBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));

			//	}*/
			//}
			//break;
			//case MDUpdateType_OFFER:
			//{
			//	deleteNormalQty(uRec.getMDenteryId(), 'S');

			//	/*bool isNew = isPriceNew(uRec.getPrice(), 'S');
			//	if (isNew)
			//	{
			//			_md5.deleteSellDepth(uRec.getPrice());
			//	}
			//	else
			//	{
			//			_md5.updateSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));

			//	}*/
			//}
			//break;
			//case MDUpdateType_IMPLIED_BID:
			//{
			//	deleteImpliedQty(uRec.getPrice(), 'B');

			//	bool isNew = isPriceNew(uRec.getPrice(), 'B');
			//	if (isNew)
			//	{
			//			_md5.deleteBuyDepth(uRec.getPrice());


			//	}
			//	else
			//	{
			//			_md5.updateBuyDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'B'));


			//	}
			//}
			//break;
			//case MDUpdateType_IMPLIED_OFFER:
			//{
			//	deleteImpliedQty(uRec.getPrice(), 'S');

			//	bool isNew = isPriceNew(uRec.getPrice(), 'S');
			//	if (isNew)
			//	{
			//			_md5.deleteSellDepth(uRec.getPrice());


			//	}
			//	else
			//	{
			//			_md5.updateSellDepth(uRec.getPrice(), getTotalQtyForPrice(uRec.getPrice(), 'S'));


			//	}
			//}
			//break;
			//case MDUpdateType_TRADE: break;//_md.applyTrade(0);  
			//case MDUpdateType_OPEN: break;//_md.open   = 0   ; 
			//case MDUpdateType_CLOSE: break;//_md.close  = 0   ; 
			//case MDUpdateType_HIGH: break;//_md.high   = 0   ; 
			//case MDUpdateType_LOW: break;//_md.low    = 0   ; 
			//case MDUpdateType_VOLUME: break;//_md.volume = 0   ; 
			//default: logInvalidMessgae(uRec);
			//}
		}
	}
}
