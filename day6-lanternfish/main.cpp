/********************************************************************
* Lanternfish - Advent of Code day 6.
*********************************************************************
*
* Lanternfish spawn all the time.
* 
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

const string FISH_FILE = "lanternfish.txt";
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

vector<int> getFishFromFile(string file_path)
{
  // Grab the file handle in a way scn can keep it alive.
  scn::owning_file fish_file(file_path.c_str(), "r");
  vector<int> fish;
  //vents.reserve(1000);
  std::string line{};
  auto result = scn::make_result(fish_file);

  result = scn::getline(result.range(), line, '\n');
  print("\nline:\n{0}\n", line);
  if(!line.empty())
  {
    print("\nGot a line from file {0} chars long.\n", line.size());
    //size_t pos = 0;
    for(const char& c : line)
    {
      if (c != ',') fish.push_back(c-'0');
    }
    // grab an int, a comma, an int, a comma over and over.
    // maybe getch is better than getline.
    // use string get next non comma?
  } 
  //if(vents_file.is_open()) vents_file.close();

  return fish;
}


int main() {
  //clearScreen();
  createStyles();
  //greetInColour();
  auto fishes = getFishFromFile(FISH_FILE);
  print("Got {0} lantern fish.\n", fishes.size());  
  for( int fishy : fishes )
  {
    print("{0},", fishy);
  }
}
