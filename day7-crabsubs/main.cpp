/********************************************************************
* Crabsubs - Advent of Code day 7.
*********************************************************************
*
* "You quickly make a list of the horizontal position of each crab 
* (your puzzle input). Crab submarines have limited fuel, so you need 
* to find a way to make all of their horizontal positions match while 
* requiring them to spend as little fuel as possible."
* 
* This problem's describing a geometric median (discovered with some
* googling of finding a point the shortest total travel from all other
* points in space, also googling median because it rang a bell
* from school), and when you're dealing with a single dimension that
* coincides with the median:
* https://en.wikipedia.org/wiki/Geometric_median
* https://en.wikipedia.org/wiki/Median
*/

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <array>
#include <fstream>
#include <cmath>
#include "include/ise102.h"
#include "include/timer.h"
using namespace std;
using scn::scan;
using std::ifstream;

enum Style
{
  LOUD_FRIENDLY,
  STONE,
  RED_ALERT
};

std::map<int, fmt::text_style> styles {};

// Create colour and emphasis styles that work with the fmt library
// (print(), fmt() etc) and place them in the text_styles map.
void createStyles()
{
  //-------------------------------------------------------- Create Speech Styles
  // Add a text style to the text_styles collection.
  // name it first in the Styles enum, and use that as the insert position.
  // the makeStyle function uses the fmt library to build one.
  styles.insert({
    Style::LOUD_FRIENDLY,
    makeStyle(color::black, color::pale_golden_rod, emphasis::bold)
    });
  // I created a custom charcoal grey here with an rgb (red green blue) colour 
  // in hex. You drop the six digits into `color(0x______)` as i've done below.
  // Pick colours at https://www.w3schools.com/colors/colors_picker.asp
  styles.insert({
    Style::STONE,
    makeStyle(color(0x222222), color::silver)
    });

  styles.insert({
    Style::RED_ALERT,
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
  print(styles[LOUD_FRIENDLY], " Welcome to {0}.!! \n\n", town);
  delay(2000);
  print("\tSTONE DOOR: ");
  print(styles[STONE], strToUpper(" Hello mushy one, do not be surprised! \n"));
  print("\n");
  print(styles[RED_ALERT], "Bye Harvey.");
  delay(1000);
}

vector<int> getCrabLocationsFromFile(string file_path)
{
  // Grab the file handle in a way scn can keep it alive.
  scn::owning_file file(file_path.c_str(), "r");
  vector<int> crabs{}; // all begin at 0 fish
  std::string line{};
  auto result = scn::make_result(file);
  result = scn::getline(result.range(), line, '\n');
  if (!line.empty())
  { // for each ascii character read in
    string crab_location = "";
    for (const char& ch : line)
    {
      if (ch != ',')
      {
        crab_location.push_back(ch);
        // = static_cast<size_t>(ch) - '0';  // char to integer
        // increment fishies at that stage in breeding cycle
      }
      else // hit a comma. process anything left in the string
      {
        if (!crab_location.empty())
        {
          int crab = atoi(crab_location.c_str());
          crabs.push_back(crab);
          crab_location.clear();
        }
      }
    }
    // if file didn't end in a comma
    if (!crab_location.empty())
    {
      int crab = atoi(crab_location.c_str());
      crabs.push_back(crab);
      crab_location.clear();
    }
  }
  print("Got {0} crabs from file.\n", crabs.size());
  return crabs;
}

int factorial(int n)
{
  int result = 0;
  if (n > 1)
    return n + factorial(n - 1);
  else
    return n;
  
}

int main() {
  createStyles();

  // load in all the crab locations along a given x axis
  auto crab_locations = getCrabLocationsFromFile("crab_locations.txt");
  // find their median (middle value of a sorted set)
  // sort the locations
  std::sort(crab_locations.begin(), crab_locations.end());
  double median{ 0.0 };
  // if locations.size() is odd
  //    median is middle value
  // else
  //   median is average of two middle values
  if (crab_locations.size() % 2 == 0)
  { // even number of crabs
    int middle_high = crab_locations.size() / 2;
    int middle_low = middle_high - 1;
    median = (crab_locations[middle_low] + crab_locations[middle_high]) / 2.0;
  }
  else
  { // odd number of crabs
    median = crab_locations[crab_locations.size() / 2];
  }
  print("The median is {0}\n", median);

  // What's the total fuel usage? This is the total distance travelled by
  // all other crabs to reach the median.
  uint64_t fuel_used{ 0ull };
  
  for (size_t i{ 0ull }; i < crab_locations.size(); i++)
  {
    fuel_used += std::abs(median - crab_locations[i]);
  }
  print("Fuel used to reach median: {0}\n", fuel_used);

  double mean{ 0.0 };

  int total_of_locations = 0;
  for (size_t i{ 0ull }; i < crab_locations.size(); i++)
  {
    total_of_locations += crab_locations[i];
  }
  mean = total_of_locations / crab_locations.size();

  uint64_t fuel_used_mean{ 0 };

  for (size_t i{ 0ull }; i < crab_locations.size(); i++)
  {
    int distance = static_cast<int>(round(std::abs(mean - crab_locations[i])));
    // each step travelled costs 1 more than the last, so
    // the cost of moving 4 is 1 + 2 + 3 + 4. That would be
    // the factorial of the number.
    fuel_used_mean += factorial(distance);
  }
  print("Fuel used to reach mean: {0}\n", fuel_used_mean);
}



