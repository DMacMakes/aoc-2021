#pragma once
#include "include/Point2d.h"
namespace textpixels
{
  //////// Constructors

  Point2d::Point2d()
  {}

  /// Constructor with x, y arguments
  Point2d::Point2d(int x, int y)
  {
    this->x = x;
    this->y = y;
  }

  /// Copy constructor takes another Point2D as argument
  Point2d::Point2d(Point2d const& other)
  {
    x = other.x;
    y = other.y;
  }

  /// Operator overloads

  Point2d operator + (Point2d const& p1, Point2d const& p2)
  {
    return Point2d(p1.x + p2.x, p1.y + p2.y);
  }

  bool operator == (Point2d const& p1, Point2d const& p2)
  {
    return (p1.x == p2.x && p1.y == p2.y);
  }

  // Returns true if the x value of lhs is less than that of
  // rhs until I think of a better definition. Needed to use
  // set (which is inherently ordered) as I don't know what
  // to provide for a hash in unordered set 🤷‍♂️
  bool operator < (Point2d const& lhs, Point2d const& rhs)
  {
    return (lhs.x < rhs.x);
  }

  /// Copy assignment operator: point1 = point2; will copy point2 values to point1.
  Point2d& Point2d::operator =(Point2d const& other)
  {
    // check for self-assignment
    if (&other == this)
      return *this;
    x = other.x;
    y = other.y;
    return *this;
  }

  ///////// Regular methods 

  /// Copy another Point2D's x and y
  void Point2d::copy(Point2d const& other)
  {
    x = other.x;
    y = other.y;
  }


  bool Point2d::isEqualTo(Point2d const& other)
  {
    return(x == other.x && y == other.y);
  }
}