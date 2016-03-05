#include "ThreadPool.h"
#include <iostream>
#include <sstream>
#include <assert.h>


void verysimpleUsage()
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

int main()
{
    //verysimpleUsage();
    test_allJobsAreDone();
    test_onIdleCallback();
}
