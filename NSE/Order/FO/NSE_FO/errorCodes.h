#pragma once
#ifndef FIN_NNF_TRIMMED_FO_ERROR_CODES_H
#define FIN_NNF_TRIMMED_FO_ERROR_CODES_H
#include <string>

namespace FIN {
    namespace NSE {
        namespace TRIMMED {
            namespace FO {

                // Exchange Error Code
                static const int ErrorCode_OE_PRICE_EXCEEDS_DAY_MIN_MAX = 16284;
                static const int ErrorCode_INVALID_INSTRUMENT_TYPE = 293;
                static const int ErrorCode_ORDER_NUMBER_INVALID = 509;
                static const int ErrorCode_ORD_CXL_INITIATOR_AUC_NOT_ALLOWED = 8049;
                static const int ErrorCode_AUCTION_NUMBER_INVALID = 8485;
                static const int ErrorCode_MARKET_CLOSED = 16000;
                static const int ErrorCode_e$invalid_user = 16001;
                static const int ErrorCode_ERROR_BAD_TRANS_CODE = 16003;
                static const int ErrorCode_E$user_already_signed_on = 16004;



                // Exchange ErrorCode Description
                static const std::string ErrorString_UNKNOWN_ERROR_CODE("NNF_UNKNOWN_ERROR_CODE");
                static const std::string ErrorString_OE_PRICE_EXCEEDS_DAY_MIN_MAX("The order price is out of the day’s price range. ");
                static const std::string ErrorString_INVALID_INSTRUMENT_TYPE("Invalid instrument type");
                static const std::string ErrorString_ORDER_NUMBER_INVALID("Order does not exist");
                static const std::string ErrorString_ORD_CXL_INITIATOR_AUC_NOT_ALLOWED("Initiator is not allowed to cancel auction order");
                static const std::string ErrorString_AUCTION_NUMBER_INVALID("Auction number does not exist");
                static const std::string ErrorString_MARKET_CLOSED("The trading system is not available for trading.");
                static const std::string ErrorString_e$invalid_user("Header user ID is not equal to user ID in the order packet.");
                static const std::string ErrorString_ERROR_BAD_TRANS_CODE("Invalid Transcode.");
                static const std::string ErrorString_E$user_already_signed_on("The user is already signed on.");


                class ExchErrorCodes
                {
                public:
                    static const std::string& getErrorText(int ec)
                    {
                        switch (ec)
                        {
                        case ErrorCode_OE_PRICE_EXCEEDS_DAY_MIN_MAX: return ErrorString_OE_PRICE_EXCEEDS_DAY_MIN_MAX;
                        case ErrorCode_INVALID_INSTRUMENT_TYPE: return ErrorString_INVALID_INSTRUMENT_TYPE;
                        case ErrorCode_ORDER_NUMBER_INVALID: return ErrorString_ORDER_NUMBER_INVALID;
                        case ErrorCode_ORD_CXL_INITIATOR_AUC_NOT_ALLOWED: return ErrorString_ORD_CXL_INITIATOR_AUC_NOT_ALLOWED;
                        case ErrorCode_AUCTION_NUMBER_INVALID: return ErrorString_AUCTION_NUMBER_INVALID;
                        case ErrorCode_MARKET_CLOSED: return ErrorString_MARKET_CLOSED;
                        case ErrorCode_e$invalid_user: return ErrorString_e$invalid_user;
                        case ErrorCode_ERROR_BAD_TRANS_CODE: return ErrorString_ERROR_BAD_TRANS_CODE;
                        case ErrorCode_E$user_already_signed_on: return ErrorString_E$user_already_signed_on;
                        default: return ErrorString_UNKNOWN_ERROR_CODE;
                        }
                    }
                };

            }
        }
    }
}

#endif
