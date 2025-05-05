#ifndef FIN_IO_SERVICE_HANDLER_H
#define FIN_IO_SERVICE_HANDLER_H

#include <boost/asio.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

namespace FIN { 
    class IOServiceHandler
    {
        public:
            IOServiceHandler() 
                : service(), thread() 
            {
            }

            ~IOServiceHandler() 
            {
            }

            void start()
            {
                if (thread) 
                    return;

                thread.reset(
                        new boost::thread(
                            boost::bind(&boost::asio::io_service::run, &service)
                            ));
            }

            void stop()
            {
                if (!thread) 
                    return; 

                service.stop();
                thread->join();
                service.reset();
                thread.reset();
            }

            boost::asio::io_service & getIOServiceRef()
            {
                return service ;
            }
        private:
            boost::asio::io_service service;
            boost::scoped_ptr<boost::thread> thread;
    };
}
#endif
