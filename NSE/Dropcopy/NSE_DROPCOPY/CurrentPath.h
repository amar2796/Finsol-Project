#pragma once
#ifndef FIN_COMMON_PATH_H
#define	FIN_COMMON_PATH_H

//#define WIN32_LEAN_AND_MEAN
#include <winsock2.h> //Note include this header file before windows.h other give error
#include <windows.h>

#include <string>


namespace FIN
{
	class CurrentPath
	{
	public:
		CurrentPath() {}

		std::string getCurrentPath()
		{
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
