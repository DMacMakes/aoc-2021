/********************************************************************
* Vents - Advent of Code day 5.
*********************************************************************
* 
* Vents are linear on ocean floor, mapped as a grid. An input file 
* describes them as beginning and end points, e.g., 0,9 -> 5,9
* Meaning: Two points, x,y cords.
* For Part A, find the horizontal or vertical vents, map those on 
* a grid. Find which cells in the floor grid have more than one
* vent present (overlap). See example below. Some can be colinear,
* which at first makes no sense since they're the same vent, but
* then the resolution of the grid could be quite low compared to 
* vent widths. A 300x100 km stretch of ocean floor might be 3
* cells across, and two vents could be parallel with 10s of kms 
* separation:
* X X 1 X X
* 1 1 2 1 X
* X X 1 X X
* X X 2 2 2
* 
* New Approach:
* Try a fast and loose approach this time; part B and diagonal
* lines could be a thing.
*/

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <fstream>
#include "include/ise102.h"
#include "include/Point2d.h"
using namespace std;
using scn::scan;
using textpixels::Point2d;
using std::ifstream;

enum Styles
{
  LOUD_FRIENDLY,
  STONE,
  RED_ALERT
};

std::map<int, fmt::text_style> text_styles {};

// Create colour and emphasis styles that work with the fmt library
// (print(), fmt() etc) and place them in the text_styles map.
void createStyles()
{
  //-------------------------------------------------------- Create Speech Styles
  // Add a text style to the text_styles collection.
  // name it first in the Styles enum, and use that as the insert position.
  // the makeStyle function uses the fmt library to build one.
  text_styles.insert({
    Styles::LOUD_FRIENDLY,
    makeStyle(color::black, color::pale_golden_rod, emphasis::bold)
    });
  // I created a custom charcoal grey here with an rgb (red green blue) colour 
  // in hex. You drop the six digits into `color(0x______)` as i've done below.
  // Pick colours at https://www.w3schools.com/colors/colors_picker.asp
  text_styles.insert({
    Styles::STONE,
    makeStyle(color(0x222222), color::silver)
    });

  text_styles.insert({
    Styles::RED_ALERT,
    makeStyle(color::white, color(0xDD0033), emphasis::bold)
    });
}

// Demo/test a few text styles from the text_styles map, along with
// clearScreen and delay (which can require buffer flush in delay)
void greetInColour()
{
  clearScreen();
  //----------------------------------------------------- Encounters
  string town{ "Litsbane" };
  print("\tSUNFLOWER: ");
  print(text_styles[LOUD_FRIENDLY], " Welcome to {0}.!! \n\n", town);
  delay(2000);
  print("\tSTONE DOOR: ");
  print(text_styles[STONE], strToUpper(" Hello mushy one, do not be surprised! \n"));
  print("\n");
  print(text_styles[RED_ALERT], "Bye Harvey.");
  delay(1000);
}

// A vent has a beginning point and end point, each two dimensional (x,y)
struct Vent
{
  Point2d begin{};
  Point2d end{};
};

// A rectangle defined by two points on an x,y plane, the top left
// and bottom right extents.
struct RectExtents
{
  Point2d top_left{};
  Point2d bot_right{};
};

struct Cell
{
  textpixels::Point2d location{};
  //vector<Vent&> vents;
  int vent_count{ 0 };
};

//vector<vector<Cell>> ocean_floor;
// vents vertical or horizontal (one dimensional)
//vector<vector<Cell>> vents_vh; // maybe use cells here?

// If we drew a bounding box around all the vents on the ocean
// floor, what would the location of the top left and bottom right
// corners be? What are the min and max values for x and y?
RectExtents getVentGridExtents(vector<Vent>& vents)
{
  // Start out with the x,y coordinates of the first vent, 
  // work out from there.
  static RectExtents extents{ vents[0].begin, vents[0].begin };

  for (auto& vent : vents)
  {
    // if either end of the vent is further up or left than the current top
    // left extent, make that the new top or left extent.
    if (extents.top_left.x > vent.begin.x) extents.top_left.x = vent.begin.x;
    if (extents.top_left.x > vent.end.x) extents.top_left.x = vent.end.x;
    if (extents.top_left.y > vent.begin.y) extents.top_left.y = vent.begin.y;
    if (extents.top_left.y > vent.end.y) extents.top_left.y = vent.end.y;

    // If either end of vent is further down or right than the current bottom
    // right extent, make that the new bottom or right extent. (get lowest x or y)
    if (extents.bot_right.x < vent.begin.x) extents.bot_right.x = vent.begin.x;
    if (extents.bot_right.x < vent.end.x) extents.bot_right.x = vent.end.x;
    if (extents.bot_right.y < vent.begin.y) extents.bot_right.y = vent.begin.y;
    if (extents.bot_right.y < vent.end.y) extents.bot_right.y = vent.end.y;
  }

  return extents;
}

vector<Vent> getVentsFromFile(string file_path)
{
  // Grab the file handle in a way scn can keep it alive.
  scn::owning_file vents_file(file_path.c_str(), "r");
  vector<Vent> vents{};
  vents.reserve(1000);
  string line{};
  auto result = scn::make_result(vents_file);

  do
  {
    result = scn::getline(result.range(), line, '\n');
    string bin{};
    Vent vent{};
    //int a, b, c, d;
    scn::scan(line, "{0},{1} -> {2},{3}", 
                vent.begin.x, vent.begin.y, vent.end.x, vent.end.y);
    // scan in the points on either side of the arrow, 
    //print(text_styles[RED_ALERT], "Got a line:", line);
    print("start: x={0} y={1}", vent.begin.x, vent.begin.y);
    print("\tend: x={0} y={1} \n", vent.end.x, vent.end.y);
    vents.push_back(vent);
  } while (!result.empty()); // line != "");
  vents.shrink_to_fit();
  // Clean up memory/file handles
  if(vents_file.is_open()) vents_file.close();

  return vents;
}

vector<Vent> getHorizontalAndVerticalVents(vector<Vent>& all_vents)
{
  // if begin.x == end.x || begin.y ==
  vector<Vent> hv_vents{};
  hv_vents.reserve(all_vents.size());  // hold up to all vents
  
  for (Vent& vent : all_vents)
  {
    if (vent.begin.x == vent.end.x || vent.begin.y == vent.end.y) 
      hv_vents.push_back(vent);
  }
  
  return hv_vents;
  
}

// Read vent start-end points in from file
// Collect any vents that are straight horizontal or vertical.
// Decide dimensions of grid based on 
// Go through cells and test which of these collected vents 
// cross the 
// Mark them in a grid of points that have 
void checkForOverlappingVents()
{
  vector<Vent> all_vents = getVentsFromFile("vent-line-locations.txt");
  print("\n\n");
  print(text_styles[Styles::LOUD_FRIENDLY], "Found {0} vents.", all_vents.size());
  print("\n\n");
  auto hv_vents = getHorizontalAndVerticalVents(all_vents);
  print(text_styles[Styles::STONE], "Found {0} horizontal or vertical vents", hv_vents.size());
  RectExtents gridExtents = getVentGridExtents(hv_vents);
  // TODO: Print out our look at gridextents with debugger.
  print("\n\n");
  print("Extents: {0},{1} -> {2},{3}\n\n", gridExtents.top_left.x, gridExtents.top_left.y, 
                                            gridExtents.bot_right.x, gridExtents.bot_right.y);
  // Read file into all_vents;  
}

int main() {
  //clearScreen();
  createStyles();
  //greetInColour();
  
  checkForOverlappingVents();
}
