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
  //print("\nline:\n{0}\n", line);
  if(!line.empty())
  {
    //print("\nGot a line from file {0} chars long.\n", line.size());
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

void printFishes(vector<int>& fishes, string message)
{
  print(message);
  for( int fishy : fishes )
  {
    print("{0},", fishy);
  }
  // erase last comma, add newline.
  print("\b\n");
}

void simulateFishes(vector<int>& fishes, int days_to_sim)
{
  const int ADULT_SPAWN_CYCLE = 6;
  const int JUVENILE_SPAWN_CYCLE = 8;
  int days_left = days_to_sim;
  // each day
  // subtract 1 from all fish unless they're on 0,
  // in chich case loop to 6.
  // if you had to loop, spawn another fish into the collection with
  // 8 days remaining.
  print("Simulating...");
  for(; days_to_sim > 0; days_to_sim--)
  {
    // will this feed us fish for ever when we use for - in?
    // No, iterator isn't updated after loop begins.
    // for ( auto& fishy : fishes)
    // using auto, the addition of a new fish led to skipping of one entry,
    // so switching to a manual iterator. Also grabbing the fish count
    // before growing the vector in case of hijinx.
    print("Day {0}.\n", days_left - days_to_sim + 1);
    int fish_count = fishes.size();
    for (int j = 0; j < fish_count; j++)
    {
      //int& fishy = fishes[j];
      if ( fishes[j] > 0 )
      {
        fishes[j] -= 1;
      } else
      {
        fishes[j] = ADULT_SPAWN_CYCLE;
        fishes.push_back(JUVENILE_SPAWN_CYCLE);
      }
    }
    // printFishes(fishes, "After " + to_string(days_left - days_to_sim +1) + " days: ");
  }
}

int main() {
  //clearScreen();
  createStyles();
  //greetInColour();
  const int DAYS_TO_SIM = 160; //80; //18;
  
  //vector<int> fishes {3,4,3,1,2};
  auto fishes = getFishFromFile(FISH_FILE);
  print("Got {0} lantern fish.\n", fishes.size());  

   
  //printFishes(fishes, "Initial state: ");
  // TODO: Do a test using the example data first to make sure it works.
  // TODO: Simulate the spawning of lanternfish, starting with these 80, for 300 days. 
  // FEARRR!: Will it become too much to store? Is this an exercise in allocation? Will it crash?
  simulateFishes(fishes, DAYS_TO_SIM);

  // Might need threads to make this perform beyond day 160,
  // though you'd need locks during adds.
  print("\nAfter {0} days we have ", DAYS_TO_SIM);
  print(text_styles[Styles::LOUD_FRIENDLY], "{0} fish.\n", fishes.size());
}
