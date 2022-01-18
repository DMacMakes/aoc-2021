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
#include <thread>
#include <future>
#include <atomic>
#include "include/ise102.h"
#include "include/Point2d.h"
#include "include/timer.h"
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

const size_t ADULT_SPAWN_CYCLE = 6;
const size_t JUVENILE_SPAWN_CYCLE = 8;
const size_t SPAWNING_BUFFER = 9;
const size_t SPAWNING = 0;
const string FISH_FILE = "lanternfish.txt";
const size_t FISH_N_BUFFER = 10;
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

vector<uint8_t> getFishFromFile(string file_path)
{
  // Grab the file handle in a way scn can keep it alive.
  scn::owning_file fish_file(file_path.c_str(), "r");
  vector<uint8_t> fish;
  long long int estimated_fish = 10'000'000'000;
  fish.reserve(estimated_fish);
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
      if (c != ',') fish.push_back(static_cast<uint8_t>(c-'0'));
    }
    // grab an int, a comma, an int, a comma over and over.
    // maybe getch is better than getline.
    // use string get next non comma?
  } 
  //if(vents_file.is_open()) vents_file.close();

  return fish;
}

void printFishes(vector<uint8_t>& fishes, string message)
{
  print(message);
  for( uint8_t fishy : fishes )
  {
    print("{0},", fishy);
  }
  // erase last comma, add newline.
  print("\b\n");
}

atomic_uint64_t fishes_spawned_today = 0;


void simulateDay(vector<uint8_t> fishes, const uint64_t lower, const uint64_t upper)
{
  int fishes_spawned = 0;
  for (uint64_t j = 0; j <= upper; j++)
    {
      //int& fishy = fishes[j];
      if ( fishes[j] > 0 )
      {
        fishes[j] -= 1;
      } else
      {
        fishes[j] = ADULT_SPAWN_CYCLE;
        fishes_spawned += 1;
      }
    }
    fishes_spawned_today += fishes_spawned;
}


uint64_t simulateFishThreaded(vector<uint8_t> fishes, int days_to_sim)
{
  int days_left = days_to_sim;

  uint64_t fishes_count = fishes.size();
  uint64_t fishes_spawned_today = 0;

  fishes.reserve(800'000'000);
  for (; days_left > 0; days_left--)
  {
    fishes_count = fishes.size();
    fishes_spawned_today = 0;
    
    for (uint64_t i = 0; i < fishes_count; i++)
    {
      if (fishes[i] > 0)
      {
        fishes[i] -= 1;
      }
      else
      {
        fishes[i] = ADULT_SPAWN_CYCLE;
        fishes_spawned_today += 1;
      }
    } 
    if (fishes_spawned_today > 0)
      // Add newly spawned fish to the collection as juveniles (longer spawn cycle)
      fishes.resize(fishes_count + fishes_spawned_today, JUVENILE_SPAWN_CYCLE);
  }
  return(static_cast<uint64_t>(fishes.size()));
}

uint64_t simulateFishPortion(vector<uint8_t> fishes, int days_to_sim)
{
  // simulaaate
  int days_left = days_to_sim;
  int num_threads = 8;
  // simulate days until there are enough fish to multi thread 
  uint64_t fishes_count = fishes.size();
  uint64_t fishes_spawned_today = 0;
  for (; fishes.size() < 8; days_left--)
  {
    fishes_count = fishes.size();
    fishes_spawned_today = 0;
    for (uint64_t i = 0; i < fishes_count; i++)
    {
      if ( fishes[i] > 0 )
      {
        fishes[i] -= 1;
      } else
      {
        fishes[i] = ADULT_SPAWN_CYCLE;
        fishes_spawned_today += 1;
      }
    }
    if (fishes_spawned_today > 0) 
      // Add newly spawned fish to the collection as juveniles (longer spawn cycle)
      fishes.resize(fishes_count + fishes_spawned_today, JUVENILE_SPAWN_CYCLE);
  }
  
  // With 4 or more total fish, pass them off to threads (via futures/async)
  // to be simulated.
  const uint64_t starting_fish_count = fishes.size();
  uint64_t total_fish{ 0 };

  vector<future<uint64_t>> futures (num_threads);
  for (int i = 0; i < num_threads; i++)
  {
    size_t start = (starting_fish_count / num_threads) * i;
    size_t end = (starting_fish_count / num_threads) * (i+1);
    if (i == num_threads-1) end = starting_fish_count;
    //print("Futuring portion with indices {0} to {1}..\n", start, end);
    vector<uint8_t> portion (fishes.begin()+start,
                            fishes.begin()+end);
    futures[i] = std::async(
        simulateFishThreaded, portion, days_left);
    //total_fish += simulateFishThreaded(portion, days_to_sim);
  }
  vector<uint64_t> sim_results(num_threads,0);
  for( int i = 0; i < num_threads; i++ )
  {
    sim_results[i] = futures[i].get();
    total_fish += sim_results[i];
  } 
  return total_fish; // final number of fishies goes here
}

