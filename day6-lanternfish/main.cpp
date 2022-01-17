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

const uint8_t ADULT_SPAWN_CYCLE = 6;
const uint8_t JUVENILE_SPAWN_CYCLE = 8;
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
  // work with the reference, remember to use ref to call this so the reference actually works.
  for (uint64_t j = 0; j <= upper; j++)
    {
      //int& fishy = fishes[j];
      if ( fishes[j] > 0 )
      {
        fishes[j] -= 1;
      } else
      {
        fishes[j] = ADULT_SPAWN_CYCLE;
        //fishes.push_back(JUVENILE_SPAWN_CYCLE);
        // update atomic new fish counter
        fishes_spawned += 1;
      }
    }
    fishes_spawned_today += fishes_spawned;
}

int func() { return 1; }

uint64_t simulateFishPortion(vector<uint8_t> fishes, int days_to_sim)
{
  // simulaaate
  //delay(50);
  int days_left = days_to_sim;
  fishes.reserve(1'000'000'000); // reserve room for 1 billion fish
  for(; days_left > 0; days_left--)
  {
    uint64_t fishes_count = fishes.size();
    uint64_t fishes_spawned_today = 0;
    // work with the reference, remember to use ref to call this so the reference actually works.
    for (uint64_t i = 0; i < fishes_count; i++)
    {
      if ( fishes[i] > 0 )
      {
        fishes[i] -= 1;
      } else
      {
        fishes[i] = ADULT_SPAWN_CYCLE;
        //fishes.push_back(JUVENILE_SPAWN_CYCLE);
        // update atomic new fish counter
        fishes_spawned_today += 1;
      }
    }
    if (fishes_spawned_today > 0) 
      // Add newly spawned fish to the collection as juveniles (longer spawn cycle)
      fishes.resize(fishes_count + fishes_spawned_today, JUVENILE_SPAWN_CYCLE);
  }
    
    // do all the fish spawn day iterating
  // if any have spawned, add the spawned dudes to this vector.
  // finally, return a vector (i guess each will need one named)
  // Actually, only need to return the size of the vector at end of x days.
  // this would be a good time to use a future: 
  // https://stackoverflow.com/questions/7686939/c-simple-return-value-from-stdthread
  return static_cast<uint64_t>(fishes.size()); // final number of fishies goes here
}

uint64_t simulateFishes(vector<uint8_t>& starting_fishes, const int days_to_sim)
{
  dmac::Timer timer;
  
  const uint64_t starting_fish_count = starting_fishes.size();
  const int num_threads=8;
  timer.start();
  vector<future<uint64_t>> futures (num_threads);
  for (int f = 0; f < num_threads; f++)
  {
    //std::future<uint64_t> ret = std::async(&func);
    int start = (starting_fish_count / num_threads) * f;
    int end = (starting_fish_count / num_threads) * (f+1);
    if (f == num_threads-1) end = starting_fish_count;
    print("creating portion with indices {0} to {1}\n", start, end);
    vector<uint8_t> portion (starting_fishes.begin()+start,
                            starting_fishes.begin()+end);
    futures[f] = std::async(
        simulateFishPortion, portion, days_to_sim);
  }
  vector<uint64_t> sim_results(num_threads,0);
  uint64_t total_fish_after_sim {0};
  for( int f = 0; f < num_threads; f++ )
  {
    sim_results[f] = futures[f].get();
    total_fish_after_sim += sim_results[f];
  }
  print(text_styles[Styles::LOUD_FRIENDLY], "\n\nTotal fish after {0} days: {1}.\n\n",
      days_to_sim, total_fish_after_sim);
  //int i = ret.get();
  //int days_left = days_to_sim;
  // each day
  // subtract 1 from all fish unless they're on 0,
  // in chich case loop to 6.
  // if you had to loop, spawn another fish into the collection with
  // 8 days remaining.
  /*print("Simulating...");
  int num_threads = 4;
  // create num_threads collections containing their portion of the starting_fishes.
  // Pass those containers to the threads, so noone's sharing.
  const uint64_t starting_fish_count = starting_fishes.size();
  vector<thread> threads {};
  for(int i =0; i < num_threads; i++)
  {
    // create a container with 1/num_threads of the starting fishes
    // pass it to a thread to simulate. 
    int start = (starting_fish_count / 4) * i;
    int end = (starting_fish_count / 4) * i+1;
    if (i == num_threads-1) end = starting_fish_count;
    vector<uint8_t> portion (starting_fishes.begin()+start,
                            starting_fishes.begin()+end);
    threads.push_back(thread(simulateFishPortion, portion, days_to_sim));
  }

    for ( auto& th : threads )
    {
      if(th.joinable())
        th.join();
    }
    */
  /*
  for(; days_left > 0; days_left--)
  {
    vector<thread> threads {};
    // Using manual iterator because ranged loop was missing entries when I added
    // to vector. Also grabbing the fish count before growing the vector in case of hijinx.
    
    
    const uint64_t fish_count = fishes.size();
    fishes_spawned_today = 0;
    // Divide collection into 4 parts so it can be sent to four threads.
    // Will a reference work? will this be too much memory usage/copying?
    // Create some threads
    // Ask each to process fish_count / num_threads fish, have them record how many
    // fish must be added to the vector. When they are done, they can lock the 
    // "fish_spawned_today" variable and add to it.
    for(int i = 0; i<num_threads; i++)
    {
      uint64_t start = (fish_count / num_threads) * i;
      uint64_t end = (fish_count / num_threads) * (i+1);
      if (i == num_threads-1) end = fish_count - 1;
      threads.push_back(thread(simulateDay, start, end));
    }
    for ( auto& th : threads )
    {
      if(th.joinable())
        th.join();
    }*/
    timer.stop();
    //When all threads have joined,
    // use fishes.resize(fish_spawned_today, JUVENILE_SPAWN_CYCLE) to add all the
    // extras at once.. or use a for loop if that reallocates too much.
    /*if (fishes_spawned_today > 0) 
      fishes.resize(fish_count + fishes_spawned_today, JUVENILE_SPAWN_CYCLE);
    // printFishes(fishes, "After " + to_string(days_left - days_to_sim +1) + " days: ");
    print("Day {0}, {1} fish.\n", days_to_sim - days_left + 1, fishes.size()); 
  }*/
  print("Time to sim: {0} seconds.", timer.timeTakenSeconds());
  return(total_fish_after_sim);
}


/*void simulateFishesAllOneContainer(const int days_to_sim)
{
  
  int days_left = days_to_sim;
  dmac::Timer timer;
  // each day
  // subtract 1 from all fish unless they're on 0,
  // in chich case loop to 6.
  // if you had to loop, spawn another fish into the collection with
  // 8 days remaining.
  print("Simulating...");
  timer.start();
  int num_threads = 4;
  for(; days_left > 0; days_left--)
  {
    vector<thread> threads {};
    // Using manual iterator because ranged loop was missing entries when I added
    // to vector. Also grabbing the fish count before growing the vector in case of hijinx.
    
    
    const uint64_t fish_count = fishes.size();
    fishes_spawned_today = 0;
    // Divide collection into 4 parts so it can be sent to four threads.
    // Will a reference work? will this be too much memory usage/copying?
    // Create some threads
    // Ask each to process fish_count / num_threads fish, have them record how many
    // fish must be added to the vector. When they are done, they can lock the 
    // "fish_spawned_today" variable and add to it.
    for(int i = 0; i<num_threads; i++)
    {
      uint64_t start = (fish_count / num_threads) * i;
      uint64_t end = (fish_count / num_threads) * (i+1);
      if (i == num_threads-1) end = fish_count - 1;
      threads.push_back(thread(simulateDay, start, end));
    }
    for ( auto& th : threads )
    {
      if(th.joinable())
        th.join();
    }
    timer.stop();
    //When all threads have joined,
    // use fishes.resize(fish_spawned_today, JUVENILE_SPAWN_CYCLE) to add all the
    // extras at once.. or use a for loop if that reallocates too much.
    if (fishes_spawned_today > 0) 
      fishes.resize(fish_count + fishes_spawned_today, JUVENILE_SPAWN_CYCLE);
    // printFishes(fishes, "After " + to_string(days_left - days_to_sim +1) + " days: ");
    print("Day {0}, {1} fish.\n", days_to_sim - days_left + 1, fishes.size());
  }
  print("Time to sim: {0} seconds.", timer.timeTakenSeconds());
}
*/

int main() {
  //clearScreen();
  createStyles();
  unsigned int num_threads_supported = std::thread::hardware_concurrency();
  //std::cout << n << " concurrent threads are supported.\n";
  //greetInColour();
  const int DAYS_TO_SIM = 200; //80; //18;
  
  //vector<uint8_t> fishes {3,4,3,1,2};
  // TODO: turn fish into much smaller ints (8 bit) to get optimsation.
  //vector<uint8_t> fishes;
  vector<uint8_t> starting_fishes = getFishFromFile(FISH_FILE);
  print("Got {0} lantern fish.\n", starting_fishes.size());  

  //printFishes(fishes, "Initial state: ");
  // FEARRR!: Will it become too much to store? Is this an exercise in allocation? Will it crash?
  
  // TODO: Solve the problem of the great growth of the vector leading to
  // slow-down and memory usage - already serious at 160 days - that will
  // make it hard to hit 256.   
  uint64_t fishes_at_end = simulateFishes(starting_fishes, DAYS_TO_SIM);
  std::string answer {};
  scn::prompt("Are we good?", "{0}", answer);

  // Might need threads to make this perform beyond day 160,
  // though you'd need locks during adds.j
  // No more threads than cpu cores either.
  //print("\nAfter {0} days we have ", DAYS_TO_SIM);
  //print(text_styles[Styles::LOUD_FRIENDLY], "{0} fish.\n", fishes.size());
}
