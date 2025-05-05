#ifndef FIN_COMMON_THROTTLING_H
#define FIN_COMMON_THROTTLING_H

#include <iostream>
#include <chrono>
#include <stdlib.h>
#include "Nanolog.hpp"

//#include "nanoLog/NanoLog.hpp"
namespace FIN {

	template < typename T >
	class CircularArray
	{
	public:
		CircularArray(long size)
		{
			setSize(size - 2);
			LOG_INFO << "Throttle array size " + std::to_string(_maxSize);
		}

		bool full()
		{
			return _fullOnce;
		}

		void store(T val)
		{
			if (_fullOnce)
				moveFront();

			ptr[_writeIndex++] = val;

			if (_writeIndex >= _maxSize)
			{
				_fullOnce = true;
				_writeIndex %= _maxSize;
			}

		}

		T front()
		{
			return ptr[_frontIndex];
		}

	private:

		void moveFront()
		{
			++_frontIndex;
			if (_frontIndex >= _maxSize)
				_frontIndex %= _maxSize;
		}

		void setSize(long size)
		{
			_maxSize = size;
			ptr = new T[_maxSize];
			_frontIndex = 0;
			_writeIndex = 0;
			_fullOnce = false;
		}

		T* ptr;
		long _frontIndex;
		long _writeIndex;
		bool _fullOnce;
		long _maxSize;
	};

	class SlidingWindowThrottling
	{
	public:

		SlidingWindowThrottling(long msgLimit, double timeLimit)
			: _circularArray(msgLimit)
		{
			_msgLimit = msgLimit;
			//  _timeLimit = timeLimit * 1000000;
			_timeLimit = timeLimit * 1000;
			LOG_INFO << "* Throttle Limit: " << std::to_string(msgLimit);
			LOG_INFO << "* Throttle Limit Window in second: " << std::to_string(timeLimit / 1000);
		}

		void checkThrottleLimit()
		{
			long long curr = 0;

			bool print = true;
			while (1)
			{
				if (!_circularArray.full())
				{
					curr = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					_circularArray.store(curr);
					return;
				}
				else
				{
					curr = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
					if (curr - _circularArray.front() > _timeLimit)
					{
						_circularArray.store(curr);
						return;
					}
					else
					{
						if (print)
						{
							auto time_to_sleep_micro = _timeLimit - (curr - _circularArray.front());
							auto time_to_wait = time_to_sleep_micro / 1000000.0;// _timeLimit / 1000000.0 - (curr - _circularArray.front()) / 1000000.0;
							print = false;
							LOG_INFO << "****************************************************************************************************************************";
							LOG_INFO << "      Throtlle Limit: " << std::to_string(_msgLimit)
								<< " / " << std::to_string(_timeLimit / 1000000.0) << " seconds "
								<< " | Waiting for: " << time_to_wait << " seconds " << " micro second " << time_to_sleep_micro;
							LOG_INFO << "****************************************************************************************************************************";

							std::this_thread::sleep_for(std::chrono::microseconds(time_to_sleep_micro));

							LOG_INFO << "Throttle reset after sleep for " << time_to_sleep_micro << " in micro and " << time_to_wait << " in seconds";

						}
					}
				}
			}
		}

		long getMsgLimit() const
		{
			return _msgLimit;
		}

		double getTimeLimit() const
		{
			return _timeLimit;
		}

	private:
		long _msgLimit;
		long long _timeLimit;

		int n = 0;
		CircularArray < long long > _circularArray;
	};
}

#endif
