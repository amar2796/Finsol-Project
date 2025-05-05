#ifndef FIN_ORDER_GW_NNF_TRIMMED_ORDER_BOOK_H
#define FIN_ORDER_GW_NNF_TRIMMED_ORDER_BOOK_H

#include <vector>

namespace FIN {
    namespace OrdGtwy {

        enum OrderStatus
        {
            OrderStatus_New,
            OrderStatus_Partial_Fill,
            OrderStatus_Fill,
            OrderStatus_Canceled,
            OrderStatus_Replace,
            OrderStatus_Replace_Rejected,
            OrderStatus_Cancel_Rejected,
            OrderStatus_MAX
        };

        enum ExecType
        {
            ExecType_New,
            ExecType_Partial_Fill,
            ExecType_Fill,
            ExecType_Canceled,
            ExecType_Replace,
            ExecType_Replace_Rejected,
            ExecType_Cancel_Rejected,
            ExecType_MAX
        };

        enum Side
        {
            Side_Buy,
            Side_Sell,
            Side_MAX
        };

        struct OrderExchResponse
        {
        private:

            FIN_TYPE::EXCH_ORDERID exchOrderID;
            FIN_TYPE::EXCH_ORDERID executionID;
            OrderStatus            orderStatus;
            ExecType               execType;
            Side                   side;
            FIN_TYPE::PRICE        orderPrice;
            FIN_TYPE::PRICE        lastFilledPrice;
            FIN_TYPE::QTY          orderQty;
            FIN_TYPE::QTY          filledQty;
            FIN_TYPE::TIME         exchangeEntryTime;
            FIN_TYPE::TIME         exchangeModifyTime;
            FIN_TYPE::TIME         exchangeTransactTime;
            FIN_TYPE::TEXT         errorText;
            FIN_TYPE::SECURITY_ID  securityID;
            FIN_TYPE::CL_ORDERID   clOrderID;
            FIN_TYPE::CL_ORDERID   origClOrderID;


        public:

            OrderExchResponse()
            {
            }

            void print() const
            {
                std::cout << "-----------------ORDER RESPONSE----------------" << std::endl;
                std::cout << "exchOrderID          : " << exchOrderID << std::endl;
                std::cout << "executionID          : " << executionID << std::endl;
                std::cout << "orderStatus          : " << orderStatus << std::endl;
                std::cout << "execType             : " << execType << std::endl;
                std::cout << "side                 : " << side << std::endl;
                std::cout << "orderPrice           : " << orderPrice << std::endl;
                std::cout << "lastFilledPrice      : " << lastFilledPrice << std::endl;
                std::cout << "orderQty             : " << orderQty << std::endl;
                std::cout << "filledQty            : " << filledQty << std::endl;
                std::cout << "exchangeEntryTime    : " << exchangeEntryTime << std::endl;
                std::cout << "exchangeModifyTime   : " << exchangeModifyTime << std::endl;
                std::cout << "exchangeTransactTime : " << exchangeTransactTime << std::endl;
                std::cout << "errorText            : " << errorText << std::endl;
                std::cout << "securityID           : " << securityID << std::endl;
                std::cout << "clOrderID            : " << clOrderID << std::endl;
                std::cout << "origClOrderID        : " << origClOrderID << std::endl;
            }
        };

        /*
        class HistoricalOrderBook
        {
            HistoricalOrderBook( const std::string &fileName ) ;
            std::unordered_set < OrderExchResponse > getHistoricalActiveOrder() ;

            private :
            typedef std::unordered_map < FIN_TYPE::EXCH_ORDERID, OrderExchResponse * > MapOrderBook ;

            void readFromFile() ;
            std::string _fileName ;

            MapOrderBook _orderBookMap ;
        };
        */

    }
}

#endif
