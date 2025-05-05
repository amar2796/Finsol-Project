#ifndef FIN_MD_MARKET_DATA_STORE
#define FIN_MD_MARKET_DATA_STORE

#include <unordered_set>
#include "commons.h"
#include "marketDataMessage.h"	
#include "threadSafeQueue.h"	
#include "configReader.h"
#include "lockfreeQueue.h"
#include "publisher.h"
#include <boost/lockfree/spsc_queue.hpp>
//#include "../MarketDepth_5.h"
//#include "../MarketDepth_10.h"


namespace FIN {
	namespace MDGtwy {

		struct ProfTimer {
			void Start(void) {
				QueryPerformanceCounter(&mTimeStart);
			};
			void Stop(void) {
				QueryPerformanceCounter(&mTimeStop);
			};
			double GetDurationInSecs(void)
			{
				LARGE_INTEGER freq;
				QueryPerformanceFrequency(&freq);
				double duration = (double)(mTimeStop.QuadPart - mTimeStart.QuadPart) / (double)freq.QuadPart;
				return duration;
			}

			LARGE_INTEGER mTimeStart;
			LARGE_INTEGER mTimeStop;
		};
		struct UpdateRecord
		{
			UpdateRecord();
			UpdateRecord(const UpdateRecord& rhs)
			{
				this->secId = rhs.secId;
				this->price = rhs.price;
				this->qty = rhs.qty;
				this->updateType = rhs.updateType;
				this->updateAction = rhs.updateAction;
				this->updateTime = rhs.updateTime;
				this->MDEntryId = rhs.MDEntryId;
			}

			void reset();
			void print() const;
			SIZE getQty() const;
			PRICE getPrice() const;
			int32_t getMDenteryId() const;

			//int getPosition() const;
			std::string secId;
			PRICE price;
			SIZE qty;
			//int entryPosition;
			MDUpdateType updateType;
			MDUpdateAction updateAction;
			int32_t updateTime;
			int32_t MDEntryId;
		};

		//#define __LOCK__MD__ std::unique_lock < std::mutex > lock(_mutex);
#define __LOCK__MD__ ;// std::lock_guard < std::mutex > l(_mutex);
#define __UN__LOCK__MD__ ;

		typedef std::shared_ptr < UpdateRecord > UpdateRecordPtr;
		typedef std::vector < UpdateRecordPtr > UpdateRecords;
		typedef std::shared_ptr < UpdateRecords > UpdateRecordsPtr;

		class SymbolDataStore;
		typedef LockFreeQueue < SymbolDataStore* > UpdateQueue;
		typedef LockFreeQueue < BROADCAST::MarketData > UpdateQueueMD;

		typedef boost::lockfree::spsc_queue<SymbolDataStore*, boost::lockfree::capacity<1024>> spsc_queue;

		class SymbolDataStore
		{

		public:

			SymbolDataStore(const std::string& secId);
			void print() const;
			short variable_depth_count = 0;
			void applyUpdate(const UpdateRecord&);
			long long t = 0;
			BROADCAST::MarketData& getDataToSend(short count)
			{
				__LOCK__MD__

					//BROADCAST::MarketData md( std::move(_mdSend) );
					//_isUpdated = false;

					__UN__LOCK__MD__
					_mdSend.copyMD(_md, count);
				return _mdSend;
			}
			BROADCAST::MarketData_default& getDataToSend()
			{
				_md_send_default.copyMD(_md);
				return _md_send_default;
			}
			bool isOhlcUpdated() const { return _ohlcUpdated; }

			OHLC getOHLC()
			{
				_ohlcUpdated = false;
				return _ohlc;
			}

			void updateSendForSymbol(UpdateQueue& q, short count)
			{

				__LOCK__MD__

					if (!_isUpdated)
					{
						_mdSend.copyMD(_md, count);
						q.push(this);
						_isUpdated = true;
					}
			}

			void setSymbol(const std::string& symbol)
			{
				/////std::cout << "In Symbol: " << symbol << std::endl;
				///memset(_mdSend._symbol, 0, 5);
				///
				///size_t copySize = symbol.size();
				///if (copySize > 5)
				///	copySize = 5;
				///
				///memcpy( _mdSend._symbol , symbol.c_str() , copySize);

			}

			void setExpiry(const uint32_t expiry)
			{
				//////_md._expiryYearMon = expiry;
				////_mdSend._expiryYearMon = expiry;
			}

			void setExchange(const uint16_t exch)
			{
				//_md._exchange = exch;
				_mdSend.setExchange(exch);
				_ohlc.setExchange(exch);
			}

			//private :

			void logInvalidMessgae(const UpdateRecord&);

