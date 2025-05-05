#pragma once
#ifndef FIN_COMMON_PATH_H
#define	FIN_COMMON_PATH_H

//#define WIN32_LEAN_AND_MEAN
//#include <winsock2.h> //Note include this header file before windows.h other give error
#include <windows.h>

#include <iostream>
#include <string>
//#include <boost/filesystem.hpp>
//
//using namespace boost::filesystem;
namespace FIN
{
	class CurrentPath
	{
	public:
		CurrentPath() {}

		std::string getCurrentPath()
		{

			//boost::filesystem::path full_path(boost::filesystem::current_path());

			/*std::cout << "PATH " << full_path << std::endl;
			size_t slashPosition1 = full_path.string().rfind("\\");

			auto path = full_path.string().substr(0, slashPosition1 + 1);
			std::cout << "PATH AF " << path << std::endl;*/
			//return full_path.string();

			char buffer[512];
			GetModuleFileNameA(NULL, buffer, 512);     // windows operation so will also made generice for both os.
			std::string path(buffer);
			size_t slashPosition1 = path.rfind("\\");
			path = path.substr(0, slashPosition1 + 1);
			return path;
		}

	};


}

#endif