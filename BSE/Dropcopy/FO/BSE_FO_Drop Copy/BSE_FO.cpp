//#include "stdafx.h"

#include "SessionSettings.h"
#include "ReadConfigurationFile.h"
#include "TradeApplication.h"
#include "readContractFile.h"
#include "DropFile.h"
using namespace std;


int main(int argc, char* argv[])
{
	//At revision: 4230
	SetConsoleTitle(TEXT("BSE DropCopy gateway"));
	SessionSettings settingsFile;						 
	ReadConfigurationFile configFile(&settingsFile);	//read config file and initilize log
	boost::asio::io_service omsSenderIO;
	TradeApplication TrdApp(&settingsFile, omsSenderIO);				//read contract file and session id file

	TrdApp.connectToBSEExchange();

	return 0;
}


