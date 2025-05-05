#ifndef FIN_COMMON_MAP_H
#define FIN_COMMON_MAP_H

#include <unordered_map>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;
typedef boost::shared_mutex Lock;
typedef boost::unique_lock<Lock> WriteLock;
typedef boost::shared_lock<Lock> ReadLock;
namespace FIN
{

    template <typename key, typename value>
    class ThreadSafeMap
    {
    public:
        bool find(key _key)
        {
            ReadLock r_lock(myLock);
            {
                if (map_.find(_key) != map_.end())
                    return true;
                else
                    return false;
            }
            return false;
        }
        value get(key _key)
        {
            ReadLock r_lock(myLock);
            {
                if (map_.find(_key) != map_.end())
                    return map_[_key];
            }
        }

        void insert(key _key, value value_)
        {
            WriteLock w_lock(myLock);
            map_[_key] = value_;
        }
        void insert_pair(key _key, value value_)
        {
            WriteLock w_lock(myLock);
            {
                map_[_key].first = value_.first;
                map_[_key].second = value_.second;
            }
        }
        

        void erase(key _key)
        {
            WriteLock w_lock(myLock);
            map_.erase(_key);
        }

    private:
        std::unordered_map<key, value> map_;
        std::mutex _mutex;

        Lock myLock;
    };
}
#endif
