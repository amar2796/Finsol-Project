#ifndef FIN_COMMON_LOGGER_H
#define FIN_COMMON_LOGGER_H

#include <fstream>
#include <chrono>
#include <queue>
#include <atomic>

namespace FIN
{

    enum class LogLevel
    {
        DEV,

        INFO,
        WARN,
        ERROR,
        FATAL
    };

    const char * toString( LogLevel logLevel )
    {
        switch ( logLevel )
        {
            case LogLevel::DEV   : return "DEV"   ; break ;
            case LogLevel::INFO  : return "INFO"  ; break ;
            case LogLevel::WARN  : return "WARN"  ; break ;
            case LogLevel::ERROR : return "ERROR" ; break ;
            case LogLevel::FATAL : return "FATAL" ; break ;
        }
        return "INVALID" ;
    }

    uint64_t timeNow()
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()
                ).count() ;
    }

    std::string timeNowReadable( uint64_t timeStamp )
    {
        std::time_t time = timeStamp / 1000000 ;
        auto gmtime = std::gmtime( &time) ;
        char buffer[32];
        strftime( buffer, 32, "%Y-%m-%d_%T.", gmtime ) ;
        char microseconds[7];
        sprintf(microseconds, "%06llu", timeStamp % 1000000 );
        std::string t1 ( buffer ) ;
        std::string t2 ( microseconds ) ;
        return t1 + t2 ;
    }

    class Buffer
    {
        struct Item
        {
            Item ( std::string && str ) 
                : _str ( std::move ( str ) )
            {
            }
            std::string _str ;
        };
    };

    class LogQueue
    {
        private : 
            std::queue < std::string > _queue ;
            std::atomic_flag queueFlag ;
    };

    class FileWriter
    {
        public :
            FileWriter ( const std::string & logDirectory, const std::string & fileName, 
                    uint64_t maxFileSize ) 
                : _fileName ( logDirectory + fileName ) ,
                _maxSizeInMB ( maxFileSize * 1024 * 1028 )
        {
            _currentFileNumber = 0 ;
            rollFile() ;
        }

            void write( const std::string & str )
            {
                auto pos = _fileOutStream->tellp() ;
                (*_fileOutStream) << str << std::endl ;
                _bytesWritten += _fileOutStream->tellp() - pos ;
                if ( _bytesWritten > _maxSizeInMB ) 
                {
                    rollFile() ;
                }
            }

        private :
            void rollFile()
            {
                if ( _fileOutStream ) 
                {
                    _fileOutStream->flush() ;
                    _fileOutStream->close() ;
                }
                
                _bytesWritten = 0 ;
                _fileOutStream.reset ( new std::ofstream());
                std::string currentFile = _fileName ;
                currentFile.append( "_" ) ; 

                uint64_t t = timeNow() ;
                currentFile.append(timeNowReadable(t))  ; 
                currentFile.append( "-" ) ; 
                currentFile.append( std::to_string ( ++_currentFileNumber )) ; 
                currentFile.append( ".txt" ) ; 
                _fileOutStream->open( currentFile, std::ofstream::out | std::ofstream::trunc ) ;
            }

            std::string _fileName ;
            uint32_t _currentFileNumber ;
            int64_t _maxSizeInMB ;
            std::streamoff _bytesWritten ;
            std::unique_ptr < std::ofstream > _fileOutStream ;
    };
}

#endif
