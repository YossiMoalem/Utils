#ifndef BLOCKING_QUEUE
#define BLOCKING_QUEUE

#include <deque>
#include <mutex>
#include <condition_variable>
#include <sys/eventfd.h>
#include <unistd.h>             /* read and write */
#include <stdint.h>             /* Definition of uint64_t */
#include <type_traits>
#include <functional>
#include <iterator>

template< typename DataType >
using DisposeMethod = std::function< void ( DataType d )> ;

namespace
{
template< typename DataType >
    typename std::enable_if < ! std::is_pointer < DataType >::value > ::type
    dealocate( DataType dt )
{ ; }

template< typename DataType >
    typename std::enable_if < std::is_pointer < DataType >::value >::type
dealocate( DataType dt )
{ delete dt; }
} //namespace

template< typename DataType ,
 template <class ... > class QueueType = std::deque >
class SelectableBlockingQueue 
{
 public:
 template< typename ... QueueCtorArgs >
   SelectableBlockingQueue( QueueCtorArgs ... queueCtorArgs ):
       _queue( queueCtorArgs ...  ),
       _disposeMethod([] ( DataType dt ) { dealocate ( dt ); } ),
       _eventFD( eventfd( 0, EFD_SEMAPHORE ) ),
       _stop( false )
    {}
   SelectableBlockingQueue( const SelectableBlockingQueue & ) = delete;
   SelectableBlockingQueue & operator = ( const SelectableBlockingQueue & ) = delete;

   ~SelectableBlockingQueue()
   {
       _stop = true;
       _queueEmptyCondition.notify_all();
       this->flush();
   }
    
    void setDisposeMethod( DisposeMethod< DataType > disposeMethod )
    {
        _disposeMethod = disposeMethod;
    }

   bool pushFront( const DataType & data )
   { 
       std::unique_lock< std::mutex > lock(_queueMutex);
       return _doPush ( data, true ); 
   }

    template < typename Iterator >
   bool pushFront(  const Iterator first, const Iterator last )
   { 
       std::unique_lock< std::mutex > lock(_queueMutex);
       return _doPush ( first, last, true ); 
   }

   bool pushBack( const DataType & data )
   { 
       std::unique_lock< std::mutex > lock(_queueMutex);
       return _doPush ( data, false ); 
   }
    
    template < typename Iterator >
   bool pushBack(  const Iterator first, const Iterator last )
   { 
       std::unique_lock< std::mutex > lock(_queueMutex);
       return _doPush ( first, last, false ); 
   }


   int getFD() const
   {
       return _eventFD;
   }

   bool empty() const
   {
       std::unique_lock< std::mutex > lock(_queueMutex);
       return _queue.empty();
   }

   void flush()
   {
       std::unique_lock< std::mutex > lock(_queueMutex);
       while( ! _queue.empty() )
       {
           DataType d;
           _doPop( d );
           _disposeMethod( d );
       }
       _queueEmptyCondition.notify_one();
   }

   bool try_pop( DataType & data )
   {
       std::unique_lock< std::mutex > lock(_queueMutex);
       return _doPop( data );
   }

   bool pop( DataType & data )
   {
       std::unique_lock< std::mutex > lock(_queueMutex);
       while( _queue.empty() && ! _stop )
       {
           _queueEmptyCondition.wait( _queueMutex );
       }
       return _doPop( data );
   }

 private:
    template < typename Iterator >
    bool _doPush( const Iterator first, const Iterator last, bool front )
    {
        Iterator currentItem = first;
        while ( currentItem != last )
        {
            _doPush( * currentItem, front );
            currentItem++;
        }
        return true;
    }

   bool _doPush( const DataType & data, bool first )
   {
       if ( first )
       {
           _queue.push_front( data );
       } else {
           _queue.push_back( data );
       }
       uint64_t dummy = 1;
       write( _eventFD, &dummy, sizeof( dummy ) );
       _queueEmptyCondition.notify_all();
       return true;
   }

   bool _doPop( DataType & data )
   {
       if( _queue.empty() )
       {
           return false;
       }

       ssize_t dummy;
       read( _eventFD, & dummy, sizeof( dummy ) );
       data = _queue.front();
       _queue.pop_front();
       return true;
   }

 private:
   mutable std::mutex               _queueMutex;
   std::condition_variable_any      _queueEmptyCondition;
   QueueType< DataType >            _queue;
   DisposeMethod< DataType >        _disposeMethod;
   int                              _eventFD;
   bool                             _stop;
};
#endif
