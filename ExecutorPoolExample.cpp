#include "ExecutorPool.h"
#include <iostream>

void f(int t ){ std::cout <<"Called"; }



 int main()
{
    JobPool< int > jobPool_1(1, []( int i ){std::cout << i <<std::endl; }); 
    for ( int i = 0; i < 10; ++i )
        jobPool_1.addJob( i );

    JobPool< int, int > jobPool_2(1, []( int i, int ii ){std::cout << i <<":"<<ii <<std::endl; }); 
    for ( int i = 0; i < 3; ++i )
        for ( int ii = 0; ii < 3; ++ii )
            jobPool_2.addJob( i, ii );
    sleep(1);
}
