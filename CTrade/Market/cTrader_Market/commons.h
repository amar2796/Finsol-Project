#ifndef FIN_MARKET_GW_COMMON_H
#define FIN_MARKET_GW_COMMON_H
#include <string>
#include "common.h"
namespace FIN {
	namespace MDGtwy {

		enum MDUpdateType
		{
			MDUpdateType_BID,
			MDUpdateType_OFFER,
			MDUpdateType_TRADE,
			MDUpdateType_OPEN,
			MDUpdateType_CLOSE,
			MDUpdateType_HIGH,
			MDUpdateType_LOW,
			MDUpdateType_VOLUME,
			MDUpdateType_IMPLIED_BID,
			MDUpdateType_IMPLIED_OFFER,

			MDUpdateType_INVALID
		};

		enum MDUpdateAction
		{
			MDUpdateAction_NEW,
			MDUpdateAction_CHANGE,
			MDUpdateAction_DELETE,

			MDUpdateAction_INVALID
		};

		static const std::string strMDUpdateType_BID = "MDUpdateType_BID    ";
		static const std::string strMDUpdateType_OFFER = "MDUpdateType_OFFER  ";
		static const std::string strMDUpdateType_TRADE = "MDUpdateType_TRADE  ";
		static const std::string strMDUpdateType_OPEN = "MDUpdateType_OPEN   ";
		static const std::string strMDUpdateType_CLOSE = "MDUpdateType_CLOSE  ";
		static const std::string strMDUpdateType_HIGH = "MDUpdateType_HIGH   ";
		static const std::string strMDUpdateType_LOW = "MDUpdateType_LOW    ";
		static const std::string strMDUpdateType_VOLUME = "MDUpdateType_VOLUME ";
		static const std::string strMDUpdateType_INVALID = "MDUpdateType_INVALID";
		static const std::string strMDUpdateType_IMPLIED_BID = "MDUpdateType_IMPLIED_BID    ";
		static const std::string strMDUpdateType_IMPLIED_OFFER = "MDUpdateType_IMPLIED_OFFER  ";

		static const std::string strEmptyString = "";

		std::string getName(const MDUpdateType& t);

		static const std::string  strMDUpdateAction_NEW = "MDUpdateAction_NEW    ";
		static const std::string  strMDUpdateAction_CHANGE = "MDUpdateAction_CHANGE ";
		static const std::string  strMDUpdateAction_DELETE = "MDUpdateAction_DELETE ";
		static const std::string  strMDUpdateAction_INVALID = "MDUpdateAction_INVALID";

		std::string getName(const MDUpdateAction& t);
	}
}

#endif
