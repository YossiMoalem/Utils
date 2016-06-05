#include "ThreadPool.h"
#include <iostream>
#include <sstream>
#include <assert.h>


void test( )
{int i  = 1;
    std::stringstream ss;
    ss <<"Hi: " << i <<std::endl;
    std::cout <<ss.str();
    sleep(1);
    ss <<"Bye: " << i << std::endl ;
    std::cout <<ss.str();

}
void verysimpleUsage_function()
{
    ThreadPool threadPool( 4 );
    for ( int i = 0; i < 10; ++i )
        threadPool.addJob ( &test );

    std::stringstream ss;
    ss <<"Done" <<std::endl;
    std::cout <<ss.str();
    sleep( 5 );
}

void verysimpleUsage_lambda()
{
    ThreadPool threadPool( 4 );
    for ( int i = 0; i < 10; ++i )
        threadPool.addJob ( [ i ] () { 
                std::stringstream ss;
                ss <<"Hi: " << i <<std::endl;
                std::cout <<ss.str();
                sleep(1);
                ss <<"Bye: " << i << std::endl ;
                std::cout <<ss.str();
                } );

    std::stringstream ss;
    ss <<"Done" <<std::endl;
    std::cout <<ss.str();
    sleep( 5 );
}

void test_allJobsAreDone()
{
    const int numberOfJobs = 1000;
    std::vector< int > ind ( numberOfJobs );
    ThreadPool threadPool( 4 );
    for ( int i = 0; i < numberOfJobs; ++i )
        threadPool.addJob ( [ i, & ind ] () { 
                    ind[ i ] = 1;
                } );
    sleep( 2 );
    for ( int i = 0; i < numberOfJobs; ++i )
        assert ( ind[ i ] == 1 );
}

void test_onIdleCallback()
{
    bool callbackCalled = false;
    bool allJobsAdded = false;
    const int numberOfJobs = 100;
    std::vector< int > ind ( numberOfJobs );
    ThreadPool threadPool( 4, [ & callbackCalled, & ind, &allJobsAdded ]
        ()
        {
            if ( allJobsAdded )
            {
                callbackCalled = true;
                for ( int i = 0; i < numberOfJobs; ++i )
                    assert ( ind[ i ] == 1 );
            }

        } );
    for ( int i = 0; i < numberOfJobs; ++i )
    { 
        threadPool.addJob ( [ i, & ind ] () { 
                    ind[ i ] = 1;
                } );
    }
    allJobsAdded = true;
    sleep( 2 );
    assert ( callbackCalled );
}

void test_addWorkAfterFirstLoadIsDone()
{
    const size_t numberOfJobs = 10;
    std::vector< int > ind ( numberOfJobs, 0 );
    std::vector< std::function< void () > > work ( numberOfJobs );
    for( size_t i = 0; i < numberOfJobs; ++i )
    {
        work.emplace_back( [ i, & ind ] () { ind [ i ] ++ ; } );
    }

    int callbackCounter = 0;
    ThreadPool threadPool( 4, [ & callbackCounter, & ind, &work, &threadPool ]
    ()
    {
        if ( callbackCounter < 10 )
        {
            for ( size_t i = 0; i < numberOfJobs; ++i )
            {
                assert ( ind[ i ] ++ == callbackCounter );
            }
            threadPool.addJob( work.begin(), work.end() );
        }
    } ) ;
    threadPool.addJob( work.begin(), work.end() );
    sleep( 5 );
}

int main()
{
    verysimpleUsage_lambda();
    verysimpleUsage_function();
    test_allJobsAreDone();
    test_onIdleCallback();
    test_addWorkAfterFirstLoadIsDone();
}