uint64_t simulateFishes(vector<uint8_t>& starting_fishes, const int days_to_sim)
{
  dmac::Timer timer;
  
  const uint64_t starting_fish_count = starting_fishes.size();
  const int slices=300;
  uint64_t total_fish_after_sim {0};
  for (int i = 0; i < slices; i++)
  {
    timer.start();
    int start = (starting_fish_count / slices) * i;
    int end = (starting_fish_count / slices) * (i+1);
    if (i == slices-1) end = starting_fish_count;
    print("Simulating slice {0} to {1}..\n", start, end);
    vector<uint8_t> portion (starting_fishes.begin()+start,
                            starting_fishes.begin()+end);
    total_fish_after_sim += simulateFishPortion(portion, days_to_sim);
    print(text_styles[Styles::LOUD_FRIENDLY], "{0}", i+1);
    print(text_styles[Styles::LOUD_FRIENDLY], "/{0}ths of total fish : {1}.\n",
      slices, total_fish_after_sim);
    timer.stop();
    print("Slice took {0} seconds.\n", timer.timeTakenSeconds());
  }
  print(text_styles[Styles::LOUD_FRIENDLY], "\n\nTotal fish after {0} days: {1}.\n\n",
      days_to_sim, total_fish_after_sim);
  return(total_fish_after_sim);
}

void simulateFishIndividually(const int& days_to_sim)
{

  
  //vector<uint8_t> fishes {3,4,3,1,2};  // example data
  vector<uint8_t> starting_fishes = getFishFromFile(FISH_FILE);
  print("Got {0} lantern fish.\n", starting_fishes.size());  

  uint64_t fishes_at_end = simulateFishes(starting_fishes, days_to_sim);
}

vector<size_t> getFishCohortsFromFile(string file_path)
{
  // Grab the file handle in a way scn can keep it alive.
  scn::owning_file fish_file(file_path.c_str(), "r");
  vector<size_t> fishies(FISH_N_BUFFER, 0 ); // all begin at 0 fish
  std::string line{};
  auto result = scn::make_result(fish_file);
  result = scn::getline(result.range(), line, '\n');
  if(!line.empty())
  { // for each ascii character read in
    for(const char& ch : line)
    {
      if (ch != ',')
      {
        size_t cycle_day = static_cast<size_t>(ch) - '0';  // char to integer
        // increment fishies at that stage in breeding cycle
        fishies[cycle_day] += static_cast<size_t>(1);
      }
    }
  }
  print("Got fishies.\n");

  return fishies;
}


void printFishCohortSizes(vector<size_t> const& fishies)
{
  size_t total_fish{ 0 };
  print("\tFish with: \n");
  for (size_t i = 0; i <= SPAWNING_BUFFER; i++)
  {
    if (i == SPAWNING_BUFFER)
    {
      print("\tSpawning buffer:\t{0}\n", fishies[i]);
    }
    else 
    {
      print("\t{0} days left in spawn cycle:\t", i);
      print(text_styles[Styles::STONE], "{0}\n", fishies[i]);
      total_fish += fishies[i];
    }
  }
  print("Total fish across cohorts: {0}\n\n", total_fish);
}

// Jake suggested solution; avoid storing and simulating all of the
// fish. There are only 9 possible states (0-8), so we can track those 9 numbers, 
// each representing the number of fish that are on that day of their sport cycle.

// create an array of big ints with 10 entries - 1 per cycle day and a buffer slot
// for rotating the entries
// 
// read in fish from file
// for each number read in
//   add 1 to that index an an array of uint64_t
// for each day
//   move all the 0s to the buffer slot, they're going to be added to day 6 shortly
//   
//   for entries 1 - 8, move them left (so they're 0 -7)
//   add the number of fish in the buffer to day 6 - adults starting a new cycle
//   move the number of fish in the buffer to day 8, new babies
void simulateFishAsGroups(const int& days_to_sim)
{
  auto fishies = getFishCohortsFromFile(FISH_FILE);
  printFishCohortSizes(fishies);
  dmac::Timer timer;
  timer.start();
  
  for (size_t days_left = days_to_sim; days_left > 0; days_left--)
  {
    // move all the spawners (0 days left to spawn) to the buffer slot. 
    // They reset their cycle and their offspring will appear in day 8
    fishies[SPAWNING_BUFFER] = fishies[SPAWNING];
    // Move left. All the fish with 6 days left, for example, now have 5.
    for (size_t cohort = 1; cohort <= JUVENILE_SPAWN_CYCLE; cohort++)
    {
      //   for entries 1 - 8, move/copy them left (so they're 0 -7)
      fishies[cohort - 1] = fishies[cohort];
    }
    // add number of fish in the buffer to day 6: adding adults starting a new
    // cycle to the juveniles who've been alive for 2 days
    fishies[ADULT_SPAWN_CYCLE] += fishies[SPAWNING_BUFFER];
    // Spawn the new juveniles our adults made  
    fishies[JUVENILE_SPAWN_CYCLE] = fishies[SPAWNING_BUFFER];
  }
  
  timer.stop();
  printFishCohortSizes(fishies);
  print("Time to simulate: {0}ms\n", timer.timeTakenMilli());
}

int main() {
  createStyles();
  const int DAYS_TO_SIM = 256; //80; //18;  

  //simulateFishIndividually(DAYS_TO_SIM);

  simulateFishAsGroups(DAYS_TO_SIM);
}

