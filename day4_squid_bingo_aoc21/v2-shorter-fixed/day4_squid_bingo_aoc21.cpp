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


vector<BingoCard> extractCards(std::ifstream& bingo_input)
{
  vector<BingoCard> cards;
  string line;
  int row = 0;
  BingoCard card_temp;
  while (bingo_input.good())
  {
    getline(bingo_input, line);
    if (line != "")
    {
      // The file will contain 5 rows of 5 space separated numbers
      // after an empty line
      // TODO: change to row_numbers (because the real row has BingoCardCells
      auto row_numbers = splitStringOfNumbers(line, ' ');

      // split returns an array. Copy the result to our card. 
      //std::copy_n(card_row.begin(), CARD_COL_COUNT, card_temp[row].begin());
      for (int i = 0; i < CARD_COL_COUNT; i++)
      {
        card_temp[row][i].number = row_numbers[i];
      }
      // TODO: instead of copying the values over, we gotta update 
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
bool bingoCardWins(BingoCard& card)
{
  // for each row in card
  for (auto& row : card)
  {
    //  set win = true;
    bool win = true;
    //  for each cell in row
    for (auto& cell : row)
    {
      //    if cell.dabbed = false then win = false
      if (!cell.dabbed) {
        win = false;
        break;
      }
    }
    if (win) {
      //cout << "Win.. check it's valid";
      return true;
    }//  if win == true, (having been through all cells) return true (they won)
  }

  // for col = 0 to 4
  for (int col = 0; col < CARD_COL_COUNT; col++)
  {
    //   set win = true
    bool win = true;
    //   for row = 0 to 4
    for (int row = 0; row < CARD_ROW_COUNT; row++)
    {
      //     if cell at col,row isn't dabbed then win = false
      if (!card[col][row].dabbed)
      {
        win = false;
        break;
      }
    }
    if (win)
    {
      //cout << "Win.. check it's valid";
      return true;
    }
  }
  // awww, we found our way down here with no win
  return false;
}

int calculateBingoScore(const BingoCard& card, const int& winning_number)
{
            // score is sum of undabbed numbers on card * winning number drawn.
  int undabbed_sum = 0;
  for (const auto& row : card)
  {
    for (const auto& cell : row)
    {
      if (!cell.dabbed) undabbed_sum += cell.number;
    }
  }

  return undabbed_sum * winning_number;  // Satan wins again.
}

int playBingoToWin(const vector<int>& draw_numbers, vector<BingoCard>& cards)
{
  // with each draw number
  for (int number : draw_numbers)
  {
    cout << "Drawn: " << number ;
    //   check all cards for number
    for (auto& card : cards)
    {
      for (int row = 0; row < CARD_ROW_COUNT; row++)
      {
        for (int col = 0; col < CARD_COL_COUNT; col++)
        {
          auto& cell = card[row][col];
          //     if a card has the number
          if (!cell.dabbed && cell.number == number)
          {
            //       mark the cell as dabbed
            cell.dabbed = true;
            if (bingoCardWins(card))
            {
              cout << "----- WINNAH! ------\n\n";
              int score = calculateBingoScore(card, number);
              return score;
            }
            //cout << "X Not this card mate.\n";
            //       if the cells row and/or column has all cells dabbed
            //          bingo! Calculate the score, report it, exit.
            //
            // score is sum of undabbed numbers on card * winning number drawn.
          }
        }
      }
    }
    cout << "..\n";
  }
  return -1;  // there was no winning card.
}

int main()
{
  std::ifstream bingo_input;
  bingo_input.open("bingo_input.txt");
  // get bingo numbers
  string draw_raw = "";
  getline(bingo_input, draw_raw);
  
  // The first line in the file contains numbers drawn in a game of bingo,
  // comma separated. Call a function to get back an array of them as int.
  vector<int> draw_numbers = splitStringOfNumbers(draw_raw, ',');
  cout << "Extracted " << draw_numbers.size() << " bingo draw numbers.\n\n";
  
  vector<BingoCard> cards = extractCards(bingo_input);
  cout << "Extracted " << cards.size() << " 5x5 bingo cards.\n\n";
  bingo_input.close();


  int card_score = playBingoToWin(draw_numbers, cards);
  cout << "The winning card produced a score of " << card_score;

}

