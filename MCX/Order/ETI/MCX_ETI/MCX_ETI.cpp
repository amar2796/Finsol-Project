// MCX_ETI.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

extern "C" {
#include <openssl/applink.c>
}
#include "InitApp.h"
//#include "tbb/concurrent_hash_map.h"

LONG WINAPI CustomUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo) {

	std::cerr << "Unhandled exception occurred!" << std::endl;
	LOG_INFO << "UNHANDLED EXCEPTION CAUGHT";
	if (ExceptionInfo != nullptr)
	{
		EXCEPTION_RECORD* exceptionRecord = ExceptionInfo->ExceptionRecord;
		CONTEXT* contextRecord = ExceptionInfo->ContextRecord;

		if (exceptionRecord != nullptr)
		{
			std::cerr << "Exception Code: 0x" << std::hex << exceptionRecord->ExceptionCode << std::dec << std::endl;
			std::cerr << "Exception Address: 0x" << std::hex << exceptionRecord->ExceptionAddress << std::dec << std::endl;

			std::cerr << "Exception Flags: " << exceptionRecord->ExceptionFlags << std::endl;
		}


		LOG_INFO << "UNHANDLED EXCEPTION CAUGHT";
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

void holdScreen()
{
	while (true)
	{
		getchar();
	}
}
int main()
{

	try
	{

		SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
		InitApp app;

	}
	catch (std::exception& e)
	{
		std::cout << "Exception : " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "Unknown exception caught" << std::endl;
	}

	holdScreen();
	return 0;

}


