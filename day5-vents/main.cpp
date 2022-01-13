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
#include <array>
#include <fstream>
#include <set>
#include <cmath>
#include "include/ise102.h"
#include "include/Point2d.h"
#include "include/Cell.h"
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

const string VENTS_FILE = "vent-line-locations.txt";
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
  Point2d loc_begin{};
  Point2d loc_end{};
  bool is_horiz = false;
  bool is_vert = false;
};

// A rectangle defined by two points on an x,y plane, the top left
// and bottom right extents.
struct RectExtents
{
  Point2d top_left{};
  Point2d bot_right{};
};




// If we drew a bounding box around all the vents on the ocean
// floor, what would the location of the top left and bottom right
// corners be? What are the min and max values for x and y?
RectExtents getVentGridExtents(vector<Vent>& vents)
{
  // Start out with the x,y coordinates of the first vent, 
  // work out from there.
  static RectExtents extents{ vents[0].loc_begin, vents[0].loc_end };

  for (auto& vent : vents)
  {
    // if either end of the vent is further up or left than the current top
    // left extent, make that the new top or left extent.
    if (extents.top_left.x > vent.loc_begin.x) extents.top_left.x = vent.loc_begin.x;
    if (extents.top_left.x > vent.loc_end.x) extents.top_left.x = vent.loc_end.x;
    if (extents.top_left.y > vent.loc_begin.y) extents.top_left.y = vent.loc_begin.y;
    if (extents.top_left.y > vent.loc_end.y) extents.top_left.y = vent.loc_end.y;

    // If either end of vent is further down or right than the current bottom
    // right extent, make that the new bottom or right extent. (get lowest x or y)
    if (extents.bot_right.x < vent.loc_begin.x) extents.bot_right.x = vent.loc_begin.x;
    if (extents.bot_right.x < vent.loc_end.x) extents.bot_right.x = vent.loc_end.x;
    if (extents.bot_right.y < vent.loc_begin.y) extents.bot_right.y = vent.loc_begin.y;
    if (extents.bot_right.y < vent.loc_end.y) extents.bot_right.y = vent.loc_end.y;
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
                vent.loc_begin.x, vent.loc_begin.y, vent.loc_end.x, vent.loc_end.y);
    // scan in the points on either side of the arrow, 
    //print(text_styles[RED_ALERT], "Got a line:", line);
    //print("start: x={0} y={1}", vent.loc_begin.x, vent.loc_begin.y);
    //print("\tend: x={0} y={1} \n", vent.loc_end.x, vent.loc_end.y);
    vents.push_back(vent);
  } while (!result.empty()); // line != "");
  vents.shrink_to_fit();
  // Clean up memory/file handles
  if(vents_file.is_open()) vents_file.close();

  return vents;
}

// Checks if a vent has the same beginning and ending y values,
// making it a vertical line in our grid
bool isVertical(Vent& vent)
{
  return (vent.loc_begin.x == vent.loc_end.x);
}

// Checks if the vent has the same beginning and ending x values,
// making it a horizontal line in our grid
bool isHorizontal(Vent& vent)
{
  return (vent.loc_begin.y == vent.loc_end.y);
}

vector<Vent> getHorizontalAndVerticalVents(vector<Vent>& all_vents)
{
  // if begin.x == end.x || begin.y ==
  vector<Vent> hv_vents{};
  hv_vents.reserve(all_vents.size());  // hold up to all vents
  
  for (Vent& vent : all_vents)
  {
      vent.is_vert = isVertical(vent);
      vent.is_horiz = isHorizontal(vent);
      if (vent.is_vert || vent.is_horiz)
      {
        hv_vents.push_back(vent);
      }
  }
  hv_vents.shrink_to_fit();
  return hv_vents;
}

// A single vent is a line that crosses multiple cells in a grid drawn
// on the floor of the ocean. The input data provides a beginning point
// and an end. This function creates a collection of points covering
// the entire length of a provided vent (currently only horizontal and
// vertical vents).
vector<Point2d> getAllVentLocations(Vent& vent)
{
  int min = 0;
  int max = 0;
  vector<Point2d> all_locations{};
  if (isHorizontal(vent)) // (vent.is_horiz)
  {
    // No guarantee vents beginning is before end. 
    min = std::min(vent.loc_begin.x, vent.loc_end.x);
    max = std::max(vent.loc_begin.x, vent.loc_end.x);
    // add a point for each missing value of x
    for (int i = min; i <= max; i += 1)
    {
      all_locations.push_back({ i, vent.loc_begin.y });
    }

  }
  else if (isVertical(vent)) //(vent.is_vert)
  {
    min = std::min(vent.loc_begin.y, vent.loc_end.y);
    max = std::max(vent.loc_begin.y, vent.loc_end.y);
    // add a point for each missing value of x
    for (int i = min; i <= max; i += 1)
    {
      all_locations.push_back({ vent.loc_begin.x, i });
    }
  }
  else
  {
    // vent is neither vertical or horizontal. According to the
    // rules, it must be a 45 degree line. Gotta find min and max x and y
    // values then and create our points between by incrementing.
    // x can go up while y goes down or up, so it'll take a little
    // thinkin'.

    // Get starting and ending x values. The difference will be the 
    // same as that of the y, because diagonal. Need a library that can
    // create a range of values from a to be regardless of which is larger,
    // counting up or counting down.
    Point2d start, end;
    if (vent.loc_begin.x < vent.loc_end.x)
    {
      start = vent.loc_begin;
      end = vent.loc_end;
    }
    else
    {
      start = vent.loc_end;
      end = vent.loc_begin;
    }
    int difference = end.x - start.x;
    auto points = std::vector<Point2d>(difference+1); 
    // map out all the 45 degree line points between start and end.
    for (int i = 0; i <= difference; i++)
    {
      points[i].x = start.x + i;
      // y may go from high to low, low to high, so account for them.
      if (start.y < end.y)
      {
        points[i].y = start.y + i;
      }
      else
      {
        points[i].y = start.y - i;
      }
      all_locations.push_back(points[i]);
    }
  }
  return all_locations;
}

