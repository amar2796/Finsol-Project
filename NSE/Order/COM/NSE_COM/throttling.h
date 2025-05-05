/*

#ifndef FIN_COMMON_THROTTLING_H
#define FIN_COMMON_THROTTLING_H

#include <iostream>
#include <chrono>
#include <stdlib.h>
//#include <glog/logging.h>
//#include "NanoLog.hpp"
namespace FIN {

	template < typename T >
	class CircularArray
	{
	public:
		CircularArray(long size)
		{
			setSize(size);
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
			_timeLimit = timeLimit * 1000000;

			LOG_INFO << "* Throttle Limit: " << _msgLimit;
			LOG_INFO << "* Throttle Limit Window: " << timeLimit;
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
							print = false;
							LOG_INFO << "****************************************************************************************************************************";
							LOG_INFO << "      Throtlle Limit: " << _msgLimit
								<< " / " << std::to_string(_timeLimit / 1000000.0) << " seconds "
								<< " | Waiting for: " << std::to_string(_timeLimit / 1000000.0 - (curr - _circularArray.front()) / 1000000.0) << " seconds";
							LOG_INFO << "****************************************************************************************************************************";
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
		INT32 _msgLimit;
		INT64 _timeLimit;

		int n = 0;
		CircularArray < INT64 > _circularArray;
	};
}

#endif

*/



#ifndef FIN_COMMON_THROTTLING_H
#define FIN_COMMON_THROTTLING_H
#pragma once

#include <iostream>
#include <deque>
#include <chrono>
#include <thread>
#include "Nanolog.hpp"

namespace FIN {

	class SlidingWindowThrottling
	{
	public:
		SlidingWindowThrottling(long msgLimit, double timeLimitSec)
			: _msgLimit(msgLimit), _timeLimit(timeLimitSec * 1000000) 
		{
			LOG_INFO << "* Throttle Limit: " << std::to_string(_msgLimit);
			LOG_INFO << "* Throttle Limit Window: " << _timeLimit/1000000;
		}

		void checkThrottleLimit() 
		{
			int64_t currTime = currentMicroseconds();

			// Remove timestamps that are outside the time window
			while (!timestamps.empty() && (currTime - timestamps.front() > _timeLimit)) 
			{
				timestamps.pop_front();
			}

			if (timestamps.size() < _msgLimit) 
			{
				timestamps.push_back(currTime);
				return;
			}

			// If limit is reached, wait until a message expires
			int64_t waitTime = _timeLimit - (currTime - timestamps.front());
			LOG_INFO << "Throttling... Waiting for " << waitTime / 1000000.0 << " seconds";
			std::cout<< "Throttling... Waiting for " << waitTime / 1000000.0 << " seconds\n";
			std::this_thread::sleep_for(std::chrono::microseconds(waitTime));

			// After waiting, re-check and process
			//checkThrottleLimit();
		}

	private:
		long _msgLimit;
		int64_t _timeLimit;
		std::deque<int64_t> timestamps; // Optimized FIFO storage

		int64_t currentMicroseconds() 
		{
			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		}
	};
}

#endif


