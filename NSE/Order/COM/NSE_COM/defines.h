#ifndef FIN_DEFINES_H
#define FIN_DEFINES_H

#include <thread>
namespace FIN
{

    /*
     * TODO : Make this platform independent use int64_t kind of data types here.
     * NOTE : For backward compatibility only.
     * */
    typedef double  DOUBLE;
    typedef long LONG;
    typedef long long LONGLONG;

    typedef LONG SIZE;
    typedef DOUBLE PRICE;

    inline bool isSafeShortConvert(int val)
    {
        if (val < SHRT_MIN || val > SHRT_MAX) {
            return false;
        }
        return true;
    }

    inline bool isSafeCharConvert(int val)
    {
        if (val< CHAR_MIN || val>CHAR_MAX)
        {
            return false;
        }
        return true;
    }
    inline bool isSafeUCharConvert(int val)
    {
        if (val < 0 || val>255)
        {
            return false;
        }
        return true;
    }


    //**********************************************************************//
    // Remove above code  if willing to test the LME functionality again :D.//
    //**********************************************************************//
#define DEVLOG_1( ARG )\
    std::cout << "[" <<std::this_thread::get_id() << "]" "("<< __FILE__<<":"<<__LINE__ << ") " << ARG<< std::endl

#define DEVLOG( ARG )\
    std::cout << ARG<< std::endl
    namespace FIN_TYPE
    {
        typedef double DOUBLE;
        typedef int64_t LONG;
        typedef uint64_t UNSIGNED_LONG;
        typedef uint8_t UNSIGNED_CHAR;

        //typedef char EXCH_ORDERID[20] ;
        typedef LONG EXCH_ORDERID;
        typedef char SECURITY_ID[20];
        typedef char TEXT[100];

        typedef LONG CL_ORDERID;
        typedef DOUBLE PRICE;
        typedef LONG QTY;
        typedef UNSIGNED_LONG TIME;
        typedef UNSIGNED_CHAR ENUM;

    }
}

#endif
