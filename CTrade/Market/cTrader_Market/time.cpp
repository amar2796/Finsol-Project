#include "time.h"

namespace FIN
{
	std::chrono::high_resolution_clock::time_point startTime1;
	std::chrono::high_resolution_clock::time_point endTime1;

	void setStartTime(std::chrono::high_resolution_clock::time_point s)
	{
		startTime1 = s;
	}

	std::chrono::high_resolution_clock::time_point getStartTime()
	{
		return startTime1;
	}
}