			void applyNew(const UpdateRecord&);
			void applyChange(const UpdateRecord&);
			void applyDelete(const UpdateRecord&);
			void applyTrade(const UpdateRecord&);
			void dumpOHLInFile(PRICE, PRICE, PRICE);

			MarketData _md;
			MarketData_5Depth _md5;
			//MarketData_10Depth _md10;

			//MarketData_10Depth _md10;
			BROADCAST::MarketData _mdSend;
			BROADCAST::MarketData_default	 _md_send_default;

			OHLC _ohlc;
			bool _ohlcUpdated = false;



			std::mutex _mutex;
			bool _isUpdated = false;

			//typedef std::unordered_map < INT32, std::pair<PRICE, SIZE >> ImpliedPriceQtyMap;
			typedef std::unordered_map < INT32, std::pair<PRICE, SIZE > > NormalPriceQtyMap;
			//typedef std::unordered_map < INT32, std::pair<PRICE, SIZE > >::iterator ImpliedPriceQtyMapItr;
			typedef std::unordered_map < INT32, std::pair<PRICE, SIZE > >::iterator NormalPriceQtyMapItr;

			//ImpliedPriceQtyMap _bmapImpliedPriceQty;
			//ImpliedPriceQtyMap _smapImpliedPriceQty;
			NormalPriceQtyMap  _bmapNormalPriceQty;
			NormalPriceQtyMap  _smapNormalPriceQty;

			//ImpliedPriceQtyMapItr _mapImpliedPriceQtyItr;
			NormalPriceQtyMapItr  _mapNormalPriceQtyItr;

			/*void addUpdateImpliedQty(const INT32 MDentryID, const SIZE qty, const char UpdateType)
			{
				if (UpdateType == 'B')
					_bmapImpliedPriceQty[MDentryID] = qty;
				else
					_smapImpliedPriceQty[MDentryID] = qty;
			}*/

			void addUpdateNormalQty(const INT32 MDentryID, const PRICE price, const SIZE qty, const char UpdateType)
			{
				if (UpdateType == 'B')
					_bmapNormalPriceQty.insert(std::make_pair(MDentryID, std::make_pair(price, qty)));
				else
					_smapNormalPriceQty.insert(std::make_pair(MDentryID, std::make_pair(price, qty)));
			}

			/*void deleteImpliedQty(const PRICE price, const char UpdateType)
			{
				if (UpdateType == 'B')
					_bmapImpliedPriceQty.erase(price);
				else
					_smapImpliedPriceQty.erase(price);
			}*/

			void checkBuySellViaId(int32_t MDEntryId)
			{
				auto it = _bmapNormalPriceQty.find(MDEntryId);
				if (it != _bmapNormalPriceQty.end())
				{
					deleteNormalQty(MDEntryId, 'B');
				}
				else 
				{
					deleteNormalQty(MDEntryId, 'S');
				}
			}

			void deleteNormalQty(const int32_t MDentryID, const char UpdateType)
			{
				if (UpdateType == 'B')
				{
					bool temp = false;
					UNSIGNED_INT_16 pos=0;

					PRICE newPrice;
					SIZE newQty;
					for (const auto& pair : _bmapNormalPriceQty) {
						if (pair.first == MDentryID) {
							newPrice = pair.second.first;
							newQty = pair.second.second;
							temp = true;
							break;
						}
					}
					if (temp)
					{
						if (_md5.getPositionForBuyPrice(newPrice, pos))
						{
							_md5.deleteBuyDepthViaId(newPrice, pos, newQty);
						}
					}
					_bmapNormalPriceQty.erase(MDentryID);
				}
				else
				{
					bool temp = false;
					UNSIGNED_INT_16 pos=0;

					PRICE newPrice;
					SIZE newQty;
					for (const auto& pair : _smapNormalPriceQty) {
						if (pair.first == MDentryID) {
							newPrice = pair.second.first;
							newQty = pair.second.second;
							temp = true;
							break;
						}
					}
					if (temp)
					{
						if (_md5.getPositionForSellPrice(newPrice, pos))
						{
							_md5.deleteSellDepthViaId(newPrice, pos, newQty);
						}
					}
					_smapNormalPriceQty.erase(MDentryID);
				}
			}

			bool isPriceNew(const PRICE price, const char UpdateType)
			{

				if (UpdateType == 'B')
				{
					/*if (_bmapImpliedPriceQty.find(price) == _bmapImpliedPriceQty.end()
						&& _bmapNormalPriceQty.find(price) == _bmapNormalPriceQty.end())
						return true;*/
					//replace
					
					for (const auto& pair : _bmapNormalPriceQty) {
						if (pair.second.first == price) {
							return true;
						}
					}
				}
				else
				{
					/*if (_smapImpliedPriceQty.find(price) == _smapImpliedPriceQty.end()
						&& _smapNormalPriceQty.find(price) == _smapNormalPriceQty.end())
						return true;*/
					//replace
					
					for (const auto& pair : _smapNormalPriceQty) {
						if (pair.second.first == price) {
							return true;
						}
					}
				}


				return false;
			}

