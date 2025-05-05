#pragma once
#ifndef FIN_COMMON_H
#define FIN_COMMON_H

#include <iostream>
namespace FIN
{
	namespace FIN_CONST
	{

		//! making some constants so that if there is any changes we need to only change in one place not everywhere.
		static const std::string EMPTY_STRING = "";
		//static std::string FIN_CONFIG_FILE = "";
		static const std::string FIN_CONFIG_FILE = "MCX_ETI.conf";
		static const std::string FILE_NAME = "FileName";
		static const std::string  TOKEN_NAME = "conf\\subscribedSymbols.txt";
		static const std::string CME_MARKET = "CME";
		static const std::string INTEGRAL_MARKET = "MCX";
		static const std::string CMEepochFile = "conf\\epoch.txt";
		static const std::string CMESymbolVolume = "conf\\SymbolVolumeFile.txt";
		static const std::string CMETradeSeqFile = "conf\\TradeSeriesAndSeqNo.txt";
		static const std::string MD_SUBSCRIPTION = "MD_SUBSCRIPTION";
		static const std::string PUBLISHER = "PUBLISHER";
		static const std::string SEP_DOT = ".";
		static const std::string LOG_PATH = "\\logs\\ETI_Log";
		static const std::string APPL_SEQ_PATH = "\\ApllSeq.txt";
		static const std::string PEMFILE = "\\PublicKey.pem";
		static const std::string CUSTODIAN_FILE = "\\custodian.txt";
		const int PORT = 8090;

		const uint16_t Exchange_CME = 0;
		const uint16_t Exchange_LME = 1;
		const uint16_t Exchange_NSECD = 7;
		const uint16_t Exchange_NSEFO = 8;
		const uint16_t Exchange_NSECM = 9;

		const uint16_t BroadCastMsgType_Invalid = 0;
		const uint16_t BroadCastMsgType_MarketData = 1;

	}



}

#endif 