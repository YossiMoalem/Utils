#include "ThreadPool.h"
#include <iostream>
#include <sstream>


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
    std::cout <<ss;
    sleep( 5 );
}

int main()
{
    verysimpleUsage();
}