vector<Cell> mapVentsToOceanFloor(vector<Vent>& vents)
{
    // TODO Fix this so we're checking every location you would
    // have between loc_begin and loc_end. For these horizontal
    // and vertical ones we can probably use something like std::iota
    // to create the iteration?
  print("Mapping {0} vents to ocean floor..\n\n", vents.size());
  vector<Cell> floor{};
  //int multi_vent_cell_count = 0;
  for (auto& vent : vents)
  {
    vector<Point2d> vent_locations = getAllVentLocations(vent);

    for (auto& location : vent_locations)
    {
      vector<Cell>::iterator it;
      Cell temp_cell = Cell(location);
      it = std::find(floor.begin(), floor.end(), temp_cell);// floor.find(temp_cell);
      if (floor.size() == 0 || it == floor.end())
      {
        floor.push_back(std::move(temp_cell));
      }
      else  // A cell on the ocean floor was already created for this location.
      {
        //print(text_styles[Styles::RED_ALERT], "Overlap!");
        //print(" Floor already contains cell at {0},{1}.\n",
        //  it->location.x, it->location.y);
        it->vent_count += 1;
        //multi_vent_cell_count += 1;
      }
    }
  }
  //print("\n{0} cells on the floor are split by more than one vent.\n\n", multi_vent_cell_count);

  return floor;
}

void mapVents2(vector<Vent>& vents, vector<vector<int>>& floor)
{
  print("Mapping {0} vents to ocean floor..\n\n", vents.size());
  for (auto& vent : vents)
  {
    vector<Point2d> vent_locations = getAllVentLocations(vent);
    for (auto& location : vent_locations)
    {
      floor[location.x][location.y] += 1;
    }
  }
}
// Read vent start-end points in from file
// Collect any vents that are straight horizontal or vertical.
// Decide dimensions of grid based on 
// Go through cells and test which of these collected vents 
// cross the 
// Mark them in a grid of points that have 
void checkForOverlappingVents()
{
  vector<Vent> all_vents = getVentsFromFile(VENTS_FILE);
  print("\n\n");
  print(text_styles[Styles::LOUD_FRIENDLY], "Found {0} vents.", all_vents.size());
  print("\n\n");
  auto hv_vents = getHorizontalAndVerticalVents(all_vents);
  print(text_styles[Styles::STONE], "Found {0} horizontal or vertical vents", hv_vents.size());
  RectExtents gridExtents = getVentGridExtents(hv_vents);
  // TODO: Print out or look at gridextents with debugger.
  print("\n\n");
  print("Extents: {0},{1} -> {2},{3}\n\n", gridExtents.top_left.x, gridExtents.top_left.y,
    gridExtents.bot_right.x, gridExtents.bot_right.y);

  // go through all the cells (1000x1000=1`000`000 cells. Seems bad.
  // go through every vent, add 1 to an entry in array.
  // Read file into all_vents;
  // Do linear algebra on all the lines seeing which overlap at what location.
  // for each vent I could go through all other vents to see which overlap it?  
  //array<array<int, 100>, 100> ;

  // vector grid 1000 x 1000 (c++ doesn't want 1000x1000 array on stack)
  auto floor_grid = vector<vector<int>> (1024, vector<int>(1024,0));
  //array<array<int, 1000>, 1000> ocean_floor_grid{};
  print("\nMade ocean floor bro\n");
  //mapVents2(hv_vents, floor_grid);
  mapVents2(all_vents, floor_grid);

  print("\nDone.\n");

  int overlaps = 0;
  for (int i = 0; i < 1024; i++)
  {
    for (int j = 0; j < 1024; j++)
    {
      if (floor_grid[i][j] > 1) overlaps += 1;
    }
  }
  print("Overlaps: {0}\n", overlaps);
  // Alternative:
  // Go through all the vents, adding cells to a map via a custom compare lambda
  // that checks if begin and end points are equal. Point2D already has a == operator.
  /* vector<Cell> ocean_floor{ mapVentsToOceanFloor(hv_vents) };
  int multi_vent_cell_count = 0;
  for (auto& cell : ocean_floor)
  {
    if (cell.vent_count > 1)
    {
      multi_vent_cell_count += 1;
      print(text_styles[Styles::RED_ALERT], "{0}", cell.vent_count);
      //print(" vents cross the cell at {0},{1}\n",
      //  cell.location.x, cell.location.y);
    }
  }
  print("\n\nFound {0} multi-vent cells on ocean floor, out of a total {1} cells.\n\n", multi_vent_cell_count, ocean_floor.size()); */
}

int main() {
  //clearScreen();
  createStyles();
  //greetInColour();
  
  checkForOverlappingVents();
}
