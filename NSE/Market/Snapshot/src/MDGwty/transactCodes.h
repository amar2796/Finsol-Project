#ifndef FIN_EXCH_API_NSE_TRANSACT_CODES
#define FIN_EXCH_API_NSE_TRANSACT_CODES

namespace FIN {
	namespace NSE {
		namespace COM {
			enum TransCode
			{
				TransCode_INVALID,
				TransCode_MBP = 7208,
				TransCode_Ticker_Trade = 7202,
				TransCode_MarketWatch = 7201,
				Transcode_Call_Auction_MBP = 7214,
				Transcode_Broadcast_call_auction_Market_watch = 7215,
				TransCode_Security_Update_Information = 7305,
				TransaCode_Participant_Update_Info = 7306,
				TransCode_SECURITY_STATUS_UPDATE_INFORMATION = 7320,
				TransCode_BCAST_SECURITY_STATUS_CHG_PREOPEN = 7210,
				TransCode_BROADCAST_LIMIT_EXCEEDED = 9010,
				Transcode_MS_AUCTION_INQ_DATA = 6582,
				Transcode_C_AUCTION_STATUS_CHANGE = 6581,
				Transcode_BC_OPEN_MESSAGE = 6511,
				Transcode_BC_CLOSE_MESSAGE = 6521,
				Transcode_BC_PREOPEN_SHUTDOWN_MSG = 6531,
				Transcode_BC_NORMAL_MKT_PREOPEN_ENDED = 6571,
				Transcode_BC_CLOSING_START = 6583,
				Transcode_BC_CLOSING_END = 6584,
				TransCode_BC_SYMBOL_STATUS_CHANGE_ACTION = 7764,
				TransCode_BCAST_MBO_MBP_UPDATE = 7200,
				TransCode_SECURITY_OPEN_PRICE = 6013,
				TransCode_BC_CIRCUIT_CHECK = 6541,
				TransCode_BCAST_INDICES = 7207,
				TransCode_BCAST_INDICES_VIX = 7216,
				TransCode_BCAST_IND_INDICES = 7203,
				TransCode_BCAST_BUY_BACK = 7211,
				TransCode_AUCTION_INQUIRY_IN = 1090,
				TransCode_BCAST_TRADE_EXECUTION_RANGE = 7220,
				TransCode_MS_ASSET_DATA=7213,

				TransCode_IFSC_MBP = 10500,
				TransCode_IFSC_TT_1 = 10600,
				TransCode_IFSC_TT_2 = 10650,
				TransCode_IFSC_OHL = 10700,
				TransCode_IFSC_OI = 10750,
				TransCode_IFSC_BHAV_EOD_1 = 10800,
				TransCode_IFSC_BHAV_EOD_2 = 10850,

			};
		}
	}
}

#endif
