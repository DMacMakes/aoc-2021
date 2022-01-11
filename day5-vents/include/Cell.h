#pragma once
#include "Point2d.h"
using textpixels::Point2d;

class Cell
{
public:
  mutable textpixels::Point2d location{};
  mutable int vent_count{ 0 };    // Needed because std::set returns a pointer to a const
                                  // version of the cell :\
  
  //vector<Vent&> vents;
  Cell(Point2d location) : location{ location } {}  // constructor

  // Add == support so Cell can be used in set/unordered_set
  friend bool operator==(const Cell& lhs, const Cell& rhs)
  {
    return lhs.location == rhs.location;
  }

  // < operator needed to use in ordered sets (so it can sort things). Easier option than 
  // figuring out what a hash is in an unordered set. The reason I need
  // to provide help in not ordering a set still eludes me.
  friend bool operator<(const Cell& lhs, const Cell& rhs)
  {
    return lhs.location < rhs.location;
  }
};

