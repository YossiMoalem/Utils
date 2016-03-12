#include "SelectableBlockingQueue.h"
#include <assert.h>
#include <thread>
#include <boost/circular_buffer.hpp>

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_flush( QueueCtorArgs ... queueCtorArgs )
{
    // Run this under valgrind to see that all allocations are released.
    SelectableBlockingQueue< int, QueueType > intQueue { queueCtorArgs ...  };
    SelectableBlockingQueue< int *, QueueType > intPtrQueue { queueCtorArgs ... };
    SelectableBlockingQueue< int *, QueueType > intPtrQueueCustomDeleter{ queueCtorArgs ... };

    intPtrQueueCustomDeleter.setDisposeMethod( [] ( int * ptr ) {
        delete ptr ; } );

    intQueue.pushBack( 1 );
    intQueue.pushBack( 2 );
    intQueue.pushBack( 3 );

    intPtrQueue.pushBack( new int  );
    intPtrQueue.pushBack( new int  );
    intPtrQueue.pushBack( new int  );

    intPtrQueueCustomDeleter.pushBack( new int  );
    intPtrQueueCustomDeleter.pushBack( new int  );
    intPtrQueueCustomDeleter.pushBack( new int  );

    intQueue.flush();
    intPtrQueue.flush();
    intPtrQueueCustomDeleter.flush();
}

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_pushPop( QueueCtorArgs ... queueCtorArgs )
{
    SelectableBlockingQueue< int, QueueType > intQueue{ queueCtorArgs ... }  ;
    intQueue.pushBack( 1 );
    intQueue.pushBack( 2 );
    intQueue.pushBack( 3 );

    int i1;
    int i2;
    int i3;
    assert ( intQueue.pop( i1 ) ) ;
    assert ( intQueue.pop( i2 ) );
    assert ( intQueue.pop( i3 ) );

    assert ( i1 == 1 );
    assert ( i2 == 2 );
    assert ( i3 == 3 );
} 

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_tryPop( QueueCtorArgs ... queueCtorArgs )
{
    SelectableBlockingQueue< int, QueueType > intQueue { queueCtorArgs ... };

    int i1 = 1;
    assert( intQueue.try_pop( i1 ) == false && i1 == 1 );
    assert( intQueue.try_pop( i1 ) == false && i1 == 1 );
    assert( intQueue.try_pop( i1 ) == false && i1 == 1 );

    intQueue.pushBack( 2 );
    assert( intQueue.try_pop( i1 ) == true && i1 == 2 );
}

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_pushFrontPop( QueueCtorArgs ... queueCtorArgs )
{
    SelectableBlockingQueue< int, QueueType > intQueue { queueCtorArgs ... };
    intQueue.pushFront( 1 );
    intQueue.pushFront( 2 );
    intQueue.pushFront( 3 );

    int i1;
    int i2;
    int i3;
    assert ( intQueue.pop( i1 ) );
    assert ( intQueue.pop( i2 ) );
    assert ( intQueue.pop( i3 ) );

    assert ( i1 == 3 );
    assert ( i2 == 2 );
    assert ( i3 == 1 );

}

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_block( QueueCtorArgs ... queueCtorArgs )
{
    SelectableBlockingQueue< int, QueueType > intQueue{ queueCtorArgs ... };
    std::thread t( [ & intQueue ] () { sleep( 1 );
            intQueue.pushBack( 1 );
            } ) ;
    int i1 = 0;
    assert ( intQueue.pop( i1 ) );
    assert( i1 = 1 ); 
    t.join();
}

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_select( QueueCtorArgs ... queueCtorArgs )
{
    SelectableBlockingQueue< int, QueueType > intQueue { queueCtorArgs ... };
    int queueEventFD = intQueue.getFD();
    fd_set readFdSet;
    FD_ZERO( & readFdSet );
    FD_SET( queueEventFD, & readFdSet );

    std::thread t( [ & intQueue ] () { sleep( 1 );
            intQueue.pushBack( 1 );
            } ) ;

    int res = select( queueEventFD + 1 , & readFdSet, NULL, NULL, NULL );
    assert ( res == 1 );
    assert( FD_ISSET( queueEventFD, & readFdSet ) );

    int i1 = 0;
    assert ( intQueue.pop( i1 ) );
    assert( i1 = 1 ); 
    t.join();
}

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_addIterator(QueueCtorArgs ... queueCtorArgs )
{
    std::vector< int > originalVector { 1,2,3,4,5,6,7,8,9,0 };
    std::vector< int > popedVector( originalVector.size() );
    SelectableBlockingQueue< int, QueueType > intQueue { queueCtorArgs ...  };
    intQueue.pushBack( originalVector.begin(), originalVector.end() );
    for ( size_t i = 0; i < originalVector.size(); ++i )
    {
        assert( intQueue.pop(  popedVector[ i ] ) );
    }
    for ( size_t i = 0; i < originalVector.size(); ++i )
    {
        assert ( originalVector[ i ] == popedVector[ i ] );
    }
}

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_addFrontIterator(QueueCtorArgs ... queueCtorArgs )
{
    std::vector< int > originalVector { 1,2,3,4,5,6,7,8,9,0 };
    std::vector< int > popedVector( originalVector.size() );
    SelectableBlockingQueue< int, QueueType > intQueue { queueCtorArgs ...  };
    intQueue.pushFront( originalVector.begin(), originalVector.end() );
    for ( size_t i = 0; i < originalVector.size(); ++i )
    {
        assert( intQueue.pop(  popedVector[ i ] ) );
    }
    for ( size_t i = 0; i < originalVector.size(); ++i )
    {
        assert ( originalVector[ i ] == popedVector[ originalVector.size() - i - 1 ] );
    }
}

#define TEST(Name) std::cout <<"Testing: "<< #Name <<std::endl; Name

template< 
 template <class ... > class QueueType,
 typename ... QueueCtorArgs >
void test_all( QueueCtorArgs ... queueCtorArgs )
{
    TEST( test_flush )          < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
    TEST( test_pushPop )        < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
    TEST( test_tryPop )         < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
    TEST( test_pushFrontPop )   < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
    TEST( test_block )          < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
    TEST( test_select )         < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
    TEST( test_addIterator )    < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
    TEST( test_addFrontIterator ) < QueueType, QueueCtorArgs ... >( queueCtorArgs ... );
}

int main()
{
    std::cout <<"Testing With std::deque"<<std::endl;
    test_all< std::deque > ();
    std::cout <<"Testing With Boost::circular_buffer"<<std::endl;
    test_all< boost::circular_buffer > ( 10 );
    SelectableBlockingQueue< int, std::deque> intQueue{}; 

}
