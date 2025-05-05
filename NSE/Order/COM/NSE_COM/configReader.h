#pragma once
#ifndef FIN_CONFIG_READER_H
#define FIN_CONFIG_READER_H

#include "common.h"

#include <mutex>
#include <unordered_set>
#include <windows.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <fstream>

namespace FIN
{
    //load and parse configuration files (any format)
    class ConfigReader
    {
    public:
        ConfigReader() {}

        //Loads and parses the specified configuration file.
        void loadData(const std::string& configFile)
        {
            _fileName = configFile;
            parseFile();
        }

        template <typename T>
        T getValue(const std::string& key, const T& defaultValue)
        {
            auto it = _data.find(key);
            if (it != _data.end()) {
                std::istringstream ss(it->second);
                T value;

                //stream extraction operator, which extracts data from the stream and converts it to the appropriate type
                if (ss >> value) {
                    return value;
                }
            }
            return defaultValue;
        }

        template <>
        std::string getValue<std::string>(const std::string& key, const std::string& defaultValue)
        {
            auto it = _data.find(key);
            return (it != _data.end()) ? it->second : defaultValue;
        }

    private:
        std::string _fileName;
        std::unordered_map<std::string, std::string> _data; // Stores parsed key-value pairs.


        // @brief Parses the file, storing key-value pairs in the _data map.
        void parseFile()
        {
            _data.clear();
            std::ifstream file(_fileName);
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << _fileName << std::endl;
                return;
            }

            std::string line, currentSection;
            while (std::getline(file, line)) {
                // Trim leading and trailing whitespace
                line.erase(0, line.find_first_not_of(" \t\r\n"));
                line.erase(line.find_last_not_of(" \t\r\n") + 1);

                // Skip comments and empty lines
                if (line.empty() || line[0] == ';' || line[0] == '#') {
                    continue;
                }

                // Section header
                if (line.front() == '[' && line.back() == ']') {
                    currentSection = line.substr(1, line.size() - 2);
                }
                else {
                    // Key-value pair
                    size_t eqPos = line.find('=');
                    if (eqPos != std::string::npos) {
                        std::string key = line.substr(0, eqPos);
                        std::string value = line.substr(eqPos + 1);

                        // Trim whitespace
                        key.erase(0, key.find_first_not_of(" \t\r\n"));
                        key.erase(key.find_last_not_of(" \t\r\n") + 1);
                        value.erase(0, value.find_first_not_of(" \t\r\n"));
                        value.erase(value.find_last_not_of(" \t\r\n") + 1);

                        // Store in map as Section.Key
                        _data[currentSection + "." + key] = value;
                    }
                }
            }
            file.close();
        }
    };


    class SecurityFileHandler
    {
    public:
        SecurityFileHandler()
        {
        }

        void init()
        {
            std::lock_guard < std::mutex > lock(_mutex);
            _confReader.loadData(CONSTANT::NSECOM_ORDER_CONFIG);
            std::string fileName = _confReader.getValue < std::string >(
                CONSTANT::MD_SUBSCRIPTION + CONSTANT::SEP_DOT + CONSTANT::FILE_NAME,
                CONSTANT::EMPTY_STRING
            );
            registerFile(fileName);
        }

        bool addSymbolForSubscription(const std::string& symbol)
        {
            std::lock_guard < std::mutex > lock(_mutex);
            if (_symbolsInFile.insert(symbol).second)
            {
                std::ofstream myfile(_fileName, std::ios::app);
                if (myfile.is_open())
                {
                    myfile << symbol << std::endl;
                    myfile.close();
                    return true;
                }
            }

            return false;
        }

        bool isSymbolSubscribed(const std::string& symbol)
        {
            std::lock_guard < std::mutex > lock(_mutex);
            return (_symbolsInFile.find(symbol) != _symbolsInFile.end());
        }

        std::unordered_set< std::string > getAllSubscribedSymbols()
        {
            std::lock_guard < std::mutex > lock(_mutex);
            return _symbolsInFile;
        }

    private:

        void registerFile(const std::string& fileName)
        {
            _fileName = fileName;
            std::string line;
            std::ifstream myfile(_fileName);
            if (myfile.is_open())
            {
                while (getline(myfile, line))
                {
                    _symbolsInFile.insert(line);
                }
                myfile.close();
            }
        }


        std::string _fileName;

        std::unordered_set < std::string > _symbolsInFile;

        std::mutex _mutex;
        ConfigReader _confReader;
    };
}

#endif
