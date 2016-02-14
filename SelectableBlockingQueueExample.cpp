#include "SelectableBlockingQueue.h"
#include <assert.h>
#include <thread>

void test_flush()
{
    // Run this under valgrind to see that all allocations are released.
    SelectableBlockingQueue< int > intQueue;
    SelectableBlockingQueue< int * > intPtrQueue;
    SelectableBlockingQueue< int * > intPtrQueueCustomDeleter( [] ( int * ptr ) { 
            delete ptr; } );

    intQueue.push( 1 );
    intQueue.push( 2 );
    intQueue.push( 3 );

    intPtrQueue.push( new int  );
    intPtrQueue.push( new int  );
    intPtrQueue.push( new int  );

    intPtrQueueCustomDeleter.push( new int  );
    intPtrQueueCustomDeleter.push( new int  );
    intPtrQueueCustomDeleter.push( new int  );

    intQueue.flush();
    intPtrQueue.flush();
    intPtrQueueCustomDeleter.flush();
}

void test_pushPop()
{
    SelectableBlockingQueue< int > intQueue;
    intQueue.push( 1 );
    intQueue.push( 2 );
    intQueue.push( 3 );

    int i1;
    int i2;
    int i3;
    intQueue.pop( i1 );
    intQueue.pop( i2 );
    intQueue.pop( i3 );

    assert ( i1 == 1 );
    assert ( i2 == 2 );
    assert ( i3 == 3 );
} 

void test_tryPop()
{
    SelectableBlockingQueue< int > intQueue;

    int i1 = 1;
    assert( intQueue.try_pop( i1 ) == false && i1 == 1 );
    assert( intQueue.try_pop( i1 ) == false && i1 == 1 );
    assert( intQueue.try_pop( i1 ) == false && i1 == 1 );

    intQueue.push( 2 );
    assert( intQueue.try_pop( i1 ) == true && i1 == 2 );
}

void test_pushFrontPop()
{
    SelectableBlockingQueue< int > intQueue;
    intQueue.pushFront( 1 );
    intQueue.pushFront( 2 );
    intQueue.pushFront( 3 );

    int i1;
    int i2;
    int i3;
    intQueue.pop( i1 );
    intQueue.pop( i2 );
    intQueue.pop( i3 );

    assert ( i1 == 3 );
    assert ( i2 == 2 );
    assert ( i3 == 1 );

}

void test_block()
{
    SelectableBlockingQueue< int > intQueue;
    std::thread t( [ & intQueue ] () { sleep( 1 );
            intQueue.push( 1 );
            } ) ;
    int i1 = 0;
    intQueue.pop( i1 );
    assert( i1 = 1 ); 
    t.join();
}

void test_select()
{
    SelectableBlockingQueue< int > intQueue;
    int queueEventFD = intQueue.getFD();
    fd_set readFdSet;
    FD_ZERO( & readFdSet );
    FD_SET( queueEventFD, & readFdSet );

    std::thread t( [ & intQueue ] () { sleep( 1 );
            intQueue.push( 1 );
            } ) ;

    int res = select( queueEventFD + 1 , & readFdSet, NULL, NULL, NULL );
    assert ( res == 1 );
    assert( FD_ISSET( queueEventFD, & readFdSet ) );

    int i1 = 0;
    intQueue.pop( i1 );
    assert( i1 = 1 ); 
    t.join();
}

int main()
{
    test_flush();
    test_pushPop();
    test_tryPop();
    test_pushFrontPop();
    test_block();
    test_select();
    test_select();
}
