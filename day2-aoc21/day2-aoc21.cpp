// day2-aoc21.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// Pilot the submarine.
//   Submarine accepts movement commands:
//     forward X, down X or up X to move X units in those directions.
//     down and up affect depth (thus up reduces depth, down increases)
//  There's already a planned course (the provided directions_input.txt
//  You start at 0,0 (x and y in a plane, starting at surface).
//  Figure out your final x and y positions after following the directions.
//  Multiply final horizontal  progress and depth, supply this as answer to part A.
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
using std::vector, std::string, std::ifstream;
using std::cout, std::endl;
const int HORIZONTAL = 1;
const int VERTICAL = 2;

struct Movement
{
  int dimension = 0;
  int distance = 0;
};

// For each line in the file, determine if the direction given is
// "up", "down" or "right", then set the dimension and distance 
// properties of a Movement data type accordingly.
// Returns Movement object.
Movement getMovement(string& course_line)
{
  // direction and distance are separated in the file by a space
  Movement move{0,0};
  int space_at = static_cast<int>(course_line.find_first_of(' '));
  string direction = course_line.substr(0, space_at);
  
  if (direction == "up" || direction == "down")
    move.dimension = VERTICAL;
  else
    move.dimension = HORIZONTAL;
  
  move.distance = std::stoi(course_line.substr(space_at + 1));
  if (direction == "up") move.distance *= -1;  // moving up is losing depth. Origin of y is at surface.
  return move;
}

// A file has been provided containing direction and distance (string, then number).
// See the Movement struct and getMovement function for more information.
vector<Movement> readCourseFromFile(const string path)
{
  vector<Movement> course; // depths will be moved by return value optimisation
  course.reserve(1001); // allocate a big chunk to avoid subsequent .
  std::ifstream course_input;
  course_input.open(path);
  while (course_input.good())
  {
    string course_line;
    std::getline(course_input, course_line);
    // i.e. down 2
    if (course_line != "")
    {
      course.push_back(getMovement(course_line));
    }
  }
  course_input.close();
  course.shrink_to_fit(); // free any overallocated memory
  cout << "Got " << course.size() << " course directions from " << path << "\n\n";

  return course;
}

// Uses all horizontal dimension moves for forward movement, all
// vertical for depth. Returns forward movement * depth in "Units"
int calcSubTranslationPart1(const vector<Movement>& course)
{
  // Part 1 total movement calculations: horizontal movement and depth
  int forward_movement{ 0 };
  int depth{ 0 };
  for (auto& move : course)
  {
    if (move.dimension == VERTICAL) depth += move.distance;
    if (move.dimension == HORIZONTAL) forward_movement += move.distance;
  }
  return forward_movement * depth;
}

// Part 2 changed calculations involving aim:
// "down and up" don't mean depth, they mean change your aim. 
// "forward" moves you in x but also depth (based on current aim)
// Returns forward movement * depth.
int calcSubTranslationPart2(const vector<Movement>& course)
{
  int forward_movement{ 0 };
  int depth = { 0 };
  int aim{ 0 };
  for (auto& move : course)
  {
    if (move.dimension == HORIZONTAL)
    {
      // horizontal moves you forward but also uses "aim" to calculate new depth.
      forward_movement += move.distance;
      depth += (aim * move.distance);
    }
    else if (move.dimension == VERTICAL)
    {
      // Up decreases aim by X, down increases by X.
      aim += move.distance;
    }
  }
  return forward_movement * depth;
}

int main()
{
  vector<Movement> course = readCourseFromFile("course_input.txt");
  
  int sub_translation_p1 = calcSubTranslationPart1(course);
  std::cout << " Part 1 calculation (vertical controls depth, horizontal controls movement in x)\n";
  std::cout << " ---- Depth x forward movement = " << sub_translation_p1 << ".\n\n";
  
  int sub_translation_p2 = calcSubTranslationPart2(course);
  std::cout << " Part 2 calculation (vertical controls aim, horizontal controls movement in x and depth)\n";
  std::cout << " Depth x forward movement = " << sub_translation_p2 << ".\n\n";
}

