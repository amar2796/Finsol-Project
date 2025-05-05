#pragma once
#ifndef FIN_CONSTANTS_H
#define FIN_CONSTANTS_H
#include<string>
namespace FIN {

    //const double NSE_PRICE_MULTIPLIER_USD = 10000;//USD
    //const double NSE_PRICE_MULTIPLIER_CENTS = 100;
    const double NSE_CDS_PRICE_MULTIPLIER = 10000000;
    const double NSE_FO_PRICE_MULTIPLIER = 100;
    const double NSE_CM_PRICE_MULTIPLIER = 100;

    enum ProClientIndicator
    {
        CLIENT = 1,
        PRO = 2
    };

    enum SIDE
    {
        SIDE_BUY = 1,
        SIDE_SELL = 2

    };

    const std::string NNF_Product_FUTIDX = "FUTIDX";
    const std::string NNF_Product_FUTSTK = "FUTSTK";
    const std::string NNF_Product_FUTCUR = "FUTCUR";

    const std::string NNF_Product_OPTIDX = "OPTIDX";
    const std::string NNF_Product_OPTSTK = "OPTSTK";
    const std::string NNF_Product_OPTCUR = "OPTCUR";

    const std::string NNF_OptionType_XX = "XX";
    const std::string NNF_OptionType_PE = "PE";
    const std::string NNF_OptionType_CE = "CE";
    const std::string NNF_OptionType_CA = "CA";
    const std::string NNF_OptionType_PA = "PA";

}

#endif
