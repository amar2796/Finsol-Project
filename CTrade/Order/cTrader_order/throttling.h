#ifndef FIN_COMMON_THROTTLING_H
#define FIN_COMMON_THROTTLING_H

#include <iostream>
#include <chrono>
#include <stdlib.h>

namespace FIN {

    template < typename T >
        class CircularArray
        {
            public : 
                CircularArray( long size ) 
                {
                    setSize( size ) ;
                }

                bool full() 
                { 
                    return _fullOnce ; 
                }
                
                void store( T val ) 
                {
                    if ( _fullOnce )
                        moveFront() ;

                    ptr [ _writeIndex ++ ] = val ;
                    
                    if ( _writeIndex >= _maxSize ) 
                    {
                        _fullOnce = true ;
                        _writeIndex %= _maxSize ;
                    }

                }

                T front() 
                { 
                    return ptr[_frontIndex] ;
                }

            private :

                void moveFront()
                {
                    ++_frontIndex ;
                    if ( _frontIndex >= _maxSize ) 
                        _frontIndex %= _maxSize ;
                }

                void setSize ( long size ) 
                {
                    _maxSize = size ;
                    ptr = new T [_maxSize] ;
                    _frontIndex = 0 ;
                    _writeIndex = 0 ;
                    _fullOnce = false ;
                }

                T * ptr ;
                long _frontIndex ;
                long _writeIndex ;
                bool _fullOnce ;
                long _maxSize ;
        };

    class SlidingWindowThrottling
    {
        public : 

            SlidingWindowThrottling ( long msgLimit, double timeLimit )
                : _circularArray ( msgLimit )
            {
                _msgLimit = msgLimit ;
                _timeLimit = timeLimit * 1000000 ;
            }

            void checkThrottleLimit() 
            {
                long long curr = 0 ;

                bool print = true ;
                while (1)
                {
                    if ( !_circularArray.full() ) 
                    {
                        curr  = std::chrono::duration_cast< std::chrono::microseconds >(
                                   std::chrono::system_clock::now().time_since_epoch()).count();
                        _circularArray.store ( curr ) ;
                        //std::cout << "Allowed : " << curr << " | " << ++ n << std::endl ;
                        return ;
                    }
                    else
                    {
                        curr  = std::chrono::duration_cast< std::chrono::microseconds >(
                                   std::chrono::system_clock::now().time_since_epoch()).count();
                        if ( curr - _circularArray.front() > _timeLimit )
                        {
                            _circularArray.store ( curr ) ;
                          //  std::cout << "Allowed : " << curr << " | " << ++ n << std::endl ;
                            return ;
                        }
                        else
                        {
                            if ( print ) 
                            {
                                curr  = std::chrono::duration_cast< std::chrono::microseconds >(
                                        std::chrono::system_clock::now().time_since_epoch()).count();
                                int diff = (1000000 - ( curr  - _circularArray.front() )) ;
                                std::cout << "In Throtlle breach : " << curr << " | front : " << _circularArray.front() << " | Diff : " <<  diff << std::endl ;
                                print = false ;
                                //usleep ( diff ) ;
                            }
                        }
                    }
                }
            }

            long getMsgLimit() const
            { 
                return _msgLimit ; 
            }

            double getTimeLimit() const
            {
                return _timeLimit ;
            }

        private :
            long _msgLimit ;
            double _timeLimit ;

            int n = 0 ;
            CircularArray < long long > _circularArray ;
    };
}

#endif
