#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "SessionSettings.h"
#include "ReadConfigurationFile.h"

using namespace std;
namespace FIN {
    class readContractFile {

    public:
        std::vector < pair< int, vector<string> > > contractFileData;
        readContractFile() {
        }

        readContractFile(const std::string fileName) {
            readLinesFromFile(fileName);
        }

        
        void readLinesFromFile(const std::string);
        std::string findValue(int, int);

    };
}
