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
  Cell(Point2d location)   // construct with a location of a point (from vent data)
    : location{ location }, vent_count{ 1 } {}  // so vent_count = 1

  // Add == support so Cell can be used in set/unordered_set
  friend bool operator==(Cell const& lhs, Cell const& rhs)
  {
    return lhs.location == rhs.location;
  }

  // < operator needed to use in ordered sets (so it can sort things). Easier option than 
  // figuring out what a hash is in an unordered set. The reason I need
  // to provide help in not ordering a set still eludes me.
  friend bool operator<(Cell const& lhs, Cell const& rhs)
  {
    return lhs.location < rhs.location;
  }
};

