#include "DoubleDispatcher.h"
#include <assert.h>

class Shape { public: virtual ~Shape () {} };
class Circle : public Shape {};
class Rectangle : public Shape {};

static const std::string SimpleCircleDrawer     = "SimpleCircleDrawer";
static const std::string SimpleRectangleDrawer  = "SimpleRectangleDrawer";
static const std::string FancyCircleDrawer      = "FancyCircleDrawer";
static const std::string FancyiRactangleDrawer  = "FancyiRactangleDrawer";

static constexpr int SimpleCircleCalculate      = 1;
static constexpr int SimpleRectangleCalculate   = 2;
static constexpr int FancyCircleCalculate       = 3;
static constexpr int FancyiRactangleCalculate   = 4;

class CalculateInterface
{
 public:
   virtual int operator()( const Circle * ) = 0;
   virtual int operator()( const Rectangle * ) = 0;
};

class DrawerInterface
{
 public:
   virtual const std::string & operator()( const Circle * ) = 0;
   virtual const std::string & operator()( const Rectangle * ) = 0;
};

class Simple
{
    class Drawer : public DrawerInterface
    {
     public:
       const std::string & operator()( const Circle * )
       { return SimpleCircleDrawer; }
       const std::string & operator()( const Rectangle * )
       { return SimpleRectangleDrawer; }
    };
    class Calculate : public CalculateInterface
    {
       int operator()( const Circle * )
        { return SimpleCircleCalculate; }
       int operator()( const Rectangle * )
       { return SimpleRectangleCalculate; }
    };
    public:
    Drawer drawer;
    Calculate calculate;
};
class Fancy
{
    class Drawer : public DrawerInterface
    {
     public:
       const std::string & operator()( const Circle * )
       { return FancyCircleDrawer; }
       const std::string & operator()( const Rectangle * )
       { return FancyiRactangleDrawer; }
    };
    class Calculate : public CalculateInterface
    {
        int operator()( const Circle * )
        { return FancyCircleCalculate; }
        int operator()( const Rectangle * )
        { return FancyiRactangleCalculate; }
    };
    public:
    Drawer drawer;
    Calculate calculate;
};
const std::string & testDraw( DrawerInterface * d, Shape * s )
{
    DynamicDispatcher < DrawerInterface, Shape, Circle, Rectangle >::dispatch( d, s );
}

int testCalculate( CalculateInterface * c, Shape * s )
{
    return DynamicDispatcher < CalculateInterface, Shape, Circle, Rectangle >::dispatch( c, s );
}

int main ()
{
    Shape * circle = new Circle;
    Shape * rectangle = new Rectangle;
    Simple simple;
    Fancy fancy;
    assert ( testDraw ( &simple.drawer, circle ) == SimpleCircleDrawer );
    assert ( testDraw ( &fancy.drawer, rectangle ) == FancyiRactangleDrawer );
    assert ( testCalculate( &fancy.calculate, circle ) == FancyCircleCalculate );
    assert ( testCalculate( &simple.calculate, rectangle ) == SimpleRectangleCalculate );
}
