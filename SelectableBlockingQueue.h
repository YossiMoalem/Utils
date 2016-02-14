#ifndef BLOCKING_QUEUE
#define BLOCKING_QUEUE

#include <deque>
#include <mutex>
#include <condition_variable>
#include <sys/eventfd.h>
#include <unistd.h>             /* read and write */
#include <stdint.h>             /* Definition of uint64_t */
#include <type_traits>

template< typename DataType >
using DisposeMethod = std::function< void ( DataType d )> ;

namespace
{

template< typename DataType >
    typename std::enable_if < ! std::is_pointer < DataType >::value > ::type
    dealocate( DataType dt )
{ }

template< typename DataType >
    typename std::enable_if < std::is_pointer < DataType >::value >::type
dealocate( DataType dt )
{
    delete dt;
}
} //namespace

template< typename DataType >
class SelectableBlockingQueue 
{
 public:
   SelectableBlockingQueue() :
       SelectableBlockingQueue( [] ( DataType dt ) { dealocate< DataType > ( dt ) ; } )
    {}

   SelectableBlockingQueue( DisposeMethod< DataType > disposeMethod ):
       _disposeMethod( disposeMethod ),
       _eventFD( eventfd( 0, EFD_SEMAPHORE ) )
    {}

   SelectableBlockingQueue( const SelectableBlockingQueue & ) = delete;
   SelectableBlockingQueue & operator = ( const SelectableBlockingQueue & ) = delete;

   ~SelectableBlockingQueue()
   {
       this->flush();
   }

   bool pushFront( const DataType & data )
   { 
       return _doPush ( data, true ); 
   }

   bool push( const DataType & data )
   { 
       return _doPush ( data, false ); 
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
       _queueEmptyCondition.notify_all();
   }

   bool try_pop( DataType & data )
   {
       std::unique_lock< std::mutex > lock(_queueMutex);
       return _doPop( data );
   }

   void pop( DataType & data )
   {
       std::unique_lock< std::mutex > lock(_queueMutex);
       while( _queue.empty() )
       {
           _queueEmptyCondition.wait( _queueMutex );
       }
       _doPop( data );
   }

 private:
   bool _doPush( const DataType & data, bool first )
   {
       if ( first )
       {
           std::unique_lock< std::mutex > lock(_queueMutex);
           _queue.push_front( data );
       } else {
           std::unique_lock< std::mutex > lock(_queueMutex);
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
   std::deque<DataType>             _queue;
   DisposeMethod< DataType >        _disposeMethod;
   int                             _eventFD;
};
#endif
