#include <atomic>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <mutex>
#include <utility>

/**
 * @brief For Single Reader And Single Writer Only
 */

template <typename T>
class LockFreeQueue 
{
    private:
        struct Node 
        {
            Node( T val ) : value(val), next(nullptr) { }
            T value;
            Node* next;
        };
        Node* first;                     // for producer only
        std::atomic<Node*> last;         // shared
        std::condition_variable conditionalVariable;
        std::atomic<bool> isAlive;

        void internalPop(T& result )
        {
            Node *nodeBeingProcessed = first;
            result  = std::move( first->value );         // C: copy it back
            first   = first->next;             // D: publish that we took it
            delete nodeBeingProcessed;
        }

    public:
        LockFreeQueue() 
        {
            first = last = new Node( T() );           //Dummy Node
            isAlive = true;
        }

        ~LockFreeQueue() {
            while( first != nullptr ) {               //Clean all elements including dummy.
                Node* tmp = first;
                first = tmp->next;
                delete tmp;
            }
        }

        //----------------------------------------------------------------------

        bool status()
        {
            if( isAlive )
            {
                return true;
            }
            else
            {
                return false;
            }
        }


        //----------------------------------------------------------------------

        void clean()
        {
            while( first != nullptr ) {               //Clean all elements including dummy.
                Node* tmp = first;
                first = tmp->next;
                delete tmp;
            }
        }

        //----------------------------------------------------------------------

        void start()
        {
            if(!isAlive)
            {
                clean();
                first = last = new Node( T() );         //Dummy Node
                isAlive = true;
            }
        }

        //----------------------------------------------------------------------

        void putToSleep()
        {
            isAlive = false;
            clean();
            conditionalVariable.notify_one();
        }

        //----------------------------------------------------------------------

        void push( const T& t ) 
        {
            if( isAlive )
            {
                (last.load())->next = new Node( T() );    // Dummy Node
                (last.load())->value = t;
                last  = (last.load())->next;
                conditionalVariable.notify_one();
            }
        }

        //----------------------------------------------------------------------

        bool try_pop(T& result )
        {
            if( first != last.load() )
            {
                internalPop( result );
                return true;
            }
            return false;
        }


        //----------------------------------------------------------------------

        bool pop( T& result) 
        {
            if(! try_pop( result ) )
            {
				//std::cout << "Queue Empty now" << std::endl;
                //If queue is empty, wait on conditional variable, need to check if element has been
                //successfully retrieved or not.
				//printf( "Empty\n" );
                std::mutex mutex;
                std::unique_lock<std::mutex> lock(mutex);
                conditionalVariable.wait( lock );
                if( isAlive )
                {
                    return try_pop( result );
                }
            }
            else
            {
                //Successfully retrieved the element.
                return true;
            }
            //element not retrieved.
            return false;
        }

};
