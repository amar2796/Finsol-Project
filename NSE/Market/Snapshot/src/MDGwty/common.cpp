#include "common.h"
namespace FIN {
	namespace MDGtwy {
		std::string getName(const MDUpdateType & t)
		{
			switch (t)
			{
			case MDUpdateType_BID:  return strMDUpdateType_BID;
			case MDUpdateType_OFFER:  return strMDUpdateType_OFFER;
			case MDUpdateType_TRADE:  return strMDUpdateType_TRADE;
			case MDUpdateType_OPEN:  return strMDUpdateType_OPEN;
			case MDUpdateType_CLOSE:  return strMDUpdateType_CLOSE;
			case MDUpdateType_HIGH:  return strMDUpdateType_HIGH;
			case MDUpdateType_LOW:  return strMDUpdateType_LOW;
			case MDUpdateType_VOLUME:  return strMDUpdateType_VOLUME;
			case MDUpdateType_INVALID:  return strMDUpdateType_INVALID;
			}
			return strEmptyString;
		}

		std::string getName(const MDUpdateAction & t)
		{
			switch (t)
			{
			case  MDUpdateAction_NEW:  return strMDUpdateAction_NEW;
			case  MDUpdateAction_CHANGE:  return strMDUpdateAction_CHANGE;
			case  MDUpdateAction_DELETE:  return strMDUpdateAction_DELETE;
			case  MDUpdateAction_INVALID:  return strMDUpdateAction_INVALID;
			}
			return strEmptyString;
		}
	}
}
