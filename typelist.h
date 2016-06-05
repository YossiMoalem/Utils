#include <type_traits>
#include <typeinfo>
#include <tuple>

//----------------------------------------
template< class ... Types > 
struct MostDerived
{
    typedef typename MostDerived< std::tuple< Types ... > > :: value value;
};


template< class Head, class ... Tail > 
struct MostDerived < std::tuple< Head, Tail ... > >
{
    typedef typename MostDerived< Tail ... > :: value Candidate;
    typedef typename std::conditional <
        std::is_base_of< Candidate, Head >::value,
            Head,
            Candidate >::type value;
};

template< class Head > 
struct MostDerived < std::tuple< Head > >
{
    typedef Head value;
};

template< class Head, class Tail > 
struct MostDerived < std::tuple< Head, Tail > >
{
    typedef typename std::conditional <
        std::is_base_of< Tail, Head >::value,
            Head,
            Tail >::type
            value;
};

//----------------------------------------
template< class Head, class ... Tail >
struct ConcatinateToHead;

template< class Head, class ... Tail >
struct ConcatinateToHead< Head, std::tuple< Tail ... > >
{
    typedef typename std::conditional<
        std::is_same< Head, std::tuple<> > :: value,
        std::tuple< Tail... >,
        std::tuple< Head, Tail ... > > :: type  value;
};

template< class Head, class ... Tail >
struct ConcatinateToHead
{
    typedef typename std::conditional<
        std::is_same< Head, std::tuple<> > :: value,
        std::tuple< Tail... >,
        std::tuple< Head, Tail ... > > :: type  value;
};

//----------------------------------------
template< class Head, class ... Tail >
struct ConcatinateToTail;

template< class Head, class ... Tail >
struct ConcatinateToTail< Head, std::tuple< Tail ... > >
{
    typedef typename std::conditional<
        std::is_same< Head, std::tuple<> > :: value,
        std::tuple< Tail... >,
        std::tuple< Tail ..., Head > > :: type  value;
};

template< class Head, class ... Tail >
struct ConcatinateToTail
{
    typedef typename std::conditional<
        std::is_same< Head, std::tuple<> > :: value,
        std::tuple< Tail... >,
        std::tuple< Tail ..., Head > > :: type  value;
};

//----------------------------------------
template< class Type, class ... Types>
struct FilterType;

template < class Type, class ... Ts >
struct FilterType< Type, std::tuple< Ts ... > >
{
    typedef typename FilterType< Type, Ts ... > :: value  value;
};

template < class Type, class Head, class ...Tail >
struct FilterType< Type, Head, Tail ... >
{
    typedef typename std::conditional<
        std::is_same< Type, Head > :: value,
        typename FilterType< Type, Tail ... > :: value,
        typename ConcatinateToHead< Head, typename FilterType< Type, Tail ... > :: value > :: value
        > :: type value;
};

template < class Type >
struct FilterType< Type >
{
    typedef std::tuple<> value;
};

//----------------------------------------

template< class ToBeSorted, class Sorted >
struct Sort_
{
    typedef typename MostDerived< ToBeSorted >::value mostDerived;
    typedef typename ConcatinateToTail< mostDerived, Sorted >::value sorted;
    typedef typename FilterType< mostDerived, ToBeSorted >::value leftToBeSorted;
    typedef typename Sort_< leftToBeSorted, sorted >:: value tailSorted;
    typedef typename std::conditional<
        std::is_same< leftToBeSorted, std::tuple<> >::value,
        sorted,
        typename Sort_< leftToBeSorted, sorted > :: tailSorted > ::type  value;
};
template< class Sorted >
struct Sort_< std::tuple<>, Sorted>
{
    typedef std::tuple<> tailSorted;
    typedef Sorted value;
};


template < class ... Types >
struct Sort
{
    typedef typename Sort_< std::tuple< Types ... >, std::tuple< > > :: value value;
};

    /*
            */
