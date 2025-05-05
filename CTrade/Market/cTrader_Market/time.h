#ifndef FIN_COMMON_TIME_H
#define FIN_COMMON_TIME_H

#include <iostream>
#include <chrono>

namespace FIN
{
	/*class Time
	{
	public:*/
	//const std::chrono::steady_clock::time_point startTime;
	//const std::chrono::steady_clock::time_point endTime;
	void setStartTime(std::chrono::high_resolution_clock::time_point);
	std::chrono::high_resolution_clock::time_point getStartTime();

}

#endif

