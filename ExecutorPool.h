#ifndef JOB_POOL_H
#define JOB_POOL_H

#include <vector>
#include <thread>
#include <atomic>
#include <tuple>
#include "SelectableBlockingQueue.h"

namespace 
{
template <int... Is>
    struct index {};

template <int N, int... Is>
    struct generateSequence : generateSequence<N - 1, N - 1, Is...> {};

template <int... Is>
    struct generateSequence<0, Is...> : index<Is...> {};
}

template< typename ... FunctionArguments >
class JobPool
{
    using ArgumentTuple = std::tuple< FunctionArguments ... >;

    public:
    JobPool( size_t numberOfThreads,
            std::function< void ( FunctionArguments... ) > executor,
            std::function< void () > onIdleCallback = nullptr ) :
    _workingWorkers( 0 ),
    _onIdleCallback ( onIdleCallback ),
    _executor( executor ),
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
                    std::tuple < FunctionArguments... >  jobArguments;
                    if ( _jobs.pop( jobArguments ) )
                    {
                        if( _onIdleCallback )
                            ++_workingWorkers;
                            _execute( jobArguments, generateSequence<sizeof...(FunctionArguments)>{});
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

    ~JobPool()
    {
        _stop = true;
        /*
        size_t numberOfThreads = _workers.size();
        for ( size_t i = 0; i < numberOfThreads; ++i )
        {
            addJob( [] () { return; } ) ;
        }
        */
        for ( auto & worker : _workers )
        {
            worker.detach();
        }
    }

    void addJob( FunctionArguments ... functionArguments )
    {
        auto job = std::tuple < FunctionArguments ... > ( functionArguments ... );
        _jobs.pushBack( job );
    }

private:

template < int... Is>
void _execute(ArgumentTuple tup, index<Is...>)
{
        _executor(std::get<Is>(tup)...);
}

    private:
    std::vector< std::thread >                              _workers;
    SelectableBlockingQueue< ArgumentTuple >                _jobs;
    std::atomic< int >                                      _workingWorkers;
    std::function< void () >                                _onIdleCallback;
    std::function< void ( FunctionArguments ... ) >             _executor;
    bool                                                    _stop;
};
#endif
