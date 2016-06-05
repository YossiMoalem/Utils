#include "typelist.h"

#include <iostream>

class Shape { public: virtual ~Shape () {} };
class Poligon : public Shape {};
class Rectangle : public Poligon{};
class AppleRectangle : public Rectangle{};
class Circle : public Shape {};
class Triangle: public Poligon {};

#define allshapes   Poligon, Rectangle, AppleRectangle, Circle, Triangle, Shape
int main()
{
      //Stage 0
      typedef MostDerived < allshapes >::value MostDerived0;
      static_assert( std::is_same< MostDerived0, Triangle >::value, "" );
      typedef FilterType< MostDerived0, allshapes >::value  noTriangle;
      static_assert( std::is_same< std::tuple< Poligon, Rectangle, AppleRectangle, Circle, Shape >, noTriangle >::value, "");

      //Stage 
      typedef MostDerived < noTriangle >::value MostDerived11;
      static_assert( std::is_same< MostDerived11, Circle >::value, "" );
      typedef FilterType< MostDerived11, noTriangle >::value  allButSome;
      static_assert( std::is_same< std::tuple< Poligon, Rectangle, AppleRectangle, Shape >, allButSome >::value, "");
      
      //Stage 1
      typedef MostDerived < allButSome >::value MostDerived1;
      static_assert( std::is_same< MostDerived1, AppleRectangle >::value, "" );
      typedef FilterType< MostDerived1, allButSome >::value  allButApple;
      static_assert( std::is_same< std::tuple< Poligon, Rectangle, Shape >, allButApple >::value, "");

      //Stage 2
      typedef MostDerived < allButApple >::value MostDerived2;
      static_assert( std::is_same< MostDerived2, Rectangle >::value, "" );
      typedef FilterType< MostDerived2, allButApple >::value  allButRects;
      static_assert( std::is_same< std::tuple< Poligon, Shape >, allButRects >::value, "");

      //Stage 3
      typedef MostDerived < allButRects >::value MostDerived3;
      static_assert( std::is_same< MostDerived3, Poligon >::value, "" );
      typedef FilterType< MostDerived3, allButRects >::value  allButConcPoligons;
      static_assert( std::is_same< std::tuple< Shape >, allButConcPoligons >::value, "");


      //Sort:
      typedef typename Sort< allshapes >::value Sorted;
      static_assert( std::is_same< Sorted, std::tuple< Triangle, Circle, AppleRectangle, Rectangle, Poligon, Shape > > :: value, "" );
    
      typedef typename Sort< Circle, Triangle, Shape, Poligon, Rectangle, AppleRectangle >::value Sorted2;
      static_assert( std::is_same< Sorted2, std::tuple< AppleRectangle, Rectangle, Triangle, Poligon, Circle, Shape > > :: value, "" );
}
