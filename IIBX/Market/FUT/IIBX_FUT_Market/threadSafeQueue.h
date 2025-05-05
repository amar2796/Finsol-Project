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

				T pop()
				{
					std::unique_lock<std::mutex> mlock(_mutex);
					while (_queue.empty())
					{
						_condVar.wait(mlock);
					}
					auto item = _queue.front();
					_queue.pop();
					return item;
				}

				T front()
				{
					std::unique_lock<std::mutex> mlock(_mutex);
					while (_queue.empty())
					{
						_condVar.wait(mlock);
					}
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
					mlock.unlock();
					_condVar.notify_one();
				}

				void push(T&& item)
				{
					std::unique_lock<std::mutex> mlock(_mutex);
					_queue.push(std::move(item));
					mlock.unlock();
					_condVar.notify_one();
				}

				bool isEmpty()
				{
					return _queue.empty();
				}

				int size()
				{
					std::unique_lock<std::mutex> mlock(_mutex);
					int count = _queue.size();
					mlock.unlock();
					return count;
				}
			private:
				std::queue<T> _queue;
				std::mutex _mutex;
				std::condition_variable _condVar;
		};
}
#endif
