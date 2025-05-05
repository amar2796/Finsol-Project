#pragma once

#ifndef FIN_APPLSEQ_H
#define FIN_APPLSEQ_H
#include <fstream>


#include <boost/filesystem/operations.hpp> // includes boost/filesystem/path.hpp
#include <boost/filesystem/fstream.hpp> // ditto
#include <iostream>
#include "threadSafeQueue.h"
// for std::cout
using namespace boost::filesystem;
class ApplSeq
{
public:
	ApplSeq()
	{
		GetCurrentDate();
		std::thread t(&ApplSeq::PushToFile, this);
		t.detach();
	}

	void PushToFile()
	{
		while (true)
		{
			auto content = applSeq.pop();
			writeInFile(content);
		}
	}

	int writeInFile(int applicationSeq)
	{
		try
		{
			boost::filesystem::ofstream myfile1(seq_path);
			if (myfile1.is_open())
			{
				auto write = DateString + currentDate + "\n" + TradeString + std::to_string(applicationSeq);
				myfile1 << write;
			}
			myfile1.close();
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
		return 0;
	}
	void ReadFileForApllSeq()
	{

		try
		{
			std::ifstream file;
			std::string line;
			file.open(seq_path);
			if (file.is_open()) {
				while (getline(file, line))
				{
					if (line != "")
					{
						if (line.find(TradeString) != std::string::npos)
						{
							size_t pos = line.find(TradeString);
							std::string value = line.substr(pos + TradeString.length());
							ApllSeq = std::stoi(value);
							LOG_INFO << "Last processed trade seq " << value;
						}
						else if (line.find(DateString) != std::string::npos)
						{
							size_t pos = line.find(DateString);
							std::string value = line.substr(pos + DateString.length());
							storedDate = value;
							LOG_INFO << "Stored Date " << value;
						}
					}
				}

				file.close();
			}
			if (storedDate == "")
			{
				if (ApllSeq == 0)
				{
					applSeq.push(0);
				}
				else
				{
					applSeq.push(ApllSeq);
				}

			}
			else if (ApllSeq == 0 || storedDate != currentDate)
			{
				applSeq.push(0);
			}
		}
		catch (...)
		{
			LOG_INFO << "WHILE READING APLL SEQ NUMBER FILE ";
		}

	}
	void GetCurrentDate()
	{
		try
		{

			auto currentTime = std::chrono::system_clock::now();
			std::time_t timeNow = std::chrono::system_clock::to_time_t(currentTime);
			std::tm* timeInfo = std::localtime(&timeNow);

			char buffer[80];
			std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);
			std::string formattedDate(buffer);
			currentDate = formattedDate;
		}
		catch (std::exception& ex)
		{
			LOG_INFO << ex.what();
		}
	}
	unsigned ApllSeq = 0;
	std::string seq_path;
	std::string storedDate;
	std::string currentDate;

	const std::string TradeString = "Trade_Seq=";
	const std::string DateString = "Date=";

	FIN::ThreadSafeQueue<int> applSeq;


private:

};



#endif