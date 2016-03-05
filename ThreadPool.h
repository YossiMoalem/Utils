#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include "SelectableBlockingQueue.h"

class ThreadPool
{
    public:
    ThreadPool( size_t numberOfThreads ) :
    _stop( false )
    {
        _workers.reserve( numberOfThreads );
        for ( size_t i = 0; i < numberOfThreads; ++i )
        {
            _workers.emplace_back( [ this ] 
            ()
            {
                while ( ! _stop )
                {
                    std::function< void( ) > job;
                    _jobs.pop( job );
                    job();
                }
            } ) ;
        }
    }

    ~ThreadPool()
    {
        _stop = true;
        size_t numberOfThreads = _workers.size();
        for ( size_t i = 0; i < numberOfThreads; ++i )
        {
            addJob( [] () { return; } ) ;
        }
        for ( auto & worker : _workers )
        {
            worker.join();
        }
    }

    template< typename Function,
        typename ... FunctionArguments >
    void addJob( Function && function, FunctionArguments ... functionArguments )
    {
        auto job = std::function< void () > ( std::bind ( function, functionArguments ... ) );
        _jobs.pushBack( job );
    }

    private:
    std::vector< std::thread >                              _workers;
    SelectableBlockingQueue< std::function < void () > >    _jobs;
    bool                                                    _stop;
};
#endif
