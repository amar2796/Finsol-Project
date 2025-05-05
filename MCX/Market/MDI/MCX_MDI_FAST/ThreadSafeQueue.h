#ifndef FIN_COMMON_LOCKED_QUEUE
#define FIN_COMMON_LOCKED_QUEUE

#include <queue>
#include <thread>
#include <mutex>
#include <string.h>
#include <condition_variable>
#include <chrono>


namespace FIN {

	template <typename T>
	class ThreadSafeQueue
	{
	public:

		void pop()
		{
			std::unique_lock<std::mutex> mlock(_mutex);
			if (_queue.empty())
				return;
			else
				_queue.pop();
		}

		T front()
		{
			std::unique_lock<std::mutex> mlock(_mutex);
			_condVar.wait(mlock, [this] { return !_queue.empty(); });
			return _queue.front();
		}

		void pop(T& item)
		{
			std::unique_lock<std::mutex> mlock(_mutex);
			while (_queue.empty())
			{
				_condVar.wait(mlock);
			}
			item = _queue.front();
			_queue.pop();
		}

		void push(const T& item)
		{
			std::unique_lock<std::mutex> mlock(_mutex);
			_queue.push(item);
			_condVar.notify_one();
		}

		void push(T&& item)
		{
			std::unique_lock<std::mutex> mlock(_mutex);
			_queue.push(std::move(item));
			_condVar.notify_one();
		}

		bool isEmpty()
		{
			std::unique_lock<std::mutex> mlock(_mutex);
			return _queue.empty();
		}

		int size()
		{
			std::unique_lock<std::mutex> mlock(_mutex);
			return _queue.size();
		}

	private:
		std::queue<T> _queue;
		std::mutex _mutex;
		std::condition_variable _condVar;
	};
}
#endif
