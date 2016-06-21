#ifndef DOUBLE_DISPATCHER_IMPL
#define DOUBLE_DISPATCHER_IMPL

#include <type_traits>
#include "typelist.h"

template< typename Executor, typename BaseClass, typename ... Ts >
class DynamicDispatcherImpl;

template < typename Executor,
    typename BaseClass,
    typename Class,
    typename ... Classes
    >
class DynamicDispatcherImpl < Executor, BaseClass, std::tuple< Class, Classes ... > >
{
    public:
    static auto dispatch( Executor * e, BaseClass* p1 ) ->
        decltype( std::declval< Executor >().operator()( static_cast< Class * > ( p1 ) ) )
    {
        if ( Class * t1 = dynamic_cast< Class * >( p1 ) )
        {
            return (*e)( t1 );
        } else {
            return DynamicDispatcherImpl< Executor, BaseClass, std::tuple< Classes ... > >::dispatch( e, p1 );
        }
    }
};


template< typename Executor, 
    typename BaseClass, 
    typename LastClass >
class DynamicDispatcherImpl < Executor, BaseClass, std::tuple< LastClass > >
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



template< typename Executor, typename BaseClass, typename t1, typename ... Ts >
class DynamicDispatcher
{
    public:
    static auto dispatch( Executor * e, BaseClass* p1 ) ->
        decltype( std::declval< Executor >().operator()( static_cast< t1* > ( p1 ) ) )
        {
            typedef std::tuple< t1, Ts ... > AllClasses;
            DynamicDispatcherImpl< Executor, BaseClass, AllClasses > ::dispatch( e, p1 );
    }
};
#endif
