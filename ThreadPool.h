#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <thread>
#include <atomic>
#include "SelectableBlockingQueue.h"

class ThreadPool
{
    public:
    ThreadPool( size_t numberOfThreads, 
                std::function< void () > onIdleCallback = nullptr ) :
    _workingWorkers( 0 ),
    _onIdleCallback ( onIdleCallback ),
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
                    if ( _jobs.pop( job ) )
                    {
                        if( _onIdleCallback )
                            ++_workingWorkers;
                        job();
                        if( _onIdleCallback )
                        {
                            --_workingWorkers;
                            if ( _workingWorkers == 0 && _jobs.empty() )
                                _onIdleCallback();
                        }

                    }
                }
            } ) ;
        }
    }

    ThreadPool( const ThreadPool & ) = delete;
    ThreadPool&  operator= (const ThreadPool& ) = delete;

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
        auto job = std::function< void () > ( std::bind ( std::forward< Function > ( function ), 
        std::forward< FunctionArguments > ( functionArguments ) ... ) );
        _jobs.pushBack( job );
    }

    private:
    std::vector< std::thread >                              _workers;
    SelectableBlockingQueue< std::function < void () > >    _jobs;
    std::atomic< int >                                      _workingWorkers;
    std::function< void () >                                _onIdleCallback;
    bool                                                    _stop;
};
#endif