			SIZE getTotalQtyForPrice(const PRICE price, const char UpdateType)
			{
				SIZE qty = 0;
				if (UpdateType == 'B')
				{
					/*_mapImpliedPriceQtyItr = _bmapImpliedPriceQty.find(price);
					if (_mapImpliedPriceQtyItr != _bmapImpliedPriceQty.end())
						qty += _mapImpliedPriceQtyItr->second;*/



						/*_mapNormalPriceQtyItr = _bmapNormalPriceQty.;
						if (_mapNormalPriceQtyItr != _bmapNormalPriceQty.end())
						{
							qty += _mapNormalPriceQtyItr->second;
						}*/

					for (const auto& pair : _bmapNormalPriceQty) {
						if (pair.second.first == price) {
							qty += pair.second.second;
						}
					}
				}
				else
				{
					/*_mapImpliedPriceQtyItr = _smapImpliedPriceQty.find(price);
					if (_mapImpliedPriceQtyItr != _smapImpliedPriceQty.end())
						qty += _mapImpliedPriceQtyItr->second;*/


					/*_mapNormalPriceQtyItr = _smapNormalPriceQty.find(price);
					if (_mapNormalPriceQtyItr != _smapNormalPriceQty.end())
						qty += _mapNormalPriceQtyItr->second;*/

					for (const auto& pair : _smapNormalPriceQty) {
						if (pair.second.first == price) {
							qty += pair.second.second;
						}
					}
				}
				return qty;
			}
		};


		typedef std::unordered_map < std::string, SymbolDataStore*> MarketDataMap;

		class MarketDataStore
		{
		public:
			short variable_depth_count;
			void init(const uint16_t);
			void print(std::string key);
			void reset();
			bool addSymbol(std::vector<std::string>&);
			bool deleteSymbol(const std::string&);
			bool isSymbolSubscribed(const std::string&);
			void addUpdateRecords(const UpdateRecordsPtr);
			void startProcessingData();
			bool isValidSymbol(const std::string&);


			std::unordered_map < std::string, std::vector<std::string>> getAllSubscribedSymbols()
			{
				return _securityFileHandler.getAllSubscribedSymbols();
			}

			void updateMDForTT(const std::string symbol)
			{
				//variable_depth_count = depthCount;
				//std::cout << "DEPTH COUNT " << depthCount << std::endl;
				//LOG_INFO << "[TT] Reading symbol and creating map ";
				auto itr = _mDataMap.find(symbol);
				if (itr == _mDataMap.end())
				{
					//LOG_INFO << "[TT] Reading symbol and creating map for " << symbol;
					itr = _mDataMap.emplace(symbol, new SymbolDataStore(symbol)).first;

					itr->second->setSymbol(symbol);
					//	//itr->second->setExpiry(expiry);
					//	//itr->second->setExchange(exch);

					//	SymbolNotice notice( secId);
					//	std::string message = "Subscribed || Snapshot Rcvd: " + secId;
					//	notice.setMessage(message);
					//	notice.setNoticeProducer(Producer_Gateway);
					//	notice.setUpdateTime(time(NULL));
					//	MDGtwy::Publisher::getInstance()->send(notice);

				}
			}



			void applyUpdate(const UpdateRecord&);
			void messageUpdatesProcessed()
			{
				_symbolItr = _registeredSymbols.begin();
				while (_symbolItr != _registeredSymbols.end())
				{
					MDGtwy::Publisher::getInstance()->send(std::move((*_symbolItr)->_md5));

					if ((*_symbolItr)->isOhlcUpdated())
					{
						MDGtwy::Publisher::getInstance()->send(std::move((*_symbolItr)->getOHLC()));
					}
					++_symbolItr;
				}
				_registeredSymbols.clear();

			}
			void sendContractString(const std::string msg)
			{
				MDGtwy::Publisher::getInstance()->send(msg);
			}

		private:
			MarketDataMap _mDataMap;

			SecurityFileHandler _securityFileHandler;
			uint16_t  _exchangeName;

			std::unordered_set< SymbolDataStore* > _registeredSymbols;
			std::unordered_set< SymbolDataStore* >::iterator _symbolItr;

			int c = 0;
			long v = 0;
			//std::string sec = "13511963514316561868";
			UpdateQueue _qUpdates;





		};

	}
}
#endif
