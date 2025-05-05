#include "CustodianReader.h"
#include <Windows.h>
CustodianReader::CustodianReader()
{
	readeCustodianFile();
}

CustodianReader::~CustodianReader()
{

}
std::string CustodianReader::getCurrentPath()
{

	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::string path(buffer);
	size_t slashPosition1 = path.rfind("\\");
	path = path.substr(0, slashPosition1 + 1);
	return path;
}

void CustodianReader::readeCustodianFile()
{
	try
	{
		std::ifstream file;
		std::string line;

		file.open(getCurrentPath() + CUSTODIAN_FILE);
		if (file.is_open()) {
			while (std::getline(file, line))
			{

				if (line != "")
				{
					size_t pos = line.find(':');

					if (pos != std::string::npos) {
						std::string key = line.substr(0, pos);
						std::string value = line.substr(pos + 1);

						LOG_INFO << "[INFO] " << key + " : " + value;
						std::cout << "[INFO] " << "Custodian data : " << key << " : " << value << std::endl;
						custodian[key] = value;
					}
				}
			}

			file.close();
		}

	}
	catch (std::exception& ex)
	{
		LOG_INFO << "[INFO] " << ex.what();
	}


}
