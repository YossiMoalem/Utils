#include "DoubleDispatcher.h"

#include <iostream>
class Shape { public: virtual ~Shape () {} };
class Circle : public Shape {};
class Rectangle : public Shape {};

class CalculateInterface
{
 public:
   virtual int operator()( const Circle * ) = 0;
   virtual int operator()( const Rectangle * ) = 0;
};

class DrawerInterface
{
 public:
   virtual void operator()( const Circle * ) = 0;
   virtual void operator()( const Rectangle * ) = 0;
};

class Simple
{
    class Drawer : public DrawerInterface
    {
     public:
       void operator()( const Circle * )
       { std::cout <<"Draw Ciecle" <<std::endl; }
       void operator()( const Rectangle * )
       { std::cout <<"Draw Rectangle" <<std::endl; }
    };
    class Calculate : public CalculateInterface
    {
        int operator()( const Circle * )
        { std::cout <<"Calculating Cicle" <<std::endl; }
       int operator()( const Rectangle * )
       { std::cout <<"Calculating Rectangle" <<std::endl; }
    };
    public:
    Drawer d;
    Calculate c;
};
class Fancy
{
    class Drawer : public DrawerInterface
    {
     public:
       void operator()( const Circle * )
       { std::cout <<"Draw Fancy Ciecle" <<std::endl; }
       void operator()( const Rectangle * )
       { std::cout <<"Draw Fancy Rectangle" <<std::endl; }
    };
    class Calculate : public CalculateInterface
    {
        int operator()( const Circle * )
        { std::cout <<"Fancy Calculating Cicle" <<std::endl; }
        int operator()( const Rectangle * )
        { std::cout <<"Fancy Calculating Rectangle" <<std::endl; }
    };
    public:
    Drawer d;
    Calculate c;
};
void testDraw( DrawerInterface * d, Shape * s )
{
    DynamicDispatcher < DrawerInterface, Shape, Circle, Rectangle >::dispatch( d, s );
}
void testCalculate( CalculateInterface * c, Shape * s )
{
    DynamicDispatcher < CalculateInterface, Shape, Circle, Rectangle >::dispatch( c, s );
}

int main ()
{
    Shape * s1 = new Circle;
    Shape * s2 = new Rectangle;
    Shape * s3 = new Shape;
    Simple s;
    Fancy f;
    testDraw ( &s.d, s1 );
    testDraw ( &f.d, s2 );
    testCalculate( &f.c, s1 );
    testCalculate( &s.c, s2 );

}
