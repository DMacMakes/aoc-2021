// day4_squid_bingo_aoc21.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// You're already almost 1.5km (almost a mile) below the surface of the ocean, already so deep that you can't see any sunlight. What you can see, however, is a giant squid that has attached itself to the outside of your submarine.

// Maybe it wants to play bingo ?
// Bingo is played on a set of boards each consisting of a 5x5 grid of numbers.Numbers are chosen at random, and the chosen number is marked on all boards on which it appears. (Numbers may not appear on all boards.) If all numbers in any row or any column of a board are marked, that board wins. (Diagonals don't count.)
// The submarine has a bingo subsystem to help passengers(currently, you and the giant squid) pass the time.It automatically generates a random order in which to draw numbers and a random set of boards(your puzzle input).For example :

// 7,4,9,5,11,17,23,2,0,14,21,24,10,16,13,6,15,25,12,22,18,20,8,19,3,26,1
// 
// 22 13 17 11  0
// 8  2 23  4 24
// 21  9 14 16  7
// 6 10  3 18  5
// 1 12 20 15 19
// 
// 3 15  0  2 22
// 9 18 13 17  5
// 19  8  7 25 23
// 20 11 10 24  4
// 14 21 16 12  6

#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <cassert>
using std::cout, std::string, std::vector, std::getline;
using std::array;
// bingo card data type would just hold an array of arrays (5 digits)
//typedef 

struct BingoCardCell {
  int number = 0;
  bool dabbed = false;
};

// convert the bastard over to card cells
using BingoCard = array<array<BingoCardCell, 5>, 5>;
using bingocard = array<array<int, 5>, 5>; //BingoCard;
const int CARD_ROW_COUNT = 5;
const int CARD_COL_COUNT = 5;

/*vector<int> splitString(const string& raw_data, const string& divider)
{
  assert(divider != "");
  splitString(raw_data, divider[0]);
}*/

// Use string::find to locate commas, then substr to pull out those characters up to the comma
// and convert to integers. Store in a vector and return.
vector<int> splitStringOfNumbers(const string& raw_data, const char& divider)
{
  vector<int> numbers_found{};
  numbers_found.reserve(100); // 0-99 could be drawn
  
  size_t begin_find = 0;
  size_t split_point = 0;
  size_t begin_cut = 0;
  size_t end_cut = 0;
  //int num_end = 0;
  do 
  {
    // find the first thing after end cut that isn't the divider. That's begin_cut.
    // end cut either starts at 0 or represents the end of the last cut.
    begin_cut = raw_data.find_first_not_of(divider, end_cut); // skip any duplicate dividers
    if (begin_cut == std::string::npos) break; // didn't find any non-divider thing in vector.
    // find the next divider, if there is one.
    end_cut = raw_data.find(divider, begin_cut);

    //split_point = raw_data.find(divider, begin_find);
    //if (split_point != std::string::npos)
    if (end_cut != std::string::npos)  // a divider was found beyond the begin_cut.
    {
      // Grab a the digits leading from the start of the range to just before the comma
      // store as an integer in collection.
      numbers_found.push_back(std::stoi(raw_data.substr(begin_cut, end_cut)));
      // Start the next find beyond the split point
      //begin_find = split_point + 1;
      //cout << "found.. split point = " << split_point;
    }
    else // no commas found
    {
      // If the raw data didn't end with a comma we need to grab those as final drawn number. 
      if (begin_cut < raw_data.size()) // there are non-divider digits left.
        numbers_found.push_back(std::stoi(raw_data.substr(begin_cut)));
      break;
    }
  } while (begin_cut < raw_data.size());
  
  if (numbers_found.empty()) cout << "No numbers found";
  numbers_found.shrink_to_fit();
  return numbers_found;
}


vector<bingocard> extractCards(std::ifstream& bingo_input)
{
  vector<bingocard> cards;
  string line;
  int row = 0;
  bingocard card_temp;
  while (bingo_input.good())
  {
    getline(bingo_input, line);
    if (line != "")
    {
      // The file will contain 5 rows of 5 space separated numbers
      // after an empty line
      auto card_row = splitStringOfNumbers(line, ' ');

      // split returns an array. Copy the result to our card. 
      std::copy_n(card_row.begin(), CARD_COL_COUNT, card_temp[row].begin());
      row += 1;
      if (row == CARD_ROW_COUNT)
      {
        cards.push_back(card_temp);
        row = 0;
      }
    }
  }
  cout << "\n";
  return cards;
}


int main()
{
  std::ifstream bingo_input;
  bingo_input.open("bingo_input.txt");
  // get bingo numbers
  string draw_raw = "";
  getline(bingo_input, draw_raw);
  
  //cout << "Got drawn numbers (raw) : \n" << draw_raw << "\n\n";
  // The first line in the file contains numbers drawn in a game of bingo,
  // comma separated. Call a function to get back an array of them as int.
  vector<int> draw_numbers = splitStringOfNumbers(draw_raw, ',');
  cout << "Extracted " << draw_numbers.size() << " bingo draw numbers.\n\n";
  
  // get bingo cards
  vector<bingocard> cards = extractCards(bingo_input);
  cout << "Extracted " << cards.size() << " 5x5 bingo cards.\n\n";
  bingo_input.close();

  // with each draw number
  //   check all cards for number
  //     if a card has the number
  //       mark found for the number in a collection of card marks (5x5 collections of bools)
  //       (OR the cards themselves should have a number and a true/false found value. so instead of
  //       array off array of ints it should be array of array of tuple (int, bool). maybe a struct cell(number, dabbed)

}

