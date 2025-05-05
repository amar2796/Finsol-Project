#include <iostream>
#include <exception>
#include <cstdlib>
#include <thread>
#include "../BSEEDORDER/HeaderFile/SessionSettings.h"
#include "../BSEEDORDER/CommonFile/ReadConfigurationFile.h"
#include "../BSEEDORDER/Processing/TradeApplication.h"
#include "../BSEEDORDER/NanoLog/Nanolog.hpp"

// Global exception handler function
void handleGlobalException() {
    try {
        throw; // Rethrow the current exception to catch it globally
    }
    catch (const std::exception& e) {
        std::cerr << "Caught std::exception: " << e.what() << std::endl;
        LOG_INFO << "[EXCEPTION] Caught std::exception: " << e.what();
    }
    catch (...) {
        std::cerr << "Caught unknown exception." << std::endl;
        LOG_INFO << "[EXCEPTION] Caught unknown exception.";
    }
    std::abort(); // Terminate the program after handling the exception
}

// Function to initialize logging
void initializeLog(const std::string& _path) {
    try {
        std::time_t t = time(NULL) + 19800;
        auto gmtime = std::gmtime(&t);
        char buffer[32];
        strftime(buffer, 32, "%Y%m%d%T", gmtime);
        std::string t1(buffer);
        t1.erase(std::remove(t1.begin(), t1.end(), ':'), t1.end());
        std::string fileName = "ETI_logger-" + t1;
        nanolog::initialize(nanolog::GuaranteedLogger(), _path + "\\", fileName, 200); // 200 is 200mb max file size

        std::cout << "[INFO] " << "[BINARY UPDATED DATE] " << __DATE__ << "  CO : 4247" << std::endl;
        LOG_INFO << "[INFO] " << "[BINARY UPDATED DATE] " << __DATE__ << " CO : 4247";
        std::cout << "[INFO] " << "Logger initialization completed " << std::endl;
    }
    catch (std::exception& ex) {
        std::cout << "[INFO] " << "[EXCEPTION] " << ex.what() << std::endl;
        LOG_INFO << "[INFO] " << "[EXCEPTION] " << ex.what();
    }
}

int main(int argc, char* argv[]) {
    // Set the global exception handler
    std::set_terminate(handleGlobalException);

    try 
    {
        SetConsoleTitle(TEXT("BSEED Order"));

        SessionSettings* settingsFile = new SessionSettings;
        ReadConfigurationFile* configFile = new ReadConfigurationFile(settingsFile);

        configFile->ReadConfigurationParametersFromFile();

        initializeLog(settingsFile->log_path);

        boost::asio::io_service omsSenderIO;
        TradeApplication* TrdApp = new TradeApplication(settingsFile, omsSenderIO);

        std::thread trade_connect_to_exchange(&TradeApplication::connectToBSEExchange, TrdApp);
        TrdApp->startOmsRcv();
        trade_connect_to_exchange.join();

        // Clean up: Delete dynamically allocated resources
        delete TrdApp;
        delete configFile;
        delete settingsFile;
    }
    catch (std::exception& ex) {
        std::cerr << "[INFO] Exception caught: " << ex.what() << std::endl;
        LOG_INFO << "[INFO] Exception caught: " << ex.what();
    }
    catch (...) {
        std::cerr << "[INFO] Unknown exception caught." << std::endl;
        LOG_INFO << "[INFO] Unknown exception caught.";
    }

    return 0;
}
