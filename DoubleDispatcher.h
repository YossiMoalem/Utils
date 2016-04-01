#ifndef DOUBLE_DISPATCHER_IMPL
#define DOUBLE_DISPATCHER_IMPL

#include <type_traits>

template< typename Executor, typename ... Ts >
class DynamicDispatcher;

template < typename Executor,
    typename BaseClass,
    typename Class,
    typename ... Classes
    >
class DynamicDispatcher< Executor, BaseClass, Class, Classes ... >
{
    public:
    static auto dispatch( Executor * e, BaseClass* p1 ) ->
        decltype( std::declval< Executor >().operator()( static_cast< Class * > ( p1 ) ) )
    {
        if ( Class * t1 = dynamic_cast< Class * >( p1 ) )
        {
            return (*e)( t1 );
        } else {
            return DynamicDispatcher< Executor, BaseClass, Classes ... >::dispatch( e, p1 );
        }
    }
};


template< typename Executor, 
    typename BaseClass, 
    typename LastClass >
class DynamicDispatcher< Executor, BaseClass, LastClass >
{
    public:
    static auto dispatch( Executor * e, BaseClass* p1 ) ->
        decltype( std::declval< Executor >().operator()( static_cast< LastClass * > ( p1 ) ) )
    {
        if ( LastClass * t1 = dynamic_cast< LastClass * >( p1 ) )
        {
            return (*e)( t1 );
        } else {
            throw 1;
        }
    }
};
#endif
