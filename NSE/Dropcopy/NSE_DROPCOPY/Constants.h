#pragma once
#ifndef FIN_CONSTANTS
#define FIN_CONSTANTS
#include <string.h>
#include <iostream>

namespace FIN
{

	const std::string TAG_11 = "11,";		//Original Trade
	const std::string TAG_12 = "12,";		//Modified Trade
	const std::string TAG_13 = "13,";		//Cancelled Trade
	const std::string TAG_17 = "17,";		//Approved Trade
	const std::string TAG_18 = "18,";		//Rejected Trade
	const std::string TAG_19 = "19,";		//Give up Trade
	const std::string COMMA = ",";
	const std::string RL = "1,RL,1,";
	const std::string SPACE_1 = "1,";
	const std::string LONG_SPACE = ",";
	const std::string ONE_SPACE_COMMA = " ,";
	const std::string OPEN = "OPEN,";
	const std::string UNCOVER = "UNCOVER,";
	const std::string NIL = ",NIL,";
	//const std::string ZERO_2 = ".00";
	const std::string PRO_CLIENT = "1,";



}


#endif